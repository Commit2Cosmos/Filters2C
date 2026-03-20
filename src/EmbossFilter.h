/*
Kruskal
Xinying Yang ada-xy325
Oliver Whitehead ada-ow25
Anton Belov ada-ab2125 (ada-85a5785b61)
Hao Liu ada-hl4425
Jiayu Wu ada-jw1125
Yuhang Dai ada-yd1125
Julia Williams ada-jjw25
*/

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
    /**
     * @enum Direction
     * @brief Chooses the apparent light direction for the emboss effect.
     */
    enum class Direction { NW, NE, SE, SW };
    /**
     * @enum ColorSpace
     * @brief Chooses where embossing is applied for colour images.
     */
    enum class ColorSpace { Grayscale, HSV, HSL };

    /**
     * @brief Construct an emboss filter.
     * @param strength Scalar multiplier applied to the emboss kernel.
     * @param dir Direction of the simulated light source.
     * @param cs Colour handling mode for multi-channel images.
     */
    explicit EmbossFilter(
        float strength = 1.0f,
        Direction dir = Direction::NW,
        ColorSpace cs = ColorSpace::HSV
    );

    /**
     * @brief Parse a direction keyword used by the CLI.
     * @param s Direction string such as "NW" or "SE".
     * @return Matching emboss direction.
     */
    static Direction parseDirection(const std::string& s);
    /**
     * @brief Parse a colour-space keyword used by the CLI.
     * @param s Colour-space string such as "Grayscale", "HSV", or "HSL".
     * @return Matching emboss colour-space mode.
     */
    static ColorSpace parseColorSpace(const std::string& s);

    /**
     * @brief Apply the emboss effect to an image in-place.
     * @param img Image to modify.
     */
    void apply(Image& img) override;
        
private:
    /// User-selected strength multiplier for the emboss kernel.
    float m_strength;
    /// Colour handling mode used when processing RGB images.
    ColorSpace m_colorspace;

    /**
     * @brief Rotate a square emboss kernel in 90-degree steps.
     * @param k Kernel to rotate.
     * @param turns Number of clockwise quarter-turns.
     * @return Rotated kernel.
     */
    static Kernel rotateKernel(const Kernel& k, int turns);
    
    friend class EmbossFilterTest;
};
        
#endif // EMBOSS_FILTER_H
