#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <cassert>

#include "Image.h"
#include "BoxBlurFilter.h"
#include "GaussianBlurFilter.h"
#include "MedianBlurFilter.h"

// Numerical test for blur function
void testBoxBlur() {
    Image img(3, 3, 1); // 3x3 grayscale image

    // Set up a simple pattern
    // 100 100 100
    // 100 0   100
    // 100 100 100
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            img.setPixel(x, y, 0, (x == 1 && y == 1) ? 0 : 100);
        }
    }

    BoxBlurFilter blur(3);
    blur.apply(img);

    // Expected result after box blur with kernel size 3:
    // Each pixel should be the average of its neighbors
    // For the center pixel (1,1), it should be (8*100 + 0) / 9 = ~88
    assert(img.getPixel(1, 1, 0) == 88);

}

void testGaussianBlur() {
    Image img(3, 3, 1); // 3x3 greyscale image

    // Set up a simple pattern
    // 100 100 100
    // 100 0   100
    // 100 100 100
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            img.setPixel(x, y, 0, (x == 1 && y == 1) ? 0 : 100);
        }
    }

    GaussianBlurFilter blur(3);
    blur.apply(img);

    // Expected result after Gaussian blur with kernel size 3 and sigma 2:
    // The center pixel should be blurred but less than the box blur value of 88
    unsigned char centerValue = img.getPixel(1, 1, 0);
    assert(centerValue < 88);

}

void testMedianBlur() {
    Image img(3, 3, 1); // 3x3 grayscale image

    // Set up a simple pattern
    // 100 100 100
    // 100 0   100
    // 100 100 100
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            img.setPixel(x, y, 0, (x == 1 && y == 1) ? 0 : 100);
        }
    }

    MedianBlurFilter blur(3);
    blur.apply(img);

    // Expected result after median blur with kernel size 3:
    // The center pixel should become the median of its neighbors, which is 100
    assert(img.getPixel(1, 1, 0) == 100);

}

// Loads an image from the repo, applies filters,
// and saves outputs for visual inspection.
// --------------------------------------------------
void testBlurOnImage() {
    std::cout << "\nRunning Blur Filters on Image..." << std::endl;

    // Replace this path with the actual image path in your repository
    std::string inputPath = "Images/gracehopper.png";

    // Load original image
    Image original(inputPath);

    // Box blur test
    {
        Image img = original; // start from original each time
        BoxBlurFilter blur(3);
        blur.apply(img);
        img.save("boxblur_gracehopper.png");
    }

    // Gaussian blur test
    {
        Image img = original;
        GaussianBlurFilter blur(5, 2.0);
        blur.apply(img);
        img.save("gaussianblur_gracehopper.png");
    }

    // Median blur test
    {
        Image img = original;
        MedianBlurFilter blur(3);
        blur.apply(img);
        img.save("medianblur_gracehopper.png");
    }

}

int main() {
    std::cout << "Running Blur Class Tests...";

    testBoxBlur();
    testGaussianBlur();
    testMedianBlur();
    testBlurOnImage();

    return 0;
}
