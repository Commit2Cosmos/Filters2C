#ifndef THRESHOLD_H
#define THRESHOLD_H

#include "Filter.h"
#include <string>

class Threshold : public Filter
{
private:
    int thresholdValue;
    std::string colourSpace;

public:
    Threshold(int value = 127, const std::string& cs = "HSV");

    void apply(Image& image) override;
};

#endif