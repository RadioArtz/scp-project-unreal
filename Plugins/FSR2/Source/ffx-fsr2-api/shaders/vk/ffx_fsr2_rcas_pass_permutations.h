#include "ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97.h"
#include "ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48.h"
#include "ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5.h"
#include "ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542.h"

typedef union ffx_fsr2_rcas_pass_PermutationKey {
    struct {
        uint32_t FFX_FSR2_OPTION_REPROJECT_USE_LANCZOS_TYPE : 1;
        uint32_t FFX_FSR2_OPTION_HDR_COLOR_INPUT : 1;
        uint32_t FFX_FSR2_OPTION_LOW_RESOLUTION_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_JITTERED_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_INVERTED_DEPTH : 1;
        uint32_t FFX_FSR2_OPTION_APPLY_SHARPENING : 1;
        uint32_t FFX_HALF : 1;
    };
    uint32_t index;
} ffx_fsr2_rcas_pass_PermutationKey;

typedef struct ffx_fsr2_rcas_pass_PermutationInfo {
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

} ffx_fsr2_rcas_pass_PermutationInfo;

static const uint32_t g_ffx_fsr2_rcas_pass_IndirectionTable[] = {
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    3,
    3,
    1,
    1,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
    2,
    2,
    0,
    0,
};

static const ffx_fsr2_rcas_pass_PermutationInfo g_ffx_fsr2_rcas_pass_PermutationInfo[] = {
    { g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_size, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_SampledImageResourceNames, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_SampledImageResourceBindings, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_SampledImageResourceCounts, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_SampledImageResourceSets, 1, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_StorageImageResourceNames, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_StorageImageResourceBindings, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_StorageImageResourceCounts, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_UniformBufferResourceNames, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_UniformBufferResourceBindings, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_UniformBufferResourceCounts, g_ffx_fsr2_rcas_pass_f501ffbbc8d3294b66318180d320df97_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_size, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_SampledImageResourceNames, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_SampledImageResourceBindings, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_SampledImageResourceCounts, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_SampledImageResourceSets, 1, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_StorageImageResourceNames, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_StorageImageResourceBindings, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_StorageImageResourceCounts, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_UniformBufferResourceNames, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_UniformBufferResourceBindings, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_UniformBufferResourceCounts, g_ffx_fsr2_rcas_pass_64fc2a489eea6a5232d160ede1f50c48_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_size, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_SampledImageResourceNames, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_SampledImageResourceBindings, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_SampledImageResourceCounts, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_SampledImageResourceSets, 1, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_StorageImageResourceNames, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_StorageImageResourceBindings, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_StorageImageResourceCounts, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_UniformBufferResourceNames, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_UniformBufferResourceBindings, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_UniformBufferResourceCounts, g_ffx_fsr2_rcas_pass_2b2bdb6d959ddc2f77c82809aae938c5_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_size, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_SampledImageResourceNames, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_SampledImageResourceBindings, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_SampledImageResourceCounts, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_SampledImageResourceSets, 1, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_StorageImageResourceNames, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_StorageImageResourceBindings, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_StorageImageResourceCounts, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_UniformBufferResourceNames, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_UniformBufferResourceBindings, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_UniformBufferResourceCounts, g_ffx_fsr2_rcas_pass_db8e49507f672cfc07d47b22e2ef2542_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

