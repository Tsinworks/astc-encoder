# 4.x series change log

This page summarizes the major functional and performance changes in each
release of the 4.x series.

All performance data on this page is measured on an Intel Core i5-9600K
clocked at 4.2 GHz, running `astcenc` using AVX2 and 6 threads.

<!-- ---------------------------------------------------------------------- -->
## 4.0.0

**Status:** In development

The 4.0.0 release introduces some major performance enhancement, and a number
of larger changes to the heuristics used in the codec to find a more effective
cost:quality trade off.

* **General:**
  * **Change:** The `-array` option for specifying the number of image planes
    for ASTC 3D volumetric block compression been renamed to `-zdim`.
  * **Change:** The build root package directory is now `bin` instead of
    `astcenc`, allowing the CMake install step to write binaries into
    `/usr/local/bin` if the user wishes to do so.
  * **Feature:** A new `-ssw` option for specifying the shader sampling swizzle
    has been added as convenience alternative to the `-cw` option. This is
    needed to correct error weighting during compression if not all components
    are read in the shader. For example, to extract and compress two components
    from an RGBA input image, weighting the two components equally when
    sampling through .ra in the shader, use `-esw ggga -ssw ra`. In this
    example `-ssw ra` is equivalent to the alternative `-cw 1 0 0 1` encoding.
  * **Feature:** The `-a` alpha weighting option has been re-enabled in the
    backend, and now again applies alpha scaling to the RGB error metrics when
    encoding. This is based on the maximum alpha in each block, not the
    individual texel alpha values used in the earlier implementation.
  * **Feature:** The command line tool now has `-repeats <count>` for testing,
    which will iterate around compression and decompression `count` times.
    Reported performance metrics also now separate compression and
    decompression scores.
  * **Feature:** The core codec is now warning clean up to /W4 for both MSVC
    `cl.exe` and `clangcl.exe` compilers.
  * **Feature:** The core codec now supports arm64 for both MSVC `cl.exe` and
    `clangcl.exe` compilers.
  * **Feature:** `NO_INVARIANCE` builds with AVX2 will enable `-mfma` and
    `-ffp-contract=fast` when using Clang or GCC. This reduces image quality
    by up to 0.2dB (normally much less), but improves performance by 10-15%.
  * **Optimization:** Angular endpoint min/max weight selection is restricted
    to weight `QUANT_11` or lower. Higher quantization levels assume default
    0-1 range, which is less accurate but much faster.
  * **Optimization:** Maximum weight quantization for later trials is selected
    based on the weight quantization of the best encoding from the 1 plane 1
    partition trial. This significantly reduces the search space for the later
    trials with more planes or partitions.
  * **Optimization:** Small data tables now use in-register SIMD permutes
    rather than gathers (AVX2) or unrolled scalar lookups (SSE/NEON). This can
    be a significant optimization for paths that are load unit limited.
  * **Optimization:** Decompressed image block writes in the decompressor now
    use a vectorized approach to writing each row of texels in the block,
    including to ability to exploit masked stores if the target supports them.
  * **Optimization:** Weight scrambling has been moved into the physical layer;
    the rest of the codec now uses linear order weights.
  * **Optimization:** Weight packing has been moved into the physical layer;
    the rest of the codec now uses unpacked weights in the 0-64 range.
  * **Optimization:** Consistently vectorize the creation of unquantized weight
    grids when they are needed.
  * **Optimization:** Remove redundant per-decimation mode copies of endpoint
    and weight structures, which were really read-only duplicates.
  * **Optimization:** Early-out the same endpoint mode color calculation if it
    cannot be applied.
  * **Optimization:** Numerous type size reductions applied to arrays to reduce
    both context working buffer size usage and stack usage.
- - -

_Copyright © 2022, Arm Limited and contributors. All rights reserved._
