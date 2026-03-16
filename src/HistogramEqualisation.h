#ifndef HISTOGRAMEQUALISATION_H
#define HISTOGRAMEQUALISATION_H

#include "Filter.h"
#include "Image.h"
#include <vector>

class HistogramEqualisation : public Filter
{
public:
    void apply(Image& image) override;

private:
    std::vector<unsigned char> buildEqualisationLUT(const std::vector<unsigned char>& values) const;
};

#endif