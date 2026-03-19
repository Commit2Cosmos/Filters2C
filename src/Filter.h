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
    /// Virtual destructor for safe polymorphic cleanup.
    virtual ~Filter() = default;

    /**
     * @brief Apply the filter to a 2-D image.
     * @param image Image to modify.
     */
    virtual void apply(Image& image) = 0;

    /**
     * @brief Apply the filter to a 3-D volume.
     * @param volume Volume to modify.
     *
     * The base implementation does nothing so image-only filters are not forced
     * to implement 3-D behaviour.
     */
    virtual void apply(Volume& volume) {
    }
};

#endif // FILTER_H
