#include "Projection.h"
#include <iostream>
#include <algorithm> // Only for std::clamp (allowed) and std::swap

namespace {

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

} // namespace

// --- Maximum Intensity Projection ---
Image Projection::applyMaxIP(const Volume& volume, int z_min, int z_max) {
    int start_z, end_z;
    getZBounds(volume.getDepth(), z_min, z_max, start_z, end_z);

    int width = volume.getWidth();
    int height = volume.getHeight();
    int channels = volume.getChannels();
    Image result(width, height, channels);

    // pre-calculate the size of one full slice jump
    int slice_stride = width * height * channels;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                unsigned char max_val = 0;
                
                // Calculate starting 1D index once per Z-column
                int base_index = (y * width * channels) + (x * channels) + c;
                int current_index = base_index + (start_z * slice_stride);
                
                // Fast memory striding through the Z-direction
                for (int z = start_z; z <= end_z; ++z) {
                    unsigned char val = volume.getVoxelRaw1D(current_index);
                    if (val > max_val) max_val = val;
                    
                    current_index += slice_stride; // jump to next slice
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
    getZBounds(volume.getDepth(), z_min, z_max, start_z, end_z);

    int width = volume.getWidth();
    int height = volume.getHeight();
    int channels = volume.getChannels();
    Image result(width, height, channels);

    int slice_stride = width * height * channels;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                unsigned char min_val = 255; 
                
                int base_index = (y * width * channels) + (x * channels) + c;
                int current_index = base_index + (start_z * slice_stride);
                
                for (int z = start_z; z <= end_z; ++z) {
                    unsigned char val = volume.getVoxelRaw1D(current_index);
                    if (val < min_val) min_val = val;
                    
                    current_index += slice_stride; 
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
    getZBounds(volume.getDepth(), z_min, z_max, start_z, end_z);

    int width = volume.getWidth();
    int height = volume.getHeight();
    int channels = volume.getChannels();
    Image result(width, height, channels);

    int num_slices = (end_z - start_z) + 1;
    int slice_stride = width * height * channels;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                unsigned int sum = 0; 
                
                int base_index = (y * width * channels) + (x * channels) + c;
                int current_index = base_index + (start_z * slice_stride);
                
                for (int z = start_z; z <= end_z; ++z) {
                    sum += volume.getVoxelRaw1D(current_index);
                    current_index += slice_stride;
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
    getZBounds(volume.getDepth(), z_min, z_max, start_z, end_z);

    int width = volume.getWidth();
    int height = volume.getHeight();
    int channels = volume.getChannels();
    Image result(width, height, channels);

    int num_slices = (end_z - start_z) + 1;
    int target_median_count = num_slices / 2; 
    int slice_stride = width * height * channels; 

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                
                int histogram[256] = {0}; 
                
                // Calculate the base 1D index for (x, y, z=0, c) exactly ONCE
                int base_index = (y * width * channels) + (x * channels) + c;
                
                // Calculate the starting index based on z_min
                int current_index = base_index + (start_z * slice_stride);
                
                for (int z = start_z; z <= end_z; ++z) {
                    // get raw value, ignoring clamps as we are inside bounds
                    unsigned char val = volume.getVoxelRaw1D(current_index);
                    histogram[val]++;
                    
                    // jump to the same pixel in the next slice
                    current_index += slice_stride; 
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
