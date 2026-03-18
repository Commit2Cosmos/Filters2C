#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <cassert>

#include "Image.h"
#include "Volume.h"
#include "Slice.h"
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

    GaussianBlurFilter blur(3, 2.0);
    blur.apply(img);

    // Expected result after Gaussian blur with kernel size 3 and sigma 2:
    // The center pixel should be blurred but less than the box blur value of 88
    unsigned char centerValue = img.getPixel(1, 1, 0);
    assert(centerValue < 88);

}

void testGaussianBlur3D() {
    Volume vol(3, 3, 3, 1); // 3x3x3 grayscale volume

    // Fill everything with 100 except center voxel = 0
    for (int z = 0; z < 3; ++z) {
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                vol.setVoxel(x, y, z, 0, (x == 1 && y == 1 && z == 1) ? 0 : 100);
            }
        }
    }

    GaussianBlurFilter blur(3, 2.0);
    blur.apply(vol);

    // Center voxel should be blurred upward from 0
    unsigned char centerValue = vol.getVoxel(1, 1, 1, 0);
    assert(centerValue > 0);

    // It should still be lower than a surrounding voxel
    unsigned char neighborValue = vol.getVoxel(1, 1, 0, 0);
    assert(centerValue < neighborValue);
}

void testGaussianBlur3DConstantVolume() {
    Volume vol(3, 3, 3, 1);

    // Constant volume should remain unchanged after Gaussian blur
    for (int z = 0; z < 3; ++z) {
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                vol.setVoxel(x, y, z, 0, 100);
            }
        }
    }

    GaussianBlurFilter blur(3, 2.0);
    blur.apply(vol);

    for (int z = 0; z < 3; ++z) {
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                assert(vol.getVoxel(x, y, z, 0) == 100);
            }
        }
    }
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
void testMedianBlur3D() {
    Volume vol(3, 3, 3, 1); // 3x3x3 grayscale volume

    // Fill the volume with 100, except the center voxel which is 0
    for (int z = 0; z < 3; ++z) {
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                vol.setVoxel(x, y, z, 0, 100);
            }
        }
    }
    vol.setVoxel(1, 1, 1, 0, 0);

    MedianBlurFilter blur(3);
    blur.apply(vol);

    // The 3x3x3 neighbourhood contains twenty-six 100s and one 0,
    // so the median at the center should be 100.
    assert(vol.getVoxel(1, 1, 1, 0) == 100);
}

// Loads an image from the repo, applies filters,
// and saves outputs for visual inspection.
// --------------------------------------------------
void testBlurOnImage() {
    std::cout << "\nRunning Blur Filters on Image..." << std::endl;

    // Replace this path with the actual image path in your repository
    std::string inputPath = "../Images/gracehopper.png";

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

// Loads an volume scan from the repo, applies filters,
// and saves outputs for visual inspection.
// --------------------------------------------------
void testBlurOnVolume() {
    std::cout << "\nRunning Blur Filters on Volume..." << std::endl;

    std::vector<std::string> filepaths;
    for (int i = 0; i < 32; ++i) {
        char buffer[64];
        std::snprintf(buffer, sizeof(buffer), "Scans/TestVolume/vol%03d.png", i);
        filepaths.push_back(buffer);
    }

    Volume volume;
    bool loaded = volume.loadFromFiles(filepaths);
    assert(loaded);
   
    {
        Image yzSlice = Slice::extractYZ(volume, 16);
        assert(yzSlice.getWidth() > 0);
        assert(yzSlice.getHeight() > 0);
        yzSlice.save("original_volume_yz.png");
    }

    {
        Volume blurredVolume = volume;
        GaussianBlurFilter blur(3, 2.0);
        blur.apply(blurredVolume);

        Image yzSlice = Slice::extractYZ(blurredVolume, 16);
        assert(yzSlice.getWidth() > 0);
        assert(yzSlice.getHeight() > 0);
        yzSlice.save("gaussianblur_volume_yz.png");
    }

    {
        Volume blurredVolume = volume;
        MedianBlurFilter blur(3);
        blur.apply(blurredVolume);

        Image yzSlice = Slice::extractYZ(blurredVolume, 16);
        assert(yzSlice.getWidth() > 0);
        assert(yzSlice.getHeight() > 0);
        yzSlice.save("medianblur_volume_yz.png");
    }
}
int main() {
    std::cout << "Running Blur Class Tests...";

    testBoxBlur();
    testGaussianBlur();
    testGaussianBlur3D();
    testGaussianBlur3DConstantVolume();

    testMedianBlur();
    testMedianBlur3D();
    testBlurOnImage();
    testBlurOnVolume();
    return 0;
}
