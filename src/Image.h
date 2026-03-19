#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <string>

class Image {
private:
    int width;
    int height;
    int channels; // e.g., 1 for Grayscale, 3 for RGB, 4 for RGBA
    std::vector<unsigned char> data; // Stores the raw pixel data

public:
    // Constructors & Destructor
    Image();
    Image(int w, int h, int c);
    Image(const std::string& filepath); // Uses stb_image to load
    ~Image() = default;

    // File I/O
    bool load(const std::string& filepath);
    bool save(const std::string& filepath) const; // Uses stb_image_write

    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getChannels() const { return channels; }
    const unsigned char* getData() const { return data.data(); }
    // Read/write for optimisations
    unsigned char* getDataUnsafe() { return data.data(); }

    // Pixel Access (no boundary checks)
    inline unsigned char getPixelSimple(int x, int y, int channel) const {
        return data[(y * width + x) * channels + channel];
    } 

    // Pixel Access (clamping for "extend" strategy)
    inline unsigned char getPixel(int x, int y, int channel) const {
        x = std::clamp(x, 0, width - 1);  
        y = std::clamp(y, 0, height - 1);
        return data[(y * width + x) * channels + channel];
    }

    // Pixel Setter
    inline void setPixel(int x, int y, int channel, unsigned char value) {
        if (x < 0 || x >= width || y < 0 || y >= height || channel < 0 || channel >= channels) return;
        data[(y * width + x) * channels + channel] = value;
    }
    
    // --- Color Space Conversions ---

    // Converts the current image to Grayscale (1 channel)
    // Must drop the Alpha channel if it exists 
    void toGrayscale(); 

    // Note: HSV values often require floats (H: 0-360, S: 0.0-1.0, V: 0.0-1.0).
    std::vector<float> toHSV() const; // return a separate vector of floats rather than overwriting
    std::vector<float> toHSL() const; // return a separate vector of floats rather than overwriting

    // Takes modified HSV data and updates the Image's RGB unsigned char data
    void updateFromHSV(const std::vector<float>& hsvData);
    void updateFromHSL(const std::vector<float>& hslData);
};

#endif // IMAGE_H