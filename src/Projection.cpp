#include "Projection.h"
#include <iostream>
#include <algorithm> // Only for std::clamp (allowed) and std::swap

// Local Helper Functions
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


    // CUSTOM MEDIAN ALGORITHM: Quickselect
    // O(N) average case for finding a median.
    
    // Rearranges the array such that elements <= pivot are on the left,
    // and elements > pivot are on the right. Returns the final index of the pivot.
    int partition(std::vector<unsigned char>& arr, int left, int right) {
        // Choose rightmost element as the pivot value
        unsigned char pivot = arr[right];
        
        // 'i' tracks boundary where the next smaller-than-pivot element should go
        int i = left;
        
        for (int j = left; j < right; ++j) {
            // If current element smaller than or equal to pivot
            if (arr[j] <= pivot) {
                // Swap it into the "left" section of partition
                std::swap(arr[i], arr[j]);
                i++; // Move boundary forward
            }
        }
        
        // Place the pivot in final sorted position (between two partitions)
        std::swap(arr[i], arr[right]);
        return i; // Return index where the pivot settled
    }


    // Recursive function to find the element that would be at index 'k' if the array were sorted.
    // For median, 'k' typically arr.size() / 2.
    unsigned char quickSelectMedian(std::vector<unsigned char>& arr, int left, int right, int k) {
        // Base case: If the range contains only one element, we've found our target
        if (left == right) return arr[left];
        
        // Partition array and get position of the pivot
        int pivotIndex = partition(arr, left, right);
        
        // If pivot landed exactly on index 'k', we've found the k-th smallest element
        if (k == pivotIndex) {
            return arr[k];
        } 
        // If 'k' is less than the pivot index, the target is in the left partition
        else if (k < pivotIndex) {
            return quickSelectMedian(arr, left, pivotIndex - 1, k);
        } 
        // If 'k' is greater than the pivot index, the target is in the right partition
        else {
            return quickSelectMedian(arr, pivotIndex + 1, right, k);
        }
    }
}

// --- Maximum Intensity Projection ---
Image Projection::applyMaxIP(const Volume& volume, int z_min, int z_max) {
    int start_z, end_z;
    getZBounds(volume.getDepth(), z_min, z_max, start_z, end_z);

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
    getZBounds(volume.getDepth(), z_min, z_max, start_z, end_z);

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
    getZBounds(volume.getDepth(), z_min, z_max, start_z, end_z);

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
    getZBounds(volume.getDepth(), z_min, z_max, start_z, end_z);

    int width = volume.getWidth();
    int height = volume.getHeight();
    int channels = volume.getChannels();
    Image result(width, height, channels);

    int num_slices = (end_z - start_z) + 1;
    int median_index = num_slices / 2; 

    // Pre-allocate vector to hold values for sorting 
    std::vector<unsigned char> values(num_slices);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                
                // Collect all values in the z-direction
                for (int z = start_z; z <= end_z; ++z) {
                    values[z - start_z] = volume.getVoxel(x, y, z, c);
                }
                
                // Use custom Quickselect algorithm to find the median
                unsigned char median_val = quickSelectMedian(values, 0, num_slices - 1, median_index);
                
                result.setPixel(x, y, c, median_val);
            }
        }
    }
    return result;
}