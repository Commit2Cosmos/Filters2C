#include "Projection.h"
#include <iostream>
#include <algorithm> // Only for std::clamp (allowed) and std::swap

// Local Helper Functions
namespace helper{

    // Helper to validate and convert 1-based user indices to 0-based volume indices
    void getZBounds(int depth, int z_min, int z_max, int& out_start, int& out_end) {
        // iff default -1 is passed for z_max, it means the entire volume
        if (z_max == -1) z_max = depth;
        
        // convert from 1-based to 0-based 
        out_start = z_min - 1;
        out_end = z_max - 1;

        //  clamp into bounds
        out_start = std::clamp(out_start, 0, depth - 1);
        out_end = std::clamp(out_end, 0, depth - 1);

        // swap if user entered indexes backwards
        if (out_start > out_end) std::swap(out_start, out_end);
    }

}

// --- Maximum Intensity Projection ---
Image Projection::applyMaxIP(const Volume& volume, int z_min, int z_max) {
    int start_z, end_z;
    helper::getZBounds(volume.getDepth(), z_min, z_max, start_z, end_z);

    int width = volume.getWidth();
    int height = volume.getHeight();
    int channels = volume.getChannels();
    Image result(width, height, channels);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                unsigned char max_val = 0;
                
                // Find maximum intensity in the z-direction
                for (int z = start_z; z <= end_z; ++z) {
                    unsigned char val = volume.getVoxel(x, y, z, c);
                    if (val > max_val) max_val = val;
                }
                result.setPixel(x, y, c, max_val);
            }
        }
    }
    return result;
}

// --- Minimum Intensity Projection ---
Image Projection::applyMinIP(const Volume& volume, int z_min, int z_max) {
    int start_z, end_z;
    helper::getZBounds(volume.getDepth(), z_min, z_max, start_z, end_z);

    int width = volume.getWidth();
    int height = volume.getHeight();
    int channels = volume.getChannels();
    Image result(width, height, channels);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                unsigned char min_val = 255; // Start at max possible value
                
                // Find minimum intensity in the z-direction
                for (int z = start_z; z <= end_z; ++z) {
                    unsigned char val = volume.getVoxel(x, y, z, c);
                    if (val < min_val) min_val = val;
                }
                result.setPixel(x, y, c, min_val);
            }
        }
    }
    return result;
}

// --- Mean Intensity Projection ---
Image Projection::applyMeanIP(const Volume& volume, int z_min, int z_max) {
    int start_z, end_z;
    helper::getZBounds(volume.getDepth(), z_min, z_max, start_z, end_z);

    int width = volume.getWidth();
    int height = volume.getHeight();
    int channels = volume.getChannels();
    Image result(width, height, channels);

    int num_slices = (end_z - start_z) + 1;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                unsigned int sum = 0; // Use a larger type to prevent overflow during addition
                
                // Find average intensity in the z-direction
                for (int z = start_z; z <= end_z; ++z) {
                    sum += volume.getVoxel(x, y, z, c);
                }
                
                unsigned char avg_val = static_cast<unsigned char>(sum / num_slices);
                result.setPixel(x, y, c, avg_val);
            }
        }
    }
    return result;
}

// --- Median Intensity Projection ---
Image Projection::applyMedianIP(const Volume& volume, int z_min, int z_max) {
    int start_z, end_z;
    helper::getZBounds(volume.getDepth(), z_min, z_max, start_z, end_z);

    int width = volume.getWidth();
    int height = volume.getHeight();
    int channels = volume.getChannels();
    Image result(width, height, channels);

    int num_slices = (end_z - start_z) + 1;
    int target_median_count = num_slices / 2; 

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                
                // create 256 bin histogram array, initialised to 0
                int histogram[256] = {0}; 
                
                // count the occurrences of each pixel intensity in the Z-column
                for (int z = start_z; z <= end_z; ++z) {
                    unsigned char val = volume.getVoxel(x, y, z, c);
                    histogram[val]++;
                }
                
                // find the median by accumulating the counts
                int running_sum = 0;
                unsigned char median_val = 0;
                
                for (int i = 0; i < 256; ++i) {
                    running_sum += histogram[i];
                    if (running_sum > target_median_count) {
                        median_val = static_cast<unsigned char>(i);
                        break; 
                    }
                }
                
                result.setPixel(x, y, c, median_val);
            }
        }
    }
    return result;
}
