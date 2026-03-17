#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <algorithm>
#include <string>
#include "Image.h"
#include "Volume.h"
#include "Slice.h"

// --- Volume Class Tests ---

void testVolumeLoadingFromFolder() {
    // We use a namespace alias to keep the code clean
    namespace fs = std::filesystem;

    std::string folderPath = "Scans/TestVolume"; 
    std::vector<std::string> filepaths;

    // 1. Safety check: Does the folder even exist?
    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        std::cerr << "\n[WARNING]: Scans folder not found at " << folderPath 
                  << ".\nPlease check your terminal's current working directory!" << std::endl;
        return; // Skip so we don't crash
    }

    // 2. Iterate through the folder and collect all file paths
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        // We only want regular files (no hidden folders or sub-directories)
        if (entry.is_regular_file()) {
            // ensure it's specifically a PNG
            if (entry.path().extension() == ".png") {
                filepaths.push_back(entry.path().string());
            }
        }
    }

    if (filepaths.empty()) {
        std::cerr << "\n[WARNING]: Scans folder is empty or contains no PNGs!" << std::endl;
        return;
    }

    // 3. Load the files into the Volume class
    Volume vol;
    bool success = vol.loadFromFiles(filepaths); // now sorts filepaths itself
    
    // 4. Assertions to prove it worked
    assert(success == true);
    assert(vol.getDepth() == filepaths.size()); // Depth should equal number of files
    assert(vol.getWidth() > 0);
    assert(vol.getHeight() > 0);
    assert(vol.getChannels() > 0);
}


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
}

int main() {
    std::cout << "Running Volume Class Tests..." << std::endl;
    
    testVolumeLoadingFromFolder();
    testVolumeBasicAndExtend();
    
    return 0;
}
