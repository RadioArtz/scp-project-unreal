#include "ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f.h"
#include "ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f.h"

typedef union ffx_fsr2_reconstruct_previous_depth_pass_PermutationKey {
    struct {
        uint32_t FFX_FSR2_OPTION_USE_LANCZOS_LUT : 1;
        uint32_t FFX_FSR2_OPTION_HDR_COLOR_INPUT : 1;
        uint32_t FFX_FSR2_OPTION_LOW_RESOLUTION_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_JITTERED_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_INVERTED_DEPTH : 1;
        uint32_t FFX_FSR2_OPTION_APPLY_SHARPENING : 1;
        uint32_t FFX_HALF : 1;
    };
    uint32_t index;
} ffx_fsr2_reconstruct_previous_depth_pass_PermutationKey;

typedef struct ffx_fsr2_reconstruct_previous_depth_pass_PermutationInfo {
    const uint32_t       blobSize;
    const unsigned char* blobData;

    const uint32_t  numSamplerResources;
    const char**    samplerResourceNames;
    const uint32_t* samplerResourceBindings;
    const uint32_t* samplerResourceCounts;
    const uint32_t* samplerResourceSets;

    const uint32_t  numCombinedSamplerResources;
    const char**    combinedSamplerResourceNames;
    const uint32_t* combinedSamplerResourceBindings;
    const uint32_t* combinedSamplerResourceCounts;
    const uint32_t* combinedSamplerResourceSets;

    const uint32_t  numSampledImageResources;
    const char**    sampledImageResourceNames;
    const uint32_t* sampledImageResourceBindings;
    const uint32_t* sampledImageResourceCounts;
    const uint32_t* sampledImageResourceSets;

    const uint32_t  numStorageImageResources;
    const char**    storageImageResourceNames;
    const uint32_t* storageImageResourceBindings;
    const uint32_t* storageImageResourceCounts;
    const uint32_t* storageImageResourceSets;

    const uint32_t  numUniformTexelBufferResources;
    const char**    uniformTexelBufferResourceNames;
    const uint32_t* uniformTexelBufferResourceBindings;
    const uint32_t* uniformTexelBufferResourceCounts;
    const uint32_t* uniformTexelBufferResourceSets;

    const uint32_t  numStorageTexelBufferResources;
    const char**    storageTexelBufferResourceNames;
    const uint32_t* storageTexelBufferResourceBindings;
    const uint32_t* storageTexelBufferResourceCounts;
    const uint32_t* storageTexelBufferResourceSets;

    const uint32_t  numUniformBufferResources;
    const char**    uniformBufferResourceNames;
    const uint32_t* uniformBufferResourceBindings;
    const uint32_t* uniformBufferResourceCounts;
    const uint32_t* uniformBufferResourceSets;

    const uint32_t  numStorageBufferResources;
    const char**    storageBufferResourceNames;
    const uint32_t* storageBufferResourceBindings;
    const uint32_t* storageBufferResourceCounts;
    const uint32_t* storageBufferResourceSets;

    const uint32_t  numInputAttachmentResources;
    const char**    inputAttachmentResourceNames;
    const uint32_t* inputAttachmentResourceBindings;
    const uint32_t* inputAttachmentResourceCounts;
    const uint32_t* inputAttachmentResourceSets;

    const uint32_t  numRTAccelerationStructureResources;
    const char**    rtAccelerationStructureResourceNames;
    const uint32_t* rtAccelerationStructureResourceBindings;
    const uint32_t* rtAccelerationStructureResourceCounts;
    const uint32_t* rtAccelerationStructureResourceSets;

} ffx_fsr2_reconstruct_previous_depth_pass_PermutationInfo;

static const uint32_t g_ffx_fsr2_reconstruct_previous_depth_pass_IndirectionTable[] = {
    13,
    13,
    13,
    13,
    5,
    5,
    5,
    5,
    10,
    10,
    10,
    10,
    3,
    3,
    3,
    3,
    9,
    9,
    9,
    9,
    2,
    2,
    2,
    2,
    7,
    7,
    7,
    7,
    0,
    0,
    0,
    0,
    13,
    13,
    13,
    13,
    5,
    5,
    5,
    5,
    10,
    10,
    10,
    10,
    3,
    3,
    3,
    3,
    9,
    9,
    9,
    9,
    2,
    2,
    2,
    2,
    7,
    7,
    7,
    7,
    0,
    0,
    0,
    0,
    14,
    14,
    14,
    14,
    6,
    6,
    6,
    6,
    11,
    11,
    11,
    11,
    4,
    4,
    4,
    4,
    15,
    15,
    15,
    15,
    8,
    8,
    8,
    8,
    12,
    12,
    12,
    12,
    1,
    1,
    1,
    1,
    14,
    14,
    14,
    14,
    6,
    6,
    6,
    6,
    11,
    11,
    11,
    11,
    4,
    4,
    4,
    4,
    15,
    15,
    15,
    15,
    8,
    8,
    8,
    8,
    12,
    12,
    12,
    12,
    1,
    1,
    1,
    1,
};

static const ffx_fsr2_reconstruct_previous_depth_pass_PermutationInfo g_ffx_fsr2_reconstruct_previous_depth_pass_PermutationInfo[] = {
    { g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_size, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_4536f7e7f7ca497207c61e9c7399639c_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_size, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_be8be95e4a5bee85b62df9e8eeb0e5df_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_size, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_9c9634f5018d340fdc43b3e9e4485f14_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_size, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_fbe76aac6e4805a6ae0f835d8b8a8627_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_size, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_be158c4b94a289710ab50930072326a7_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_size, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_d879891512c72d9376e60d01ee1de734_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_size, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_4770c5556fbf3b04af331ce1c8d0aa6b_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_size, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_193205e7f5e1700df0f9cf070522932f_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_size, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_43d3aca05ffdb8c9a9748360bd492449_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_size, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_2ea764755e53f12661cdc7d93a2124c5_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_size, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_6a5efc4393ca395eb778f2e6707a03e5_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_size, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_b3a4f0cf047a469b0dbea538d07628c1_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_size, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_63ba605d8150b697bec5f5bdd960e726_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_size, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_33efe9e76f14cbea166368d60daf11b1_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_size, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_915ac48d81992de77dcba6ee54bb634f_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_size, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_SampledImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_SampledImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_SampledImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_SampledImageResourceSets, 3, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_StorageImageResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_StorageImageResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_StorageImageResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_UniformBufferResourceNames, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_UniformBufferResourceBindings, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_UniformBufferResourceCounts, g_ffx_fsr2_reconstruct_previous_depth_pass_9cd1c534b54f98dec142b2edc375b53f_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

