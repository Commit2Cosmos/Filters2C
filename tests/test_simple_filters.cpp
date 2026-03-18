#include <iostream>
#include <cmath>

#include "Image.h"
#include "GrayScale.h"
#include "Brightness.h"
#include "Threshold.h"
#include "HistogramEqualisation.h"
#include "SaltAndPepper.h"

// ------------------------------------------------------------
// Test 1: Greyscale filter
// This test creates a small 2x1 RGB image with two known pixels,
// applies the greyscale filter, and checks whether each output
// channel matches the expected luminance-based grayscale value.
// ------------------------------------------------------------
bool testGreyscale()
{
    // Create a 2x1 image with 3 channels (RGB)
    Image img(2, 1, 3);

    // Pixel 0 = (100,150,200)
    img.setPixel(0, 0, 0, 100);
    img.setPixel(0, 0, 1, 150);
    img.setPixel(0, 0, 2, 200);

    // Pixel 1 = (255,0,0)
    img.setPixel(1, 0, 0, 255);
    img.setPixel(1, 0, 1, 0);
    img.setPixel(1, 0, 2, 0);

    // Apply greyscale conversion
    GrayScale filter;
    filter.apply(img);

    // Expected grayscale values
    unsigned char expected0 = static_cast<unsigned char>(
        0.2126 * 100 + 0.7152 * 150 + 0.0722 * 200
    ); // ≈ 143

    unsigned char expected1 = static_cast<unsigned char>(
        0.2126 * 255 + 0.7152 * 0 + 0.0722 * 0
    ); // ≈ 54

    bool ok = true;

    // Pixel 0 should become (expected0, expected0, expected0)
    ok &= (img.getPixel(0, 0, 0) == expected0);
    ok &= (img.getPixel(0, 0, 1) == expected0);
    ok &= (img.getPixel(0, 0, 2) == expected0);

    // Pixel 1 should become (expected1, expected1, expected1)
    ok &= (img.getPixel(1, 0, 0) == expected1);
    ok &= (img.getPixel(1, 0, 1) == expected1);
    ok &= (img.getPixel(1, 0, 2) == expected1);

    return ok;
}

// ------------------------------------------------------------
// Test 2: Brightness filter
// This test checks whether a fixed brightness increment is added
// correctly to each channel, and whether values are clamped
// correctly when they exceed 255.
// ------------------------------------------------------------
bool testBrightness()
{
    // Create a 2x1 RGB image
    Image img(2, 1, 3);

    // Pixel 0 = (100,120,140)
    img.setPixel(0, 0, 0, 100);
    img.setPixel(0, 0, 1, 120);
    img.setPixel(0, 0, 2, 140);

    // Pixel 1 = (250,10,0)
    img.setPixel(1, 0, 0, 250);
    img.setPixel(1, 0, 1, 10);
    img.setPixel(1, 0, 2, 0);

    // Increase brightness by 20
    Brightness filter(20);
    filter.apply(img);

    bool ok = true;

    // Pixel 0 -> (120,140,160)
    ok &= (img.getPixel(0, 0, 0) == 120);
    ok &= (img.getPixel(0, 0, 1) == 140);
    ok &= (img.getPixel(0, 0, 2) == 160);

    // Pixel 1 -> (255,30,20) because of clamping
    ok &= (img.getPixel(1, 0, 0) == 255);
    ok &= (img.getPixel(1, 0, 1) == 30);
    ok &= (img.getPixel(1, 0, 2) == 20);

    return ok;
}

// ------------------------------------------------------------
// Test 3: Histogram equalisation
// This test uses a very small grayscale image with only two
// intensity values, so the expected remapping is easy to verify.
// ------------------------------------------------------------
bool testHistogramEqualisation()
{
    // Grayscale image with 4 pixels: [50, 50, 200, 200]
    // Only 1 channel is needed because this is a grayscale image
    Image img(4, 1, 1);

    img.setPixel(0, 0, 0, 50);
    img.setPixel(1, 0, 0, 50);
    img.setPixel(2, 0, 0, 200);
    img.setPixel(3, 0, 0, 200);

    // Apply histogram equalisation
    HistogramEqualisation filter;
    filter.apply(img);

    bool ok = true;

    // Expected mapping after equalisation:
    // lower intensity should map to 0
    // higher intensity should map to 255
    ok &= (img.getPixel(0, 0, 0) == 0);
    ok &= (img.getPixel(1, 0, 0) == 0);
    ok &= (img.getPixel(2, 0, 0) == 255);
    ok &= (img.getPixel(3, 0, 0) == 255);

    return ok;
}

// ------------------------------------------------------------
// Test 4: Threshold filter
// This test checks whether grayscale pixel values are correctly
// converted into binary values using the threshold rule.
// ------------------------------------------------------------
bool testThreshold()
{
    // Use a grayscale image to make the expected output unambiguous
    Image img(3, 1, 1);

    // Three test pixels: below threshold, equal to threshold, above threshold
    img.setPixel(0, 0, 0, 50);
    img.setPixel(1, 0, 0, 127);
    img.setPixel(2, 0, 0, 200);

    // Apply thresholding
    Threshold filter;
    filter.apply(img);

    bool ok = true;

    // Assuming your threshold logic is:
    // pixel > 127 -> 255
    // else        -> 0
    ok &= (img.getPixel(0, 0, 0) == 0);
    ok &= (img.getPixel(1, 0, 0) == 0);
    ok &= (img.getPixel(2, 0, 0) == 255);

    return ok;
}

// ------------------------------------------------------------
// Test 5: Salt-and-pepper noise
// This test checks a special edge case: when the noise ratio is 0,
// no pixels should be modified at all.
// ------------------------------------------------------------
bool testSaltAndPepper()
{
    // Create a 2x1 RGB image
    Image img(2, 1, 3);

    // Pixel 0 = (100, 120, 140)
    img.setPixel(0, 0, 0, 100);
    img.setPixel(0, 0, 1, 120);
    img.setPixel(0, 0, 2, 140);

    // Pixel 1 = (200, 210, 220)
    img.setPixel(1, 0, 0, 200);
    img.setPixel(1, 0, 1, 210);
    img.setPixel(1, 0, 2, 220);

    // With zero noise ratio, the image should remain unchanged
    SaltAndPepper filter(0.0);
    filter.apply(img);

    bool ok = true;

    // Verify pixel 0 is unchanged
    ok &= (img.getPixel(0, 0, 0) == 100);
    ok &= (img.getPixel(0, 0, 1) == 120);
    ok &= (img.getPixel(0, 0, 2) == 140);

    // Verify pixel 1 is unchanged
    ok &= (img.getPixel(1, 0, 0) == 200);
    ok &= (img.getPixel(1, 0, 1) == 210);
    ok &= (img.getPixel(1, 0, 2) == 220);

    return ok;
}

// ------------------------------------------------------------
// Main test runner
// Runs all five tests, prints pass/fail messages, and returns
// exit code 0 only if all tests pass.
// ------------------------------------------------------------
int main()
{
    int passed = 0;
    int total = 5;

    // Run greyscale test
    if (testGreyscale()) {
        std::cout << "Greyscale test passed.\n";
        passed++;
    } else {
        std::cout << "Greyscale test failed.\n";
    }

    // Run brightness test
    if (testBrightness()) {
        std::cout << "Brightness test passed.\n";
        passed++;
    } else {
        std::cout << "Brightness test failed.\n";
    }

    // Run threshold test
    if (testThreshold()) {
        std::cout << "Threshold test passed.\n";
        passed++;
    } else {
        std::cout << "Threshold test failed.\n";
    }

    // Run histogram equalisation test
    if (testHistogramEqualisation()) {
        std::cout << "Histogram equalisation test passed.\n";
        passed++;
    } else {
        std::cout << "Histogram equalisation test failed.\n";
    }

    // Run salt-and-pepper noise test
    if (testSaltAndPepper()) {
        std::cout << "Salt and pepper test passed.\n";
        passed++;
    } else {
        std::cout << "Salt and pepper test failed.\n";
    }

    std::cout << passed << " / " << total << " tests passed.\n";

    return (passed == total) ? 0 : 1;
}
