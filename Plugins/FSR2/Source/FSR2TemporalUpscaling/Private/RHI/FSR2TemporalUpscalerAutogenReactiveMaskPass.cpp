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
#include "FSR2TemporalUpscalerShaders.h"
#include "FSR2TemporalUpscalerSubPass.h"
#include "FSR2TemporalUpscalerRHIBackend.h"
#include "ShaderParameterStruct.h"

BEGIN_UNIFORM_BUFFER_STRUCT(FFSR2GenerateReactiveParameters, )
	SHADER_PARAMETER(float, scale)
	SHADER_PARAMETER(float, threshold)
	SHADER_PARAMETER(uint32, flags)
	SHADER_PARAMETER(float, _Padding)
END_UNIFORM_BUFFER_STRUCT()

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FFSR2GenerateReactiveParameters, "cbGenerateReactive");

class FFSR2AutogenReactiveMaskCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FFSR2AutogenReactiveMaskCS);
	SHADER_USE_PARAMETER_STRUCT(FFSR2AutogenReactiveMaskCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FFSR2GenerateReactiveParameters, cbGenerateReactive)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, r_input_color_pre_alpha)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, r_input_color_post_alpha)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, rw_output_reactive_mask)
	END_SHADER_PARAMETER_STRUCT()

	using FPermutationDomain = FFSR2GlobalShader::FPermutationDomain;

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return FFSR2GlobalShader::ShouldCompilePermutation(Parameters);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FFSR2GlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
	static uint32* GetBoundSRVs()
	{
		static uint32 SRVs[] = { FFX_FSR2_RESOURCE_IDENTIFIER_INPUT_COLOR, FFX_FSR2_RESOURCE_IDENTIFIER_PREPARED_INPUT_COLOR };
		return SRVs;
	}
	static uint32* GetBoundUAVs()
	{
		static uint32 UAVs[] = { FFX_FSR2_RESOURCE_IDENTIFIER_INPUT_REACTIVE_MASK };
		return UAVs;
	}
	static uint32 GetNumBoundSRVs()
	{
		return 2;
	}
	static uint32 GetNumBoundUAVs()
	{
		return 1;
	}
	static uint32 GetNumConstants()
	{
		return 1;
	}
	static uint32* GetBoundCBs()
	{
		static uint32 CBs[] = { FFX_FSR2_CONSTANTBUFFER_IDENTIFIER_FSR2 };
		return CBs;
	}
	static uint32 GetConstantSizeInDWords(uint32 Index)
	{
		static uint32 Sizes[] = { sizeof(FFSR2PassParameters) / sizeof(uint32) };
		return Sizes[Index];
	}
	static void BindParameters(FRDGBuilder& GraphBuilder, FFSR2BackendState* Context, const FfxGpuJobDescription* job, FParameters* Parameters)
	{
		for (uint32 i = 0; i < job->computeJobDescriptor.pipeline.constCount; i++)
		{
			switch (job->computeJobDescriptor.pipeline.cbResourceBindings[i].resourceIdentifier)
			{
				case FFX_FSR2_CONSTANTBUFFER_IDENTIFIER_FSR2:
				{
					FFSR2GenerateReactiveParameters PassParams;
					FMemory::Memcpy(&PassParams, job->computeJobDescriptor.cbs[i].data, sizeof(FFSR2GenerateReactiveParameters));
					Parameters->cbGenerateReactive = TUniformBufferRef<FFSR2GenerateReactiveParameters>::CreateUniformBufferImmediate(PassParams, UniformBuffer_SingleDraw);
					break;
				}
				case FFX_FSR2_CONSTANTBUFFER_IDENTIFIER_SPD:
				case FFX_FSR2_CONSTANTBUFFER_IDENTIFIER_RCAS:
				default:
				{
					break;
				}
			}
		}

		for (uint32 i = 0; i < job->computeJobDescriptor.pipeline.srvCount; i++)
		{
			switch (job->computeJobDescriptor.pipeline.srvResourceBindings[i].resourceIdentifier)
			{
				case FFX_FSR2_RESOURCE_IDENTIFIER_INPUT_COLOR:
				{
					Parameters->r_input_color_pre_alpha = Context->GetRDGTexture(GraphBuilder, job->computeJobDescriptor.srvs[i].internalIndex);
					break;
				}
				case FFX_FSR2_RESOURCE_IDENTIFIER_PREPARED_INPUT_COLOR:
				{
					Parameters->r_input_color_post_alpha = Context->GetRDGTexture(GraphBuilder, job->computeJobDescriptor.srvs[i].internalIndex);
					break;
				}
				default:
				{
					break;
				}
			}
		}

		for (uint32 i = 0; i < job->computeJobDescriptor.pipeline.uavCount; i++)
		{
			switch (job->computeJobDescriptor.pipeline.uavResourceBindings[i].resourceIdentifier)
			{
				case FFX_FSR2_RESOURCE_IDENTIFIER_INPUT_REACTIVE_MASK:
				{
					Parameters->rw_output_reactive_mask = GraphBuilder.CreateUAV(Context->GetRDGTexture(GraphBuilder, job->computeJobDescriptor.uavs[i].internalIndex));
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}
};
IMPLEMENT_GLOBAL_SHADER(FFSR2AutogenReactiveMaskCS, "/Plugin/FSR2/Private/ffx_fsr2_autogen_reactive_pass.usf", "CS", SF_Compute);

IFSR2SubPass* GetAutogenReactiveMaskPass(FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf)
{
	return new TFSR2SubPass<FFSR2AutogenReactiveMaskCS>(TEXT("FidelityFX-FSR2/AutogenReactiveMask (CS)"), passId, desc, outPipeline, bSupportHalf);
}
