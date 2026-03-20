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
#include <cmath>
#include "Image.h"
#include "Volume.h"
#include "Slice.h"

// --- Slice Class Tests ---
void testSliceXZ() {
    // Create a 2x2 volume with 3 depth slices
    Volume vol(2, 2, 3, 1);
    
    // We are extracting the XZ plane for Y=1 (1-based index)
    // Internal 0-based index will be y=0.
    
    // Z=0 (user Z=1)
    vol.setVoxel(0, 0, 0, 0, 10); // x=0
    vol.setVoxel(1, 0, 0, 0, 20); // x=1
    
    // Z=1 (user Z=2)
    vol.setVoxel(0, 0, 1, 0, 30); // x=0
    vol.setVoxel(1, 0, 1, 0, 40); // x=1
    
    // Z=2 (user Z=3)
    vol.setVoxel(0, 0, 2, 0, 50); // x=0
    vol.setVoxel(1, 0, 2, 0, 60); // x=1

    // Extract XZ slice at user y=1
    Image xz_slice = Slice::extractXZ(vol, 1);

    // Check dimensions: width should be X (2), height should be Z (3)
    assert(xz_slice.getWidth() == 2);
    assert(xz_slice.getHeight() == 3);

    // CRUCIAL INVERSION TEST:
    // The lower left of the output should be (1,1) 
    // This means volume Z=0 must be at image Y=2 (bottom row)
    // and volume Z=2 must be at image Y=0 (top row)
    

    // Top row (img y=0) should be Z=2
    assert(xz_slice.getPixel(0, 0, 0) == 50); 
    assert(xz_slice.getPixel(1, 0, 0) == 60); 

    // Middle row (img y=1) should be Z=1
    assert(xz_slice.getPixel(0, 1, 0) == 30);
    assert(xz_slice.getPixel(1, 1, 0) == 40);

    // Bottom row (img y=2) should be Z=0
    assert(xz_slice.getPixel(0, 2, 0) == 10);
    assert(xz_slice.getPixel(1, 2, 0) == 20);
}

void testSliceYZ() {
    // Create a 2x2 volume with 3 depth slices, 1 channel
    Volume vol(2, 2, 3, 1);
    
    // We are extracting the YZ plane for X=1 (1-based index)
    // Internal 0-based index will be x=0.
    
    // Z=0 (user Z=1)
    vol.setVoxel(0, 0, 0, 0, 15); // y=0
    vol.setVoxel(0, 1, 0, 0, 25); // y=1
    
    // Z=1 (user Z=2)
    vol.setVoxel(0, 0, 1, 0, 35); // y=0
    vol.setVoxel(0, 1, 1, 0, 45); // y=1
    
    // Z=2 (user Z=3)
    vol.setVoxel(0, 0, 2, 0, 55); // y=0
    vol.setVoxel(0, 1, 2, 0, 65); // y=1

    // Extract YZ slice at user x=1
    Image yz_slice = Slice::extractYZ(vol, 1);

    // Check dimensions: width should be Y (2), height should be Z (3)
    assert(yz_slice.getWidth() == 2);
    assert(yz_slice.getHeight() == 3);

    // CRUCIAL INVERSION TEST:
    // The lower left of the output should be (y=1, z=1)
    // This means volume Z=0 must be at image Y=2 (bottom row)
    // and volume Z=2 must be at image Y=0 (top row)

    // Top row (img y=0) should be Z=2
    assert(yz_slice.getPixel(0, 0, 0) == 55); // img x=0 maps to vol y=0
    assert(yz_slice.getPixel(1, 0, 0) == 65); // img x=1 maps to vol y=1

    // Middle row (img y=1) should be Z=1
    assert(yz_slice.getPixel(0, 1, 0) == 35);
    assert(yz_slice.getPixel(1, 1, 0) == 45);

    // Bottom row (img y=2) should be Z=0
    assert(yz_slice.getPixel(0, 2, 0) == 15);
    assert(yz_slice.getPixel(1, 2, 0) == 25);
    }

int main() {
    std::cout << "Running Slice Class Tests..." << std::endl;
    
    testSliceXZ();
    testSliceYZ();

    return 0;
}