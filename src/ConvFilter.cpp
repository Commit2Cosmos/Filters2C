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
        results[c] = convolveChannel(image, c);
    }
    return results;
}


float ConvFilter::getTheoreticalMax(float filterMax) const {
    return sqrt(2.f * filterMax * filterMax);
}


// ── normalise ────────────────────────────────────────────────────────────────

/**
 * @brief Scales float results so that the global maximum maps to 255.
 *
 * Call this on the output of computeResults() before writeResults()
 * to ensure large-weight kernels (e.g. Scharr) don't saturate.
 */
void ConvFilter::normalise(std::vector<std::vector<float>>& results, float theoreticalMax) const {
    float scale = 255.0f / theoreticalMax;
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
std::vector<float> ConvFilter::convolveChannel(const Image& image, int ch) const {
    const int w = image.getWidth();
    const int h = image.getHeight();
    const int kh = static_cast<int>(kernels_[0].size());
    const int kw = static_cast<int>(kernels_[0][0].size());
    const int halfH = kh / 2;
    const int halfW = kw / 2;

    const bool isFused = kernels_.size() > 1;
    std::vector<float> result(w * h, 0.0f);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float v1 = 0.0f;
            float v2 = 0.0f;

            for (int ky = 0; ky < kh; ++ky) {
                for (int kx = 0; kx < kw; ++kx) {
                    int srcX = x + kx - halfW;
                    int srcY = y + ky - halfH;
                    float pixel = static_cast<float>(image.getPixel(srcX, srcY, ch));

                    v1 += pixel * kernels_[0][ky][kx];
                    if (isFused) {
                        v2 += pixel * kernels_[1][ky][kx];
                    }
                }
            }
            // Apply magnitude calculation if dual-kernel, otherwise return single sum
            result[y * w + x] = isFused ? std::sqrt(v1 * v1 + v2 * v2) : v1;
        }
    }

    return result;
}