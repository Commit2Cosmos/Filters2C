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
    unsigned char* getData() { return data.data(); }
 
    // Pixel Access
    // Consider adding boundary checks here to handle the "extend" edge requirement
    unsigned char getPixel(int x, int y, int channel) const;
    void setPixel(int x, int y, int channel, unsigned char value);
 
   
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