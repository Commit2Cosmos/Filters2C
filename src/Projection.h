#ifndef PROJECTION_H
#define PROJECTION_H

#include "Volume.h"
#include "Image.h"

class Projection {
public:
    // Maximum Intensity Projection (MIP)
    // Finds the maximum intensity of all pixels with a given (x,y) in the z-direction 
    static Image applyMaxIP(const Volume& volume, int z_min = 1, int z_max = -1);

    // Minimum Intensity Projection (MinIP)
    // Finds the minimum intensity of all pixels with a given (x,y) in the z-direction
    static Image applyMinIP(const Volume& volume, int z_min = 1, int z_max = -1);

    // Mean Intensity Projection (MeanIP)
    // Finds the average (mean) intensity of all pixels with a given (x,y) in the z-direction
    static Image applyMeanIP(const Volume& volume, int z_min = 1, int z_max = -1);

    // Median Intensity Projection (MedianIP)
    // Finds the median intensity; requires custom median-finding algorithm
    static Image applyMedianIP(const Volume& volume, int z_min = 1, int z_max = -1);
};

#endif // PROJECTION_H