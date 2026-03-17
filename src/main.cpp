// A minimal example file showing how to include the stb image headers,
// and how to read and write image files using these headers.
//
// Compilation with (e.g.): 
//    g++-15 main.cpp -o main [MacOS]
//    g++ main.cpp -o main [Ubuntu/WSL]
//
// Minimal example written by T.M. Davison (2023-26) [CHANGE THIS!]

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>

// Uncomment these as you start to implement the different classes
// #include "Image.h"
// #include "Filter.h"
// #include "Volume.h"
// #include "Projection.h"
// #include "Slice.h"

int main(int argc, char* argv[]) {

    /* 
    You need to change all of the code in this main function; it is just a placeholder
    to demonstrate how to read and write images with the stbi header files.
    */
    int w, h, c;
    unsigned char* data;

    // Read in image file
    data = stbi_load("../Images/small.png", &w, &h, &c, 0);

    // Print image size to screen
    std::cout << "Image loaded with size " << w << " x " << h << " with " << c << " channel(s)." << std::endl; 

    // Save image to new filename
    int success = stbi_write_png("output.png", w, h, c, data, 0);

    // Deallocate memory
    stbi_image_free(data);

    return 0;

}