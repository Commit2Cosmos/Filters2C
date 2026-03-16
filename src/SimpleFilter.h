#ifndef SIMPLEFILTER_H
#define SIMPLEFILTER_H

#include "Filter.h"
#include "Image.h"

/**
 * @class SimpleFilter
 * @brief Implements simple image filters such as Greyscale.
 */
class SimpleFilter : public Filter {
public:

    // Override the abstract method from Filter
    void apply(Image& image) override;

};

#endif