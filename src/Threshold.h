#ifndef THRESHOLD_H
#define THRESHOLD_H

#include "Filter.h"

class Threshold : public Filter
{
public:
    void apply(Image& image) override;
};

#endif