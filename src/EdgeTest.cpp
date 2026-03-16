/**
 * Standalone test for edge detection filters.
 * Contains a minimal Image implementation — does NOT touch the project's Image.cpp.
 *
 * Compile:
 *   g++ -std=c++20 -O2 -I../src test_edge.cpp ../src/ConvFilter.cpp \
 *       ../src/SobelFilter.cpp ../src/PrewittFilter.cpp \
 *       ../src/ScharrFilter.cpp ../src/RobertsCrossFilter.cpp \
 *       -o test_edge -lm
 *
 * Run:
 *   ./test_edge ../Images/gracehopper.png
 */

// stb implementations — only defined once here
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Image.h"
#include "SobelFilter.h"
#include "PrewittFilter.h"
#include "ScharrFilter.h"
#include "RobertsCrossFilter.h"

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>

// ═══════════════════════════════════════════════════════════════════
// Minimal Image implementation (just enough to test ConvFilter)
// ═══════════════════════════════════════════════════════════════════

Image::Image() : width(0), height(0), channels(0) {}

Image::Image(int w, int h, int c)
    : width(w), height(h), channels(c), data(w * h * c, 0) {}

Image::Image(const std::string& filepath) : width(0), height(0), channels(0) {
    load(filepath);
}

bool Image::load(const std::string& filepath) {
    int w, h, c;
    unsigned char* raw = stbi_load(filepath.c_str(), &w, &h, &c, 0);
    if (!raw) return false;
    width = w; height = h; channels = c;
    data.assign(raw, raw + w * h * c);
    stbi_image_free(raw);
    return true;
}

bool Image::save(const std::string& filepath) const {
    return stbi_write_png(filepath.c_str(), width, height, channels,
                          data.data(), width * channels) != 0;
}

unsigned char Image::getPixel(int x, int y, int channel) const {
    // Extend (clamp) padding
    x = std::max(0, std::min(x, width - 1));
    y = std::max(0, std::min(y, height - 1));
    return data[(y * width + x) * channels + channel];
}

void Image::setPixel(int x, int y, int channel, unsigned char value) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    data[(y * width + x) * channels + channel] = value;
}

void Image::toGrayscale() {
    if (channels == 1) return;
    std::vector<unsigned char> grey(width * height);
    for (int i = 0; i < width * height; ++i) {
        int r = data[i * channels + 0];
        int g = data[i * channels + 1];
        int b = data[i * channels + 2];
        grey[i] = static_cast<unsigned char>(0.2126 * r + 0.7152 * g + 0.0722 * b);
    }
    data = std::move(grey);
    channels = 1;
}

// Stubs — not needed for this test but required by linker
std::vector<float> Image::toHSV()  const { return {}; }
std::vector<float> Image::toHSL()  const { return {}; }
void Image::updateFromHSV(const std::vector<float>&) {}
void Image::updateFromHSL(const std::vector<float>&) {}

// ═══════════════════════════════════════════════════════════════════
// Main — load image, convert to greyscale, run 4 filters, save outputs
// ═══════════════════════════════════════════════════════════════════

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./test_edge <image_path>" << std::endl;
        return 1;
    }

    // Load original image
    Image original(argv[1]);
    if (original.getWidth() == 0) {
        std::cerr << "Failed to load: " << argv[1] << std::endl;
        return 1;
    }
    std::cout << "Loaded " << original.getWidth() << "x" << original.getHeight()
              << " (" << original.getChannels() << " channels)" << std::endl;

    // Convert to greyscale
    original.toGrayscale();
    std::cout << "Converted to greyscale" << std::endl;

    // Save greyscale version for reference
    original.save("greyscale.png");

    // Test each filter
    struct TestCase {
        const char* name;
        Filter* filter;
    };

    SobelFilter       sobel;
    PrewittFilter     prewitt;
    ScharrFilter      scharr;
    RobertsCrossFilter roberts;

    TestCase tests[] = {
        {"sobel",   &sobel},
        {"prewitt", &prewitt},
        {"scharr",  &scharr},
        {"roberts", &roberts},
    };

    for (auto& t : tests) {
        // Copy greyscale image (each filter modifies in-place)
        Image img = original;
        t.filter->apply(img);

        std::string outfile = std::string("output_") + t.name + ".png";
        img.save(outfile);
        std::cout << "Saved " << outfile << std::endl;
    }

    std::cout << "Done! Compare output images with PPT examples." << std::endl;
    return 0;
}
