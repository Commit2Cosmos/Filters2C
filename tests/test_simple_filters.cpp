#include <iostream>
#include <cmath>

#include "Image.h"
#include "GrayScale.h"
#include "Brightness.h"
#include "Threshold.h"

bool testGreyscale()
{
    Image img(2, 1, 3);

    // Pixel 0 = (100,150,200)
    img.setPixel(0, 0, 0, 100);
    img.setPixel(0, 0, 1, 150);
    img.setPixel(0, 0, 2, 200);

    // Pixel 1 = (255,0,0)
    img.setPixel(1, 0, 0, 255);
    img.setPixel(1, 0, 1, 0);
    img.setPixel(1, 0, 2, 0);

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

bool testBrightness()
{
    Image img(2, 1, 3);

    // Pixel 0 = (100,120,140)
    img.setPixel(0, 0, 0, 100);
    img.setPixel(0, 0, 1, 120);
    img.setPixel(0, 0, 2, 140);

    // Pixel 1 = (250,10,0)
    img.setPixel(1, 0, 0, 250);
    img.setPixel(1, 0, 1, 10);
    img.setPixel(1, 0, 2, 0);

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

bool testThreshold()
{
    // Use a grayscale image to make the expected output unambiguous
    Image img(3, 1, 1);

    img.setPixel(0, 0, 0, 50);
    img.setPixel(1, 0, 0, 127);
    img.setPixel(2, 0, 0, 200);

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

int main()
{
    int passed = 0;
    int total = 3;

    if (testGreyscale()) {
        std::cout << "Greyscale test passed.\n";
        passed++;
    } else {
        std::cout << "Greyscale test failed.\n";
    }

    if (testBrightness()) {
        std::cout << "Brightness test passed.\n";
        passed++;
    } else {
        std::cout << "Brightness test failed.\n";
    }

    if (testThreshold()) {
        std::cout << "Threshold test passed.\n";
        passed++;
    } else {
        std::cout << "Threshold test failed.\n";
    }

    std::cout << passed << " / " << total << " tests passed.\n";

    return (passed == total) ? 0 : 1;
}