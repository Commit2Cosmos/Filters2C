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

#include "Volume.h"
#include <iostream>
#include <algorithm> // For std::clamp
#include <cctype> // for std::isdigit
#include "stb_image.h"

namespace {
    // extracts the LAST number from the filename and returns it as an integer
    // done this way so can work with filepaths that include folders with numbers in. e.g. folder1/scan2.png
    int extractSliceNumber(const std::string& str) {
        int num = 0;
        int multiplier = 1;
        bool found_digit = false;
        
        // iterate backwards through string
        for (int i = str.length() - 1; i >= 0; --i) {
            if (std::isdigit(str[i])) {
                num += (str[i] - '0') * multiplier;
                multiplier *= 10;
                found_digit = true;
            } else if (found_digit) {
                // found the last block of digits and hit non-digit, so stop.
                break;
            }
        }
        return num;
    }

    // holds filepath and its index
    struct FileNode {
        std::string path;
        int index;
    };
} // namespace


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

    // Pre-compute the slice numbers once per file (O(N) parsing)
    std::vector<FileNode> nodes;
    nodes.reserve(filepaths.size());  // reserve memory rather than dynamically resizing
    for (const auto& path : filepaths) {
        nodes.push_back({path, extractSliceNumber(path)});
    }

    // CUSTOM NATURAL INSERTION SORT
    for (size_t i = 1; i < nodes.size(); ++i) {
        FileNode key = nodes[i];
        int j = i - 1;

        // Compare the pre-extracted integers directly
        while (j >= 0 && nodes[j].index > key.index) {
            nodes[j + 1] = nodes[j];
            j = j - 1;
        }
        nodes[j + 1] = key;
    }

    depth = nodes.size();
    data.clear();

    // loop through files
    for (int z = 0; z < depth; ++z) {
        int img_w, img_h, img_c;
        
        // Access the string path from our sorted struct
        unsigned char* raw_data = stbi_load(nodes[z].path.c_str(), &img_w, &img_h, &img_c, 0);

        if (!raw_data) {
            std::cerr << "Failed to load slice: " << nodes[z].path << std::endl;
            return false;
        }

        // For first slice, set the volume's global dimensions
        if (z == 0) {
            width = img_w;
            height = img_h;
            channels = img_c;
            data.reserve(width * height * depth * channels); 
        } else {
            if (img_w != width || img_h != height || img_c != channels) {
                std::cerr << "Error: Slice dimensions mismatch at " << nodes[z].path << std::endl;
                stbi_image_free(raw_data);
                return false;
            }
        }

        int slice_size = width * height * channels;
        data.insert(data.end(), raw_data, raw_data + slice_size);
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


// Used in projections, skips clamps
unsigned char Volume::getVoxelRaw1D(int index) const {
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
