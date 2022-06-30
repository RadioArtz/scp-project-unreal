#include "ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6.h"
#include "ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52.h"
#include "ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e.h"
#include "ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e.h"
#include "ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4.h"
#include "ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637.h"
#include "ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802.h"
#include "ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca.h"

typedef union ffx_fsr2_prepare_input_color_pass_PermutationKey {
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
    6,
    6,
    0,
    0,
    6,
    6,
    0,
    0,
    6,
    6,
    0,
    0,
    6,
    6,
    0,
    0,
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
    0,
    0,
    6,
    6,
    0,
    0,
    6,
    6,
    0,
    0,
    6,
    6,
    0,
    0,
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
    5,
    5,
    2,
    2,
    5,
    5,
    2,
    2,
    5,
    5,
    2,
    2,
    5,
    5,
    2,
    2,
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
    5,
    5,
    2,
    2,
    5,
    5,
    2,
    2,
    5,
    5,
    2,
    2,
    5,
    5,
    2,
    2,
};

static const ffx_fsr2_prepare_input_color_pass_PermutationInfo g_ffx_fsr2_prepare_input_color_pass_PermutationInfo[] = {
    { g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_size, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_f861c5249fad3638c9a7421498a376b6_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_size, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_63b498ef955d11ea4464b15d2caaaa52_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_size, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_4e2d0f0a858e4fa5c9a30e8e51d0a93e_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_size, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_c07c3517ef6a624a3ec322e7db67936e_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_size, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_e32555c9773b92cfdb1b0a0562e36dd4_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_size, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_0c200335aea32da0f1d5ee0853b14637_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_size, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_32de2639ad257dbb2b96f4414cb42802_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_size, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_data, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_SampledImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_SampledImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_SampledImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_SampledImageResourceSets, 3, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_StorageImageResourceNames, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_StorageImageResourceBindings, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_StorageImageResourceCounts, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_StorageImageResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_UniformBufferResourceNames, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_UniformBufferResourceBindings, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_UniformBufferResourceCounts, g_ffx_fsr2_prepare_input_color_pass_610818f6fa6920aac417819e7869f0ca_UniformBufferResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

