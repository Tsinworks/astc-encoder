#pragma once

#include "../Source/astcenc_internal.h"
#include "nvtt/nvtt.h"
#include "nvtt/nvtt_wrapper.h"
#include "universal.hpp"

#define DECL_FUNCTION_PROTO(Name, Ret, ...) typedef Ret (*PFN_##Name)(__VA_ARGS__)
#define DECL_FUNCTION__VAR(Name) PFN_##Name _##Name

#define DECL_FUNCTIONS(Define)                     \
    Define(nvttVersion);                           \
    Define(nvttIsCudaSupported);                   \
    Define(nvttUseCurrentDevice);                  \
    Define(nvttCreateSurface);                     \
    Define(nvttDestroySurface);                    \
    Define(nvttCreateContext);                     \
    Define(nvttDestroyContext);                    \
    Define(nvttSetContextCudaAcceleration);        \
    Define(nvttContextCompress);                   \
    Define(nvttCreateCompressionOptions);          \
    Define(nvttDestroyCompressionOptions);         \
    Define(nvttContextEstimateSize);               \
    Define(nvttResetCompressionOptions);           \
    Define(nvttSetCompressionOptionsFormat);       \
    Define(nvttSetCompressionOptionsQuality);      \
    Define(nvttSetCompressionOptionsColorWeights); \
    Define(nvttSetCompressionOptionsPixelFormat);  \
    Define(nvttSetCompressionOptionsPixelType);    \
    Define(nvttCreateOutputOptions);               \
    Define(nvttDestroyOutputOptions);              \
    Define(nvttResetOutputOptions);                \
    Define(nvttSetOutputOptionsSrgbFlag);          \
    Define(nvttSetOutputOptionsOutputHandler);     \
    Define(nvttSetOutputOptionsErrorHandler);      \
    Define(nvttSetOutputOptionsOutputHeader);      \
    Define(nvttCreateCPUInputBuffer);              \
    Define(nvttDestroyCPUInputBuffer);             \
    Define(nvttContextEstimateSizeData);           \
    Define(nvttEncodeASTCCPU);                     \
    Define(nvttEncodeASTCGPU)

DECL_FUNCTION_PROTO(nvttVersion, unsigned int);
DECL_FUNCTION_PROTO(nvttIsCudaSupported, NvttBoolean);
DECL_FUNCTION_PROTO(nvttUseCurrentDevice, void);

DECL_FUNCTION_PROTO(nvttCreateSurface, NvttSurface*);
DECL_FUNCTION_PROTO(nvttDestroySurface, void, NvttSurface*);
DECL_FUNCTION_PROTO(nvttCreateContext, NvttContext*);
DECL_FUNCTION_PROTO(nvttDestroyContext, void, NvttContext*);
DECL_FUNCTION_PROTO(nvttSetContextCudaAcceleration, void, NvttContext*, NvttBoolean);
DECL_FUNCTION_PROTO(nvttContextCompress, NvttBoolean, NvttContext*, NvttBoolean, const NvttSurface*, int, int, const NvttCompressionOptions*, const NvttOutputOptions*);
DECL_FUNCTION_PROTO(nvttContextEstimateSize, int, const NvttContext*, const NvttSurface*, int, const NvttCompressionOptions*);

DECL_FUNCTION_PROTO(nvttCreateCompressionOptions, NvttCompressionOptions*);
DECL_FUNCTION_PROTO(nvttDestroyCompressionOptions, void, NvttCompressionOptions*);

DECL_FUNCTION_PROTO(nvttResetCompressionOptions, void, NvttCompressionOptions*);
DECL_FUNCTION_PROTO(nvttSetCompressionOptionsFormat, void, NvttCompressionOptions*, NvttFormat);
DECL_FUNCTION_PROTO(nvttSetCompressionOptionsQuality, void, NvttCompressionOptions*, NvttQuality);
DECL_FUNCTION_PROTO(nvttSetCompressionOptionsColorWeights, void, NvttCompressionOptions*, float, float, float, float);
DECL_FUNCTION_PROTO(nvttSetCompressionOptionsPixelFormat, void, NvttCompressionOptions*, unsigned int, unsigned int, unsigned int, unsigned int);
DECL_FUNCTION_PROTO(nvttSetCompressionOptionsPixelType, void, NvttCompressionOptions*, NvttPixelType);

DECL_FUNCTION_PROTO(nvttCreateOutputOptions, NvttOutputOptions*);
DECL_FUNCTION_PROTO(nvttDestroyOutputOptions, void, NvttOutputOptions*);

DECL_FUNCTION_PROTO(nvttResetOutputOptions, void, NvttOutputOptions*);
DECL_FUNCTION_PROTO(nvttSetOutputOptionsSrgbFlag, void, NvttOutputOptions*, NvttBoolean);
DECL_FUNCTION_PROTO(nvttSetOutputOptionsOutputHandler, void, NvttOutputOptions*, nvttBeginImageHandler, nvttOutputHandler, nvttEndImageHandler);
DECL_FUNCTION_PROTO(nvttSetOutputOptionsErrorHandler, void, NvttOutputOptions*, nvttErrorHandler);
DECL_FUNCTION_PROTO(nvttSetOutputOptionsOutputHeader, void, NvttOutputOptions*, NvttBoolean);

DECL_FUNCTION_PROTO(nvttEncodeASTCCPU, void, const NvttCPUInputBuffer*, int, NvttBoolean,
    void*, NvttBoolean, NvttBoolean, NvttTimingContext*);
DECL_FUNCTION_PROTO(nvttEncodeASTCGPU, void, const NvttGPUInputBuffer*, int, NvttBoolean,
    void*, NvttBoolean, NvttTimingContext*);

DECL_FUNCTION_PROTO(nvttCreateCPUInputBuffer, NvttCPUInputBuffer*, const NvttRefImage*, NvttValueType, int, int, int, float, float, float, float, NvttTimingContext*, unsigned*);
DECL_FUNCTION_PROTO(nvttDestroyCPUInputBuffer, void, NvttCPUInputBuffer*);

DECL_FUNCTION_PROTO(nvttContextEstimateSizeData, int, const NvttContext*, int, int, int, int, const NvttCompressionOptions*);

namespace astc {
class GpuContext : public CpuContext {
    using Super = CpuContext;
public:
    GpuContext(
        const astcenc_config* configp,
        unsigned int thread_count, astcenc_error& err);
    ~GpuContext() override;

    astcenc_error compress(
        astcenc_image* imagep,
        const astcenc_swizzle* swizzle,
        uint8_t* data_out,
        size_t data_len,
        unsigned int thread_index)
        override;

    astcenc_error compress_reset() override;

    astcenc_error decompress(const uint8_t* data,
        size_t data_len,
        astcenc_image* image_outp,
        const astcenc_swizzle* swizzle,
        unsigned int thread_index)
        override;

    astcenc_error decompress_reset() override;

private:
    bool loadLib(const char* DllPath);
    void initGpu(const astcenc_config* configp);

    DECL_FUNCTIONS(DECL_FUNCTION__VAR);

    void* _DLL;
    unsigned int _Version;
    bool _SupportGPU;
    bool _PreferGPU;

    struct Config {
        int block_x, block_y, block_z;
        float rw, gw, bw, aw;
        astcenc_preset preset;
    } _Config;

    NvttContext* _nvContext;
    NvttCompressionOptions* _nvCompressionOptions;
    NvttOutputOptions* _nvOutputOptions;
};

}