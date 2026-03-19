#ifndef VOLUME_H
#define VOLUME_H

#include <vector>
#include <string>

class Image; 

/**
 * @class Volume
 * @brief Stores a stack of image slices as a 3-D voxel volume.
 *
 * Voxel data is kept in a single flat array in z-major slice order for memory
 * efficiency and cache-friendly traversal during projections and filters.
 */
class Volume {
private:
    /// Volume width along the x-axis.
    int width;
    /// Volume height along the y-axis.
    int height;
    /// Number of slices along the z-axis.
    int depth;
    /// Number of channels per voxel.
    int channels; 
    
    /// Flat interleaved voxel buffer storing the full volume.
    std::vector<unsigned char> data; 

public:
    /**
     * @brief Construct an empty volume.
     */
    Volume();
    /**
     * @brief Construct a blank volume of the requested size.
     * @param w Volume width in voxels.
     * @param h Volume height in voxels.
     * @param d Volume depth in slices.
     * @param c Number of channels per voxel.
     */
    Volume(int w, int h, int d, int c);
    ~Volume() = default;

    /**
     * @brief Load a volume from an ordered list of image files.
     * @param filepaths Paths to the slices that make up the z-axis.
     * @return True on success, false if any slice fails to load or dimensions
     * are inconsistent.
     */
    bool loadFromFiles(const std::vector<std::string>& filepaths);

    /// @return Volume width in voxels.
    int getWidth() const { return width; }
    /// @return Volume height in voxels.
    int getHeight() const { return height; }
    /// @return Number of slices in the volume.
    int getDepth() const { return depth; }
    /// @return Number of channels per voxel.
    int getChannels() const { return channels; }
    /// @return Read-only pointer to the raw voxel buffer.
    const unsigned char* getData() const { return data.data(); }

    /**
     * @brief Read one voxel component using clamped border handling.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @param z Z-coordinate.
     * @param channel Channel index.
     * @return Requested channel value after coordinate clamping.
     */
    unsigned char getVoxel(int x, int y, int z, int channel) const;
    /**
     * @brief Read a voxel value directly from the flat storage buffer.
     * @param index Raw 1-D buffer index.
     * @return Stored byte at that index.
     */
    unsigned char getVoxelRaw1D(int index) const;
    /**
     * @brief Write one voxel component if the coordinates are in range.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @param z Z-coordinate.
     * @param channel Channel index.
     * @param value New 8-bit value.
     */
    void setVoxel(int x, int y, int z, int channel, unsigned char value);
};

#endif // VOLUME_H
