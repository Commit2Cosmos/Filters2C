#ifndef FILTER_H
#define FILTER_H

// Forward declarations to avoid circular dependencies
class Image; 
class Volume;

/**
 * @class Filter
 * @brief Abstract base class for all image and volume filters.
 */
class Filter {
public:
    // Virtual destructor ensures derived class destructors are called correctly
    virtual ~Filter() = default;

    // Pure virtual function makes this an abstract class.
    // Every specific filter MUST implement this method for 2D Images.
    virtual void apply(Image& image) = 0;

    // Overloaded virtual function for 3D Volumes.
    // We provide a default implementation that does nothing (or could throw an error)
    // so that 2D-only filters don't are not forced to implement 3D logic.
    virtual void apply(Volume& volume) {
        // Default implementation: do nothing or throw an exception.
        // 3D filters (like 3D Gaussian or 3D Median) will override this.
    }
};

#endif // FILTER_H