#ifndef HISTOGRAMEQUALISATION_H
#define HISTOGRAMEQUALISATION_H

#include "Filter.h"
#include <string>
#include <vector>

class HistogramEqualisation : public Filter
{
private:
    std::string colourSpace;

    std::vector<unsigned char> buildEqualisationLUT(
        const std::vector<unsigned char>& values) const;

public:
    HistogramEqualisation(const std::string& cs = "HSV");

    void apply(Image& image) override;
};

#endif