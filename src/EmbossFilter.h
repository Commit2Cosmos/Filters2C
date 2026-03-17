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
 * Results are biased by +128 to centre the output around mid-grey,
 * so combineResponses is not used and normalise is overridden.
 */
class EmbossFilter : public ConvFilter {
public:
    enum Direction { NW, NE, SE, SW };
    enum class ColorSpace  { RGB, Grayscale, HSV };

    explicit EmbossFilter(
        float strength = 1.0f,
        Direction dir = Direction::NW,
        ColorSpace cs = ColorSpace::RGB
    );

    static Direction parseDirection(const std::string& s);
    static ColorSpace parseColorSpace(const std::string& s);

    void apply(Image& img) override;

protected:
    // Overridden to shift output by +128 (centres result around mid-grey).
    // Without this, flat areas map to 0 (black) instead of 128 (grey).
    std::vector<float> combineResponses(
        const std::vector<float>& respX,
        const std::vector<float>& respY) const override;
        
private:
    float m_strength;
    ColorSpace m_colorspace;

    static Kernel rotateKernel(const Kernel& k, int turns);
    
    friend class EmbossFilterTest;
};
        
#endif // EMBOSS_FILTER_H
