#include "ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128.h"
#include "ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7.h"
#include "ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4.h"
#include "ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002.h"

typedef union ffx_fsr2_prepare_input_color_pass_16bit_PermutationKey {
    struct {
        uint32_t FFX_FSR2_OPTION_REPROJECT_USE_LANCZOS_TYPE : 1;
        uint32_t FFX_FSR2_OPTION_HDR_COLOR_INPUT : 1;
        uint32_t FFX_FSR2_OPTION_LOW_RESOLUTION_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_JITTERED_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_INVERTED_DEPTH : 1;
        uint32_t FFX_FSR2_OPTION_APPLY_SHARPENING : 1;
    };
    uint32_t index;
} ffx_fsr2_prepare_input_color_pass_16bit_PermutationKey;

typedef struct ffx_fsr2_prepare_input_color_pass_16bit_PermutationInfo {
    const uint32_t       blobSize;
    const unsigned char* blobData;


    const uint32_t  numCBVResources;
    const char**    cbvResourceNames;
    const uint32_t* cbvResourceBindings;
    const uint32_t* cbvResourceCounts;
    const uint32_t* cbvResourceSpaces;

    const uint32_t  numSRVResources;
    const char**    srvResourceNames;
    const uint32_t* srvResourceBindings;
    const uint32_t* srvResourceCounts;
    const uint32_t* srvResourceSpaces;

    const uint32_t  numUAVResources;
    const char**    uavResourceNames;
    const uint32_t* uavResourceBindings;
    const uint32_t* uavResourceCounts;
    const uint32_t* uavResourceSpaces;

    const uint32_t  numSamplerResources;
    const char**    samplerResourceNames;
    const uint32_t* samplerResourceBindings;
    const uint32_t* samplerResourceCounts;
    const uint32_t* samplerResourceSpaces;

    const uint32_t  numRTAccelerationStructureResources;
    const char**    rtAccelerationStructureResourceNames;
    const uint32_t* rtAccelerationStructureResourceBindings;
    const uint32_t* rtAccelerationStructureResourceCounts;
    const uint32_t* rtAccelerationStructureResourceSpaces;
} ffx_fsr2_prepare_input_color_pass_16bit_PermutationInfo;

static const uint32_t g_ffx_fsr2_prepare_input_color_pass_16bit_IndirectionTable[] = {
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
};

static const ffx_fsr2_prepare_input_color_pass_16bit_PermutationInfo g_ffx_fsr2_prepare_input_color_pass_16bit_PermutationInfo[] = {
    { g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_size, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_data, 1, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_CBVResourceNames, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_CBVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_CBVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_CBVResourceSpaces, 2, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_SRVResourceNames, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_SRVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_SRVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_SRVResourceSpaces, 3, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_UAVResourceNames, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_UAVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_UAVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_16bit_166e9e5927816e97b90d2543aef98128_UAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_size, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_data, 1, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_CBVResourceNames, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_CBVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_CBVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_CBVResourceSpaces, 2, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_SRVResourceNames, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_SRVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_SRVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_SRVResourceSpaces, 3, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_UAVResourceNames, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_UAVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_UAVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_16bit_ced40edf64f9d2a35db3bd52a316bff7_UAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_size, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_data, 1, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_CBVResourceNames, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_CBVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_CBVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_CBVResourceSpaces, 2, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_SRVResourceNames, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_SRVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_SRVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_SRVResourceSpaces, 3, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_UAVResourceNames, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_UAVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_UAVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_16bit_d499105beebb63020898dcb3074f1aa4_UAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_size, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_data, 1, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_CBVResourceNames, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_CBVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_CBVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_CBVResourceSpaces, 2, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_SRVResourceNames, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_SRVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_SRVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_SRVResourceSpaces, 3, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_UAVResourceNames, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_UAVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_UAVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_16bit_531d36896573e61a7dcd649fe3ffd002_UAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

