#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include "Filter.h"
#include <string>

/**
 * @class Brightness
 * @brief Adjusts image brightness by a fixed amount or automatic estimate.
 *
 * The filter either adds a user-supplied offset to every pixel or computes an
 * offset that shifts the average intensity toward a mid-level target.
 */
class Brightness : public Filter
{
private:
    /// Fixed intensity offset applied when auto mode is disabled.
    int value;
    /// Enables automatic brightness correction based on image mean intensity.
    bool autoMode;

public:
    /**
     * @brief Construct a fixed brightness adjustment.
     * @param v Intensity offset added to each channel value.
     */
    Brightness(int v);

    /**
     * @brief Construct from a CLI-friendly string mode.
     * @param mode Either "auto" for automatic correction or a numeric string
     * representing the fixed offset.
     */
    Brightness(const std::string& mode);

    /**
     * @brief Apply the brightness adjustment in-place.
     * @param image Image to modify.
     */
    void apply(Image& image) override;
};

#endif
