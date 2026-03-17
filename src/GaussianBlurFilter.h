#ifndef GAUSSIAN_BLUR_FILTER_H
#define GAUSSIAN_BLUR_FILTER_H

#include "Filter.h"
#include <vector>

class Image;
class Volume;

class GaussianBlurFilter : public Filter {
private:
    int kernelSize;
    double sigma;

    bool isValidKernelSize(int kernelSize) const;
    std::vector<double> createGaussianKernel1D(int kernelSize, double sigma) const;

public:
    GaussianBlurFilter(int kernelSize, double sigma);

    void apply(Image& image) override;
    void apply(Volume& volume) override;
};

#endif // GAUSSIAN_BLUR_FILTER_H