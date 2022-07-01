#include "nvtt.hpp"
#include "nvtt/nvtt.h"
#include "nvtt/nvtt_lowlevel.h"
#include "nvtt/nvtt_wrapper.h"
#include <stdio.h>
#if _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif
#include <unordered_map>

#define LOAD_FUNCTION(Name) \
    _##Name = (PFN_##Name)::GetProcAddress((HMODULE)_DLL, #Name)
#define ZERO_FUNCTION(Name) _##Name = nullptr

namespace astc {
struct BlockDim {
    uint8_t x, y;

    bool operator==(const BlockDim& dim) const
    {
        return x == dim.x && y == dim.y;
    }
};

struct BlockDimHasher {
    size_t operator()(const BlockDim& Dim) const
    {
        return ((Dim.x << 4) | Dim.y);
    }
};

static std::unordered_map<BlockDim, NvttFormat, BlockDimHasher> FormatTable = {
    { { 4, 4 }, NVTT_Format_ASTC_LDR_4x4 },
    { { 5, 4 }, NVTT_Format_ASTC_LDR_5x4 },
    { { 5, 5 }, NVTT_Format_ASTC_LDR_5x5 },
    { { 6, 5 }, NVTT_Format_ASTC_LDR_6x5 },
    { { 6, 6 }, NVTT_Format_ASTC_LDR_6x6 },
    { { 8, 5 }, NVTT_Format_ASTC_LDR_8x5 },
    { { 8, 6 }, NVTT_Format_ASTC_LDR_8x6 },
    { { 8, 8 }, NVTT_Format_ASTC_LDR_8x8 },
    { { 10, 5 }, NVTT_Format_ASTC_LDR_10x5 },
    { { 10, 6 }, NVTT_Format_ASTC_LDR_10x6 },
    { { 10, 8 }, NVTT_Format_ASTC_LDR_10x8 },
    { { 10, 10 }, NVTT_Format_ASTC_LDR_10x10 },
    { { 12, 10 }, NVTT_Format_ASTC_LDR_12x10 },
    { { 12, 12 }, NVTT_Format_ASTC_LDR_12x12 },
};

inline NvttChannelOrder to_nvtt_channel_order(bool& recon_normal,
    astcenc_swz swz)
{
    switch (swz) {
    case ASTCENC_SWZ_R:
        return NVTT_ChannelOrder_Red;
    case ASTCENC_SWZ_G:
        return NVTT_ChannelOrder_Green;
    case ASTCENC_SWZ_B:
        return NVTT_ChannelOrder_Blue;
    case ASTCENC_SWZ_A:
        return NVTT_ChannelOrder_Alpha;
    case ASTCENC_SWZ_0:
        return NVTT_ChannelOrder_Zero;
    case ASTCENC_SWZ_1:
        return NVTT_ChannelOrder_One;
    case ASTCENC_SWZ_Z:
        recon_normal = true;
    default:
        return NVTT_ChannelOrder_MaxEnum;
    }
}

GpuContext::GpuContext(const astcenc_config* configp, unsigned int thread_count, astcenc_error& err)
    : CpuContext(configp, thread_count, err)
    , _DLL(nullptr)
    , _Version(0)
    , _SupportGPU(false)
    , _PreferGPU(false)
    , _nvContext(nullptr)
    , _nvCompressionOptions(nullptr)
    , _nvOutputOptions(nullptr)
{
    DECL_FUNCTIONS(ZERO_FUNCTION);
    if (configp->profile == ASTCENC_PRF_LDR) {
    // configp->input_preset >= ASTCENC_PRESET_THOROUGH
        char* prefer_gpu = getenv("ASTCENC_PREFER_GPU");
        char* nvtt_lib = getenv("ASTCENC_NVTT_LIB");
        if (prefer_gpu && !strcmp("1", prefer_gpu)) {
            bool is_loaded = false;
            if (nvtt_lib) {
                is_loaded = loadLib(nvtt_lib);
            }
            else {
                is_loaded = loadLib("nvtt30106.dll");
            }
            if (!is_loaded) {
                ::AddDllDirectory(L"C:\\Program Files\\NVIDIA Corporation\\NVIDIA Texture Tools\\");
                loadLib("nvtt30106.dll");
            }
            _PreferGPU = _SupportGPU;
        }
    }

    if (_PreferGPU) {
        initGpu(configp);
        printf("Use GPU-ASTC\n");
    } else {
        printf("Use CPU-ASTC\n");
    }
}

GpuContext::~GpuContext()
{
    if (_nvCompressionOptions) {
        _nvttDestroyCompressionOptions(_nvCompressionOptions);
        _nvCompressionOptions = nullptr;
    }

    if (_nvContext) {
        _nvttDestroyContext(_nvContext);
        _nvContext = nullptr;
    }

    if (_nvOutputOptions) {
        _nvttDestroyOutputOptions(_nvOutputOptions);
        _nvOutputOptions = nullptr;
    }

    if (_DLL) {
        ::FreeLibrary((HMODULE)_DLL);
        _DLL = nullptr;
    }
}

bool GpuContext::loadLib(const char* DllPath)
{
    DECL_FUNCTIONS(ZERO_FUNCTION);
    _DLL = ::LoadLibraryExA(DllPath, NULL, 
        LOAD_LIBRARY_SEARCH_USER_DIRS | 
        LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | 
        LOAD_LIBRARY_SEARCH_SYSTEM32 |
        LOAD_LIBRARY_SEARCH_APPLICATION_DIR);
    if (_DLL) {
        DECL_FUNCTIONS(LOAD_FUNCTION);
        _Version = _nvttVersion ? _nvttVersion() : 0;
        _SupportGPU = _nvttIsCudaSupported && _nvttIsCudaSupported() == NVTT_True;
        return _Version > 0;
    } else {
        return false;
    }
}

void GpuContext::initGpu(const astcenc_config* configp)
{
    _nvContext = _nvttCreateContext();
    if (_nvttIsCudaSupported() == NVTT_True) {
        if (_nvContext) {
            _nvttSetContextCudaAcceleration(_nvContext, NVTT_True);
        }
    }
    _nvCompressionOptions = _nvttCreateCompressionOptions();
    _Config.block_x = configp->block_x;
    _Config.block_y = configp->block_y;
    _Config.block_z = configp->block_z;

    _Config.rw = configp->cw_r_weight;
    _Config.gw = configp->cw_g_weight;
    _Config.bw = configp->cw_b_weight;
    _Config.aw = configp->cw_a_weight;

    _Config.preset = configp->input_preset;

    _nvttSetCompressionOptionsColorWeights(
        _nvCompressionOptions, configp->cw_r_weight, configp->cw_g_weight,
        configp->cw_b_weight, configp->cw_a_weight);

    BlockDim dim { configp->block_x, configp->block_y };
    if (FormatTable.find(dim) != FormatTable.end()) {
        _nvttSetCompressionOptionsFormat(
            _nvCompressionOptions, FormatTable[dim]);
    }

    _nvOutputOptions = _nvttCreateOutputOptions();
}

astcenc_error GpuContext::compress(astcenc_image* imagep, const astcenc_swizzle* swizzle, uint8_t* data_out, size_t data_len, unsigned int thread_index)
{
    if (_PreferGPU) {
        if (thread_index > 0) {
            return ASTCENC_SUCCESS;
        }
        NvttRefImage refImage;
        unsigned num_tiles = 0;
        NvttCPUInputBuffer* cpuInputBuffer = NULL;
        int rawOutputSize = 0;

        refImage.data = *imagep->data;
        refImage.width = imagep->dim_x;
        refImage.height = imagep->dim_y;
        refImage.depth = 1; // image arrays
        refImage.num_channels = 4;

        bool recon_normal = false;
        refImage.channel_swizzle[0] = to_nvtt_channel_order(recon_normal, swizzle->r);
        refImage.channel_swizzle[1] = to_nvtt_channel_order(recon_normal, swizzle->g);
        refImage.channel_swizzle[2] = to_nvtt_channel_order(recon_normal, swizzle->b);
        refImage.channel_swizzle[3] = to_nvtt_channel_order(recon_normal, swizzle->a);
        refImage.channel_interleave = NVTT_True;

        NvttValueType valueType = NVTT_ValueType_FLOAT32;
        switch (imagep->data_type) {
        case ASTCENC_TYPE_U8:
            valueType = NVTT_ValueType_UINT8;
            break;
        case ASTCENC_TYPE_F16:
            break;
        case ASTCENC_TYPE_F32:
            valueType = NVTT_ValueType_FLOAT32;
            break;
        }

        cpuInputBuffer = _nvttCreateCPUInputBuffer(
            &refImage, // Array of images
            valueType, // Input data type - NVTT 3 Surfaces are floating-point
                       // images
            imagep->dim_z, // Number of images
            _Config.block_x, _Config.block_y, _Config.rw, _Config.gw,
            _Config.bw, _Config.aw, // Error metric channel weights
            nullptr, &num_tiles);

        rawOutputSize = _nvttContextEstimateSizeData(
            _nvContext, refImage.width, refImage.height, refImage.depth, 1,
            _nvCompressionOptions);

        /*
         * UE4
                    case 0:	CompressionMode += TEXT(" -veryfast"); break;
                    case 1:	CompressionMode += TEXT(" -fast"); break;
                    case 2:	CompressionMode += TEXT(" -medium"); break;
                    case 3:	CompressionMode += TEXT(" -thorough"); break;
        */

        NvttQuality quality = NVTT_Quality_Normal;
        switch (_Config.preset) {
        case ASTCENC_PRESET_FASTEST:
            quality = NVTT_Quality_Fastest;
            break;
        case ASTCENC_PRESET_FAST:
            quality = NVTT_Quality_Fastest;
            break;
        case ASTCENC_PRESET_MEDIUM:
            quality = NVTT_Quality_Normal;
            break;
        case ASTCENC_PRESET_THOROUGH:
            quality = NVTT_Quality_Production;
            break;
        case ASTCENC_PRESET_EXHAUSTIVE:
            quality = NVTT_Quality_Highest;
            break;
        }

        if (data_len == rawOutputSize) {
            _nvttEncodeASTCCPU(
                cpuInputBuffer,
                quality, // qualityLevel
                NVTT_True, // hasAlpha, TODO
                data_out, // Output
                _nvttIsCudaSupported(), // Whether to use GPU acceleration
                NVTT_False, // Whether the output resides on the GPU
                nullptr); // Timing context
        }

        _nvttDestroyCPUInputBuffer(cpuInputBuffer);
        return ASTCENC_SUCCESS;
    } else {
        return Super::compress(imagep, swizzle, data_out, data_len, thread_index);
    }
}
astcenc_error GpuContext::compress_reset()
{
    if (_PreferGPU) {
        return ASTCENC_SUCCESS;
    }
    return Super::compress_reset();
}
astcenc_error GpuContext::decompress(const uint8_t* data, size_t data_len, astcenc_image* image_outp, const astcenc_swizzle* swizzle, unsigned int thread_index)
{
    return Super::decompress(data, data_len, image_outp, swizzle, thread_index);
}
astcenc_error GpuContext::decompress_reset()
{
    return Super::decompress_reset();
}
} // namespace astc