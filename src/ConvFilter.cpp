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
 * @brief Dual-kernel constructor for gradient filters (Sobel, Prewitt, etc.).
 * @param kernelX Kernel for detecting horizontal edges (Gx).
 * @param kernelY Kernel for detecting vertical edges (Gy).
 */
ConvFilter::ConvFilter(Kernel kernelX, Kernel kernelY) {
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
 * For single-kernel filters: directly uses the convolution result.
 * For dual-kernel filters: calls combineResponses() on Gx and Gy results.
 *
 * @param image The image to filter, modified in-place.
 */
void ConvFilter::apply(Image& image) {
    const int w  = image.getWidth();
    const int h  = image.getHeight();
    const int ch = image.getChannels();

    // Compute all channels first, then write back
    // (prevents earlier channel writes from corrupting later channel reads)
    std::vector<std::vector<float>> results(ch);

    for (int c = 0; c < ch; ++c) {
        if (kernels_.size() == 1) {
            results[c] = convolveChannel(image, c, kernels_[0]);
        } else {
            std::vector<float> respX = convolveChannel(image, c, kernels_[0]);
            std::vector<float> respY = convolveChannel(image, c, kernels_[1]);
            results[c] = combineResponses(respX, respY);
        }
    }

    // Write back all channels
    for (int c = 0; c < ch; ++c) {
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                float val = results[c][y * w + x];
                val = std::max(0.0f, std::min(255.0f, val));
                image.setPixel(x, y, c, static_cast<unsigned char>(val));
            }
        }
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
    const std::vector<float>& respY) const {
    std::vector<float> result(respX.size());
    for (size_t i = 0; i < respX.size(); ++i) {
        result[i] = std::sqrt(respX[i] * respX[i] + respY[i] * respY[i]);
    }
    return result;
}
