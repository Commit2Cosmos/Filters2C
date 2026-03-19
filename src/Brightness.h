#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include "Filter.h"
#include <string>

class Brightness : public Filter
{
private:
    int value;   // brightness adjustment
    bool autoMode;

public:
    // Fixed brightness adjustment    
    Brightness(int v);

    // String-based constructor for CLI usage, e.g. "auto" or "50"
    Brightness(const std::string& mode);

    void apply(Image& image) override;
};

#endif