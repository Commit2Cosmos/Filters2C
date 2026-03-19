#ifndef CONV_FILTER_H
#define CONV_FILTER_H

#include "Filter.h"
#include "Image.h"
#include <vector>

struct Kernel {
    std::vector<float> data;
    int width;
    int height;

    // Kernel() = default;

    // Can use ConvFilter({{...}, {...}, {...})
    Kernel(std::initializer_list<std::initializer_list<float>> list) {
        height = static_cast<int>(list.size());
        width = (height > 0) ? static_cast<int>(list.begin()->size()) : 0;
        
        data.reserve(width * height);
        for (const auto& row : list) {
            for (float val : row) {
                data.push_back(val); // Flattens 2D input into 1D
            }
        }
    }
    
    Kernel(const Kernel&) = default;
    Kernel& operator=(const Kernel&) = default;

    // Move constructor and assignment
    Kernel(Kernel&&) = default;
    Kernel& operator=(Kernel&&) = default;
};

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
