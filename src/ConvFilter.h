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
    std::vector<float> convolveChannel(const Image& image, int ch) const;

    std::vector<std::vector<float>> computeResults(const Image& image) const;
    void normalise(std::vector<std::vector<float>>& results, float theoreticalMax) const;
    void writeResults(Image& image, const std::vector<std::vector<float>>& results) const;

    std::vector<Kernel> kernels_;
    
    virtual float getTheoreticalMax(float filterMax) const;
};

#endif
