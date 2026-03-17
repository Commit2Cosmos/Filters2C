#ifndef GAUSSIAN_BLUR_FILTER_H
#define GAUSSIAN_BLUR_FILTER_H

#include "Filter.h"

#include <vector>

class GaussianBlurFilter : public Filter {
public:
    explicit GaussianBlurFilter(int kernelSize, double sigma = 2.0);

    void apply(Image& image) override;

private:
    int kernelSize;
    double sigma;

    bool isValidKernelSize(int kernelSize) const;
    std::vector<std::vector<double>> createGaussianKernel(int kernelSize, double sigma) const;
};

#endif
