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
#include "FSR2TemporalUpscaler.h"
#include "FSR2Include.h"
#include "FSR2TemporalUpscalerHistory.h"
#include "RHI/FSR2TemporalUpscalerRHIBackend.h"
#include "FSR2TemporalUpscalingAPI.h"
#include "SceneTextureParameters.h"
#include "TranslucentRendering.h"
#include "ScenePrivate.h"
#include "LogFSR2.h"
#include "LegacyScreenPercentageDriver.h"
#include "PlanarReflectionSceneProxy.h"
#include "ScreenSpaceRayTracing.h"
#include "Serialization/MemoryImage.h"
#include "Serialization/MemoryLayout.h"
#include "FXSystem.h"
#include "PostProcess/SceneRenderTargets.h"
#if FSR2_ENABLE_VK
#include "VulkanRHIBridge.h"
#endif


//------------------------------------------------------------------------------------------------------
// GPU statistics for the FSR2 passes.
//------------------------------------------------------------------------------------------------------
DECLARE_GPU_STAT(FidelityFXSuperResolution2Pass)
DECLARE_GPU_STAT_NAMED(FidelityFXFSR2Dispatch, TEXT("FidelityFX FSR2 Dispatch"));

//------------------------------------------------------------------------------------------------------
// Quality mode definitions
//------------------------------------------------------------------------------------------------------
static const FfxFsr2QualityMode DefaultQualityMode = FFX_FSR2_QUALITY_MODE_QUALITY;
static const FfxFsr2QualityMode LowestResolutionQualityMode = FFX_FSR2_QUALITY_MODE_ULTRA_PERFORMANCE;
static const FfxFsr2QualityMode HighestResolutionQualityMode = FFX_FSR2_QUALITY_MODE_QUALITY;

//------------------------------------------------------------------------------------------------------
// Console variables that control how FSR2 operates.
//------------------------------------------------------------------------------------------------------
static TAutoConsoleVariable<float> CVarFSR2Sharpness(
	TEXT("r.FidelityFX.FSR2.Sharpness"),
	0.0f,
	TEXT("Range from 0.0 to 1.0, when greater than 0 this enables Robust Contrast Adaptive Sharpening Filter to sharpen the output image. Default is 0."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2AutoExposure(
	TEXT("r.FidelityFX.FSR2.AutoExposure"),
	0,
	TEXT("True to use FSR2's own auto-exposure, otherwise the engine's auto-exposure value is used."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2HistoryFormat(
	TEXT("r.FidelityFX.FSR2.HistoryFormat"), 
	0,
	TEXT("Selects the bit-depth for the FSR2 history texture format, defaults to PF_FloatRGBA but can be set to PF_FloatR11G11B10 to reduce bandwidth at the expense of quality.\n")
	TEXT(" 0 - PF_FloatRGBA\n")
	TEXT(" 1 - PF_FloatR11G11B10\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarFSR2CreateReactiveMask(
	TEXT("r.FidelityFX.FSR2.CreateReactiveMask"),
	1,
	TEXT("Enable to generate a mask from the SceneColor, GBuffer, SeparateTranslucency & ScreenspaceReflections that determines how reactive each pixel should be. Defaults to 1 (Enabled)."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskReflectionScale(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskReflectionScale"),
	0.4f,
	TEXT("Range from 0.0 to 1.0 (Default 0.4), scales the Unreal engine reflection contribution to the reactive mask, which can be used to control the amount of aliasing on reflective surfaces."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskRoughnessScale(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskRoughnessScale"),
	0.15f,
	TEXT("Range from 0.0 to 1.0 (Default 0.15), scales the GBuffer roughness to provide a fallback value for the reactive mask when screenspace & planar reflections are disabled or don't affect a pixel."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskRoughnessBias(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskRoughnessBias"),
	0.25f,
	TEXT("Range from 0.0 to 1.0 (Default 0.25), biases the reactive mask value when screenspace/planar reflections are weak with the GBuffer roughness to account for reflection environment captures."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskRoughnessMaxDistance(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskRoughnessMaxDistance"),
	6000.0f,
	TEXT("Maximum distance in world units for using material roughness to contribute to the reactive mask, the maximum of this value and View.FurthestReflectionCaptureDistance will be used. Default is 6000.0."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2ReactiveMaskRoughnessForceMaxDistance(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskRoughnessForceMaxDistance"),
	0,
	TEXT("Enable to force the maximum distance in world units for using material roughness to contribute to the reactive mask rather than using View.FurthestReflectionCaptureDistance. Defaults to 0."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskReflectionLumaBias(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskReflectionLumaBias"),
	0.0f,
	TEXT("Range from 0.0 to 1.0 (Default: 0.0), biases the reactive mask by the luminance of the reflection. Use to balance aliasing against ghosting on brightly lit reflective surfaces."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskTranslucencyBias(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskTranslucencyBias"),
	0.5f,
	TEXT("Range from 0.0 to 1.0 (Default: 0.5), scales how much contribution translucency makes to the reactive mask. Higher values will make translucent materials more reactive which can reduce smearing."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskTranslucencyLumaBias(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskTranslucencyLumaBias"),
	0.8f,
	TEXT("Range from 0.0 to 1.0 (Default 0.8), biases the translucency contribution by the luminance of the transparency. Higher values will make bright translucent materials more reactive which can reduce smearing."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskPreDOFTranslucencyScale(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskPreDOFTranslucencyScale"),
	0.8f,
	TEXT("Range from 0.0 to 1.0 (Default 0.8), scales how much contribution pre-Depth-of-Field translucency color makes to the reactive mask. Higher values will make translucent materials more reactive which can reduce smearing."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2ReactiveMaskPreDOFTranslucencyMax(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskPreDOFTranslucencyMax"),
	0,
	TEXT("Toggle to determine whether to use the max(SceneColorPostDepthOfField - SceneColorPreDepthOfField) or length(SceneColorPostDepthOfField - SceneColorPreDepthOfField) to determine the contribution of Pre-Depth-of-Field translucency. Default is 0."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskTranslucencyMaxDistance(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskTranslucencyMaxDistance"),
	500000.0f,
	TEXT("Maximum distance in world units for using translucency to contribute to the reactive mask. This is a way to remove sky-boxes and other back-planes from the reactive mask, at the expense of nearer translucency not being reactive. Default is 500000.0."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskForceReactiveMaterialValue(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskForceReactiveMaterialValue"),
	0.0f,
	TEXT("Force the reactive mask value for Reactive Shading Model materials, when > 0 this value can be used to override the value supplied in the Material Graph. Default is 0 (Off)."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2ReactiveMaskReactiveShadingModelID(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskReactiveShadingModelID"),
	MSM_NUM,
	TEXT("Treat the specified shading model as reactive, taking the CustomData0.x value as the reactive value to write into the mask. Default is MSM_NUM (Off)."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2UseNativeDX12(
	TEXT("r.FidelityFX.FSR2.UseNativeDX12"),
	1,
	TEXT("True to use FSR2's native & optimised D3D12 backend, false to use the fallback implementation based on Unreal's RHI. Default is 1."),
	ECVF_ReadOnly
);

static TAutoConsoleVariable<int32> CVarFSR2UseNativeVulkan(
	TEXT("r.FidelityFX.FSR2.UseNativeVulkan"),
	1,
	TEXT("True to use FSR2's native & optimised Vulkan backend, false to use the fallback implementation based on Unreal's RHI. Default is 1."),
	ECVF_ReadOnly
);

static TAutoConsoleVariable<int32> CVarFSR2UseExperimentalSSRDenoiser(
	TEXT("r.FidelityFX.FSR2.UseSSRExperimentalDenoiser"),
	0,
	TEXT("Set to 1 to use r.SSR.ExperimentalDenoiser when FSR2 is enabled. This is required when r.FidelityFX.FSR2.CreateReactiveMask is enabled as the FSR2 plugin sets r.SSR.ExperimentalDenoiser to 1 in order to capture reflection data to generate the reactive mask. Default is 0."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2DeDitherMode(
	TEXT("r.FidelityFX.FSR2.DeDither"),
	2,
	TEXT("Adds an extra pass to de-dither and avoid dithered/thin appearance. Default is 0 - Off. \n")
	TEXT(" 0 - Off. \n")
	TEXT(" 1 - Full. Attempts to de-dither the whole scene. \n")
	TEXT(" 2 - Hair only. Will only de-dither around Hair shading model pixels - requires the Deferred Renderer. \n"),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2QualityMode(
	TEXT("r.FidelityFX.FSR2.QualityMode"),
	DefaultQualityMode,
	TEXT("FSR2 Mode [1-4].  Lower values yield superior images.  Higher values yield improved performance.  Default is 1 - Quality.\n")
	TEXT(" 1 - Quality				1.5x \n")
	TEXT(" 2 - Balanced				1.7x \n")
	TEXT(" 3 - Performance			2.0x \n")
	TEXT(" 4 - Ultra Performance	3.0x \n"),
	ECVF_RenderThreadSafe
);

//------------------------------------------------------------------------------------------------------
// Unreal shader to convert from the Velocity texture format to the Motion Vectors used by FSR2.
//------------------------------------------------------------------------------------------------------
class FFSR2ConvertVelocityCS : public FGlobalShader
{
public:
	static const int ThreadgroupSizeX = 8;
	static const int ThreadgroupSizeY = 8;
	static const int ThreadgroupSizeZ = 1;

	DECLARE_GLOBAL_SHADER(FFSR2ConvertVelocityCS);
	SHADER_USE_PARAMETER_STRUCT(FFSR2ConvertVelocityCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		RDG_TEXTURE_ACCESS(DepthTexture, ERHIAccess::SRVCompute)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, InputDepth)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, InputVelocity)
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutputTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), ThreadgroupSizeX);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), ThreadgroupSizeY);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEZ"), ThreadgroupSizeZ);
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
	}
};
IMPLEMENT_GLOBAL_SHADER(FFSR2ConvertVelocityCS, "/Plugin/FSR2/Private/PostProcessFFX_FSR2ConvertVelocity.usf", "MainCS", SF_Compute);

//------------------------------------------------------------------------------------------------------
// Unreal shader to generate mask textures for translucency & reactivity to be used in FSR2.
//------------------------------------------------------------------------------------------------------
class FFSR2CreateReactiveMaskCS : public FGlobalShader
{
public:
	static const int ThreadgroupSizeX = 8;
	static const int ThreadgroupSizeY = 8;
	static const int ThreadgroupSizeZ = 1;

	DECLARE_GLOBAL_SHADER(FFSR2CreateReactiveMaskCS);
	SHADER_USE_PARAMETER_STRUCT(FFSR2CreateReactiveMaskCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		RDG_TEXTURE_ACCESS(DepthTexture, ERHIAccess::SRVCompute)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, InputSeparateTranslucency)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, GBufferB)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, GBufferD)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, ReflectionTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, InputDepth)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, SceneColor)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, SceneColorPreAlpha)
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, ReactiveMask)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, CompositeMask)
		SHADER_PARAMETER_SAMPLER(SamplerState, Sampler)
		SHADER_PARAMETER(float, FurthestReflectionCaptureDistance)
		SHADER_PARAMETER(float, ReactiveMaskReflectionScale)
		SHADER_PARAMETER(float, ReactiveMaskRoughnessScale)
		SHADER_PARAMETER(float, ReactiveMaskRoughnessBias)
		SHADER_PARAMETER(float, ReactiveMaskReflectionLumaBias)
		SHADER_PARAMETER(float, ReactiveMaskTranslucencyBias)
		SHADER_PARAMETER(float, ReactiveMaskTranslucencyLumaBias)
		SHADER_PARAMETER(float, ReactiveMaskPreDOFTranslucencyScale)
		SHADER_PARAMETER(uint32, ReactiveMaskPreDOFTranslucencyMax)
		SHADER_PARAMETER(float, ReactiveMaskTranslucencyMaxDistance)
		SHADER_PARAMETER(float, ForceLitReactiveValue)
		SHADER_PARAMETER(uint32, ReactiveShadingModelID)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), ThreadgroupSizeX);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), ThreadgroupSizeY);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEZ"), ThreadgroupSizeZ);
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
	}
};
IMPLEMENT_GLOBAL_SHADER(FFSR2CreateReactiveMaskCS, "/Plugin/FSR2/Private/PostProcessFFX_FSR2CreateReactiveMask.usf", "MainCS", SF_Compute);

//------------------------------------------------------------------------------------------------------
// Unreal shader to blend hair which is dithered and FSR2 doesn't handle that well.
//------------------------------------------------------------------------------------------------------
class FFSR2DeDitherCS : public FGlobalShader
{
public:
	static const int ThreadgroupSizeX = 8;
	static const int ThreadgroupSizeY = 8;
	static const int ThreadgroupSizeZ = 1;

	DECLARE_GLOBAL_SHADER(FFSR2DeDitherCS);
	SHADER_USE_PARAMETER_STRUCT(FFSR2DeDitherCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, GBufferB)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, SceneColor)
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, BlendSceneColor)
		SHADER_PARAMETER(uint32, FullDeDither)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), ThreadgroupSizeX);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), ThreadgroupSizeY);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEZ"), ThreadgroupSizeZ);
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
		OutEnvironment.SetDefine(TEXT("UNREAL_ENGINE_MAJOR_VERSION"), ENGINE_MAJOR_VERSION);
	}
};
IMPLEMENT_GLOBAL_SHADER(FFSR2DeDitherCS, "/Plugin/FSR2/Private/PostProcessFFX_FSR2DeDither.usf", "MainCS", SF_Compute);

//------------------------------------------------------------------------------------------------------
// Map of ScreenSpaceReflection shaders so that FSR2 can swizzle the shaders inside the GlobalShaderMap.
// This is necessary so that FSR2 can access the ScreenSpaceReflection data through the ReflectionDenoiser plugin without changing their appearance. 
//------------------------------------------------------------------------------------------------------
struct FFSR2ShaderMap
{
	TMemoryImagePtr<FShader> DefaultShaders[(uint32)ESSRQuality::MAX];
	TMemoryImagePtr<FShader> DenoiseShaders[(uint32)ESSRQuality::MAX];
	uint32 DefaultIndex[(uint32)ESSRQuality::MAX];
	uint32 DenoiseIndex[(uint32)ESSRQuality::MAX];
	bool bSwapped;
};

//------------------------------------------------------------------------------------------------------
// The FFSR2ShaderMapContent structure allows access to the internals of FShaderMapContent so that FSR2 can swap the Default & Denoised variants of ScreenSpaceReflections.
//------------------------------------------------------------------------------------------------------
class FFSR2ShaderMapContent
{
public:
	DECLARE_TYPE_LAYOUT(FFSR2ShaderMapContent, NonVirtual);

	using FMemoryImageHashTable = THashTable<FMemoryImageAllocator>;

	LAYOUT_FIELD(FMemoryImageHashTable, ShaderHash);
	LAYOUT_FIELD(TMemoryImageArray<FHashedName>, ShaderTypes);
	LAYOUT_FIELD(TMemoryImageArray<int32>, ShaderPermutations);
	LAYOUT_FIELD(TMemoryImageArray<TMemoryImagePtr<FShader>>, Shaders);
	LAYOUT_FIELD(TMemoryImageArray<TMemoryImagePtr<FShaderPipeline>>, ShaderPipelines);
	/** The platform this shader map was compiled with */
	LAYOUT_FIELD(TEnumAsByte<EShaderPlatform>, Platform);
};
static_assert(sizeof(FShaderMapContent) == sizeof(FFSR2ShaderMapContent), "FFSR2ShaderMapContent must match the layout of FShaderMapContent so we can access the SSR shaders!");

//------------------------------------------------------------------------------------------------------
// Definitions used by the ScreenSpaceReflections shaders needed to perform necessary swizzling.
//------------------------------------------------------------------------------------------------------
class FSSRQualityDim : SHADER_PERMUTATION_ENUM_CLASS("SSR_QUALITY", ESSRQuality);
class FSSROutputForDenoiser : SHADER_PERMUTATION_BOOL("SSR_OUTPUT_FOR_DENOISER");
struct FFSR2ScreenSpaceReflectionsPS
{
	using FPermutationDomain = TShaderPermutationDomain<FSSRQualityDim, FSSROutputForDenoiser>;
};

//------------------------------------------------------------------------------------------------------
// In order to access the separate translucency data prior to our code executing it is necessary to gain access to FSeparateTranslucencyTextures internals.
//------------------------------------------------------------------------------------------------------
class FSeparateTranslucencyTexturesAccessor
{
public:
	FSeparateTranslucencyDimensions Dimensions;
	FRDGTextureMSAA ColorTexture;
	FRDGTextureMSAA ColorModulateTexture;
	FRDGTextureMSAA DepthTexture;
};
static_assert(sizeof(FSeparateTranslucencyTextures) == sizeof(FSeparateTranslucencyTexturesAccessor), "FSeparateTranslucencyTexturesAccessor must match the layout of FSeparateTranslucencyTextures so we can access the translucency texture!");

//------------------------------------------------------------------------------------------------------
// Internal function definitions
// Many of these are replicas of UE functions used in the denoiser API implementation so that we match the default engine behaviour.
//------------------------------------------------------------------------------------------------------
static bool FSR2HasDeferredPlanarReflections(const FViewInfo& View)
{
	if (View.bIsPlanarReflection || View.bIsReflectionCapture)
	{
		return false;
	}

	// Prevent rendering unsupported views when ViewIndex >= GMaxPlanarReflectionViews
	// Planar reflections in those views will fallback to other reflection methods
	{
		int32 ViewIndex = INDEX_NONE;

		View.Family->Views.Find(&View, ViewIndex);

		if (ViewIndex >= GMaxPlanarReflectionViews)
		{
			return false;
		}
	}

	bool bAnyVisiblePlanarReflections = false;
	FScene* Scene = (FScene*)View.Family->Scene;
	for (int32 PlanarReflectionIndex = 0; PlanarReflectionIndex < Scene->PlanarReflections.Num(); PlanarReflectionIndex++)
	{
		FPlanarReflectionSceneProxy* ReflectionSceneProxy = Scene->PlanarReflections[PlanarReflectionIndex];

		if (View.ViewFrustum.IntersectBox(ReflectionSceneProxy->WorldBounds.GetCenter(), ReflectionSceneProxy->WorldBounds.GetExtent()))
		{
			bAnyVisiblePlanarReflections = true;
			break;
		}
	}

	bool bComposePlanarReflections = Scene->PlanarReflections.Num() > 0 && bAnyVisiblePlanarReflections;

	return bComposePlanarReflections;
}

static bool FSR2ShouldRenderRayTracingEffect(bool bEffectEnabled)
{
	if (!IsRayTracingEnabled())
	{
		return false;
	}

	static auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.ForceAllRayTracingEffects"));
	const int32 OverrideMode = CVar != nullptr ? CVar->GetInt() : -1;

	if (OverrideMode >= 0)
	{
		return OverrideMode > 0;
	}
	else
	{
		return bEffectEnabled;
	}
}

static int32 FSR2GetRayTracingReflectionsSamplesPerPixel(const FViewInfo& View)
{
	static IConsoleVariable* RayTracingReflectionSamplesPerPixel = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Reflections.SamplesPerPixel"));
	return RayTracingReflectionSamplesPerPixel && RayTracingReflectionSamplesPerPixel->GetInt() >= 0 ? RayTracingReflectionSamplesPerPixel->GetInt() : View.FinalPostProcessSettings.RayTracingReflectionsSamplesPerPixel;
}

static bool FSR2ShouldRenderRayTracingReflections(const FViewInfo& View)
{
	const bool bThisViewHasRaytracingReflections = View.FinalPostProcessSettings.ReflectionsType == EReflectionsType::RayTracing;

	static IConsoleVariable* RayTracingReflections = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Reflections"));
	const bool bReflectionsCvarEnabled = RayTracingReflections && RayTracingReflections->GetInt() < 0
		? bThisViewHasRaytracingReflections
		: (RayTracingReflections && RayTracingReflections->GetInt() != 0);

	const bool bReflectionPassEnabled = bReflectionsCvarEnabled && (FSR2GetRayTracingReflectionsSamplesPerPixel(View) > 0);

	return FSR2ShouldRenderRayTracingEffect(bReflectionPassEnabled);
}

bool IsFSR2SSRTemporalPassRequired(const FViewInfo& View)
{
	static const auto CVarSSRTemporalEnabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.SSR.Temporal"));
	
	if (!View.State)
	{
		return false;
	}
	return View.AntiAliasingMethod != AAM_TemporalAA || (CVarSSRTemporalEnabled && CVarSSRTemporalEnabled->GetValueOnAnyThread() != 0);
}

#if FSR2_ENABLE_VK
static FfxResource FSR2GetTextureFromRHIResource(IFSR2TemporalUpscalingAPI* ApiAccessor, FfxFsr2Context* context, FRHITexture* Texture, FfxResourceStates state = FFX_RESOURCE_STATE_COMPUTE_READ)
{
	FIntVector Size = Texture->GetSizeXYZ();
	return ffxGetTextureResourceVK(context, (VkImage)Texture->GetNativeResource(), (VkImageView)ApiAccessor->GetNativeTextureResource(Texture), Size.X, Size.Y, (VkFormat)ApiAccessor->GetNativeTextureFormat(Texture), nullptr, state);
}
#endif

static inline float FSR2_GetScreenResolutionFromScalingMode(FfxFsr2QualityMode QualityMode)
{
	return 1.0f / ffxFsr2GetUpscaleRatioFromQualityMode(QualityMode);
}

//------------------------------------------------------------------------------------------------------
// The only way to gather all translucency contribution is to compare the SceneColor data prior and after translucency.
// This requires using the FFXSystemInterface which provides a callback invoked after completing opaque rendering of SceneColor.
//------------------------------------------------------------------------------------------------------
class FFSR2FXSystem : public FFXSystemInterface
{
	FGPUSortManager* GPUSortManager;
	FFSR2TemporalUpscaler* Upscaler;
public:
	static const FName FXName;

	FFXSystemInterface* GetInterface(const FName& InName) final
	{
		return InName == FFSR2FXSystem::FXName ? this : nullptr;
	}

	void Tick(float DeltaSeconds) final {}

#if WITH_EDITOR
	void Suspend() final {}

	void Resume() final {}
#endif // #if WITH_EDITOR

	void DrawDebug(FCanvas* Canvas) final {}

	void AddVectorField(UVectorFieldComponent* VectorFieldComponent) final {}

	void RemoveVectorField(UVectorFieldComponent* VectorFieldComponent) final {}

	void UpdateVectorField(UVectorFieldComponent* VectorFieldComponent) final {}

	void PreInitViews(FRHICommandListImmediate& RHICmdList, bool bAllowGPUParticleUpdate) final {}

	void PostInitViews(FRHICommandListImmediate& RHICmdList, FRHIUniformBuffer* ViewUniformBuffer, bool bAllowGPUParticleUpdate) final {}

	bool UsesGlobalDistanceField() const final { return false; }

	bool UsesDepthBuffer() const final { return false; }

	bool RequiresEarlyViewUniformBuffer() const final { return false; }

	void PreRender(FRHICommandListImmediate& RHICmdList, const class FGlobalDistanceFieldParameterData* GlobalDistanceFieldParameterData, bool bAllowGPUParticleSceneUpdate) final {}

	void PostRenderOpaque(
		FRHICommandListImmediate& RHICmdList, 
		FRHIUniformBuffer* ViewUniformBuffer,
		const class FShaderParametersMetadata* SceneTexturesUniformBufferStruct,
		FRHIUniformBuffer* SceneTexturesUniformBuffer,
		bool bAllowGPUParticleUpdate) final
	{
		Upscaler->CopyOpaqueSceneColor(RHICmdList, ViewUniformBuffer, SceneTexturesUniformBufferStruct, SceneTexturesUniformBuffer);
	}

	FGPUSortManager* GetGPUSortManager() const 
	{
		return GPUSortManager;
	}

	FFSR2FXSystem(FFSR2TemporalUpscaler* InUpscaler, FGPUSortManager* InGPUSortManager)
	: GPUSortManager(InGPUSortManager)
		, Upscaler(InUpscaler)
	{
		check(GPUSortManager && Upscaler);
	}
	~FFSR2FXSystem() {}
};
FName const FFSR2FXSystem::FXName(TEXT("FSR2FXSystem"));

//------------------------------------------------------------------------------------------------------
// Definition of inputs & outputs for the FSR2 pass used by the native backends.
//------------------------------------------------------------------------------------------------------
struct FFSR2Pass
{
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		RDG_TEXTURE_ACCESS(ColorTexture, ERHIAccess::SRVMask)
		RDG_TEXTURE_ACCESS(DepthTexture, ERHIAccess::SRVMask)
		RDG_TEXTURE_ACCESS(VelocityTexture, ERHIAccess::SRVMask)
		RDG_TEXTURE_ACCESS(ExposureTexture, ERHIAccess::SRVMask)
		RDG_TEXTURE_ACCESS(ReactiveMaskTexture, ERHIAccess::SRVMask)
		RDG_TEXTURE_ACCESS(CompositeMaskTexture, ERHIAccess::SRVMask)
		RDG_TEXTURE_ACCESS(OutputTexture, ERHIAccess::UAVMask)
		RDG_BUFFER_ACCESS(DummyBuffer, ERHIAccess::UAVMask)
	END_SHADER_PARAMETER_STRUCT()
};

//------------------------------------------------------------------------------------------------------
// FFSR2TemporalUpscaler implementation.
//------------------------------------------------------------------------------------------------------
FFSR2TemporalUpscaler::FFSR2TemporalUpscaler()
: Api(EFSR2TemporalUpscalerAPI::Unknown)
, ApiAccessor(nullptr)
, CurrentGraphBuilder(nullptr)
, WrappedDenoiser(nullptr)
, ReflectionTexture(nullptr)
{
	FMemory::Memzero(PostInputs);

#if WITH_EDITOR
	bEnabledInEditor = true;
#endif

	FFSR2TemporalUpscaler* self = this;
	FFXSystemInterface::RegisterCustomFXSystem(
		FFSR2FXSystem::FXName, 
		FCreateCustomFXSystemDelegate::CreateLambda([self](ERHIFeatureLevel::Type InFeatureLevel, EShaderPlatform InShaderPlatform, FGPUSortManager* InGPUSortManager) -> FFXSystemInterface*
	{
		return new FFSR2FXSystem(self, InGPUSortManager);
	}));
}

FFSR2TemporalUpscaler::~FFSR2TemporalUpscaler()
{
	DeferredCleanup();
	if (ApiAccessor)
	{
		delete ApiAccessor;
	}
	for (auto Pair : SSRShaderMaps)
	{
		delete Pair.Value;
	}
	FFXSystemInterface::UnregisterCustomFXSystem(FFSR2FXSystem::FXName);
}

const TCHAR* FFSR2TemporalUpscaler::GetDebugName() const
{
	return TEXT("FFSR2TemporalUpscaler");
}

void FFSR2TemporalUpscaler::ReleaseState(FSR2StateRef State)
{
	FFSR2State* Ptr = State.GetReference();
	if (Ptr)
	{
		Ptr->AddRef();
		AvailableStates.Push(Ptr);
	}
}

void FFSR2TemporalUpscaler::DeferredCleanup() const
{
	FFSR2State* Ptr = AvailableStates.Pop();
	while (Ptr)
	{
		Ptr->Release();
		Ptr = AvailableStates.Pop();
	}
}

float FFSR2TemporalUpscaler::GetResolutionFraction(uint32 Mode)
{
	FfxFsr2QualityMode QualityMode = FMath::Clamp<FfxFsr2QualityMode>((FfxFsr2QualityMode)Mode, HighestResolutionQualityMode, LowestResolutionQualityMode);
	const float ResolutionFraction = FSR2_GetScreenResolutionFromScalingMode(QualityMode);
	return ResolutionFraction;
}

FRDGBuilder* FFSR2TemporalUpscaler::GetGraphBuilder()
{
	return CurrentGraphBuilder;
}

void FFSR2TemporalUpscaler::Initialize() const
{
	if (Api == EFSR2TemporalUpscalerAPI::Unknown)
	{
		FString RHIName = GDynamicRHI->GetName();

		// Prefer the native backends unless they've been disabled
#if FSR2_ENABLE_DX12
		if (RHIName == FFSR2Strings::D3D12 && CVarFSR2UseNativeDX12.GetValueOnAnyThread())
		{
			Api = EFSR2TemporalUpscalerAPI::D3D12;
			ApiAccessor = new FFSR2TemporalUpscalingD3D12();
		}
		else 
#endif
#if FSR2_ENABLE_VK
		if (RHIName == FFSR2Strings::Vulkan && CVarFSR2UseNativeVulkan.GetValueOnAnyThread() && FFFXFSR2Vulkan::IsNativeBackendSupported() &&
			FSR2VulkanLoader::Get().LoadVulkan(static_cast<VkInstance>(GDynamicRHI->RHIGetNativeInstance())))
		{
			Api = EFSR2TemporalUpscalerAPI::Vulkan;
			ApiAccessor = new FFSR2TemporalUpscalingVulkan();
		}
		else
#endif
		// The fallback implementation requires SM5
		if (IsFeatureLevelSupported(GMaxRHIShaderPlatform, ERHIFeatureLevel::SM5))
		{
			Api = EFSR2TemporalUpscalerAPI::Unreal;
		}
		else
		{
			Api = EFSR2TemporalUpscalerAPI::Unsupported;
			UE_LOG(LogFSR2, Error, TEXT("FSR2 Temporal Upscaler not supported by '%s' rhi"), *RHIName);
		}

		if (IsApiSupported())
		{
			// Wrap any existing denoiser API as we override this to be able to generate the reactive mask.
			WrappedDenoiser = GScreenSpaceDenoiser;
			if (!WrappedDenoiser)
			{
				WrappedDenoiser = IScreenSpaceDenoiser::GetDefaultDenoiser();
			}
			check(WrappedDenoiser);
			GScreenSpaceDenoiser = this;
		}
	}
}

void FFSR2TemporalUpscaler::AddPasses(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	const ITemporalUpscaler::FPassInputs& PassInputs,
	FRDGTextureRef* OutSceneColorTexture,
	FIntRect* OutSceneColorViewRect,
	FRDGTextureRef* OutSceneColorHalfResTexture,
	FIntRect* OutSceneColorHalfResViewRect) const
{
	// In the MovieRenderPipeline the output extents can be smaller than the input, FSR2 doesn't handle that.
	// In that case we shall fall back to the default upscaler so we render properly.
	FIntPoint InputExtents = View.ViewRect.Size();
	FIntPoint InputExtentsQuantized = View.ViewRect.Size();
	FIntPoint OutputExtents;
	QuantizeSceneBufferSize(View.GetSecondaryViewRectSize(), OutputExtents);
	OutputExtents = FIntPoint(FMath::Max(InputExtents.X, OutputExtents.X), FMath::Max(InputExtents.Y, OutputExtents.Y));

	Initialize();

	bool const bValidEyeAdaptation = View.HasValidEyeAdaptationTexture();
	bool const bWaveOps = FDataDrivenShaderPlatformInfo::GetSupportsWaveOperations(View.GetShaderPlatform());
	bool const bHasAutoExposure = (bValidEyeAdaptation || CVarFSR2AutoExposure.GetValueOnRenderThread());

	if (IsApiSupported() && (View.PrimaryScreenPercentageMethod == EPrimaryScreenPercentageMethod::TemporalUpscale) && bHasAutoExposure && (InputExtents.X < OutputExtents.X) && (InputExtents.Y < OutputExtents.Y))
	{
		RDG_GPU_STAT_SCOPE(GraphBuilder, FidelityFXSuperResolution2Pass);
		RDG_EVENT_SCOPE(GraphBuilder, "FidelityFXSuperResolution2Pass");

		CurrentGraphBuilder = &GraphBuilder;

		check(PassInputs.bAllowDownsampleSceneColor == false);

		const bool CanWritePrevViewInfo = !View.bStatePrevViewInfoIsReadOnly && View.ViewState;

		bool bHistoryValid = View.PrevViewInfo.TemporalAAHistory.IsValid() && View.ViewState && !View.bCameraCut;

		FRDGTextureRef SceneColor = PassInputs.SceneColorTexture;
		FRDGTextureRef SceneDepth = PassInputs.SceneDepthTexture;

		// Quantize the buffers to match UE behavior
		QuantizeSceneBufferSize(View.ViewRect.Size(), InputExtentsQuantized);

		//------------------------------------------------------------------------------------------------------
		// Create Reactive Mask
		//   Create a reactive mask from separate translucency.
		//------------------------------------------------------------------------------------------------------
		FRDGTextureRef VelocityTexture = PassInputs.SceneVelocityTexture;
		if (!VelocityTexture)
		{
			VelocityTexture = (*PostInputs.SceneTextures)->GBufferVelocityTexture;
		}

		FRDGTextureSRVDesc DepthDesc = FRDGTextureSRVDesc::Create(SceneDepth);
		FRDGTextureSRVDesc VelocityDesc = FRDGTextureSRVDesc::Create(VelocityTexture);
		FRDGTextureDesc ReactiveMaskDesc = FRDGTextureDesc::Create2D(InputExtentsQuantized, PF_R8, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable);
		FRDGTextureRef ReactiveMaskTexture = nullptr;
		FRDGTextureDesc CompositeMaskDesc = FRDGTextureDesc::Create2D(InputExtentsQuantized, PF_R8, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable);
		FRDGTextureRef CompositeMaskTexture = nullptr;
		FRDGTextureDesc SceneColorDesc = FRDGTextureDesc::Create2D(InputExtentsQuantized, SceneColor->Desc.Format, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable);
		
		if (CVarFSR2CreateReactiveMask.GetValueOnRenderThread())
		{
			ReactiveMaskTexture = GraphBuilder.CreateTexture(ReactiveMaskDesc, TEXT("FSR2ReactiveMaskTexture"));
			CompositeMaskTexture = GraphBuilder.CreateTexture(CompositeMaskDesc, TEXT("FSR2CompositeMaskTexture"));
			{
				FFSR2CreateReactiveMaskCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSR2CreateReactiveMaskCS::FParameters>();
				PassParameters->Sampler = TStaticSamplerState<SF_Point>::GetRHI();
				FRDGTextureRef SeparateTranslucency;
				FSeparateTranslucencyTexturesAccessor const* Accessor = reinterpret_cast<FSeparateTranslucencyTexturesAccessor const*>(PostInputs.SeparateTranslucencyTextures);
				if (Accessor && Accessor->ColorTexture.IsValid())
				{
					SeparateTranslucency = Accessor->ColorTexture.Resolve;
				}
				else
				{
					SeparateTranslucency = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackAlphaOneDummy);
				}

				FRDGTextureRef GBufferB = (*PostInputs.SceneTextures)->GBufferBTexture;
				if (!GBufferB)
				{
					GBufferB = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
				}

				FRDGTextureRef GBufferD = (*PostInputs.SceneTextures)->GBufferDTexture;
				if (!GBufferD)
				{
					GBufferD = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
				}

				FRDGTextureRef Reflections = ReflectionTexture;
				if (!Reflections)
				{
					Reflections = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
				}

				PassParameters->DepthTexture = SceneDepth;
				PassParameters->InputDepth = GraphBuilder.CreateSRV(DepthDesc);

				FRDGTextureSRVDesc SceneColorSRV = FRDGTextureSRVDesc::Create(SceneColor);
				PassParameters->SceneColor = GraphBuilder.CreateSRV(SceneColorSRV);

				//------------------------------------------------------------------------------------------------------
				// Capturing the scene color pre-alpha requires allocating the texture here, but keeping a reference to it.
				// The texture will be filled in later in the CopyOpaqueSceneColor function.
				//------------------------------------------------------------------------------------------------------
				EPixelFormat SceneColorFormat = SceneColorDesc.Format;
				FIntPoint SceneColorSize = FIntPoint::ZeroValue;
				for (auto const& ViewIt : View.Family->Views)
				{
					check(ViewIt->bIsViewInfo);
					SceneColorSize.X = FMath::Max(SceneColorSize.X, ((FViewInfo*)ViewIt)->ViewRect.Max.X);
					SceneColorSize.Y = FMath::Max(SceneColorSize.Y, ((FViewInfo*)ViewIt)->ViewRect.Max.Y);
				}
				check(SceneColorSize.X > 0 && SceneColorSize.Y > 0);

				FIntPoint QuantizedSize;
				QuantizeSceneBufferSize(SceneColorSize, QuantizedSize);

				if (SceneColorPreAlpha.GetReference())
				{
					if (SceneColorPreAlpha->GetSizeX() != QuantizedSize.X
						|| SceneColorPreAlpha->GetSizeY() != QuantizedSize.Y
						|| SceneColorPreAlpha->GetFormat() != SceneColorFormat
						|| SceneColorPreAlpha->GetNumMips() != SceneColorDesc.NumMips
						|| SceneColorPreAlpha->GetNumSamples() != SceneColorDesc.NumSamples)
					{
						SceneColorPreAlpha.SafeRelease();
						SceneColorPreAlphaRT.SafeRelease();
					}
				}

				if (SceneColorPreAlpha.GetReference() == nullptr)
				{
					FRHIResourceCreateInfo Info(TEXT("FSR2SceneColorPreAlpha"));
					SceneColorPreAlpha = RHICreateTexture2D(QuantizedSize.X, QuantizedSize.Y, SceneColorFormat, SceneColorDesc.NumMips, SceneColorDesc.NumSamples, SceneColorDesc.Flags, Info);
					SceneColorPreAlphaRT = CreateRenderTarget(SceneColorPreAlpha.GetReference(), TEXT("FSR2SceneColorPreAlpha"));
				}

				if (SceneColorPreAlphaRT)
				{
					FRDGTextureRef SceneColorPreAlphaRDG = GraphBuilder.RegisterExternalTexture(SceneColorPreAlphaRT);
					FRDGTextureSRVDesc SceneColorPreAlphaSRV = FRDGTextureSRVDesc::Create(SceneColorPreAlphaRDG);
					PassParameters->SceneColorPreAlpha = GraphBuilder.CreateSRV(SceneColorPreAlphaSRV);
				}
				else
				{
					PassParameters->SceneColorPreAlpha = GraphBuilder.CreateSRV(SceneColorSRV);
				}

				FRDGTextureSRVDesc GBufferBDesc = FRDGTextureSRVDesc::Create(GBufferB);
				FRDGTextureSRVDesc GBufferDDesc = FRDGTextureSRVDesc::Create(GBufferD);
				FRDGTextureSRVDesc ReflectionsDesc = FRDGTextureSRVDesc::Create(Reflections);
				FRDGTextureSRVDesc InputDesc = FRDGTextureSRVDesc::Create(SeparateTranslucency);
				FRDGTextureUAVDesc ReactiveDesc(ReactiveMaskTexture);
				FRDGTextureUAVDesc CompositeDesc(CompositeMaskTexture);

				PassParameters->InputSeparateTranslucency = GraphBuilder.CreateSRV(InputDesc);
				PassParameters->GBufferB = GraphBuilder.CreateSRV(GBufferBDesc);
				PassParameters->GBufferD = GraphBuilder.CreateSRV(GBufferDDesc);
				PassParameters->ReflectionTexture = GraphBuilder.CreateSRV(ReflectionsDesc);

				PassParameters->View = View.ViewUniformBuffer;

				PassParameters->ReactiveMask = GraphBuilder.CreateUAV(ReactiveDesc);
				PassParameters->CompositeMask = GraphBuilder.CreateUAV(CompositeDesc);

				PassParameters->FurthestReflectionCaptureDistance = CVarFSR2ReactiveMaskRoughnessForceMaxDistance.GetValueOnRenderThread() ? CVarFSR2ReactiveMaskRoughnessMaxDistance.GetValueOnRenderThread() : FMath::Max(CVarFSR2ReactiveMaskRoughnessMaxDistance.GetValueOnRenderThread(), View.FurthestReflectionCaptureDistance);
				PassParameters->ReactiveMaskReflectionScale = CVarFSR2ReactiveMaskReflectionScale.GetValueOnRenderThread();
				PassParameters->ReactiveMaskRoughnessScale = CVarFSR2ReactiveMaskRoughnessScale.GetValueOnRenderThread();
				PassParameters->ReactiveMaskRoughnessBias = CVarFSR2ReactiveMaskRoughnessBias.GetValueOnRenderThread();
				PassParameters->ReactiveMaskReflectionLumaBias = CVarFSR2ReactiveMaskReflectionLumaBias.GetValueOnRenderThread();
				PassParameters->ReactiveMaskTranslucencyBias = CVarFSR2ReactiveMaskTranslucencyBias.GetValueOnRenderThread();
				PassParameters->ReactiveMaskTranslucencyLumaBias = CVarFSR2ReactiveMaskTranslucencyLumaBias.GetValueOnRenderThread();
				PassParameters->ReactiveMaskPreDOFTranslucencyScale = CVarFSR2ReactiveMaskPreDOFTranslucencyScale.GetValueOnRenderThread();
				PassParameters->ReactiveMaskPreDOFTranslucencyMax = CVarFSR2ReactiveMaskPreDOFTranslucencyMax.GetValueOnRenderThread();
				PassParameters->ReactiveMaskTranslucencyMaxDistance = CVarFSR2ReactiveMaskTranslucencyMaxDistance.GetValueOnRenderThread();
				PassParameters->ForceLitReactiveValue = CVarFSR2ReactiveMaskForceReactiveMaterialValue.GetValueOnRenderThread();
				PassParameters->ReactiveShadingModelID = (uint32)CVarFSR2ReactiveMaskReactiveShadingModelID.GetValueOnRenderThread();

				TShaderMapRef<FFSR2CreateReactiveMaskCS> ComputeShaderFSR(View.ShaderMap);
				FComputeShaderUtils::AddPass(
					GraphBuilder,
					RDG_EVENT_NAME("FidelityFX-FSR2/CreateReactiveMask (CS)"),
					ComputeShaderFSR,
					PassParameters,
					FComputeShaderUtils::GetGroupCount(FIntVector(InputExtents.X, InputExtents.Y, 1),
						FIntVector(FFSR2ConvertVelocityCS::ThreadgroupSizeX, FFSR2ConvertVelocityCS::ThreadgroupSizeY, FFSR2ConvertVelocityCS::ThreadgroupSizeZ))
				);
			}
		}
		else
		{
			ReactiveMaskTexture = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
			CompositeMaskTexture = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
		}

		// If we are set to de-dither rendering then run the extra pass now - this tries to identify dither patterns and blend them to avoid over-thinning in FSR2.
		// There is specific code for SHADINGMODELID_HAIR pixels which are always dithered.
		if (CVarFSR2DeDitherMode.GetValueOnRenderThread() && (*PostInputs.SceneTextures)->GBufferBTexture)
		{
			SceneColor = GraphBuilder.CreateTexture(SceneColorDesc, TEXT("FSR2SubrectColor"));
			FFSR2DeDitherCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSR2DeDitherCS::FParameters>();
			FRDGTextureUAVDesc OutputDesc(SceneColor);

			FRDGTextureRef GBufferB = (*PostInputs.SceneTextures)->GBufferBTexture;
			FRDGTextureSRVDesc GBufferBDesc = FRDGTextureSRVDesc::Create(GBufferB);
			PassParameters->GBufferB = GraphBuilder.CreateSRV(GBufferBDesc);

			FRDGTextureSRVDesc SceneColorSRV = FRDGTextureSRVDesc::Create(PassInputs.SceneColorTexture);
			PassParameters->SceneColor = GraphBuilder.CreateSRV(SceneColorSRV);

			PassParameters->View = View.ViewUniformBuffer;

			PassParameters->BlendSceneColor = GraphBuilder.CreateUAV(OutputDesc);

			// Full de-dither requires the proper setting or not running on the Deferred renderer where we can't determine the shading model.
			PassParameters->FullDeDither = (CVarFSR2DeDitherMode.GetValueOnRenderThread() == 1) || (!GBufferB);
			if (!GBufferB)
			{
				GBufferB = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
			}

			TShaderMapRef<FFSR2DeDitherCS> ComputeShaderFSR(View.ShaderMap);
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR2/DeDither (CS)"),
				ComputeShaderFSR,
				PassParameters,
				FComputeShaderUtils::GetGroupCount(FIntVector(SceneColor->Desc.Extent.X, SceneColor->Desc.Extent.Y, 1),
					FIntVector(FFSR2DeDitherCS::ThreadgroupSizeX, FFSR2DeDitherCS::ThreadgroupSizeY, FFSR2DeDitherCS::ThreadgroupSizeZ))
			);
		}

		//------------------------------------------------------------------------------------------------------
		// Consolidate Motion Vectors
		//   UE4 motion vectors are in sparse format by default.  Convert them to a format consumable by FSR2.
		//------------------------------------------------------------------------------------------------------
		FRDGTextureDesc MotionVectorDesc = FRDGTextureDesc::Create2D(InputExtentsQuantized, PF_G16R16F, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable);
		FRDGTextureRef MotionVectorTexture = GraphBuilder.CreateTexture(MotionVectorDesc, TEXT("FSR2MotionVectorTexture"));
		{
			FFSR2ConvertVelocityCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSR2ConvertVelocityCS::FParameters>();
			FRDGTextureUAVDesc OutputDesc(MotionVectorTexture);

			PassParameters->DepthTexture = SceneDepth;
			PassParameters->InputDepth = GraphBuilder.CreateSRV(DepthDesc);
			PassParameters->InputVelocity = GraphBuilder.CreateSRV(VelocityDesc);

			PassParameters->View = View.ViewUniformBuffer;

			PassParameters->OutputTexture = GraphBuilder.CreateUAV(OutputDesc);

			TShaderMapRef<FFSR2ConvertVelocityCS> ComputeShaderFSR(View.ShaderMap);
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR2/ConvertVelocity (CS)"),
				ComputeShaderFSR,
				PassParameters,
				FComputeShaderUtils::GetGroupCount(FIntVector(SceneDepth->Desc.Extent.X, SceneDepth->Desc.Extent.Y, 1),
					FIntVector(FFSR2ConvertVelocityCS::ThreadgroupSizeX, FFSR2ConvertVelocityCS::ThreadgroupSizeY, FFSR2ConvertVelocityCS::ThreadgroupSizeZ))
			);
		}

		//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Handle Multiple Viewports
		//   The FSR2 API currently doesn't handle offsetting into buffers.  If the current viewport is not the top left viewport, generate a new texture in which this viewport is at (0,0).
		//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if (View.ViewRect.Min != FIntPoint::ZeroValue)
		{
			if (!CVarFSR2DeDitherMode.GetValueOnRenderThread())
			{
				SceneColor = GraphBuilder.CreateTexture(SceneColorDesc, TEXT("FSR2SubrectColor"));

				AddCopyTexturePass(
					GraphBuilder,
					PassInputs.SceneColorTexture,
					SceneColor,
					View.ViewRect.Min,
					FIntPoint::ZeroValue,
					View.ViewRect.Size());
			}

			FRDGTextureDesc SplitDepthDesc = FRDGTextureDesc::Create2D(InputExtentsQuantized, SceneDepth->Desc.Format, FClearValueBinding::Black, SceneDepth->Desc.Flags);
			SceneDepth = GraphBuilder.CreateTexture(SplitDepthDesc, TEXT("FSR2SubrectDepth"));

			AddCopyTexturePass(
					GraphBuilder,
					PassInputs.SceneDepthTexture,
					SceneDepth,
					View.ViewRect.Min,
					FIntPoint::ZeroValue,
					View.ViewRect.Size());
		}

		//-------------------
		// Create Resources
		//-------------------
		// Whether alpha channel is supported.
		static const auto CVarPostPropagateAlpha = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PostProcessing.PropagateAlpha"));
		const bool bSupportsAlpha = (CVarPostPropagateAlpha && CVarPostPropagateAlpha->GetValueOnRenderThread() != 0);
		EPixelFormat OutputFormat = (bSupportsAlpha || (CVarFSR2HistoryFormat.GetValueOnRenderThread() == 0)) ? PF_FloatRGBA : PF_FloatR11G11B10;

		FRDGTextureDesc OutputColorDesc = FRDGTextureDesc::Create2D(OutputExtents, OutputFormat, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable);
		FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(OutputColorDesc, TEXT("FSR2OutputTexture"));

		*OutSceneColorTexture = OutputTexture;
		*OutSceneColorViewRect = FIntRect(FIntPoint::ZeroValue, View.GetSecondaryViewRectSize());

		*OutSceneColorHalfResTexture = nullptr;
		*OutSceneColorHalfResViewRect = FIntRect::DivideAndRoundUp(*OutSceneColorViewRect, 2);

		FRDGBufferDesc DummyDesc = FRDGBufferDesc::CreateStructuredDesc((uint32)sizeof(float), 1u);
		FRDGBufferRef DummyBuf = GraphBuilder.CreateBuffer(DummyDesc, TEXT("FSR2BarrierFlushDummy"), ERDGBufferFlags::None);

		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Initialize the FSR2 Context
		//   If a context has never been created, or if significant features of the frame have changed since the current context was created, tear down any existing contexts and create a new one matching the current frame.
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FSR2StateRef FSR2State;
		const TRefCountPtr<ICustomTemporalAAHistory> PrevCustomHistory = View.PrevViewInfo.CustomTemporalAAHistory;
		FFSR2TemporalUpscalerHistory* CustomHistory = static_cast<FFSR2TemporalUpscalerHistory*>(PrevCustomHistory.GetReference());
		{
			// FSR setup
			FfxFsr2ContextDescription Params;
			FMemory::Memzero(Params);

			//------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Describe the Current Frame
			//   Collect the features of the current frame and the current FSR2 history, so we can make decisions about whether any existing FSR2 context is currently usable.
			//------------------------------------------------------------------------------------------------------------------------------------------------------------------
			switch (Api)
			{
#if FSR2_ENABLE_DX12
				case EFSR2TemporalUpscalerAPI::D3D12:
				{
					ID3D12Device* device = (ID3D12Device*)GDynamicRHI->RHIGetNativeDevice();
					Params.device = ffxGetDeviceDX12(device);
					break;
				}
#endif
#if FSR2_ENABLE_VK
				case EFSR2TemporalUpscalerAPI::Vulkan:
				{
					Params.device = ffxGetDeviceVK(static_cast<VkDevice>(GDynamicRHI->RHIGetNativeDevice()));
					break;
				}
#endif
				default:
				{
					Params.device = (FfxDevice)this;
					break;
				}
			}

			// FSR settings
			{
				// Engine params:
				Params.flags = 0;
				Params.flags |= bool(ERHIZBuffer::IsInverted) ? FFX_FSR2_ENABLE_DEPTH_INVERTED  : 0;
				Params.flags |= FFX_FSR2_ENABLE_HIGH_DYNAMIC_RANGE;
				Params.flags |= ((DynamicResolutionStateInfos.Status == EDynamicResolutionStatus::Enabled) || (DynamicResolutionStateInfos.Status == EDynamicResolutionStatus::DebugForceEnabled)) ? FFX_FSR2_ENABLE_DYNAMIC_RESOLUTION : 0;
				Params.displaySize.height = OutputExtents.Y;
				Params.displaySize.width = OutputExtents.X;
				Params.maxRenderSize.height = InputExtents.Y;
				Params.maxRenderSize.width = InputExtents.X;

				// CVar params:
				// Compute Auto Exposure requires wave operations or D3D12.
				Params.flags |= CVarFSR2AutoExposure.GetValueOnRenderThread() ? FFX_FSR2_ENABLE_AUTO_EXPOSURE : 0;
			}

			// We want to reuse FSR2 states rather than recreating them wherever possible as they allocate significant memory for their internal resources.
			// The current custom history is the ideal, but the recently released states can be reused with a simple reset too when the engine cuts the history.
			// This reduces the memory churn imposed by camera cuts.
			bool HasValidContext = CustomHistory && CustomHistory->GetState().IsValid();
			if (HasValidContext)
			{
				FfxFsr2ContextDescription const& CurrentParams = CustomHistory->GetState()->Params;
				if ((CustomHistory->GetState()->LastUsedFrame == GFrameCounterRenderThread) || (CurrentParams.maxRenderSize.width < Params.maxRenderSize.width) || (CurrentParams.maxRenderSize.height < Params.maxRenderSize.height) || (CurrentParams.displaySize.width != Params.displaySize.width) || (CurrentParams.displaySize.height != Params.displaySize.height) || (Params.flags != CurrentParams.flags) || (Params.device != CurrentParams.device))
				{
					HasValidContext = false;
				}
				else
				{
					FSR2State = CustomHistory->GetState();
				}
			}
			
			if (!HasValidContext)
			{
				TLockFreePointerListFIFO<FFSR2State, PLATFORM_CACHE_LINE_SIZE> ReusableStates;
				FFSR2State* Ptr = AvailableStates.Pop();
				while (Ptr)
				{
					FfxFsr2ContextDescription const& CurrentParams = Ptr->Params;
					if (Ptr->LastUsedFrame == GFrameCounterRenderThread)
					{
						// These states can't be reused immediately but perhaps a future frame.
						ReusableStates.Push(Ptr);
					}
					else if ((CurrentParams.maxRenderSize.width < Params.maxRenderSize.width) || (CurrentParams.maxRenderSize.height < Params.maxRenderSize.height) || (CurrentParams.displaySize.width != Params.displaySize.width) || (CurrentParams.displaySize.height != Params.displaySize.height) || (Params.flags != CurrentParams.flags) || (Params.device != CurrentParams.device))
					{
						// States that can't be trivially reused need to just be released to save memory.
						Ptr->Release();
					}
					else
					{
						FSR2State = Ptr;
						Ptr->Release();
						HasValidContext = true;
						bHistoryValid = false;
						break;
					}
					Ptr = AvailableStates.Pop();
				}

				Ptr = ReusableStates.Pop();
				while (Ptr)
				{
					AvailableStates.Push(Ptr);
					Ptr = ReusableStates.Pop();
				}
			}

			if (!HasValidContext)
			{
				// For a new context, allocate the necessary scratch memory for the chosen backend
				uint32 ScratchSize = 0;
				switch (Api)
				{
#if FSR2_ENABLE_DX12
					case EFSR2TemporalUpscalerAPI::D3D12:
					{
						ScratchSize = ffxFsr2GetScratchMemorySizeDX12();
						break;
					}
#endif
#if FSR2_ENABLE_VK
					case EFSR2TemporalUpscalerAPI::Vulkan:
					{
						FVulkanDevice* VulkanDevice = VulkanRHIBridge::GetDevice((FVulkanDynamicRHI*)GDynamicRHI);
						ScratchSize = ffxFsr2GetScratchMemorySizeVK(reinterpret_cast<VkPhysicalDevice>((uintptr_t)VulkanRHIBridge::GetPhysicalDevice(VulkanDevice)));
						break;
					}
#endif
					default:
					{
						ScratchSize = ffxFsr2GetScratchMemorySize();
						break;
					}
				}

				FSR2State = new FFSR2State(ScratchSize);
			}

			FSR2State->LastUsedFrame = GFrameCounterRenderThread;

			//-------------------------------------------------------------------------------------------------------------------------------------------------
			// Update History Data (Part 1)
			//   Prepare the view to receive this frame's history data.  This must be done before any attempt to re-create an FSR2 context, if that's needed.
			//-------------------------------------------------------------------------------------------------------------------------------------------------
			if (CanWritePrevViewInfo)
			{
				// Releases the existing history texture inside the wrapper object, this doesn't release NewHistory itself
				View.ViewState->PrevFrameViewInfo.TemporalAAHistory.SafeRelease();

				View.ViewState->PrevFrameViewInfo.TemporalAAHistory.ViewportRect = FIntRect(0, 0, OutputExtents.X, OutputExtents.Y);
				View.ViewState->PrevFrameViewInfo.TemporalAAHistory.ReferenceBufferSize = OutputExtents;

				if (!View.ViewState->PrevFrameViewInfo.CustomTemporalAAHistory.GetReference())
				{
					View.ViewState->PrevFrameViewInfo.CustomTemporalAAHistory = new FFSR2TemporalUpscalerHistory(FSR2State, const_cast<FFSR2TemporalUpscaler*>(this));
				}
			}

			//-----------------------------------------------------------------------------------------------------------------------------------------
			// Invalidate FSR2 Contexts
			//   If a context already exists but it is not valid for the current frame's features, clean it up in preparation for creating a new one.
			//-----------------------------------------------------------------------------------------------------------------------------------------
			if (HasValidContext)
			{
				FfxFsr2ContextDescription const& CurrentParams = FSR2State->Params;

				// Display size must match for splitscreen to work.
				if ((CurrentParams.maxRenderSize.width < Params.maxRenderSize.width) || (CurrentParams.maxRenderSize.height < Params.maxRenderSize.height) || (CurrentParams.displaySize.width != Params.displaySize.width) || (CurrentParams.displaySize.height != Params.displaySize.height) || (Params.flags != CurrentParams.flags) || (Params.device != CurrentParams.device))
				{
					ffxFsr2ContextDestroy(&FSR2State->Fsr2);
					HasValidContext = false;
					bHistoryValid = false;
				}
			}

			//------------------------------------------------------
			// Create FSR2 Contexts
			//   If no valid context currently exists, create one.
			//------------------------------------------------------
			if (!HasValidContext)
			{
				switch (Api)
				{
#if FSR2_ENABLE_DX12
					case EFSR2TemporalUpscalerAPI::D3D12:
					{
						ffxFsr2GetInterfaceDX12(&Params.callbacks, reinterpret_cast<ID3D12Device*>(Params.device), FSR2State->ScratchBuffer, FSR2State->ScratchSize);
						break;
					}
#endif
#if FSR2_ENABLE_VK
					case EFSR2TemporalUpscalerAPI::Vulkan:
					{
						FVulkanDevice* VulkanDevice = VulkanRHIBridge::GetDevice((FVulkanDynamicRHI*)GDynamicRHI);
						ffxFsr2UEGetInterfaceVK(&Params.callbacks, FSR2State->ScratchBuffer, FSR2State->ScratchSize, reinterpret_cast<VkPhysicalDevice>((uintptr_t)VulkanRHIBridge::GetPhysicalDevice(VulkanDevice)));
						break;
					}
#endif
					default:
					{
						ffxFsr2GetInterfaceUE(&Params.callbacks, this, FSR2State->ScratchBuffer, FSR2State->ScratchSize);
						break;
					}
				}
				FfxErrorCode ErrorCode = ffxFsr2ContextCreate(&FSR2State->Fsr2, &Params);
				check(ErrorCode == FFX_OK);
				if (ErrorCode == FFX_OK)
				{
					FMemory::Memcpy(FSR2State->Params, Params);
				}
			}
		}

		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Organize Inputs (Part 1)
		//   Some inputs FSR2 requires are available now, but will no longer be directly available once we get inside the RenderGraph.  Go ahead and collect the ones we can.
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FfxFsr2DispatchDescription* Fsr2DispatchParamsPtr = new FfxFsr2DispatchDescription;
		FfxFsr2DispatchDescription& Fsr2DispatchParams = *Fsr2DispatchParamsPtr;
		FMemory::Memzero(Fsr2DispatchParams);
		{
			// Whether to abandon the history in the state on camera cuts
			Fsr2DispatchParams.reset = !bHistoryValid;

			// CVar parameters:
			Fsr2DispatchParams.enableSharpening = (CVarFSR2Sharpness.GetValueOnRenderThread() != 0.0f);
			Fsr2DispatchParams.sharpness = FMath::Clamp(CVarFSR2Sharpness.GetValueOnRenderThread(), 0.0f, 1.0f);

			// Engine parameters:
			Fsr2DispatchParams.frameTimeDelta = View.Family->DeltaWorldTime;
			Fsr2DispatchParams.jitterOffset.x = View.TemporalJitterPixels.X;
			Fsr2DispatchParams.jitterOffset.y = View.TemporalJitterPixels.Y;
			Fsr2DispatchParams.preExposure = View.PreExposure;

			Fsr2DispatchParams.renderSize.width = InputExtents.X;
			Fsr2DispatchParams.renderSize.height = InputExtents.Y;

			// @todo parameters for motion vectors - these should be a scale but FSR2 seems to treat them as resolution?
			Fsr2DispatchParams.motionVectorScale.x = InputExtents.X;
			Fsr2DispatchParams.motionVectorScale.y = InputExtents.Y;

			// Untested parameters:
			Fsr2DispatchParams.cameraFovAngleVertical = View.ViewMatrices.ComputeHalfFieldOfViewPerAxis().Y * 2.0f;

			// Unused parameters:
			Fsr2DispatchParams.cameraNear = 0;
			Fsr2DispatchParams.cameraFar = 0;
		}

		//------------------------------
		// Add FSR2 to the RenderGraph
		//------------------------------
		FFSR2Pass::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSR2Pass::FParameters>();
		PassParameters->DummyBuffer = DummyBuf;
		PassParameters->ColorTexture = SceneColor;
		PassParameters->DepthTexture = SceneDepth;
		PassParameters->VelocityTexture = MotionVectorTexture;
		if (bValidEyeAdaptation)
		{
			PassParameters->ExposureTexture = GetEyeAdaptationTexture(GraphBuilder, View);
		}
		PassParameters->ReactiveMaskTexture = ReactiveMaskTexture;
		PassParameters->CompositeMaskTexture = CompositeMaskTexture;
		PassParameters->OutputTexture = OutputTexture;

		auto* ApiAccess = ApiAccessor;
		auto CurrentApi = Api;
		if (CurrentApi == EFSR2TemporalUpscalerAPI::Unreal)
		{
			Fsr2DispatchParams.color = ffxGetResourceFromUEResource(&FSR2State->Params.callbacks, PassParameters->ColorTexture.GetTexture());
			Fsr2DispatchParams.depth = ffxGetResourceFromUEResource(&FSR2State->Params.callbacks, PassParameters->DepthTexture.GetTexture());
			Fsr2DispatchParams.motionVectors = ffxGetResourceFromUEResource(&FSR2State->Params.callbacks, PassParameters->VelocityTexture.GetTexture());
			Fsr2DispatchParams.exposure = ffxGetResourceFromUEResource(&FSR2State->Params.callbacks, PassParameters->ExposureTexture.GetTexture());
			if (PassParameters->ReactiveMaskTexture)
			{
				Fsr2DispatchParams.reactive = ffxGetResourceFromUEResource(&FSR2State->Params.callbacks, PassParameters->ReactiveMaskTexture.GetTexture());
			}
			Fsr2DispatchParams.output = ffxGetResourceFromUEResource(&FSR2State->Params.callbacks, PassParameters->OutputTexture.GetTexture(), FFX_RESOURCE_STATE_UNORDERED_ACCESS);
			Fsr2DispatchParams.commandList = (FfxCommandList)CurrentGraphBuilder;

			ffxFsr2SetFeatureLevel(&FSR2State->Params.callbacks, View.GetFeatureLevel());
			FfxErrorCode Code = ffxFsr2ContextDispatch(&FSR2State->Fsr2, &Fsr2DispatchParams);
			check(Code == FFX_OK);
			delete Fsr2DispatchParamsPtr;
		}
		else
		{
			GraphBuilder.AddPass(RDG_EVENT_NAME("FidelityFX-FSR2"), PassParameters, ERDGPassFlags::Compute | ERDGPassFlags::Raster | ERDGPassFlags::SkipRenderPass, [&View, &PassInputs, CurrentApi, ApiAccess, PassParameters, PrevCustomHistory, Fsr2DispatchParamsPtr, FSR2State](FRHICommandListImmediate& RHICmdList)
				{
					//----------------------------------------------------------
					// Organize Inputs (Part 2)
					//   The remaining inputs FSR2 requires are available now.
					//----------------------------------------------------------
					FfxFsr2DispatchDescription DispatchParams;
					FMemory::Memcpy(DispatchParams, *Fsr2DispatchParamsPtr);
					delete Fsr2DispatchParamsPtr;

					switch (CurrentApi)
					{
#if FSR2_ENABLE_DX12
						case EFSR2TemporalUpscalerAPI::D3D12:
						{
							DispatchParams.color = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->ColorTexture->GetRHI()->GetNativeResource());
							DispatchParams.depth = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->DepthTexture->GetRHI()->GetNativeResource());
							DispatchParams.motionVectors = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->VelocityTexture->GetRHI()->GetNativeResource());
							if (PassParameters->ExposureTexture)
							{
								DispatchParams.exposure = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->ExposureTexture->GetRHI()->GetNativeResource());
							}
							DispatchParams.output = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->OutputTexture->GetRHI()->GetNativeResource(), nullptr, FFX_RESOURCE_STATE_UNORDERED_ACCESS);
							if (PassParameters->ReactiveMaskTexture)
							{
								DispatchParams.reactive = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->ReactiveMaskTexture->GetRHI()->GetNativeResource());
							}
							if (PassParameters->CompositeMaskTexture)
							{
								DispatchParams.transparencyAndComposition = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->CompositeMaskTexture->GetRHI()->GetNativeResource());
							}
							break;
						}
#endif
#if FSR2_ENABLE_VK
						case EFSR2TemporalUpscalerAPI::Vulkan:
						{
							DispatchParams.color = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->ColorTexture->GetRHI());
							DispatchParams.depth = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->DepthTexture->GetRHI());
							DispatchParams.motionVectors = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->VelocityTexture->GetRHI());
							if (PassParameters->ExposureTexture)
							{
								DispatchParams.exposure = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->ExposureTexture->GetRHI());
							}
							DispatchParams.output = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->OutputTexture->GetRHI(), FFX_RESOURCE_STATE_UNORDERED_ACCESS);
							if (PassParameters->ReactiveMaskTexture)
							{
								DispatchParams.reactive = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->ReactiveMaskTexture->GetRHI());
							}
							if (PassParameters->CompositeMaskTexture)
							{
								DispatchParams.transparencyAndComposition = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->CompositeMaskTexture->GetRHI());
							}
							break;
						}
#endif
						default:
						{
							check(false);
							break;
						}
					}

					PassParameters->ColorTexture->MarkResourceAsUsed();
					PassParameters->DepthTexture->MarkResourceAsUsed();
					PassParameters->VelocityTexture->MarkResourceAsUsed();
					if (PassParameters->ExposureTexture)
					{
						PassParameters->ExposureTexture->MarkResourceAsUsed();
					}
					PassParameters->OutputTexture->MarkResourceAsUsed();
					PassParameters->DummyBuffer->MarkResourceAsUsed();
					if (PassParameters->ReactiveMaskTexture)
					{
						PassParameters->ReactiveMaskTexture->MarkResourceAsUsed();
					}
					if (PassParameters->CompositeMaskTexture)
					{
						PassParameters->CompositeMaskTexture->MarkResourceAsUsed();
					}

					//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
					// Push barriers
					//   Some resources are in the wrong state for FSR2 to execute.  Transition them.  On some platforms, this may involve a bit of tricking the RHI into doing what we want...
					//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
					if (ApiAccess)
					{
						ApiAccess->ForceUAVTransition(RHICmdList, PassParameters->OutputTexture->GetRHI(), PassParameters->DummyBuffer->GetRHIStructuredBuffer());
					}

					{
						SCOPED_DRAW_EVENT(RHICmdList, FidelityFXFSR2Dispatch);
						SCOPED_GPU_STAT(RHICmdList, FidelityFXFSR2Dispatch);

						//-------------------------------------------------------------------------------------
						// Dispatch FSR2
						//   Push the FSR2 algorithm directly onto the underlying graphics APIs command list.
						//-------------------------------------------------------------------------------------
						switch (CurrentApi)
						{
#if FSR2_ENABLE_DX12
							case EFSR2TemporalUpscalerAPI::D3D12:
							{
								RHICmdList.EnqueueLambda([FSR2State, DispatchParams, ApiAccess](FRHICommandListImmediate& cmd) mutable
									{
										ID3D12GraphicsCommandList* cmdList = (ID3D12GraphicsCommandList*)ApiAccess->GetNativeCommandBuffer(cmd);
										DispatchParams.commandList = ffxGetCommandListDX12(cmdList);

										FfxErrorCode Code = ffxFsr2ContextDispatch(&FSR2State->Fsr2, &DispatchParams);
										check(Code == FFX_OK);
									});
								break;
							}
#endif
#if FSR2_ENABLE_VK
							case EFSR2TemporalUpscalerAPI::Vulkan:
							{
								RHICmdList.EnqueueLambda([FSR2State, DispatchParams, ApiAccess](FRHICommandListImmediate& cmd) mutable
									{
										VkCommandBuffer cmdList = (VkCommandBuffer)ApiAccess->GetNativeCommandBuffer(cmd);
										DispatchParams.commandList = ffxGetCommandListVK(cmdList);

										FfxErrorCode Code = ffxFsr2ContextDispatch(&FSR2State->Fsr2, &DispatchParams);
										check(Code == FFX_OK);
									});
								break;
							}
#endif
							default:
							{
								check(false);
								break;
							}
						}
					}

					//-----------------------------------------------------------------------------------------------------------------------------------------------
					// Flush instructions to the GPU
					//   The FSR2 Dispatch has tampered with the state of the current command list.  Flush it all the way to the GPU so that Unreal can start anew.
					//-----------------------------------------------------------------------------------------------------------------------------------------------
					RHICmdList.SubmitCommandsHint();
				});
		}

		//----------------------------------------------------------------------------------------------------------------------------------
		// Update History Data (Part 2)
		//   Extract the output produced by the FSR2 Dispatch into the history reference we prepared to receive that output during Part 1.
		//----------------------------------------------------------------------------------------------------------------------------------
		if (CanWritePrevViewInfo)
		{
			// Copy the new history into the history wrapper
			GraphBuilder.QueueTextureExtraction(OutputTexture, &View.ViewState->PrevFrameViewInfo.TemporalAAHistory.RT[0]);
		}

		DeferredCleanup();
	}
	else
	{
		GetDefaultTemporalUpscaler()->AddPasses(
			GraphBuilder,
			View,
			PassInputs,
			OutSceneColorTexture,
			OutSceneColorViewRect,
			OutSceneColorHalfResTexture,
			OutSceneColorHalfResViewRect);
	}
}

void FFSR2TemporalUpscaler::SetupMainGameViewFamily(FSceneViewFamily& InViewFamily)
{
	static const auto CVarFSR2Enabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.Enabled"));

	Initialize();

	GEngine->GetDynamicResolutionCurrentStateInfos(/* out */ DynamicResolutionStateInfos);

	if (IsApiSupported() && InViewFamily.EngineShowFlags.ScreenPercentage && !InViewFamily.GetScreenPercentageInterface() && (CVarFSR2Enabled && CVarFSR2Enabled->GetValueOnGameThread()))
	{
		FfxFsr2QualityMode QualityMode = FMath::Clamp<FfxFsr2QualityMode>((FfxFsr2QualityMode)CVarFSR2QualityMode.GetValueOnAnyThread(), HighestResolutionQualityMode, LowestResolutionQualityMode);
		const float ResolutionFraction = FSR2_GetScreenResolutionFromScalingMode(QualityMode);
		InViewFamily.SetScreenPercentageInterface(new FLegacyScreenPercentageDriver(InViewFamily, ResolutionFraction, false));
	}
}

#if FSR2_ENGINE_SUPPORTS_SCREENPERCENTAGEDATA
void FFSR2TemporalUpscaler::SetupViewFamily(FSceneViewFamily& ViewFamily, TSharedPtr<ICustomStaticScreenPercentageData> ScreenPercentageDataInterface)
{
	check(ScreenPercentageDataInterface.IsValid());
	TSharedPtr<FFSR2CustomStaticScreenPercentageData> ScreenPercentageData = StaticCastSharedPtr<FFSR2CustomStaticScreenPercentageData>(ScreenPercentageDataInterface);

	static const auto CVarFSR2Enabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.Enabled"));

	Initialize();

	GEngine->GetDynamicResolutionCurrentStateInfos(/* out */ DynamicResolutionStateInfos);

	if (IsApiSupported() && ViewFamily.EngineShowFlags.ScreenPercentage && !ViewFamily.GetScreenPercentageInterface() && (CVarFSR2Enabled && CVarFSR2Enabled->GetValueOnGameThread()))
	{
		if (ViewFamily.GetTemporalUpscalerInterface() == nullptr)
		{
			ViewFamily.SetTemporalUpscalerInterface(this);
		}

		FfxFsr2QualityMode QualityMode = FMath::Clamp<FfxFsr2QualityMode>((FfxFsr2QualityMode)ScreenPercentageData->QualityMode, HighestResolutionQualityMode, LowestResolutionQualityMode);
		const float ResolutionFraction = FSR2_GetScreenResolutionFromScalingMode(QualityMode);
		ViewFamily.SetScreenPercentageInterface(new FLegacyScreenPercentageDriver(ViewFamily, ResolutionFraction, false));
	}
}
#endif

float FFSR2TemporalUpscaler::GetMinUpsampleResolutionFraction() const
{
	if (IsApiSupported())
	{
		return FSR2_GetScreenResolutionFromScalingMode(LowestResolutionQualityMode);
	}
	else
	{
		return GetDefaultTemporalUpscaler()->GetMinUpsampleResolutionFraction();
	}
}

float FFSR2TemporalUpscaler::GetMaxUpsampleResolutionFraction() const
{
	if (IsApiSupported())
	{
		return FSR2_GetScreenResolutionFromScalingMode(HighestResolutionQualityMode);
	}
	else
	{
		return GetDefaultTemporalUpscaler()->GetMaxUpsampleResolutionFraction();
	}
}

//-------------------------------------------------------------------------------------
// The ScreenSpaceReflections shaders are specialized as to whether they expect to be denoised or not.
// When using the denoising plugin API to capture reflection data it is necessary to swap the shaders so that it appears as it would without denoising.
//-------------------------------------------------------------------------------------
void FFSR2TemporalUpscaler::SetSSRShader(FGlobalShaderMap* GlobalMap)
{
	static const auto CVarFSR2Enabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.Enabled"));
	FGlobalShaderMapSection* Section = GlobalMap->FindSection(FHashedName(TEXT("/Engine/Private/SSRT/SSRTReflections.usf")));
	if (Section)
	{
		FShaderMapContent* Content = (FShaderMapContent*)Section->GetContent();
		FFSR2ShaderMapContent* PublicContent = (FFSR2ShaderMapContent*)Content;

		bool bSwapped = (CVarFSR2Enabled && (CVarFSR2Enabled->GetValueOnAnyThread() != 0) && (CVarFSR2UseExperimentalSSRDenoiser.GetValueOnAnyThread() == 0));
		FFSR2ShaderMap** Ptr = SSRShaderMaps.Find(GlobalMap);
		if (!Ptr && (CVarFSR2Enabled && (CVarFSR2Enabled->GetValueOnAnyThread() != 0)))
		{
			FFSR2ShaderMap* NewMap = new FFSR2ShaderMap;
			for (uint32 i = 0; i < (uint32)ESSRQuality::MAX; i++)
			{
				FFSR2ScreenSpaceReflectionsPS::FPermutationDomain PermutationVector;
				PermutationVector.Set<FSSRQualityDim>((ESSRQuality)i);
				PermutationVector.Set<FSSROutputForDenoiser>(false);

				FFSR2ScreenSpaceReflectionsPS::FPermutationDomain OldPermutationVector;
				OldPermutationVector.Set<FSSRQualityDim>((ESSRQuality)i);
				OldPermutationVector.Set<FSSROutputForDenoiser>(true);

				if (Section)
				{
					FShader* NewShader = Content->GetShader(FHashedName(TEXT("FScreenSpaceReflectionsPS")), PermutationVector.ToDimensionValueId());
					FShader* OldShader = Content->GetShader(FHashedName(TEXT("FScreenSpaceReflectionsPS")), OldPermutationVector.ToDimensionValueId());
					NewMap->DefaultShaders[i] = NewShader;
					NewMap->DenoiseShaders[i] = OldShader;
					for (uint32 j = 0; j < (uint32)PublicContent->Shaders.Num(); j++)
					{
						if (PublicContent->Shaders[j].GetChecked() == NewShader)
						{
							NewMap->DefaultIndex[i] = j;
							break;
						}
					}
					for (uint32 j = 0; j < (uint32)PublicContent->Shaders.Num(); j++)
					{
						if (PublicContent->Shaders[j].GetChecked() == OldShader)
						{
							NewMap->DenoiseIndex[i] = j;
							break;
						}
					}
				}
			}
			NewMap->bSwapped = !bSwapped;
			Ptr = &SSRShaderMaps.Add(GlobalMap, NewMap);
		}
		
		if (Ptr)
		{
			FFSR2ShaderMap* Map = *Ptr;
			if (Map->bSwapped != bSwapped)
			{
				Map->bSwapped = bSwapped;
				for (uint32 i = 0; i < (uint32)ESSRQuality::MAX; i++)
				{
					PublicContent->Shaders[Map->DefaultIndex[i]] = Map->bSwapped ? Map->DenoiseShaders[i] : Map->DefaultShaders[i];
					PublicContent->Shaders[Map->DenoiseIndex[i]] = Map->bSwapped ? Map->DefaultShaders[i] : Map->DenoiseShaders[i];
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------
// The FXSystem override lets us copy the scene color after all opaque rendering but before translucency.
// This can be used to compare and pick out translucency data that isn't captured in Separate Translucency.
//-------------------------------------------------------------------------------------
void FFSR2TemporalUpscaler::CopyOpaqueSceneColor(FRHICommandListImmediate& RHICmdList, FRHIUniformBuffer* ViewUniformBuffer, const class FShaderParametersMetadata* SceneTexturesUniformBufferStruct, FRHIUniformBuffer* SceneTexturesUniformBuffer)
{
	static const auto CVarFSR2Enabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.Enabled"));
	FTextureRHIRef SceneColor;
	SceneColor = FSceneRenderTargets::Get(RHICmdList).GetSceneColorTexture();
	if (IsApiSupported() && (CVarFSR2Enabled && CVarFSR2Enabled->GetValueOnRenderThread()) && SceneColorPreAlpha.GetReference() && ViewUniformBuffer && SceneTexturesUniformBuffer && SceneColor.GetReference())
	{
		SCOPED_DRAW_EVENTF(RHICmdList, FSR2TemporalUpscaler_CopyOpaqueSceneColor, TEXT("FSR2TemporalUpscaler CopyOpaqueSceneColor"));

		RHICmdList.Transition(FRHITransitionInfo(SceneColor, ERHIAccess::RTV, ERHIAccess::CopySrc));
		RHICmdList.Transition(FRHITransitionInfo(SceneColorPreAlpha, ERHIAccess::Unknown, ERHIAccess::CopyDest));

		FRHICopyTextureInfo Info;
		Info.Size.X = FMath::Min(SceneColorPreAlpha->GetSizeX(), (uint32)SceneColor->GetSizeXYZ().X);
		Info.Size.Y = FMath::Min(SceneColorPreAlpha->GetSizeY(), (uint32)SceneColor->GetSizeXYZ().Y);
		RHICmdList.CopyTexture(SceneColor, SceneColorPreAlpha, Info);

		RHICmdList.Transition(FRHITransitionInfo(SceneColor, ERHIAccess::CopySrc, ERHIAccess::RTV));
		RHICmdList.Transition(FRHITransitionInfo(SceneColorPreAlpha, ERHIAccess::CopyDest, ERHIAccess::SRVMask));
	}
}

//-------------------------------------------------------------------------------------
// Capture the post-processing inputs structure so that the separate translucency textures are available to the reactive mask.
//-------------------------------------------------------------------------------------
void FFSR2TemporalUpscaler::SetPostProcessingInputs(FPostProcessingInputs const& NewInputs)
{
	PostInputs = NewInputs;
}

//-------------------------------------------------------------------------------------
// As the upscaler retains some resources during the frame they must be released here to avoid leaking or accessing dangling pointers.
//-------------------------------------------------------------------------------------
void FFSR2TemporalUpscaler::EndOfFrame()
{
	PostInputs.SeparateTranslucencyTextures = nullptr;
	PostInputs.SceneTextures = nullptr;
	ReflectionTexture = nullptr;
#if WITH_EDITOR
	bEnabledInEditor = true;
#endif
}

//-------------------------------------------------------------------------------------
// In the Editor it is necessary to disable the view extension via the upscaler API so it doesn't cause conflicts.
//-------------------------------------------------------------------------------------
#if WITH_EDITOR
bool FFSR2TemporalUpscaler::IsEnabledInEditor() const
{
	return bEnabledInEditor;
}

void FFSR2TemporalUpscaler::SetEnabledInEditor(bool bEnabled)
{
	bEnabledInEditor = bEnabled;
}
#endif

//-------------------------------------------------------------------------------------
// The interesting function in the denoiser API that lets us capture the reflections texture.
// This has to replicate the behavior of the engine, only we retain a reference to the output texture.
//-------------------------------------------------------------------------------------
IScreenSpaceDenoiser::FReflectionsOutputs FFSR2TemporalUpscaler::DenoiseReflections(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FReflectionsInputs& ReflectionInputs,
	const FReflectionsRayTracingConfig RayTracingConfig) const
{
	IScreenSpaceDenoiser::FReflectionsOutputs Outputs;
	Outputs.Color = ReflectionInputs.Color;
	if (FSR2ShouldRenderRayTracingReflections(View) || CVarFSR2UseExperimentalSSRDenoiser.GetValueOnRenderThread())
	{
		Outputs = WrappedDenoiser->DenoiseReflections(GraphBuilder, View, PreviousViewInfos, SceneTextures, ReflectionInputs, RayTracingConfig);
	}
	else if (IsFSR2SSRTemporalPassRequired(View))
	{
		const bool bComposePlanarReflections = FSR2HasDeferredPlanarReflections(View);

		check(View.ViewState);
		FTAAPassParameters TAASettings(View);
		TAASettings.Pass = ETAAPassConfig::ScreenSpaceReflections;
		TAASettings.SceneDepthTexture = SceneTextures.SceneDepthTexture;
		TAASettings.SceneVelocityTexture = SceneTextures.GBufferVelocityTexture;
		TAASettings.SceneColorInput = ReflectionInputs.Color;
		TAASettings.bOutputRenderTargetable = bComposePlanarReflections;

		FTAAOutputs TAAOutputs = AddTemporalAAPass(
			GraphBuilder,
			View,
			TAASettings,
			View.PrevViewInfo.SSRHistory,
			&View.ViewState->PrevFrameViewInfo.SSRHistory);

		Outputs.Color = TAAOutputs.SceneColor;
	}
	ReflectionTexture = Outputs.Color;
	return Outputs;
}

//-------------------------------------------------------------------------------------
// The remaining denoiser API simply passes through to the wrapped denoiser.
//-------------------------------------------------------------------------------------
IScreenSpaceDenoiser::EShadowRequirements FFSR2TemporalUpscaler::GetShadowRequirements(
	const FViewInfo& View,
	const FLightSceneInfo& LightSceneInfo,
	const FShadowRayTracingConfig& RayTracingConfig) const
{
	return WrappedDenoiser->GetShadowRequirements(View, LightSceneInfo, RayTracingConfig);
}

void FFSR2TemporalUpscaler::DenoiseShadowVisibilityMasks(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const TStaticArray<FShadowVisibilityParameters, IScreenSpaceDenoiser::kMaxBatchSize>& InputParameters,
	const int32 InputParameterCount,
	TStaticArray<FShadowVisibilityOutputs, IScreenSpaceDenoiser::kMaxBatchSize>& Outputs) const
{

	return WrappedDenoiser->DenoiseShadowVisibilityMasks(GraphBuilder, View, PreviousViewInfos, SceneTextures, InputParameters, InputParameterCount, Outputs);
}

IScreenSpaceDenoiser::FPolychromaticPenumbraOutputs FFSR2TemporalUpscaler::DenoisePolychromaticPenumbraHarmonics(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FPolychromaticPenumbraHarmonics& Inputs) const
{
	return WrappedDenoiser->DenoisePolychromaticPenumbraHarmonics(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs);
}

IScreenSpaceDenoiser::FReflectionsOutputs FFSR2TemporalUpscaler::DenoiseWaterReflections(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FReflectionsInputs& ReflectionInputs,
	const FReflectionsRayTracingConfig RayTracingConfig) const
{
	return WrappedDenoiser->DenoiseWaterReflections(GraphBuilder, View, PreviousViewInfos, SceneTextures, ReflectionInputs, RayTracingConfig);
}

IScreenSpaceDenoiser::FAmbientOcclusionOutputs FFSR2TemporalUpscaler::DenoiseAmbientOcclusion(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FAmbientOcclusionInputs& ReflectionInputs,
	const FAmbientOcclusionRayTracingConfig RayTracingConfig) const
{
	return WrappedDenoiser->DenoiseAmbientOcclusion(GraphBuilder, View, PreviousViewInfos, SceneTextures, ReflectionInputs, RayTracingConfig);
}

FSSDSignalTextures FFSR2TemporalUpscaler::DenoiseDiffuseIndirect(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectInputs& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return WrappedDenoiser->DenoiseDiffuseIndirect(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}

IScreenSpaceDenoiser::FDiffuseIndirectOutputs FFSR2TemporalUpscaler::DenoiseSkyLight(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectInputs& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return WrappedDenoiser->DenoiseSkyLight(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}

IScreenSpaceDenoiser::FDiffuseIndirectOutputs FFSR2TemporalUpscaler::DenoiseReflectedSkyLight(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectInputs& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return WrappedDenoiser->DenoiseReflectedSkyLight(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}

IScreenSpaceDenoiser::FDiffuseIndirectHarmonic FFSR2TemporalUpscaler::DenoiseDiffuseIndirectHarmonic(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectHarmonic& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return WrappedDenoiser->DenoiseDiffuseIndirectHarmonic(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}

bool FFSR2TemporalUpscaler::SupportsScreenSpaceDiffuseIndirectDenoiser(EShaderPlatform Platform) const
{
	return WrappedDenoiser->SupportsScreenSpaceDiffuseIndirectDenoiser(Platform);
}

FSSDSignalTextures FFSR2TemporalUpscaler::DenoiseScreenSpaceDiffuseIndirect(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectInputs& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return WrappedDenoiser->DenoiseScreenSpaceDiffuseIndirect(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}
