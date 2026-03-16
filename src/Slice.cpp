#include <iostream>

#include "Slice.h"

Image Slice::extractXZ(const Volume& volume, int y_coord_1_based) {
    // convert user's 1-based input to 0-based index
    int y_idx = y_coord_1_based - 1; 

    // Safety bounds check
    if (y_idx < 0 || y_idx >= volume.getHeight()) {
        return Image(); // Return an empty image
    }

    int out_width = volume.getWidth();   // x becomes the new horizontal
    int out_height = volume.getDepth();  // z becomes the new vertical
    int channels = volume.getChannels();

    // create image of correct size
    Image result(out_width, out_height, channels);

    for (int z = 0; z < out_height; ++z) {
        for (int x = 0; x < out_width; ++x) {
            
            // To make z=0 (index) appear at the BOTTOM left, we must write 
            // it to the LAST row of the image memory.
            int img_y = (out_height - 1) - z; 

            for (int c = 0; c < channels; ++c) {
                // Read from the 3D volume at (x, fixed_y, z)
                unsigned char voxel_val = volume.getVoxel(x, y_idx, z, c);
                
                // Write to the 2D image at (x, inverted_z, c)
                result.setPixel(x, img_y, c, voxel_val);
            }
        }
    }

    return result;
}

Image Slice::extractYZ(const Volume& volume, int x_coord_1_based){
    
    // convert user's 1-based input to 0-based index
    int x_idx = x_coord_1_based - 1;

    // Safety bounds check
    if (x_idx < 0 || x_idx >= volume.getWidth()) {
        std::cerr << "Error: x-coordinate out of bounds for slicing." << std::endl;
        return Image(); // Return an empty image
    }
    
    // For the y-z plane, the volume's height (y) becomes the output image's width 
    int out_width = volume.getHeight();  
    int out_height = volume.getDepth();  // z remains the vertical axis 
    int channels = volume.getChannels();

    // create image of correct size
    Image result(out_width, out_height, channels);

    for (int z = 0; z < out_height; ++z) {
        for (int y = 0; y < out_width; ++y) {
            
            // Just like the x-z plane, we must invert z-axis when writing to memory
            // so that z=0 (which is z=1 to the user) appears at BOTTOM left.
            int img_y = (out_height - 1) - z; 

            for (int c = 0; c < channels; ++c) {
                // Read from the 3D volume at (fixed_x, y, z)
                unsigned char voxel_val = volume.getVoxel(x_idx, y, z, c);
                
                // Write to the 2D image at (y, inverted_z, c) to result image
                result.setPixel(y, img_y, c, voxel_val);
            }
        }
    }
}
