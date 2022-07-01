#pragma once

#include "astcenc.h"

extern bool cpu_supports_avx2();
extern bool cpu_supports_sse41();

extern "C" astcenc_error astcenc_context_alloc_sse(
    const astcenc_config* configp,
    unsigned int thread_count,
    astcenc_context** context);
extern "C" astcenc_error astcenc_context_alloc_avx2(
    const astcenc_config* configp,
    unsigned int thread_count,
    astcenc_context** context);

extern "C" astcenc_error astcenc_compress_image_sse(
    astcenc_context* ctx,
    astcenc_image* imagep,
    const astcenc_swizzle* swizzle,
    uint8_t* data_out,
    size_t data_len,
    unsigned int thread_index);
extern "C" astcenc_error astcenc_compress_image_avx2(
    astcenc_context* ctx,
    astcenc_image* imagep,
    const astcenc_swizzle* swizzle,
    uint8_t* data_out,
    size_t data_len,
    unsigned int thread_index);

extern "C" astcenc_error astcenc_compress_reset_sse(
    astcenc_context* ctx);
extern "C" astcenc_error astcenc_compress_reset_avx2(
    astcenc_context* ctx);

extern "C" astcenc_error astcenc_decompress_image_sse(
    astcenc_context* ctx,
    const uint8_t* data,
    size_t data_len,
    astcenc_image* image_outp,
    const astcenc_swizzle* swizzle,
    unsigned int thread_index);
extern "C" astcenc_error astcenc_decompress_image_avx2(
    astcenc_context* ctx,
    const uint8_t* data,
    size_t data_len,
    astcenc_image* image_outp,
    const astcenc_swizzle* swizzle,
    unsigned int thread_index);

extern "C" astcenc_error astcenc_decompress_reset_sse(
    astcenc_context* ctx);
extern "C" astcenc_error astcenc_decompress_reset_avx2(
    astcenc_context* ctx);

extern "C" void astcenc_context_free_sse(
    astcenc_context* ctx);
extern "C" void astcenc_context_free_avx2(
    astcenc_context* ctx);

namespace astc {
class BaseContext {
public:
    virtual ~BaseContext() { }

    virtual astcenc_error compress(
        astcenc_image* imagep,
        const astcenc_swizzle* swizzle,
        uint8_t* data_out,
        size_t data_len,
        unsigned int thread_index)
        = 0;

    virtual astcenc_error compress_reset() = 0;

    virtual astcenc_error decompress(const uint8_t* data,
        size_t data_len,
        astcenc_image* image_outp,
        const astcenc_swizzle* swizzle,
        unsigned int thread_index)
        = 0;
    virtual astcenc_error decompress_reset() = 0;

    virtual astcenc_error get_block_info(
        const uint8_t data[16],
        astcenc_block_info* info)
        = 0;
};

class CpuContext : public BaseContext {
public:
    CpuContext(
        const astcenc_config* configp,
        unsigned int thread_count, astcenc_error& err)
        : _Context(nullptr)
        , _UseAvx2(cpu_supports_avx2())
    {
        if (_UseAvx2) {
            err = astcenc_context_alloc_avx2(configp, thread_count, &_Context);
        } else if (cpu_supports_sse41()) {
            err = astcenc_context_alloc_sse(configp, thread_count, &_Context);
        } else {
            err = ASTCENC_ERR_BAD_CPU_ISA;
        }
    }

    ~CpuContext() override
    {
        if (_Context) {
            if (_UseAvx2) {
                astcenc_context_free_avx2(_Context);
            } else if (cpu_supports_sse41()) {
                astcenc_context_free(_Context);
            } else {
            }

            _Context = nullptr;
        }
    }

    astcenc_error compress(
        astcenc_image* imagep,
        const astcenc_swizzle* swizzle,
        uint8_t* data_out,
        size_t data_len,
        unsigned int thread_index)
        override
    {
        if (_Context) {
            if (_UseAvx2) {
                return astcenc_compress_image_avx2(_Context, imagep, swizzle, data_out, data_len, thread_index);
            } else if (cpu_supports_sse41()) {
                return astcenc_compress_image_sse(_Context, imagep, swizzle, data_out, data_len, thread_index);
            } else {
                return ASTCENC_ERR_BAD_CPU_ISA;
            }
        }
        return ASTCENC_ERR_BAD_CONTEXT;
    }

    virtual astcenc_error compress_reset() override
    {
        if (_Context) {
            if (_UseAvx2) {
                return astcenc_compress_reset_avx2(_Context);
            } else if (cpu_supports_sse41()) {
                return astcenc_compress_reset_sse(_Context);
            } else {
                return ASTCENC_ERR_BAD_CPU_ISA;
            }
        }
        return ASTCENC_ERR_BAD_CONTEXT;
    }

    virtual astcenc_error decompress(const uint8_t* data,
        size_t data_len,
        astcenc_image* image_outp,
        const astcenc_swizzle* swizzle,
        unsigned int thread_index)
        override
    {
        if (_Context) {
            if (_UseAvx2) {
                return astcenc_decompress_image_avx2(_Context, data, data_len, image_outp, swizzle, thread_index);
            } else if (cpu_supports_sse41()) {
                return astcenc_decompress_image_sse(_Context, data, data_len, image_outp, swizzle, thread_index);
            } else {
                return ASTCENC_ERR_BAD_CPU_ISA;
            }
        }

        return ASTCENC_ERR_BAD_CONTEXT;
    }

    virtual astcenc_error decompress_reset() override
    {
        if (_Context) {
            if (_UseAvx2) {
                return astcenc_decompress_reset_avx2(_Context);
            } else if (cpu_supports_sse41()) {
                return astcenc_decompress_reset_sse(_Context);
            } else {
                return ASTCENC_ERR_BAD_CPU_ISA;
            }
        }

        return ASTCENC_ERR_BAD_CONTEXT;
    }

    astcenc_error get_block_info(
        const uint8_t data[16],
        astcenc_block_info* info) override;

private:
    astcenc_context* _Context;
    bool _UseAvx2;
};
} // namespace astc