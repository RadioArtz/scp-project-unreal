// This file is part of the FidelityFX Super Resolution 2.0 Unreal Engine Plugin.
//
// Copyright (c) 2022 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#include "FSR2ViewExtension.h"
#include "FSR2TemporalUpscaler.h"
#include "FSR2TemporalUpscaling.h"
#include "PostProcess/PostProcessing.h"

#include "ScenePrivate.h"

static TAutoConsoleVariable<int32> CVarEnableFSR2(
	TEXT("r.FidelityFX.FSR2.Enabled"),
	1,
	TEXT("Enable FidelityFX Super Resolution for Temporal Upscale"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarEnableFSR2InEditor(
	TEXT("r.FidelityFX.FSR2.EnabledInEditorViewport"),
	0,
	TEXT("Enable FidelityFX Super Resolution for Temporal Upscale in the Editor viewport by default."),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarFSR2AdjustMipBias(
	TEXT("r.FidelityFX.FSR2.AdjustMipBias"),
	1,
	TEXT("Allow FSR2 to adjust the minimum global texture mip bias (r.ViewTextureMipBias.Min & r.ViewTextureMipBias.Offset)"),
	ECVF_ReadOnly);

static TAutoConsoleVariable<int32> CVarFSR2ForceVertexDeformationOutputsVelocity(
	TEXT("r.FidelityFX.FSR2.ForceVertexDeformationOutputsVelocity"),
	1,
	TEXT("Allow FSR2 to enable r.VertexDeformationOutputsVelocity to ensure that materials that use World-Position-Offset render valid velocities."),
	ECVF_ReadOnly);

FFSR2ViewExtension::FFSR2ViewExtension(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister)
{
	static IConsoleVariable* CVarMinAutomaticViewMipBiasMin = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ViewTextureMipBias.Min"));
	static IConsoleVariable* CVarMinAutomaticViewMipBiasOffset = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ViewTextureMipBias.Offset"));
	static IConsoleVariable* CVarVertexDeformationOutputsVelocity = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VertexDeformationOutputsVelocity"));
	static IConsoleVariable* CVarReactiveMask = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR2.CreateReactiveMask"));
	static IConsoleVariable* CVarSeparateTranslucency = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SeparateTranslucency"));
	static IConsoleVariable* CVarSSRExperimentalDenoiser = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SSR.ExperimentalDenoiser"));
	static IConsoleVariable* CVarFSR2SSRExperimentalDenoiser = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR2.UseSSRExperimentalDenoiser"));

	GFSR2CustomStaticScreenPercentage = nullptr;
	PreviousFSR2State = CVarEnableFSR2.GetValueOnAnyThread();
	PreviousFSR2StateRT = CVarEnableFSR2.GetValueOnAnyThread();
	CurrentFSR2StateRT = CVarEnableFSR2.GetValueOnAnyThread();
	SSRExperimentalDenoiser = 0;

	IFSR2TemporalUpscalingModule& FSR2ModuleInterface = FModuleManager::GetModuleChecked<IFSR2TemporalUpscalingModule>(TEXT("FSR2TemporalUpscaling"));
	if (FSR2ModuleInterface.GetTemporalUpscaler() == nullptr)
	{
		FFSR2TemporalUpscalingModule& FSR2Module = (FFSR2TemporalUpscalingModule&)FSR2ModuleInterface;
		TSharedPtr<FFSR2TemporalUpscaler, ESPMode::ThreadSafe> FSR2TemporalUpscaler = MakeShared<FFSR2TemporalUpscaler, ESPMode::ThreadSafe>();
		FSR2Module.SetTemporalUpscaler(FSR2TemporalUpscaler);
	}

	if (CVarEnableFSR2.GetValueOnAnyThread() && ensureMsgf(GCustomStaticScreenPercentage == nullptr, TEXT("GCustomStaticScreenPercentage was already in use, and FSR2 was unable to initialize it.  Please ensure only one upscaling plugin is enabled at a time.")))
	{
		GCustomStaticScreenPercentage = FSR2ModuleInterface.GetCustomStaticScreenPercentage();

		// Set this at startup so that it will apply consistently
		if (CVarFSR2AdjustMipBias.GetValueOnGameThread())
		{
			if (CVarMinAutomaticViewMipBiasMin != nullptr)
			{
				MinAutomaticViewMipBiasMin = CVarMinAutomaticViewMipBiasMin->GetFloat();
				CVarMinAutomaticViewMipBiasMin->Set(float(0.f + log2(1.f / 3.0f) - 1.0f + FLT_EPSILON), EConsoleVariableFlags::ECVF_SetByCode);
			}
			if (CVarMinAutomaticViewMipBiasOffset != nullptr)
			{
				MinAutomaticViewMipBiasOffset = CVarMinAutomaticViewMipBiasOffset->GetFloat();
				CVarMinAutomaticViewMipBiasOffset->Set(float(-1.0f + FLT_EPSILON), EConsoleVariableFlags::ECVF_SetByCode);
			}
		}

		if (CVarFSR2ForceVertexDeformationOutputsVelocity.GetValueOnGameThread())
		{
			if (CVarVertexDeformationOutputsVelocity != nullptr)
			{
				VertexDeformationOutputsVelocity = CVarVertexDeformationOutputsVelocity->GetInt();
				CVarVertexDeformationOutputsVelocity->Set(1, EConsoleVariableFlags::ECVF_SetByCode);
			}
		}

		if (CVarReactiveMask && CVarReactiveMask->GetInt())
		{
			if (CVarSeparateTranslucency != nullptr)
			{
				SeparateTranslucency = CVarSeparateTranslucency->GetInt();
				CVarSeparateTranslucency->Set(1, EConsoleVariableFlags::ECVF_SetByCode);
			}
			
			if (CVarSSRExperimentalDenoiser != nullptr)
			{
				SSRExperimentalDenoiser = CVarSSRExperimentalDenoiser->GetInt();
				if (CVarFSR2SSRExperimentalDenoiser)
				{
					CVarFSR2SSRExperimentalDenoiser->Set(SSRExperimentalDenoiser, EConsoleVariableFlags::ECVF_SetByCode);
				}
				CVarSSRExperimentalDenoiser->Set(1, EConsoleVariableFlags::ECVF_SetByCode);
			}
		}
	}
	else
	{
		// Disable FSR2 as it could not be initialised, this avoids errors if it is enabled later.
		PreviousFSR2State = false;
		PreviousFSR2StateRT = false;
		CurrentFSR2StateRT = false;
		CVarEnableFSR2->Set(0, EConsoleVariableFlags::ECVF_SetByCode);
	}
}

void FFSR2ViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	if (InViewFamily.GetFeatureLevel() >= ERHIFeatureLevel::SM5)
	{
		int32 EnableFSR2 = CVarEnableFSR2.GetValueOnAnyThread();
		if (PreviousFSR2State != EnableFSR2)
		{
			// Update tracking of the FSR2 state when it is changed
			PreviousFSR2State = EnableFSR2;
			static IConsoleVariable* CVarMinAutomaticViewMipBiasMin = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ViewTextureMipBias.Min"));
			static IConsoleVariable* CVarMinAutomaticViewMipBiasOffset = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ViewTextureMipBias.Offset"));
			static IConsoleVariable* CVarVertexDeformationOutputsVelocity = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VertexDeformationOutputsVelocity"));
			static IConsoleVariable* CVarSeparateTranslucency = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SeparateTranslucency"));
			static IConsoleVariable* CVarReactiveMask = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR2.CreateReactiveMask"));
			static IConsoleVariable* CVarSSRExperimentalDenoiser = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SSR.ExperimentalDenoiser"));
			static IConsoleVariable* CVarFSR2SSRExperimentalDenoiser = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR2.UseSSRExperimentalDenoiser"));

			if (EnableFSR2)
			{
				IFSR2TemporalUpscalingModule& FSR2ModuleInterface = FModuleManager::GetModuleChecked<IFSR2TemporalUpscalingModule>(TEXT("FSR2TemporalUpscaling"));
				if (GCustomStaticScreenPercentage != FSR2ModuleInterface.GetCustomStaticScreenPercentage())
				{
					GFSR2CustomStaticScreenPercentage = GCustomStaticScreenPercentage;
					GCustomStaticScreenPercentage = FSR2ModuleInterface.GetCustomStaticScreenPercentage();
				}

				// When toggling reapply the settings that FSR2 wants to override
				if (CVarFSR2AdjustMipBias.GetValueOnGameThread())
				{
					if (CVarMinAutomaticViewMipBiasMin != nullptr)
					{
						MinAutomaticViewMipBiasMin = CVarMinAutomaticViewMipBiasMin->GetFloat();
						CVarMinAutomaticViewMipBiasMin->Set(float(0.f + log2(1.f / 3.0f) - 1.0f + FLT_EPSILON), EConsoleVariableFlags::ECVF_SetByCode);
					}
					if (CVarMinAutomaticViewMipBiasOffset != nullptr)
					{
						MinAutomaticViewMipBiasOffset = CVarMinAutomaticViewMipBiasOffset->GetFloat();
						CVarMinAutomaticViewMipBiasOffset->Set(float(-1.0f + FLT_EPSILON), EConsoleVariableFlags::ECVF_SetByCode);
					}
				}

				if (CVarFSR2ForceVertexDeformationOutputsVelocity.GetValueOnGameThread())
				{
					if (CVarVertexDeformationOutputsVelocity != nullptr)
					{
						VertexDeformationOutputsVelocity = CVarVertexDeformationOutputsVelocity->GetInt();
						CVarVertexDeformationOutputsVelocity->Set(1, EConsoleVariableFlags::ECVF_SetByCode);
					}
				}

				if (CVarReactiveMask && CVarReactiveMask->GetInt())
				{
					if (CVarSeparateTranslucency != nullptr)
					{
						SeparateTranslucency = CVarSeparateTranslucency->GetInt();
						CVarSeparateTranslucency->Set(1, EConsoleVariableFlags::ECVF_SetByCode);
					}
					if (CVarSSRExperimentalDenoiser != nullptr)
					{
						SSRExperimentalDenoiser = CVarSSRExperimentalDenoiser->GetInt();
						if (CVarFSR2SSRExperimentalDenoiser != nullptr)
						{
							CVarFSR2SSRExperimentalDenoiser->Set(SSRExperimentalDenoiser, EConsoleVariableFlags::ECVF_SetByCode);
						}
						CVarSSRExperimentalDenoiser->Set(1, EConsoleVariableFlags::ECVF_SetByCode);
					}
				}
			}
			// Put the variables FSR2 modifies back to the way they were when FSR2 was toggled on.
			else
			{
				if (GFSR2CustomStaticScreenPercentage)
				{
					GCustomStaticScreenPercentage = GFSR2CustomStaticScreenPercentage;
					GFSR2CustomStaticScreenPercentage = nullptr;
				}

				if (CVarFSR2AdjustMipBias.GetValueOnGameThread())
				{
					if (CVarMinAutomaticViewMipBiasMin != nullptr)
					{
						CVarMinAutomaticViewMipBiasMin->Set(MinAutomaticViewMipBiasMin, EConsoleVariableFlags::ECVF_SetByCode);
					}
					if (CVarMinAutomaticViewMipBiasOffset != nullptr)
					{
						CVarMinAutomaticViewMipBiasOffset->Set(MinAutomaticViewMipBiasOffset, EConsoleVariableFlags::ECVF_SetByCode);
					}
				}

				if (CVarFSR2ForceVertexDeformationOutputsVelocity.GetValueOnGameThread())
				{
					if (CVarVertexDeformationOutputsVelocity != nullptr)
					{
						CVarVertexDeformationOutputsVelocity->Set(VertexDeformationOutputsVelocity, EConsoleVariableFlags::ECVF_SetByCode);
					}
				}

				if (CVarReactiveMask && CVarReactiveMask->GetInt())
				{
					if (CVarSeparateTranslucency != nullptr)
					{
						CVarSeparateTranslucency->Set(SeparateTranslucency, EConsoleVariableFlags::ECVF_SetByCode);
					}
					if (CVarSSRExperimentalDenoiser != nullptr)
					{
						CVarSSRExperimentalDenoiser->Set(SSRExperimentalDenoiser, EConsoleVariableFlags::ECVF_SetByCode);
					}
				}
			}
		}
	}
}

void FFSR2ViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	if (InViewFamily.GetFeatureLevel() >= ERHIFeatureLevel::SM5)
	{
		IFSR2TemporalUpscalingModule& FSR2ModuleInterface = FModuleManager::GetModuleChecked<IFSR2TemporalUpscalingModule>(TEXT("FSR2TemporalUpscaling"));
		FFSR2TemporalUpscaler* Upscaler = FSR2ModuleInterface.GetFSR2Upscaler();
		bool IsTemporalUpscalingRequested = false;
		for (int i = 0; i < InViewFamily.Views.Num(); i++)
		{
			const FSceneView* InView = InViewFamily.Views[i];
			if (ensure(InView))
			{
				if (Upscaler)
				{
					FGlobalShaderMap* GlobalMap = GetGlobalShaderMap(InViewFamily.GetFeatureLevel());
					Upscaler->SetSSRShader(GlobalMap);
				}

				// Don't run FSR2 if Temporal Upscaling is unused.
				IsTemporalUpscalingRequested |= (InView->PrimaryScreenPercentageMethod == EPrimaryScreenPercentageMethod::TemporalUpscale);
			}
		}

#if WITH_EDITOR
		IsTemporalUpscalingRequested &= Upscaler->IsEnabledInEditor();
#endif

		if (IsTemporalUpscalingRequested && CVarEnableFSR2.GetValueOnAnyThread() && (InViewFamily.GetTemporalUpscalerInterface() == nullptr))
		{
			InViewFamily.SetTemporalUpscalerInterface(Upscaler);
		}
	}
}

void FFSR2ViewExtension::PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily)
{
	if (InViewFamily.GetFeatureLevel() >= ERHIFeatureLevel::SM5)
	{
		// When the FSR2 plugin is enabled/disabled dispose of any previous history as it will be invalid if it comes from another upscaler
		CurrentFSR2StateRT = CVarEnableFSR2.GetValueOnRenderThread();
		if (PreviousFSR2StateRT != CurrentFSR2StateRT)
		{
			// This also requires updating our tracking of the FSR2 state
			PreviousFSR2StateRT = CurrentFSR2StateRT;
			for (auto* SceneView : InViewFamily.Views)
			{
				if (SceneView->bIsViewInfo)
				{
					FViewInfo* View = (FViewInfo*)SceneView;
					View->PrevViewInfo.CustomTemporalAAHistory.SafeRelease();
					if (!View->bStatePrevViewInfoIsReadOnly && View->ViewState)
					{
						View->ViewState->PrevFrameViewInfo.CustomTemporalAAHistory.SafeRelease();
					}
				}
			}
		}
	}
}

void FFSR2ViewExtension::PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
}

void FFSR2ViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs)
{
	// FSR2 requires the separate translucency data which is only available through the post-inputs so bind them to the upscaler now.
	if (View.GetFeatureLevel() >= ERHIFeatureLevel::SM5)
	{
		if (CVarEnableFSR2.GetValueOnAnyThread())
		{
			IFSR2TemporalUpscalingModule& FSR2ModuleInterface = FModuleManager::GetModuleChecked<IFSR2TemporalUpscalingModule>(TEXT("FSR2TemporalUpscaling"));
			FSR2ModuleInterface.GetFSR2Upscaler()->SetPostProcessingInputs(Inputs);
		}
	}
}

void FFSR2ViewExtension::PostRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily)
{
	// As FSR2 retains pointers/references to objects the engine is not expecting clear them out now to prevent leaks or accessing dangling pointers.
	if (InViewFamily.GetFeatureLevel() >= ERHIFeatureLevel::SM5)
	{
		if (CVarEnableFSR2.GetValueOnAnyThread())
		{
			IFSR2TemporalUpscalingModule& FSR2ModuleInterface = FModuleManager::GetModuleChecked<IFSR2TemporalUpscalingModule>(TEXT("FSR2TemporalUpscaling"));
			FSR2ModuleInterface.GetFSR2Upscaler()->EndOfFrame();
		}
	}
}
