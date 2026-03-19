#ifndef EMBOSS_FILTER_H
#define EMBOSS_FILTER_H

#include "ConvFilter.h"
#include "Image.h"
#include <vector>

/**
 * @class EmbossFilter
 * @brief Applies an embossing effect.
 *
 * Single-kernel filter that highlights edges as raised 3D-style relief.
 * Results are biased by +128 to centre the output around mid-grey.
 */
class EmbossFilter : public ConvFilter {
public:
    enum class Direction { NW, NE, SE, SW };
    enum class ColorSpace { Grayscale, HSV, HSL };

    explicit EmbossFilter(
        float strength = 1.0f,
        Direction dir = Direction::NW,
        ColorSpace cs = ColorSpace::HSV
    );

    static Direction parseDirection(const std::string& s);
    static ColorSpace parseColorSpace(const std::string& s);

    void apply(Image& img) override;
        
private:
    float m_strength;
    ColorSpace m_colorspace;

    static Kernel rotateKernel(const Kernel& k, int turns);
    
    friend class EmbossFilterTest;
};
        
#endif // EMBOSS_FILTER_H
