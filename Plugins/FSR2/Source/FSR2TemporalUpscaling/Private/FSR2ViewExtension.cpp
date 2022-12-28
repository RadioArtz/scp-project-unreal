// This file is part of the FidelityFX Super Resolution 2.1 Unreal Engine Plugin.
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
#include "EngineUtils.h"
#include "Engine/StaticMesh.h"
#include "LandscapeProxy.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

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

static TAutoConsoleVariable<int32> CVarFSR2ForceLandscapeHISMMobility(
	TEXT("r.FidelityFX.FSR2.ForceLandscapeHISMMobility"),
	0,
	TEXT("Allow FSR2 to force the mobility of Landscape actors Hierarchical Instance Static Mesh components that use World-Position-Offset materials so they render valid velocities.\nSetting 1/'All Instances' is faster on the CPU, 2/'Instances with World-Position-Offset' is faster on the GPU."),
	ECVF_ReadOnly);

static void ForceLandscapeHISMMobility(FSceneViewFamily& InViewFamily, ALandscapeProxy* Landscape)
{
	for (FCachedLandscapeFoliage::TGrassSet::TIterator Iter(Landscape->FoliageCache.CachedGrassComps); Iter; ++Iter)
	{
		ULandscapeComponent* Component = (*Iter).Key.BasedOn.Get();
		if (Component)
		{
			UHierarchicalInstancedStaticMeshComponent* Used = (*Iter).Foliage.Get();
			if (Used && Used->Mobility == EComponentMobility::Static)
			{
				if (CVarFSR2ForceLandscapeHISMMobility.GetValueOnGameThread() == 2)
				{
					TArray<FStaticMaterial> const& Materials = Used->GetStaticMesh()->GetStaticMaterials();
					for (auto MaterialInfo : Materials)
					{
						const UMaterial* Material = MaterialInfo.MaterialInterface->GetMaterial_Concurrent();
						if (const FMaterialResource* MaterialResource = Material->GetMaterialResource(InViewFamily.GetFeatureLevel()))
						{
							check(IsInGameThread());
							bool bAlwaysHasVelocity = MaterialResource->MaterialModifiesMeshPosition_GameThread();
							if (bAlwaysHasVelocity)
							{
								Used->Mobility = EComponentMobility::Stationary;
								break;
							}
						}
					}
				}
				else
				{
					Used->Mobility = EComponentMobility::Stationary;
				}
			}
		}
	}
}

FFSR2ViewExtension::FFSR2ViewExtension(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister)
{
	static IConsoleVariable* CVarMinAutomaticViewMipBiasMin = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ViewTextureMipBias.Min"));
	static IConsoleVariable* CVarMinAutomaticViewMipBiasOffset = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ViewTextureMipBias.Offset"));
	static IConsoleVariable* CVarVertexDeformationOutputsVelocity = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VertexDeformationOutputsVelocity"));
	static IConsoleVariable* CVarReactiveMask = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR2.CreateReactiveMask"));
	static IConsoleVariable* CVarSeparateTranslucency = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SeparateTranslucency"));
	static IConsoleVariable* CVarSSRExperimentalDenoiser = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SSR.ExperimentalDenoiser"));
	static IConsoleVariable* CVarBasePassForceOutputsVelocity = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BasePassForceOutputsVelocity"));
	static IConsoleVariable* CVarFSR2SSRExperimentalDenoiser = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR2.UseSSRExperimentalDenoiser"));

	GFSR2CustomStaticScreenPercentage = nullptr;
	PreviousFSR2State = CVarEnableFSR2.GetValueOnAnyThread();
	PreviousFSR2StateRT = CVarEnableFSR2.GetValueOnAnyThread();
	CurrentFSR2StateRT = CVarEnableFSR2.GetValueOnAnyThread();
	SSRExperimentalDenoiser = 0;
	VertexDeformationOutputsVelocity = CVarVertexDeformationOutputsVelocity ? CVarVertexDeformationOutputsVelocity->GetInt() : 0;
	MinAutomaticViewMipBiasMin = CVarMinAutomaticViewMipBiasMin ? CVarMinAutomaticViewMipBiasMin->GetFloat() : 0;
	MinAutomaticViewMipBiasOffset = CVarMinAutomaticViewMipBiasOffset ? CVarMinAutomaticViewMipBiasOffset->GetFloat() : 0;
	SeparateTranslucency = CVarSeparateTranslucency ? CVarSeparateTranslucency->GetInt() : 0;
	SSRExperimentalDenoiser = CVarSSRExperimentalDenoiser ? CVarSSRExperimentalDenoiser->GetInt() : 0;
	BasePassForceOutputsVelocity = CVarBasePassForceOutputsVelocity ? CVarBasePassForceOutputsVelocity->GetInt() : 0;

	IFSR2TemporalUpscalingModule& FSR2ModuleInterface = FModuleManager::GetModuleChecked<IFSR2TemporalUpscalingModule>(TEXT("FSR2TemporalUpscaling"));
	if (FSR2ModuleInterface.GetTemporalUpscaler() == nullptr)
	{
		FFSR2TemporalUpscalingModule& FSR2Module = (FFSR2TemporalUpscalingModule&)FSR2ModuleInterface;
		TSharedPtr<FFSR2TemporalUpscaler, ESPMode::ThreadSafe> FSR2TemporalUpscaler = MakeShared<FFSR2TemporalUpscaler, ESPMode::ThreadSafe>();
		FSR2Module.SetTemporalUpscaler(FSR2TemporalUpscaler);
	}

	if (CVarEnableFSR2.GetValueOnAnyThread())
	{
		// Initialize by default for game, but not the editor unless we intend to use FSR2 in the viewport by default
		if (!GIsEditor || CVarEnableFSR2InEditor.GetValueOnAnyThread())
		{
			// In the Editor we don't want to initialize the custom static screen percentage interface at start up. This prevents a conflict with other plugins.
			if (!GIsEditor && ensureMsgf(GCustomStaticScreenPercentage == nullptr, TEXT("GCustomStaticScreenPercentage was already in use, and FSR2 was unable to initialize it.  Please ensure only one upscaling plugin is enabled at a time.")))
			{
				GCustomStaticScreenPercentage = FSR2ModuleInterface.GetCustomStaticScreenPercentage();
			}

			// Set this at startup so that it will apply consistently
			if (CVarFSR2AdjustMipBias.GetValueOnGameThread())
			{
				if (CVarMinAutomaticViewMipBiasMin != nullptr)
				{
					CVarMinAutomaticViewMipBiasMin->Set(float(0.f + log2(1.f / 3.0f) - 1.0f + FLT_EPSILON), EConsoleVariableFlags::ECVF_SetByCode);
				}
				if (CVarMinAutomaticViewMipBiasOffset != nullptr)
				{
					CVarMinAutomaticViewMipBiasOffset->Set(float(-1.0f + FLT_EPSILON), EConsoleVariableFlags::ECVF_SetByCode);
				}
			}

			if (CVarFSR2ForceVertexDeformationOutputsVelocity.GetValueOnGameThread())
			{
				if (CVarVertexDeformationOutputsVelocity != nullptr)
				{
					CVarVertexDeformationOutputsVelocity->Set(1, EConsoleVariableFlags::ECVF_SetByCode);
				}
				if (CVarBasePassForceOutputsVelocity != nullptr)
				{
					CVarBasePassForceOutputsVelocity->Set(1, EConsoleVariableFlags::ECVF_SetByCode);
				}
			}

			if (CVarReactiveMask && CVarReactiveMask->GetInt())
			{
				if (CVarSeparateTranslucency != nullptr)
				{
					CVarSeparateTranslucency->Set(1, EConsoleVariableFlags::ECVF_SetByCode);
				}

				if (CVarSSRExperimentalDenoiser != nullptr)
				{
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
			// Pretend it is disabled so that when the Editor does enable FSR2 the state change is picked up properly.
			PreviousFSR2State = false;
			PreviousFSR2StateRT = false;
			CurrentFSR2StateRT = false;
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
		static IConsoleVariable* CVarVertexDeformationOutputsVelocity = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VertexDeformationOutputsVelocity"));
		IFSR2TemporalUpscalingModule& FSR2ModuleInterface = FModuleManager::GetModuleChecked<IFSR2TemporalUpscalingModule>(TEXT("FSR2TemporalUpscaling"));
		check(FSR2ModuleInterface.GetFSR2Upscaler());
		int32 EnableFSR2 = CVarEnableFSR2.GetValueOnAnyThread();
		FSR2ModuleInterface.GetFSR2Upscaler()->Initialize();

#if WITH_EDITOR
		if (PreviousFSR2State && EnableFSR2 && GIsEditor && GIsPlayInEditorWorld && GCustomStaticScreenPercentage != FSR2ModuleInterface.GetCustomStaticScreenPercentage() && ensureMsgf(GCustomStaticScreenPercentage == nullptr, TEXT("GCustomStaticScreenPercentage was already in use, and FSR2 was unable to initialize it.  Please ensure only one upscaling plugin is enabled at a time.")))
		{
			GCustomStaticScreenPercentage = FSR2ModuleInterface.GetCustomStaticScreenPercentage();
		}
#endif

		if (EnableFSR2)
		{
			if (CVarFSR2ForceVertexDeformationOutputsVelocity.GetValueOnGameThread() && CVarVertexDeformationOutputsVelocity != nullptr && VertexDeformationOutputsVelocity == 0 && CVarVertexDeformationOutputsVelocity->GetInt() == 0)
			{
				VertexDeformationOutputsVelocity = CVarVertexDeformationOutputsVelocity->GetInt();
				CVarVertexDeformationOutputsVelocity->Set(1, EConsoleVariableFlags::ECVF_SetByCode);
			}

			if (CVarFSR2ForceLandscapeHISMMobility.GetValueOnGameThread())
			{
				// Landscape Hierarchical Instanced Static Mesh components are usually foliage and thus might use WPO.
				// To make it generate motion vectors it can't be Static which is hard-coded into the Engine.
				for (ALandscapeProxy* Landscape : TObjectRange<ALandscapeProxy>(RF_ClassDefaultObject | RF_ArchetypeObject, true, EInternalObjectFlags::PendingKill))
				{
					ForceLandscapeHISMMobility(InViewFamily, Landscape);
				}
			}
		}

		if (PreviousFSR2State != EnableFSR2)
		{
			// Update tracking of the FSR2 state when it is changed
			PreviousFSR2State = EnableFSR2;
			static IConsoleVariable* CVarMinAutomaticViewMipBiasMin = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ViewTextureMipBias.Min"));
			static IConsoleVariable* CVarMinAutomaticViewMipBiasOffset = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ViewTextureMipBias.Offset"));
			static IConsoleVariable* CVarSeparateTranslucency = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SeparateTranslucency"));
			static IConsoleVariable* CVarReactiveMask = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR2.CreateReactiveMask"));
			static IConsoleVariable* CVarSSRExperimentalDenoiser = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SSR.ExperimentalDenoiser"));
			static IConsoleVariable* CVarBasePassForceOutputsVelocity = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BasePassForceOutputsVelocity"));
			static IConsoleVariable* CVarFSR2SSRExperimentalDenoiser = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR2.UseSSRExperimentalDenoiser"));

			if (EnableFSR2)
			{
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
						CVarVertexDeformationOutputsVelocity->Set(1, EConsoleVariableFlags::ECVF_SetByCode);
					}
					if (CVarBasePassForceOutputsVelocity != nullptr)
					{
						CVarBasePassForceOutputsVelocity->Set(1, EConsoleVariableFlags::ECVF_SetByCode);
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
					if (CVarBasePassForceOutputsVelocity != nullptr)
					{
						CVarBasePassForceOutputsVelocity->Set(BasePassForceOutputsVelocity, EConsoleVariableFlags::ECVF_SetByCode);
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
