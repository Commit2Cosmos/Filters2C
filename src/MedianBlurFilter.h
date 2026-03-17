#ifndef MEDIAN_BLUR_FILTER_H
#define MEDIAN_BLUR_FILTER_H

#include "Filter.h"

class MedianBlurFilter : public Filter {
public:
    explicit MedianBlurFilter(int kernelSize);

    void apply(Image& image) override;

private:
    int kernelSize;

    bool isValidKernelSize(int kernelSize) const;
};

#endif
