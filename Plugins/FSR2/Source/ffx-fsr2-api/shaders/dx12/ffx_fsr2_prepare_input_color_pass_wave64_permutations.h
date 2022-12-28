#include "ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096.h"
#include "ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a.h"
#include "ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880.h"
#include "ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0.h"

typedef union ffx_fsr2_prepare_input_color_pass_wave64_PermutationKey {
    struct {
        uint32_t FFX_FSR2_OPTION_REPROJECT_USE_LANCZOS_TYPE : 1;
        uint32_t FFX_FSR2_OPTION_HDR_COLOR_INPUT : 1;
        uint32_t FFX_FSR2_OPTION_LOW_RESOLUTION_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_JITTERED_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_INVERTED_DEPTH : 1;
        uint32_t FFX_FSR2_OPTION_APPLY_SHARPENING : 1;
    };
    uint32_t index;
} ffx_fsr2_prepare_input_color_pass_wave64_PermutationKey;

typedef struct ffx_fsr2_prepare_input_color_pass_wave64_PermutationInfo {
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
} ffx_fsr2_prepare_input_color_pass_wave64_PermutationInfo;

static const uint32_t g_ffx_fsr2_prepare_input_color_pass_wave64_IndirectionTable[] = {
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
};

static const ffx_fsr2_prepare_input_color_pass_wave64_PermutationInfo g_ffx_fsr2_prepare_input_color_pass_wave64_PermutationInfo[] = {
    { g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_size, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_data, 1, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_CBVResourceNames, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_CBVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_CBVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_CBVResourceSpaces, 2, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_SRVResourceNames, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_SRVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_SRVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_SRVResourceSpaces, 3, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_UAVResourceNames, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_UAVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_UAVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_wave64_c5d75a17ad1406102242e610c9eff096_UAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_size, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_data, 1, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_CBVResourceNames, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_CBVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_CBVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_CBVResourceSpaces, 2, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_SRVResourceNames, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_SRVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_SRVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_SRVResourceSpaces, 3, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_UAVResourceNames, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_UAVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_UAVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_wave64_243ba1f0782310dbdd7e2f20c827ff2a_UAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_size, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_data, 1, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_CBVResourceNames, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_CBVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_CBVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_CBVResourceSpaces, 2, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_SRVResourceNames, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_SRVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_SRVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_SRVResourceSpaces, 3, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_UAVResourceNames, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_UAVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_UAVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_wave64_af3e1243c7f0f7bd7c845ac1d001e880_UAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_size, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_data, 1, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_CBVResourceNames, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_CBVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_CBVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_CBVResourceSpaces, 2, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_SRVResourceNames, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_SRVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_SRVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_SRVResourceSpaces, 3, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_UAVResourceNames, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_UAVResourceBindings, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_UAVResourceCounts, g_ffx_fsr2_prepare_input_color_pass_wave64_66b71b630fc1e833b813810e74b6e5b0_UAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

