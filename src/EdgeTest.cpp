/**
 * Standalone test for edge detection filters.
 * Uses the project's Image.cpp — make sure it is compiled together.
 *
 * Compile:
 *   g++ -std=c++20 -O2 -Isrc src/EdgeTest.cpp src/Image.cpp src/ConvFilter.cpp \
 *       src/SobelFilter.cpp src/PrewittFilter.cpp \
 *       src/ScharrFilter.cpp src/RobertsCrossFilter.cpp \
 *       -o test_edge -lm
 *
 * Run:
 *   ./test_edge Images/gracehopper.png
 */

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

    // Each edge filter handles grayscale conversion internally,
    // so no need to call toGrayscale() here.

    // Test each filter
    struct TestCase {
        const char* name;
        Filter* filter;
    };

    SobelFilter        sobel;
    PrewittFilter      prewitt;
    ScharrFilter       scharr;
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
