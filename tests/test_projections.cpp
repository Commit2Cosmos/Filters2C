#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <cassert>
#include <vector>
#include <filesystem>
#include "Volume.h"
#include "Projection.h"
#include "Image.h"


namespace fs = std::filesystem;

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
}

int main() {
    std::cout << "Running Projection Tests..." << std::endl;
    testMaxIP();
    testMinIP();
    testMeanIP();
    testMedianIP();

    std::string folder_path = "Scans/TestVolume";  // UPDATED HERE
    std::vector<std::string> filepaths;

    std::cout << "Scanning directory: " << folder_path << "..." << std::endl;

    // Read all files from the directory
    if (!fs::exists(folder_path) || !fs::is_directory(folder_path)) {
        std::cerr << "Error: Directory does not exist! Please ensure you are running this from the correct folder." << std::endl;
        return 1;
    }

    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.is_regular_file()) {
            filepaths.push_back(entry.path().string());
        }
    }

    if (filepaths.empty()) {
        std::cerr << "Error: No files found in " << folder_path << std::endl;
        return 1;
    }

    std::cout << "Found " << filepaths.size() << " files. Loading volume (This will automatically sort them)..." << std::endl;

    // Load the Volume
    Volume vol;
    if (!vol.loadFromFiles(filepaths)) {
        std::cerr << "Error: Failed to load volume." << std::endl;
        return 1;
    }

    std::cout << "Dimensions: " << vol.getWidth() << "x" << vol.getHeight() << "x" << vol.getDepth() << " with " << vol.getChannels() << " channels." << std::endl;

    // Perform and Save Projections
    // Note: We use the default -1 for z_max to process the entire volume

    std::cout << "1/4: Generating Maximum Intensity Projection..." << std::endl;
    Image max_img = Projection::applyMaxIP(vol);
    max_img.save("test_maxIP.png"); // Assuming Image.h has a save() method using stbi_write_png

    std::cout << "2/4: Generating Minimum Intensity Projection..." << std::endl;
    Image min_img = Projection::applyMinIP(vol);
    min_img.save("test_minIP.png");

    std::cout << "3/4: Generating Mean Intensity Projection..." << std::endl;
    Image mean_img = Projection::applyMeanIP(vol);
    mean_img.save("test_meanIP.png");

    std::cout << "4/4: Generating Median Intensity Projection..." << std::endl;
    Image median_img = Projection::applyMedianIP(vol);
    median_img.save("test_medianIP.png");


    return 0;
}
