#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include "Filter.h"

class Brightness : public Filter
{
private:
    int value;   // brightness adjustment

public:
    Brightness(int v);

    void apply(Image& image) override;
};

#endif