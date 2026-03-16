#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include "Image.h"
#include "Volume.h"
#include "Slice.h"

// --- Volume Class Tests ---
void testVolumeBasicAndExtend() {
    // Create a 2x2x2 Volume, 1 channel
    Volume vol(2, 2, 2, 1);

    assert(vol.getWidth() == 2);
    assert(vol.getHeight() == 2);
    assert(vol.getDepth() == 2);

    // Set voxel at (0, 1, 1)
    vol.setVoxel(0, 1, 1, 0, 128);
    assert(vol.getVoxel(0, 1, 1, 0) == 128);

    // TEST THE 3D "EXTEND" BOUNDARY CONDITION
    // Accessing z=5 should clamp to z=1
    assert(vol.getVoxel(0, 1, 5, 0) == 128);
    
    std::cout << "Volume Basic & Extend Tests Passed!" << std::endl;
}

int main() {
    std::cout << "Running Volume Class Tests..." << std::endl;
    
    testVolumeBasicAndExtend();
    
    std::cout << "All Volume Class Tests Passed Successfully!" << std::endl;
    return 0;
}