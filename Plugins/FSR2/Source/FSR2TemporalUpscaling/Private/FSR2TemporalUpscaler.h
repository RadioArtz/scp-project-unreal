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
#pragma once

#include "Engine/Engine.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessUpscale.h"
#include "PostProcess/TemporalAA.h"
#include "CustomStaticScreenPercentage.h"
#include "ScreenSpaceDenoise.h"
#include "Containers/LockFreeList.h"
#include "FSR2TemporalUpscalerHistory.h"
#include "FSR2ScreenPercentageData.h"

struct FPostProcessingInputs;

//-------------------------------------------------------------------------------------
// Simplifies cross engine support.
//-------------------------------------------------------------------------------------
typedef IScreenSpaceDenoiser::FDiffuseIndirectOutputs FSSDSignalTextures;

//-------------------------------------------------------------------------------------
// These are the available backends for FSR2, native backends are preferred by default.
//-------------------------------------------------------------------------------------
enum class EFSR2TemporalUpscalerAPI : uint8
{
	D3D12,
	Vulkan,
	Unreal,
	Unsupported,
	Unknown
};

//-------------------------------------------------------------------------------------
// The core upscaler implementation for FSR2.
// Implements the ICustomStaticScreenPercentage to enforce FSR2's quality modes.
// Implements IScreenSpaceDenoiser in order to access the reflection texture data.
//-------------------------------------------------------------------------------------
class FFSR2TemporalUpscaler final : public ITemporalUpscaler, public ICustomStaticScreenPercentage, public IScreenSpaceDenoiser
{
	friend class FFSR2FXSystem;
public:
	FFSR2TemporalUpscaler();
	virtual ~FFSR2TemporalUpscaler();

	void Initialize() const;

	const TCHAR* GetDebugName() const override;

	void ReleaseState(FSR2StateRef State);

	static float GetResolutionFraction(uint32 Mode);

	class FRDGBuilder* GetGraphBuilder();

	void AddPasses(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		const ITemporalUpscaler::FPassInputs& PassInputs,
		FRDGTextureRef* OutSceneColorTexture,
		FIntRect* OutSceneColorViewRect,
		FRDGTextureRef* OutSceneColorHalfResTexture,
		FIntRect* OutSceneColorHalfResViewRect) const override;

	void SetupMainGameViewFamily(FSceneViewFamily& InViewFamily) override;

#if FSR2_ENGINE_SUPPORTS_SCREENPERCENTAGEDATA
	// Support ICustomStaticScreenPercentage for non-game view families (like MovieRenderQueue)
	void SetupViewFamily(FSceneViewFamily& ViewFamily, TSharedPtr<ICustomStaticScreenPercentageData> ScreenPercentageDataInterface) override;
#endif

	float GetMinUpsampleResolutionFraction() const override;
	float GetMaxUpsampleResolutionFraction() const override;

	void SetSSRShader(FGlobalShaderMap* GlobalMap);

	void CopyOpaqueSceneColor(FRHICommandListImmediate& RHICmdList, FRHIUniformBuffer* ViewUniformBuffer, const class FShaderParametersMetadata* SceneTexturesUniformBufferStruct, FRHIUniformBuffer* SceneTexturesUniformBuffer);

	void SetPostProcessingInputs(FPostProcessingInputs const& Inputs);

	void EndOfFrame();

#if WITH_EDITOR
	bool IsEnabledInEditor() const;
	void SetEnabledInEditor(bool bEnabled);
#endif

	FReflectionsOutputs DenoiseReflections(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FReflectionsInputs& ReflectionInputs,
		const FReflectionsRayTracingConfig RayTracingConfig) const override;

	EShadowRequirements GetShadowRequirements(
		const FViewInfo& View,
		const FLightSceneInfo& LightSceneInfo,
		const FShadowRayTracingConfig& RayTracingConfig) const override;

	void DenoiseShadowVisibilityMasks(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const TStaticArray<FShadowVisibilityParameters, IScreenSpaceDenoiser::kMaxBatchSize>& InputParameters,
		const int32 InputParameterCount,
		TStaticArray<FShadowVisibilityOutputs, IScreenSpaceDenoiser::kMaxBatchSize>& Outputs) const override;

	FPolychromaticPenumbraOutputs DenoisePolychromaticPenumbraHarmonics(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FPolychromaticPenumbraHarmonics& Inputs) const override;

	FReflectionsOutputs DenoiseWaterReflections(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FReflectionsInputs& ReflectionInputs,
		const FReflectionsRayTracingConfig RayTracingConfig) const override;

	FAmbientOcclusionOutputs DenoiseAmbientOcclusion(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FAmbientOcclusionInputs& ReflectionInputs,
		const FAmbientOcclusionRayTracingConfig RayTracingConfig) const override;

	FSSDSignalTextures DenoiseDiffuseIndirect(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FDiffuseIndirectInputs& Inputs,
		const FAmbientOcclusionRayTracingConfig Config) const override;

	FDiffuseIndirectOutputs DenoiseSkyLight(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FDiffuseIndirectInputs& Inputs,
		const FAmbientOcclusionRayTracingConfig Config) const override;

	FDiffuseIndirectOutputs DenoiseReflectedSkyLight(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FDiffuseIndirectInputs& Inputs,
		const FAmbientOcclusionRayTracingConfig Config) const override;
	FDiffuseIndirectHarmonic DenoiseDiffuseIndirectHarmonic(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FDiffuseIndirectHarmonic& Inputs,
		const FAmbientOcclusionRayTracingConfig Config) const override;

	bool SupportsScreenSpaceDiffuseIndirectDenoiser(EShaderPlatform Platform) const override;

	FSSDSignalTextures DenoiseScreenSpaceDiffuseIndirect(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FDiffuseIndirectInputs& Inputs,
		const FAmbientOcclusionRayTracingConfig Config) const override;

private:
	inline bool IsApiSupported() const
	{
		return Api != EFSR2TemporalUpscalerAPI::Unknown && Api != EFSR2TemporalUpscalerAPI::Unsupported;
	}

	void DeferredCleanup() const;

	mutable FPostProcessingInputs PostInputs;
	FDynamicResolutionStateInfos DynamicResolutionStateInfos;
	mutable TLockFreePointerListFIFO<FFSR2State, PLATFORM_CACHE_LINE_SIZE> AvailableStates;
	mutable EFSR2TemporalUpscalerAPI Api;
	mutable class IFSR2TemporalUpscalingAPI* ApiAccessor;
	mutable class FRDGBuilder* CurrentGraphBuilder;
	mutable const IScreenSpaceDenoiser* WrappedDenoiser;
	mutable FRDGTextureRef ReflectionTexture;
	mutable FTexture2DRHIRef SceneColorPreAlpha;
	mutable TRefCountPtr<IPooledRenderTarget> SceneColorPreAlphaRT;
	TMap<class FGlobalShaderMap*, struct FFSR2ShaderMap*> SSRShaderMaps;
#if WITH_EDITOR
	bool bEnabledInEditor;
#endif
};