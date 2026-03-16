#ifndef VOLUME_H
#define VOLUME_H

#include <vector>
#include <string>

// Forward declaration if needed
class Image; 

class Volume {
private:
    int width;   // x-axis
    int height;  // y-axis
    int depth;   // z-axis (number of image slices loaded)
    int channels; 
    
    // Stores raw 3D voxel data in a flat 1D array for memory efficiency
    std::vector<unsigned char> data; 

public:
    // Constructors
    Volume();
    Volume(int w, int h, int d, int c);
    ~Volume() = default;

    // File I/O
    // Takes a list of filenames (e.g., "scan_001.png", "scan_002.png") 
    // to build the volume in the z-direction
    bool loadFromFiles(const std::vector<std::string>& filepaths);

    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getDepth() const { return depth; }
    int getChannels() const { return channels; }
    const unsigned char* getData() const { return data.data(); }

    // Similar to Image::getPixel, but maps (x, y, z) to a 1D index
    unsigned char getVoxel(int x, int y, int z, int channel) const;
    void setVoxel(int x, int y, int z, int channel, unsigned char value);
};

#endif // VOLUME_H