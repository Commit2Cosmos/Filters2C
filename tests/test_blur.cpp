#include <iostream>
#include <cassert>

#include "Image.h"
#include "Blur.h"
#include "Filter.h"

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

    Blur blur;
    blur.applyBoxBlur(img, 3);

    // Expected result after box blur with kernel size 3:
    // Each pixel should be the average of its neighbors
    // For the center pixel (1,1), it should be (8*100 + 0) / 9 = ~88
    assert(img.getPixel(1, 1, 0) == 88);

    std::cout << "Box Blur Test Passed!" << std::endl;
}