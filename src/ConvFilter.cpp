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
ConvFilter::ConvFilter(Kernel kernel) {
    kernels_.push_back(std::move(kernel));
}

/**
 * @brief Dual-kernel constructor for gradient filters.
 * @param kernelX Kernel for detecting horizontal edges (Gx).
 * @param kernelY Kernel for detecting vertical edges (Gy).
 */
ConvFilter::ConvFilter(Kernel kernelX, Kernel kernelY) {
    kernels_.reserve(2); // Optional: prevents reallocation
    kernels_.push_back(std::move(kernelX));
    kernels_.push_back(std::move(kernelY));
}


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
void ConvFilter::writeResults(Image& image, const std::vector<std::vector<float>>& results) const {
    const int ch = image.getChannels();
    const int totalPixels = image.getWidth() * image.getHeight();
    
    // Call the new non-const getter to get a writable pointer
    unsigned char* dst = image.getDataUnsafe(); 

    // Outer loop: Iterate over pixels
    for (int i = 0; i < totalPixels; ++i) {
        
        // Inner loop: Iterate over channels sequentially
        for (int c = 0; c < ch; ++c) {
            float val = std::clamp(results[c][i], 0.0f, 255.0f);
            
            // Write sequentially and increment the pointer
            *dst++ = static_cast<unsigned char>(val);
        }
    }
}


std::vector<float> ConvFilter::convolveChannel(const Image& image, int ch) const {
    const int w = image.getWidth();
    const int h = image.getHeight();
    const int kh = kernels_[0].height;
    const int kw = kernels_[0].width;
    const int halfH = kh / 2;
    const int halfW = kw / 2;
    const bool isFused = kernels_.size() > 1;
    
    std::vector<float> result(w * h, 0.0f);

    // Helper lambdas to process a single pixel
    auto computePixelSafe = [&](int x, int y) {
        float v1 = 0.0f, v2 = 0.0f;
        for (int ky = 0; ky < kh; ++ky) {
            for (int kx = 0; kx < kw; ++kx) {
                float pixel = static_cast<float>(image.getPixelSimple(x + kx - halfW, y + ky - halfH, ch));
                int kIndex = ky * kw + kx;
                v1 += pixel * kernels_[0].data[kIndex];
                if (isFused) v2 += pixel * kernels_[1].data[kIndex];
            }
        }
        return isFused ? std::sqrt(v1 * v1 + v2 * v2) : v1;
    };

    auto computePixelEdge = [&](int x, int y) {
        float v1 = 0.0f, v2 = 0.0f;
        for (int ky = 0; ky < kh; ++ky) {
            for (int kx = 0; kx < kw; ++kx) {
                float pixel = static_cast<float>(image.getPixel(x + kx - halfW, y + ky - halfH, ch));
                int kIndex = ky * kw + kx;
                v1 += pixel * kernels_[0].data[kIndex];
                if (isFused) v2 += pixel * kernels_[1].data[kIndex];
            }
        }
        return isFused ? std::sqrt(v1 * v1 + v2 * v2) : v1;
    };

    // Top border -> slow path
    for (int y = 0; y < halfH; ++y) {
        for (int x = 0; x < w; ++x) {
            result[y * w + x] = computePixelEdge(x, y);
        }
    }

    // Middle section
    for (int y = halfH; y < h - halfH; ++y) {
        // Left border -> slow path
        for (int x = 0; x < halfW; ++x) {
            result[y * w + x] = computePixelEdge(x, y);
        }
        
        // Inner section -> fast path
        for (int x = halfW; x < w - halfW; ++x) {
            result[y * w + x] = computePixelSafe(x, y);
        }
        
        // Right border -> slow path
        for (int x = w - halfW; x < w; ++x) {
            result[y * w + x] = computePixelEdge(x, y);
        }
    }

    // Bottom border -> slow path
    for (int y = h - halfH; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            result[y * w + x] = computePixelEdge(x, y);
        }
    }

    return result;
}
