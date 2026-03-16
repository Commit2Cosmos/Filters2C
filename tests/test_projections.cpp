#include <iostream>
#include <cassert>
#include <vector>
#include "Volume.h"
#include "Projection.h"
#include "Image.h"

// Helper function to create a small, predictable 3D Volume for testing
Volume createTestVolume() {
    // Create a 2x2 image with 3 slices (depth=3) and 1 channel (Grayscale)
    Volume vol(2, 2, 3, 1);

    // Slice 1 (z=0)
    vol.setVoxel(0, 0, 0, 0, 10);
    vol.setVoxel(1, 0, 0, 0, 20);
    vol.setVoxel(0, 1, 0, 0, 30);
    vol.setVoxel(1, 1, 0, 0, 40);

    // Slice 2 (z=1)
    vol.setVoxel(0, 0, 1, 0, 15);
    vol.setVoxel(1, 0, 1, 0, 5);
    vol.setVoxel(0, 1, 1, 0, 50);
    vol.setVoxel(1, 1, 1, 0, 60);

    // Slice 3 (z=2)
    vol.setVoxel(0, 0, 2, 0, 5);
    vol.setVoxel(1, 0, 2, 0, 25);
    vol.setVoxel(0, 1, 2, 0, 35);
    vol.setVoxel(1, 1, 2, 0, 20);

    return vol;
}

void testMaxIP() {
    Volume vol = createTestVolume();
    Image result = Projection::applyMaxIP(vol);

    // Expected MIP (Max across Z):
    // (0,0): max(10, 15, 5)  = 15
    // (1,0): max(20, 5, 25)  = 25
    // (0,1): max(30, 50, 35) = 50
    // (1,1): max(40, 60, 20) = 60
    assert(result.getPixel(0, 0, 0) == 15);
    assert(result.getPixel(1, 0, 0) == 25);
    assert(result.getPixel(0, 1, 0) == 50);
    assert(result.getPixel(1, 1, 0) == 60);
    std::cout << "applyMaxIP Test Passed!" << std::endl;
}

void testMinIP() {
    Volume vol = createTestVolume();
    Image result = Projection::applyMinIP(vol);

    // Expected MinIP (Min across Z):
    // (0,0): min(10, 15, 5)  = 5
    // (1,0): min(20, 5, 25)  = 5
    // (0,1): min(30, 50, 35) = 30
    // (1,1): min(40, 60, 20) = 20
    assert(result.getPixel(0, 0, 0) == 5);
    assert(result.getPixel(1, 0, 0) == 5);
    assert(result.getPixel(0, 1, 0) == 30);
    assert(result.getPixel(1, 1, 0) == 20);
    std::cout << "applyMinIP Test Passed!" << std::endl;
}

void testMeanIP() {
    Volume vol = createTestVolume();
    Image result = Projection::applyMeanIP(vol);

    // Expected AIP (Average across Z):
    // (0,0): (10 + 15 + 5) / 3  = 10
    // (1,0): (20 + 5 + 25) / 3  = 16 (integer division)
    // (0,1): (30 + 50 + 35) / 3 = 38
    // (1,1): (40 + 60 + 20) / 3 = 40
    assert(result.getPixel(0, 0, 0) == 10);
    assert(result.getPixel(1, 0, 0) == 16);
    assert(result.getPixel(0, 1, 0) == 38);
    assert(result.getPixel(1, 1, 0) == 40);
    std::cout << "applyMeanIP Test Passed!" << std::endl;
}

void testMedianIP() {
    Volume vol = createTestVolume();
    Image result = Projection::applyMedianIP(vol);

    // Expected MedianIP (Sorted Z values -> middle value):
    // (0,0): 5, 10, 15 -> median = 10
    // (1,0): 5, 20, 25 -> median = 20
    // (0,1): 30, 35, 50 -> median = 35
    // (1,1): 20, 40, 60 -> median = 40
    assert(result.getPixel(0, 0, 0) == 10);
    assert(result.getPixel(1, 0, 0) == 20);
    assert(result.getPixel(0, 1, 0) == 35);
    assert(result.getPixel(1, 1, 0) == 40);
    std::cout << "applyMedianIP Test Passed!" << std::endl;
}

int main() {
    std::cout << "Running Projection Tests..." << std::endl;
    testMaxIP();
    testMinIP();
    testMeanIP();
    testMedianIP();
    std::cout << "All Projection Tests Passed Successfully!" << std::endl;
    return 0;
}