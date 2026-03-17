#ifndef SLICE_H
#define SLICE_H

#include "Volume.h"
#include "Image.h"

class Slice{

    public:
        // doesn't need constructor or destructor as not instantiated
        // call the methods directly using Slice::

        // Extracts a x-z plane for a given y-coordinate
        static Image extractXZ(const Volume& volume, int y_coord_1_based);

        // Extracts a y-z plane for a given x-coordinate.
        static Image extractYZ(const Volume& volume, int x_coord_1_based);
};

#endif // SLICE_H