#ifndef SALTANDPEPPER_H
#define SALTANDPEPPER_H

#include "Filter.h"
#include "Image.h"

class SaltAndPepper : public Filter
{
public:
    explicit SaltAndPepper(double noiseRatio);

    void apply(Image& image) override;

private:
    double noiseRatio;
};

#endif