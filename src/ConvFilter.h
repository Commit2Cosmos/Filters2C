#ifndef CONV_FILTER_H
#define CONV_FILTER_H

#include "Filter.h"
#include "Image.h"
#include <vector>

using Kernel = std::vector<std::vector<float>>;

class ConvFilter : public Filter {
public:
    explicit ConvFilter(Kernel kernel);
    ConvFilter(Kernel kernelX, Kernel kernelY);
    virtual ~ConvFilter() = default;

    void apply(Image& image) override;

protected:
    std::vector<float> convolveChannel(
        const Image& image, int ch,
        const Kernel& kernel) const;

    virtual std::vector<float> combineResponses(
        const std::vector<float>& respX,
        const std::vector<float>& respY) const;

    std::vector<Kernel> kernels_;
    bool normalise_ = false;

};

#endif
