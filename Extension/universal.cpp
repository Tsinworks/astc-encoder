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

#define ASTCENC_DYNAMIC_LIBRARY

#include "astcenc_mathlib.cpp"
#include "astcenc_platform_isa_detection.cpp"
#include "astcenc.h"

extern "C" astcenc_error astcenc_config_init_sse(
	astcenc_profile profile,
	unsigned int block_x,
	unsigned int block_y,
	unsigned int block_z,
	float quality,
	unsigned int flags,
	astcenc_config* configp
);

extern "C" astcenc_error astcenc_config_init_avx2(
	astcenc_profile profile,
	unsigned int block_x,
	unsigned int block_y,
	unsigned int block_z,
	float quality,
	unsigned int flags,
	astcenc_config* configp
);

astcenc_error astcenc_config_init(
	astcenc_profile profile,
	unsigned int block_x,
	unsigned int block_y,
	unsigned int block_z,
	float quality,
	unsigned int flags,
	astcenc_config* configp
) {
	if (cpu_supports_avx2()) {
		return astcenc_config_init_avx2(profile, block_x, block_y, block_z, quality, flags, configp);
	}
	else if (cpu_supports_sse41()) {
		return astcenc_config_init_sse(profile, block_x, block_y, block_z, quality, flags, configp);
	}
	return ASTCENC_SUCCESS;
}

extern "C" astcenc_error astcenc_context_alloc_sse(
	const astcenc_config* configp,
	unsigned int thread_count,
	astcenc_context** context
);

extern "C" astcenc_error astcenc_context_alloc_avx2(
	const astcenc_config* configp,
	unsigned int thread_count,
	astcenc_context** context
);

/* See header for documentation. */
astcenc_error astcenc_context_alloc(
	const astcenc_config* configp,
	unsigned int thread_count,
	astcenc_context** context
) {
	if (cpu_supports_avx2()) {
		return astcenc_context_alloc_avx2(configp, thread_count, context);
	}
	else if (cpu_supports_sse41()) {
		return astcenc_context_alloc_sse(configp, thread_count, context);
	}
	return ASTCENC_ERR_BAD_CPU_ISA;
}

extern "C" astcenc_error astcenc_compress_image_sse(
	astcenc_context* ctx,
	astcenc_image* imagep,
	const astcenc_swizzle* swizzle,
	uint8_t* data_out,
	size_t data_len,
	unsigned int thread_index
);

extern "C" astcenc_error astcenc_compress_image_avx2(
	astcenc_context* ctx,
	astcenc_image* imagep,
	const astcenc_swizzle* swizzle,
	uint8_t* data_out,
	size_t data_len,
	unsigned int thread_index
);

astcenc_error astcenc_compress_image(
	astcenc_context* ctx,
	astcenc_image* imagep,
	const astcenc_swizzle* swizzle,
	uint8_t* data_out,
	size_t data_len,
	unsigned int thread_index
) {
	if (cpu_supports_avx2()) {
		return astcenc_compress_image_avx2(ctx, imagep, swizzle, data_out, data_len, thread_index);
	}
	else if (cpu_supports_sse41()) {
		return astcenc_compress_image_sse(ctx, imagep, swizzle, data_out, data_len, thread_index);
	}
	return ASTCENC_ERR_BAD_CPU_ISA;
}

extern "C" astcenc_error astcenc_compress_reset_sse(
	astcenc_context* ctx
);
extern "C" astcenc_error astcenc_compress_reset_avx2(
	astcenc_context* ctx
);

astcenc_error astcenc_compress_reset(
	astcenc_context* ctx
) {
	if (cpu_supports_avx2()) {
		return astcenc_compress_reset_avx2(ctx);
	}
	else if (cpu_supports_sse41()) {
		return astcenc_compress_reset_sse(ctx);
	}
	return ASTCENC_ERR_BAD_CPU_ISA;
}

extern "C" astcenc_error astcenc_decompress_image_sse(
	astcenc_context* ctx,
	const uint8_t* data,
	size_t data_len,
	astcenc_image* image_outp,
	const astcenc_swizzle* swizzle,
	unsigned int thread_index
);

extern "C" astcenc_error astcenc_decompress_image_avx2(
	astcenc_context* ctx,
	const uint8_t* data,
	size_t data_len,
	astcenc_image* image_outp,
	const astcenc_swizzle* swizzle,
	unsigned int thread_index
);
astcenc_error astcenc_decompress_image(
	astcenc_context* ctx,
	const uint8_t* data,
	size_t data_len,
	astcenc_image* image_outp,
	const astcenc_swizzle* swizzle,
	unsigned int thread_index
) {
	if (cpu_supports_avx2()) {
		return astcenc_decompress_image_avx2(ctx, data, data_len, image_outp, swizzle, thread_index);
	}
	else if (cpu_supports_sse41()) {
		return astcenc_decompress_image_sse(ctx, data, data_len, image_outp, swizzle, thread_index);
	}
	return ASTCENC_ERR_BAD_CPU_ISA;
}

extern "C" astcenc_error astcenc_decompress_reset_sse(
	astcenc_context* ctx
);
extern "C" astcenc_error astcenc_decompress_reset_avx2(
	astcenc_context* ctx
);
astcenc_error astcenc_decompress_reset(
	astcenc_context* ctx
) {
	if (cpu_supports_avx2()) {
		return astcenc_decompress_reset_avx2(ctx);
	}
	else if (cpu_supports_sse41()) {
		return astcenc_decompress_reset_sse(ctx);
	}
	return ASTCENC_ERR_BAD_CPU_ISA;
}

extern "C" void astcenc_context_free_sse(
	astcenc_context* ctx
);
extern "C" void astcenc_context_free_avx2(
	astcenc_context* ctx
);

void astcenc_context_free(
	astcenc_context* ctx
) {
	if (cpu_supports_avx2()) {
		astcenc_context_free_avx2(ctx);
	}
	else if (cpu_supports_sse41()) {
		astcenc_context_free_sse(ctx);
	}
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
	astcenc_block_info* info) {
	if (cpu_supports_avx2()) {
		return astcenc_get_block_info_avx2(context, data, info);
	}
	else if (cpu_supports_sse41()) {
		return astcenc_get_block_info_sse(context, data, info);
	}
	return ASTCENC_ERR_BAD_CPU_ISA;
}

extern "C" const char* astcenc_get_error_string_sse(
	astcenc_error status);

const char* astcenc_get_error_string(
	astcenc_error status)
{
	return astcenc_get_error_string_sse(status);
}

#if 0
extern "C" float sf16_to_float_sse(uint16_t p);
extern "C" float sf16_to_float_avx2(uint16_t p);
ASTCENC_PUBLIC float sf16_to_float(uint16_t p) {
	if (cpu_supports_sse41()) {
		return sf16_to_float_sse(p);
	}
	if (cpu_supports_avx2()) {
		return sf16_to_float_avx2(p);
	}
	return 0.f;
}
#endif