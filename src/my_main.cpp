#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>

#include "Image.h"
#include "GrayScale.h"
#include "Brightness.h"
#include "Threshold.h"
#include "HistogramEqualisation.h"
#include "SaltAndPepper.h"

int main(int argc, char* argv[])
{
    Image img;

    if (!img.load("Images/gracehopper.png")) {
        std::cerr << "Failed to load image." << std::endl;
        return 1;
    }

    std::cout << "Image loaded with size "
              << img.getWidth() << " x "
              << img.getHeight() << " with "
              << img.getChannels() << " channel(s)."
              << std::endl;

    // ---------------------------
    // 1. Greyscale
    // ---------------------------
    {
        Image grayImg = img;
        GrayScale grayFilter;
        grayFilter.apply(grayImg);

        if (!grayImg.save("output_grayscale.png")) {
            std::cerr << "Failed to save grayscale image." << std::endl;
            return 1;
        }
    }

    // ---------------------------
    // 2. Brightness
    // ---------------------------
    {
        Image brightImg = img;
        Brightness brightFilter(50);   // increase brightness by 50
        brightFilter.apply(brightImg);

        if (!brightImg.save("output_brightness.png")) {
            std::cerr << "Failed to save brightness image." << std::endl;
            return 1;
        }
    }

    // ---------------------------
    // 3. Threshold
    // ---------------------------
    {
        Image threshImg = img;
        Threshold thresholdFilter;
        thresholdFilter.apply(threshImg);

        if (!threshImg.save("output_threshold.png")) {
            std::cerr << "Failed to save threshold image." << std::endl;
            return 1;
        }
    }

    // ---------------------------
    // 4. Histogram Equalisation
    // ---------------------------
    {
        Image histImg = img;
        HistogramEqualisation histFilter;
        histFilter.apply(histImg);

        if (!histImg.save("output_histogram_equalisation.png")) {
            std::cerr << "Failed to save histogram equalised image." << std::endl;
            return 1;
        }
    }

    // ---------------------------
    // 5. Salt and Pepper Noise
    // ---------------------------
    {
        Image noiseImg = img;
        SaltAndPepper noiseFilter(0.10);   // 10% noise
        noiseFilter.apply(noiseImg);

        if (!noiseImg.save("output_salt_and_pepper.png")) {
            std::cerr << "Failed to save salt and pepper image." << std::endl;
            return 1;
        }
    }

    std::cout << "All filters applied successfully." << std::endl;
    return 0;
}