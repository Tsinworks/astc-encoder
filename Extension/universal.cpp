// NON_SIMD
//#include "astcenc_block_sizes.cpp"
//#include "astcenc_color_quantize.cpp"
//#define unquant_color astcenc_unquant_color
//#include "astcenc_color_unquantize.cpp"
//#undef unquant_color
//
//#include "astcenc_compute_variance.cpp"

#define ASTCENC_AVX 2
#define ASTCENC_SSE 41
#define ASTCENC_CUDA 1

#define ASTCENC_DYNAMIC_LIBRARY

#include "universal.hpp"
#include "astcenc.h"
#include "astcenc_mathlib.cpp"
#include "astcenc_platform_isa_detection.cpp"

#if ASTCENC_CUDA
#include "nvtt.hpp"
#endif

extern "C" astcenc_error astcenc_config_init_sse(
    astcenc_profile profile,
    unsigned int block_x,
    unsigned int block_y,
    unsigned int block_z,
    float quality,
    unsigned int flags,
    astcenc_config* configp);

extern "C" astcenc_error astcenc_config_init_avx2(
    astcenc_profile profile,
    unsigned int block_x,
    unsigned int block_y,
    unsigned int block_z,
    float quality,
    unsigned int flags,
    astcenc_config* configp);

astcenc_error astcenc_config_init(
    astcenc_profile profile,
    unsigned int block_x,
    unsigned int block_y,
    unsigned int block_z,
    float quality,
    unsigned int flags,
    astcenc_config* configp)
{
    if (cpu_supports_avx2()) {
        return astcenc_config_init_avx2(profile, block_x, block_y, block_z, quality, flags, configp);
    } else if (cpu_supports_sse41()) {
        return astcenc_config_init_sse(profile, block_x, block_y, block_z, quality, flags, configp);
    } else {
        return ASTCENC_SUCCESS;
    }
}

/* See header for documentation. */
astcenc_error astcenc_context_alloc(
    const astcenc_config* configp,
    unsigned int thread_count,
    astcenc_context** context)
{
    astcenc_error err = ASTCENC_SUCCESS;
    if (context && *context) {
        *context = (astcenc_context*)new astc::GpuContext(configp, thread_count, err);
    }
    return err;
}

astcenc_error astcenc_compress_image(
    astcenc_context* ctx,
    astcenc_image* imagep,
    const astcenc_swizzle* swizzle,
    uint8_t* data_out,
    size_t data_len,
    unsigned int thread_index)
{
    return ((astc::BaseContext*)ctx)->compress(imagep, swizzle, data_out, data_len, thread_index);
}

astcenc_error astcenc_compress_reset(
    astcenc_context* ctx)
{
    return ((astc::BaseContext*)ctx)->compress_reset();
}

astcenc_error astcenc_decompress_image(
    astcenc_context* ctx,
    const uint8_t* data,
    size_t data_len,
    astcenc_image* image_outp,
    const astcenc_swizzle* swizzle,
    unsigned int thread_index)
{
    return ((astc::BaseContext*)ctx)->decompress(data, data_len, image_outp, swizzle, thread_index);
}

astcenc_error astcenc_decompress_reset(
    astcenc_context* ctx)
{
    return ((astc::BaseContext*)ctx)->decompress_reset();
}

void astcenc_context_free(
    astcenc_context* ctx)
{
    delete ((astc::BaseContext*)ctx);
}

extern "C" astcenc_error astcenc_get_block_info_sse(
    astcenc_context* context,
    const uint8_t data[16],
    astcenc_block_info* info);
extern "C" astcenc_error astcenc_get_block_info_avx2(
    astcenc_context* context,
    const uint8_t data[16],
    astcenc_block_info* info);

astcenc_error astcenc_get_block_info(
    astcenc_context* context,
    const uint8_t data[16],
    astcenc_block_info* info)
{
    return ((astc::BaseContext*)context)->get_block_info(data, info);
}

extern "C" const char* astcenc_get_error_string_sse(
    astcenc_error status);

const char* astcenc_get_error_string(
    astcenc_error status)
{
    return astcenc_get_error_string_sse(status);
}

namespace astc {
astcenc_error CpuContext::get_block_info(const uint8_t data[16], astcenc_block_info* info)
{
    if (cpu_supports_avx2()) {
        return astcenc_get_block_info_avx2(_Context, data, info);
    } else if (cpu_supports_sse41()) {
        return astcenc_get_block_info_sse(_Context, data, info);
    } else {
        return ASTCENC_ERR_BAD_CPU_ISA;
    }
}
}