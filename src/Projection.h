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

#ifndef PROJECTION_H
#define PROJECTION_H

#include "Volume.h"
#include "Image.h"

/**
 * @class Projection
 * @brief Generates 2-D projections from a 3-D image volume.
 *
 * All methods are static utility functions operating along the z-axis over an
 * inclusive slice range specified using 1-based indices.
 */
class Projection {
public:
    /**
     * @brief Compute a maximum-intensity projection.
     * @param volume Source volume.
     * @param z_min First slice in the inclusive 1-based range.
     * @param z_max Last slice in the inclusive 1-based range, or -1 for the end.
     * @return Projected 2-D image.
     */
    static Image applyMaxIP(const Volume& volume, int z_min = 1, int z_max = -1);

    /**
     * @brief Compute a minimum-intensity projection.
     * @param volume Source volume.
     * @param z_min First slice in the inclusive 1-based range.
     * @param z_max Last slice in the inclusive 1-based range, or -1 for the end.
     * @return Projected 2-D image.
     */
    static Image applyMinIP(const Volume& volume, int z_min = 1, int z_max = -1);

    /**
     * @brief Compute a mean-intensity projection.
     * @param volume Source volume.
     * @param z_min First slice in the inclusive 1-based range.
     * @param z_max Last slice in the inclusive 1-based range, or -1 for the end.
     * @return Projected 2-D image.
     */
    static Image applyMeanIP(const Volume& volume, int z_min = 1, int z_max = -1);

    /**
     * @brief Compute a median-intensity projection.
     * @param volume Source volume.
     * @param z_min First slice in the inclusive 1-based range.
     * @param z_max Last slice in the inclusive 1-based range, or -1 for the end.
     * @return Projected 2-D image.
     */
    static Image applyMedianIP(const Volume& volume, int z_min = 1, int z_max = -1);
};

#endif // PROJECTION_H
