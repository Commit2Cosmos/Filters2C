#ifndef GRAYSCALE_H
#define GRAYSCALE_H

#include "Filter.h"
#include "Image.h"

class GrayScale : public Filter
{
public:
    void apply(Image& image) override;
};

#endif