#include "ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe.h"
#include "ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3.h"
#include "ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452.h"
#include "ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e.h"

typedef union ffx_fsr2_rcas_pass_PermutationKey {
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
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
    3,
    3,
    0,
    0,
};

static const ffx_fsr2_rcas_pass_PermutationInfo g_ffx_fsr2_rcas_pass_PermutationInfo[] = {
    { g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_size, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_SampledImageResourceNames, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_SampledImageResourceBindings, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_SampledImageResourceCounts, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_SampledImageResourceSets, 1, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_StorageImageResourceNames, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_StorageImageResourceBindings, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_StorageImageResourceCounts, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_UniformBufferResourceNames, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_UniformBufferResourceBindings, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_UniformBufferResourceCounts, g_ffx_fsr2_rcas_pass_af2875e263f65e45b4e1c12a2c4873fe_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_size, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_SampledImageResourceNames, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_SampledImageResourceBindings, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_SampledImageResourceCounts, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_SampledImageResourceSets, 1, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_StorageImageResourceNames, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_StorageImageResourceBindings, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_StorageImageResourceCounts, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_UniformBufferResourceNames, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_UniformBufferResourceBindings, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_UniformBufferResourceCounts, g_ffx_fsr2_rcas_pass_a01c1a3d334bc541d4539341d7d1dce3_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_size, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_SampledImageResourceNames, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_SampledImageResourceBindings, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_SampledImageResourceCounts, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_SampledImageResourceSets, 1, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_StorageImageResourceNames, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_StorageImageResourceBindings, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_StorageImageResourceCounts, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_UniformBufferResourceNames, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_UniformBufferResourceBindings, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_UniformBufferResourceCounts, g_ffx_fsr2_rcas_pass_67630a0f455ef63cc8b4cc842115a452_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_size, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_SampledImageResourceNames, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_SampledImageResourceBindings, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_SampledImageResourceCounts, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_SampledImageResourceSets, 1, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_StorageImageResourceNames, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_StorageImageResourceBindings, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_StorageImageResourceCounts, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_UniformBufferResourceNames, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_UniformBufferResourceBindings, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_UniformBufferResourceCounts, g_ffx_fsr2_rcas_pass_e8d88389761db2b07182383087be1e9e_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

