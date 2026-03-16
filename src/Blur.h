#ifndef BLUR_H
#define BLUR_H

#include "Filter.h"


// Single blur class containing all blur methods
class Blur {
public:
    // Box blur: average of neighbouring pixels
    void applyBoxBlur(Image& img, int kernelSize);

    // Gaussian blur: weighted average of neighbouring pixels
    // sigma has default value 2.0 as required
    void applyGaussianBlur(Image& img, int kernelSize, double sigma = 2.0);

    // Median blur: median of neighbouring pixels
    void applyMedianBlur(Image& img, int kernelSize);

private:
    // Helper function to check kernel size is valid
    bool isValidKernelSize(int kernelSize) const;
    // Returns the median value inside the kernel window for one channel.
    unsigned char getMedianValue(const Image& img, int centerX, int centerY, int channel, int kernelSize) const;
};

#endif
