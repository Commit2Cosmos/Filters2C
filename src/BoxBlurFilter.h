#ifndef BOX_BLUR_FILTER_H
#define BOX_BLUR_FILTER_H

#include "Filter.h"

class BoxBlurFilter : public Filter {
public:
    explicit BoxBlurFilter(int kernelSize);

    void apply(Image& image) override;

private:
    int kernelSize;

    bool isValidKernelSize(int kernelSize) const;
};

#endif
