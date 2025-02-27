//#define ASTCENC_DYNAMIC_LIBRARY

#define ASTCENC_AVX 2

#define quant_and_xfer_tables quant_and_xfer_tables_avx2
#define color_quant_tables color_quant_tables_avx2
#define color_unquant_tables color_unquant_tables_avx2
#define quant_mode_table quant_mode_table_avx2

#define astcenc_config_init astcenc_config_init_avx2
#define astcenc_context_alloc astcenc_context_alloc_avx2
#define astcenc_compress_image astcenc_compress_image_avx2
#define astcenc_compress_reset astcenc_compress_reset_avx2
#define astcenc_decompress_image astcenc_decompress_image_avx2
#define astcenc_decompress_reset astcenc_decompress_reset_avx2
#define astcenc_context_free astcenc_context_free_avx2
#define astcenc_get_block_info astcenc_get_block_info_avx2
#define astcenc_get_error_string astcenc_get_error_string_avx2

//#define rand_init rand_init_avx2
#define float_to_sf16 float_to_sf16_avx2
#define sf16_to_float sf16_to_float_avx2
#define init_block_size_descriptor init_block_size_descriptor_avx2
#define init_partition_tables init_partition_tables_avx2
#define get_2d_percentile_table get_2d_percentile_table_avx2
#define is_legal_2d_block_size is_legal_2d_block_size_avx2
#define is_legal_3d_block_size is_legal_3d_block_size_avx2
#define get_ise_sequence_bitcount get_ise_sequence_bitcount_avx2
#define decode_ise decode_ise_avx2
#define encode_ise encode_ise_avx2

#define compute_avgs_and_dirs_2_comp compute_avgs_and_dirs_2_comp_avx2
#define compute_avgs_and_dirs_3_comp compute_avgs_and_dirs_3_comp_avx2
#define compute_avgs_and_dirs_3_comp_rgb compute_avgs_and_dirs_3_comp_rgb_avx2

#define compute_avgs_and_dirs_4_comp compute_avgs_and_dirs_4_comp_avx2
#define compute_error_squared_rgb compute_error_squared_rgb_avx2
#define compute_error_squared_rgba compute_error_squared_rgba_avx2
#define find_best_partition_candidates find_best_partition_candidates_avx2
#define init_compute_averages init_compute_averages_avx2
#define compute_averages compute_averages_avx2
#define load_image_block load_image_block_avx2
#define load_image_block_fast_ldr load_image_block_fast_ldr_avx2
#define store_image_block store_image_block_avx2
#define compute_ideal_colors_and_weights_1plane compute_ideal_colors_and_weights_1plane_avx2

#define compute_ideal_colors_and_weights_2planes compute_ideal_colors_and_weights_2planes_avx2
#define compute_ideal_weights_for_decimation compute_ideal_weights_for_decimation_avx2
#define compute_quantized_weights_for_decimation compute_quantized_weights_for_decimation_avx2
#define compute_error_of_weight_set_1plane compute_error_of_weight_set_1plane_avx2
#define compute_error_of_weight_set_2planes compute_error_of_weight_set_2planes_avx2
#define pack_color_endpoints pack_color_endpoints_avx2
#define unpack_color_endpoints unpack_color_endpoints_avx2
#define unpack_weights unpack_weights_avx2
#define compute_ideal_endpoint_formats compute_ideal_endpoint_formats_avx2
#define recompute_ideal_colors_1plane recompute_ideal_colors_1plane_avx2
#define recompute_ideal_colors_2planes recompute_ideal_colors_2planes_avx2

#define prepare_angular_tables prepare_angular_tables_avx2
#define compute_angular_endpoints_1plane compute_angular_endpoints_1plane_avx2
#define compute_angular_endpoints_2planes compute_angular_endpoints_2planes_avx2
#define compress_block compress_block_avx2
#define decompress_symbolic_block decompress_symbolic_block_avx2
#define compute_symbolic_block_difference_1plane compute_symbolic_block_difference_1plane_avx2
#define compute_symbolic_block_difference_2plane compute_symbolic_block_difference_2plane_avx2
#define compute_symbolic_block_difference_1plane_1partition compute_symbolic_block_difference_1plane_1partition_avx2
#define symbolic_to_physical symbolic_to_physical_avx2
#define physical_to_symbolic physical_to_symbolic_avx2

#include "astcenc_averages_and_directions.cpp"
#include "astcenc_block_sizes.cpp"
#include "astcenc_color_quantize.cpp"

#define unquant_color astcenc_unquant_color
#include "astcenc_color_unquantize.cpp"
#undef unquant_color

#include "astcenc_compress_symbolic.cpp"
#include "astcenc_compute_variance.cpp"
#include "astcenc_decompress_symbolic.cpp"
#include "astcenc_diagnostic_trace.cpp"
#include "astcenc_entry.cpp"
#include "astcenc_find_best_partitioning.cpp"
#include "astcenc_ideal_endpoints_and_weights.cpp"
#include "astcenc_image.cpp"
#include "astcenc_integer_sequence.cpp"
//#include "astcenc_mathlib.cpp"
#include "astcenc_mathlib_softfloat.cpp"
#include "astcenc_partition_tables.cpp"
#include "astcenc_percentile_tables.cpp"
#include "astcenc_pick_best_endpoint_format.cpp"
#include "astcenc_quantization.cpp"

#define write_bits astcenc_write_bits
#define read_bits astcenc_read_bits
#include "astcenc_symbolic_physical.cpp"
#undef read_bits
#undef write_bits

#include "astcenc_weight_align.cpp"
#include "astcenc_weight_quant_xfer_tables.cpp"