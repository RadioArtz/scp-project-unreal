#include "ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8.h"
#include "ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878.h"
#include "ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b.h"
#include "ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5.h"

typedef union ffx_fsr2_prepare_input_color_pass_PermutationKey {
    struct {
        uint32_t FFX_FSR2_OPTION_REPROJECT_USE_LANCZOS_TYPE : 1;
        uint32_t FFX_FSR2_OPTION_HDR_COLOR_INPUT : 1;
        uint32_t FFX_FSR2_OPTION_LOW_RESOLUTION_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_JITTERED_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_INVERTED_DEPTH : 1;
        uint32_t FFX_FSR2_OPTION_APPLY_SHARPENING : 1;
    };
    uint32_t index;
} ffx_fsr2_prepare_input_color_pass_PermutationKey;

typedef struct ffx_fsr2_prepare_input_color_pass_PermutationInfo {
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
} ffx_fsr2_prepare_input_color_pass_PermutationInfo;

static const uint32_t g_ffx_fsr2_prepare_input_color_pass_IndirectionTable[] = {
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
};

static const ffx_fsr2_prepare_input_color_pass_PermutationInfo g_ffx_fsr2_prepare_input_color_pass_PermutationInfo[] = {
    { g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_size, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_data, 1, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_CBVResourceNames, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_CBVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_CBVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_CBVResourceSpaces, 2, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_SRVResourceNames, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_SRVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_SRVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_SRVResourceSpaces, 3, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_UAVResourceNames, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_UAVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_UAVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_88cb96691bfbced1506bdeedd7c7dee8_UAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_size, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_data, 1, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_CBVResourceNames, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_CBVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_CBVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_CBVResourceSpaces, 2, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_SRVResourceNames, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_SRVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_SRVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_SRVResourceSpaces, 3, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_UAVResourceNames, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_UAVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_UAVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_6f01c9abdd42a2ae800726db5f9a5878_UAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_size, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_data, 1, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_CBVResourceNames, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_CBVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_CBVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_CBVResourceSpaces, 2, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_SRVResourceNames, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_SRVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_SRVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_SRVResourceSpaces, 3, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_UAVResourceNames, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_UAVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_UAVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_0f3dec5681326fc5795efb243badba7b_UAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_size, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_data, 1, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_CBVResourceNames, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_CBVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_CBVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_CBVResourceSpaces, 2, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_SRVResourceNames, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_SRVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_SRVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_SRVResourceSpaces, 3, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_UAVResourceNames, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_UAVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_UAVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_bf2d33f2778ceb118aebb05ca24ae9b5_UAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

