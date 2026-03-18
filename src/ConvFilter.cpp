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
ConvFilter::ConvFilter(Kernel kernel) : kernels_({std::move(kernel)}) {}

/**
 * @brief Dual-kernel constructor for gradient filters.
 * @param kernelX Kernel for detecting horizontal edges (Gx).
 * @param kernelY Kernel for detecting vertical edges (Gy).
 */
ConvFilter::ConvFilter(Kernel kernelX, Kernel kernelY) : kernels_({std::move(kernelX), std::move(kernelY)}) {}


// ── apply ─────────────────────────────────────────────────────────────────────

/**
 * @brief Applies the convolution filter to every channel of the image.
 *
 * Writes into a separate output buffer before copying back to the image,
 * to prevent already-modified pixels from influencing later convolution reads.
 *
 * @param image The image to filter, modified in-place.
 */
void ConvFilter::apply(Image& image) {
    auto results = computeResults(image);
    writeResults(image, results);
}


// ── computeResults ───────────────────────────────────────────────────────────

/**
 * @brief Runs the convolution on every channel and returns float results.
 */
std::vector<std::vector<float>> ConvFilter::computeResults(const Image& image) const {
    const int ch = image.getChannels();
    std::vector<std::vector<float>> results(ch);

    for (int c = 0; c < ch; ++c) {
        if (kernels_.size() == 1u) {
            results[c] = convolveChannel(image, c, kernels_[0]);
        } else {
            // Fused single-pass: compute Gx and Gy simultaneously
            // instead of two separate full-image traversals, halving memory reads
            results[c] = convolveChannelFused(image, c, kernels_[0], kernels_[1]);
        }
    }
    return results;
}


// ── normalise ────────────────────────────────────────────────────────────────

/**
 * @brief Scales float results so that the global maximum maps to 255.
 *
 * Call this on the output of computeResults() before writeResults()
 * to ensure large-weight kernels (e.g. Scharr) don't saturate.
 */
void ConvFilter::normalise(std::vector<std::vector<float>>& results) const {
    float maxVal = 0.0f;
    for (auto& ch : results)
        for (auto v : ch)
            if (v > maxVal) maxVal = v;

    if (maxVal <= 0.0f) return;

    float scale = 255.0f / maxVal;
    for (auto& ch : results)
        for (auto& v : ch)
            v *= scale;
}


// ── writeResults ─────────────────────────────────────────────────────────────

/**
 * @brief Clamps float results to [0, 255] and writes them back to the image.
 */
void ConvFilter::writeResults(Image& image,
                              const std::vector<std::vector<float>>& results) const {
    const int w = image.getWidth();
    const int h = image.getHeight();
    const int ch = image.getChannels();

    for (int c = 0; c < ch; ++c)
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                float val = results[c][y * w + x];
                val = std::max(0.0f, std::min(255.0f, val));
                image.setPixel(x, y, c, static_cast<unsigned char>(val));
            }
}


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
    const Image& image, int ch, const Kernel& kernel) const {
    const int w = image.getWidth();
    const int h = image.getHeight();
    const int kh = static_cast<int>(kernel.size());
    const int kw = static_cast<int>(kernel[0].size());
    const int halfH = kh / 2;
    const int halfW = kw / 2;

    std::vector<float> result(w * h, 0.0f);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float sum = 0.0f;
            for (int ky = 0; ky < kh; ++ky) {
                for (int kx = 0; kx < kw; ++kx) {
                    int srcX = x + kx - halfW;
                    int srcY = y + ky - halfH;
                    // getPixel handles extend (clamp) padding for out-of-bounds
                    float pixel = static_cast<float>(image.getPixel(srcX, srcY, ch));
                    sum += pixel * kernel[ky][kx];
                }
            }
            result[y * w + x] = sum;
        }
    }
    return result;
}


// ── convolveChannelFused ──────────────────────────────────────────────────────

/**
 * @brief Fused single-pass dual-kernel convolution for gradient filters.
 *
 * Computes Gx and Gy simultaneously in a single loop over the image, then
 * returns sqrt(Gx² + Gy²) directly. Compared to calling convolveChannel()
 * twice, this halves the number of pixel reads and eliminates two intermediate
 * float vectors, giving roughly 2× speedup on large images.
 *
 * @param image  The source image (read-only, must be single-channel).
 * @param ch     The channel index to convolve.
 * @param kX     Horizontal gradient kernel (Gx).
 * @param kY     Vertical gradient kernel (Gy).
 * @return       Flat vector of gradient magnitudes, size = width * height.
 */
std::vector<float> ConvFilter::convolveChannelFused(
    const Image& image, int ch,
    const Kernel& kX, const Kernel& kY) const {
    const int w = image.getWidth();
    const int h = image.getHeight();
    const int kh = static_cast<int>(kX.size());
    const int kw = static_cast<int>(kX[0].size());
    const int halfH = kh / 2;
    const int halfW = kw / 2;

    std::vector<float> result(w * h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float gx = 0.0f, gy = 0.0f;
            for (int ky = 0; ky < kh; ++ky) {
                for (int kx = 0; kx < kw; ++kx) {
                    float pixel = static_cast<float>(
                        image.getPixel(x + kx - halfW, y + ky - halfH, ch));
                    gx += pixel * kX[ky][kx];
                    gy += pixel * kY[ky][kx];
                }
            }
            result[y * w + x] = std::sqrt(gx * gx + gy * gy);
        }
    }
    return result;
}
