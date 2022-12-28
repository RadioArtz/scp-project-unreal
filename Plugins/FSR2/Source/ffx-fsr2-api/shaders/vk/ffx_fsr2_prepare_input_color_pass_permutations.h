#include "ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b.h"
#include "ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514.h"
#include "ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63.h"
#include "ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f.h"
#include "ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773.h"
#include "ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1.h"
#include "ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f.h"
#include "ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48.h"

typedef union ffx_fsr2_prepare_input_color_pass_PermutationKey {
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
} ffx_fsr2_prepare_input_color_pass_PermutationKey;

typedef struct ffx_fsr2_prepare_input_color_pass_PermutationInfo {
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

} ffx_fsr2_prepare_input_color_pass_PermutationInfo;

static const uint32_t g_ffx_fsr2_prepare_input_color_pass_IndirectionTable[] = {
    7,
    7,
    3,
    3,
    7,
    7,
    3,
    3,
    7,
    7,
    3,
    3,
    7,
    7,
    3,
    3,
    4,
    4,
    1,
    1,
    4,
    4,
    1,
    1,
    4,
    4,
    1,
    1,
    4,
    4,
    1,
    1,
    7,
    7,
    3,
    3,
    7,
    7,
    3,
    3,
    7,
    7,
    3,
    3,
    7,
    7,
    3,
    3,
    4,
    4,
    1,
    1,
    4,
    4,
    1,
    1,
    4,
    4,
    1,
    1,
    4,
    4,
    1,
    1,
    6,
    6,
    2,
    2,
    6,
    6,
    2,
    2,
    6,
    6,
    2,
    2,
    6,
    6,
    2,
    2,
    5,
    5,
    0,
    0,
    5,
    5,
    0,
    0,
    5,
    5,
    0,
    0,
    5,
    5,
    0,
    0,
    6,
    6,
    2,
    2,
    6,
    6,
    2,
    2,
    6,
    6,
    2,
    2,
    6,
    6,
    2,
    2,
    5,
    5,
    0,
    0,
    5,
    5,
    0,
    0,
    5,
    5,
    0,
    0,
    5,
    5,
    0,
    0,
};

static const ffx_fsr2_prepare_input_color_pass_PermutationInfo g_ffx_fsr2_prepare_input_color_pass_PermutationInfo[] = {
    { g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_size, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_acbedc03bcf32346840f978ed3e9896b_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_size, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_d90c58950ca346cac03b627f3904a514_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_size, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_307f84c5ef7880d9661a4dab2fd6eb63_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_size, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_be1404333d29691659ec41c13d9d177f_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_size, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_62dfc2ad981d122f6811d9297c2bb773_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_size, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_5050c4ff39a37167026a52104a125fa1_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_size, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_c2f5ca74125a7271d34b76c2cc7a864f_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_size, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_39501183d96434325345a60d6a0abc48_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

