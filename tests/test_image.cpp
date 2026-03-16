#include <iostream>
#include <cassert>
#include <cmath>
#include "Image.h"
#include "Volume.h"
#include "Slice.h"

// --- Image Class Tests ---
void testImageBasicAndExtend() {
    // Create a 3x3 RGB Image
    Image img(3, 3, 3);
    
    // Test basic properties
    assert(img.getWidth() == 3);
    assert(img.getHeight() == 3);
    assert(img.getChannels() == 3);

    // Set a pixel in the center (1, 1) and test retrieval
    img.setPixel(1, 1, 0, 100); // Red channel
    img.setPixel(1, 1, 1, 150); // Green channel
    img.setPixel(1, 1, 2, 200); // Blue channel
    
    assert(img.getPixel(1, 1, 0) == 100);
    assert(img.getPixel(1, 1, 1) == 150);
    
    // Set a pixel on the top-left edge (0, 0)
    img.setPixel(0, 0, 0, 255);
    
    // TEST THE "EXTEND" BOUNDARY CONDITION
    // Accessing out of bounds (-1, -5) should clamp to (0, 0)
    assert(img.getPixel(-1, -5, 0) == 255);
    // Accessing out of bounds (10, 0) should clamp to (2, 0)
    img.setPixel(2, 0, 0, 50);
    assert(img.getPixel(10, 0, 0) == 50);

    std::cout << "Image Basic & Extend Tests Passed!" << std::endl;
}

void testImageGrayscale() {
    Image img(1, 1, 3); // 1x1 RGB image
    img.setPixel(0, 0, 0, 100); // R
    img.setPixel(0, 0, 1, 150); // G
    img.setPixel(0, 0, 2, 200); // B

    img.toGrayscale();

    // Expected Grayscale calculation:
    // Y = 0.2126 * 100 + 0.7152 * 150 + 0.0722 * 200
    // Y = 21.26 + 107.28 + 14.44 = 142.98 -> 142 (when clamped to integer)
    
    assert(img.getChannels() == 1); // Should now be 1 channel 
    assert(img.getPixel(0, 0, 0) == 142); // Check math

    std::cout << "Image Grayscale Test Passed!" << std::endl;
}

int main() {
    std::cout << "Running Image Class Tests..." << std::endl;
    
    testImageBasicAndExtend();
    testImageGrayscale();

    std::cout << "All Image Class Tests Passed Successfully!" << std::endl;
    return 0;
}