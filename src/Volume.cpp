#include "Volume.h"
#include <iostream>
#include <algorithm> // For std::clamp
#include "stb_image.h"

// Constructors
Volume::Volume() : width(0), height(0), depth(0), channels(0) {}

Volume::Volume(int w, int h, int d, int c) 
    : width(w), height(h), depth(d), channels(c) {
    // Initialise the flat 1D vector to hold the entire 3D volume of size W * H * D * C
    data.resize(w * h * d * c, 0);
}

// File I/O
bool Volume::loadFromFiles(const std::vector<std::string>& filepaths) {
    if (filepaths.empty()) {
        std::cerr << "Error: No file paths provided to load the volume." << std::endl;
        return false;
    }

    depth = filepaths.size();
    
    // Clear any existing data
    data.clear();

    for (int z = 0; z < depth; ++z) {
        int img_w, img_h, img_c;
        // Load the 2D slice [cite: 8]
        unsigned char* raw_data = stbi_load(filepaths[z].c_str(), &img_w, &img_h, &img_c, 0);

        if (!raw_data) {
            std::cerr << "Failed to load slice: " << filepaths[z] << std::endl;
            return false;
        }

        // For first slice, set the volume's global dimensions
        if (z == 0) {
            width = img_w;
            height = img_h;
            channels = img_c;
            data.reserve(width * height * depth * channels); // Pre-allocate the entire vector, rather than resize every time
        } else {
            // Safety check: Ensure all subsequent slices match the first slice's dimensions
            if (img_w != width || img_h != height || img_c != channels) {
                std::cerr << "Error: Slice dimensions mismatch at " << filepaths[z] << std::endl;
                stbi_image_free(raw_data);
                return false;
            }
        }

        // Insert this slice's data into our main 1D volume vector
        int slice_size = width * height * channels;
        data.insert(data.end(), raw_data, raw_data + slice_size);

        // Free the memory allocated by stb_image for this specific slice
        stbi_image_free(raw_data);
    }

    return true;
}

// Voxel Access (3D to 1D Mapping)
unsigned char Volume::getVoxel(int x, int y, int z, int channel) const {
    // If a 3D filter asks for a voxel outside the volume boundaries, 
    // it will clamp to the nearest edge voxel.
    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);
    z = std::clamp(z, 0, depth - 1);
    channel = std::clamp(channel, 0, channels - 1);

    // T3D to 1D index mapping formula
    int index = (z * width * height * channels) + 
                (y * width * channels) + 
                (x * channels) + 
                channel;

    return data[index];
}

void Volume::setVoxel(int x, int y, int z, int channel, unsigned char value) {
    // Ignore out-of-bounds writes
    if (x < 0 || x >= width || y < 0 || y >= height || 
        z < 0 || z >= depth || channel < 0 || channel >= channels) {
        return; 
    }

    int index = (z * width * height * channels) + 
                (y * width * channels) + 
                (x * channels) + 
                channel;

    data[index] = value;
}