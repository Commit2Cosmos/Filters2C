/*
Kruskal
Xinying Yang ada-xy325
Oliver Whitehead ada-ow25
Anton Belov ada-ab2125 (ada-85a5785b61)
Hao Liu ada-hl4425
Jiayu Wu ada-jw1125
Yuhang Dai ada-yd1125
Julia Williams ada-jjw25
*/

#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <cassert>
#include "Image.h"


bool isClose(float a, float b, float tolerance = 0.01f) {
    return std::abs(a - b) < tolerance;
}


void testLoadAndSave() {
    Image img;
    
    std::string inputPath = "Images/bourton.png";
    std::string outputPath = "image_save_test_output.png";

    // 1. Test Loading
    bool loaded = img.load(inputPath);
    if (!loaded) {
        std::cerr << "\n[WARNING]: Could not load " << inputPath 
                  << ".\nEnsure you have a dummy image there, and check your terminal's current working directory!" << std::endl;
        return; // Skip the rest of the test so it doesn't crash
    }
    
    // Assert that the image actually loaded data
    assert(img.getWidth() > 0);
    assert(img.getHeight() > 0);
    assert(img.getChannels() > 0);

    // 2. Test Saving
    bool saved = img.save(outputPath);
    assert(saved == true);

    // 3. Verify the roundtrip: Load the newly saved file into a second Image object
    Image imgLoaded;
    bool loadedAgain = imgLoaded.load(outputPath);
    assert(loadedAgain == true);
    
    // The dimensions and channels of the saved file should perfectly match the original
    assert(img.getWidth() == imgLoaded.getWidth());
    assert(img.getHeight() == imgLoaded.getHeight());
    assert(img.getChannels() == imgLoaded.getChannels());

}


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

}

void testHSVConversion() {
    Image img(1, 1, 3); // 1x1 RGB image
    
    // Test Case: Pure Red
    // Expected HSV for Red: Hue = 0.0, Saturation = 1.0, Value = 1.0
    img.setPixel(0, 0, 0, 255); // R
    img.setPixel(0, 0, 1, 0);   // G
    img.setPixel(0, 0, 2, 0);   // B

    std::vector<float> hsv = img.toHSV();
    
    // Check float math
    assert(isClose(hsv[0], 0.0f));   // H
    assert(isClose(hsv[1], 1.0f));   // S
    assert(isClose(hsv[2], 1.0f));   // V

    // Test Roundtrip: HSV back to RGB
    img.updateFromHSV(hsv);
    assert(img.getPixel(0, 0, 0) == 255);
    assert(img.getPixel(0, 0, 1) == 0);
    assert(img.getPixel(0, 0, 2) == 0);

}

void testHSLConversion() {
    Image img(1, 1, 3); // 1x1 RGB image
    
    // Test Case: Pure Green
    // Expected HSL for Green: Hue = 120.0, Saturation = 1.0, Lightness = 0.5
    img.setPixel(0, 0, 0, 0);   // R
    img.setPixel(0, 0, 1, 255); // G
    img.setPixel(0, 0, 2, 0);   // B

    std::vector<float> hsl = img.toHSL();
    
    // Check float math 
    assert(isClose(hsl[0], 120.0f)); // H
    assert(isClose(hsl[1], 1.0f));   // S
    assert(isClose(hsl[2], 0.5f));   // L

    // Test Roundtrip: HSL back to RGB
    img.updateFromHSL(hsl);
    assert(img.getPixel(0, 0, 0) == 0);
    assert(img.getPixel(0, 0, 1) == 255);
    assert(img.getPixel(0, 0, 2) == 0);

}

int main() {
    std::cout << "Running Image Class Tests..." << std::endl;
    
    testLoadAndSave();
    testImageBasicAndExtend();
    testImageGrayscale();
    testHSVConversion();
    testHSLConversion();

    return 0;
}