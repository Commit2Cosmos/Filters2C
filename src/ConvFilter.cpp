// ConvFilter.cpp

#include "ConvFilter.h"
#include <vector>
#include <algorithm>
#include <cmath>


// ── Constructors ──────────────────────────────────────────────────────────────

/**
 * @brief Single-kernel constructor for filters like Sharpen and Emboss.
 * @param kernel The 2D convolution kernel to apply.
 */
ConvFilter::ConvFilter(Kernel kernel) {}

/**
 * @brief Dual-kernel constructor for gradient filters (Sobel, Prewitt, etc.).
 * @param kernelX Kernel for detecting horizontal edges (Gx).
 * @param kernelY Kernel for detecting vertical edges (Gy).
 */
ConvFilter::ConvFilter(Kernel kernelX, Kernel kernelY) {}


// ── apply ─────────────────────────────────────────────────────────────────────

/**
 * @brief Applies the convolution filter to every channel of the image.
 *
 * Writes into a separate output buffer before copying back to the image,
 * to prevent already-modified pixels from influencing later convolution reads.
 *
 * For single-kernel filters: directly uses the convolution result.
 * For dual-kernel filters: calls combineResponses() on Gx and Gy results.
 *
 * @param image The image to filter, modified in-place.
 */
void ConvFilter::apply(Image& image) {}


// ── convolveChannel ───────────────────────────────────────────────────────────

/**
 * @brief Convolves a single channel of the image with the given kernel.
 *
 * Iterates over every pixel (x, y). For each position, centres the kernel
 * on that pixel and computes the weighted sum of the neighbourhood.
 * Out-of-bounds pixel reads are delegated to Image::getPixel, which applies
 * extend (clamp) padding internally.
 *
 * @param image  The source image (read-only).
 * @param ch     The channel index to convolve (e.g. 0=R, 1=G, 2=B).
 * @param kernel The 2D kernel to apply.
 * @return       Flat vector of float responses, size = width * height.
 */
std::vector<float> ConvFilter::convolveChannel(
    const Image& image, int ch, const Kernel& kernel) const {}


// ── combineResponses ──────────────────────────────────────────────────────────

/**
 * @brief Combines Gx and Gy response maps into a single output.
 *
 * Default implementation computes gradient magnitude: sqrt(Gx² + Gy²).
 * Subclasses may override — e.g. EmbossFilter shifts by +128 instead.
 *
 * @param respX Float response map from the Gx kernel.
 * @param respY Float response map from the Gy kernel.
 * @return      Combined float response map, same size as inputs.
 */
std::vector<float> ConvFilter::combineResponses(
    const std::vector<float>& respX,
    const std::vector<float>& respY) const {}
