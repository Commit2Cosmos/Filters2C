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

#ifndef SLICE_H
#define SLICE_H

#include "Volume.h"
#include "Image.h"

/**
 * @class Slice
 * @brief Extracts orthogonal 2-D slices from a 3-D volume.
 *
 * The methods are static utilities and accept 1-based coordinates to match the
 * command-line interface.
 */
class Slice{

    public:
        /**
         * @brief Extract the x-z plane at a given y position.
         * @param volume Source volume.
         * @param y_coord_1_based Y coordinate using 1-based indexing.
         * @return Extracted slice image, or an empty image if out of range.
         */
        static Image extractXZ(const Volume& volume, int y_coord_1_based);

        /**
         * @brief Extract the y-z plane at a given x position.
         * @param volume Source volume.
         * @param x_coord_1_based X coordinate using 1-based indexing.
         * @return Extracted slice image, or an empty image if out of range.
         */
        static Image extractYZ(const Volume& volume, int x_coord_1_based);
};

#endif // SLICE_H
