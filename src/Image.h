#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <string>
#include <algorithm>

/**
 * @class Image
 * @brief Stores and manipulates a 2-D image in interleaved channel format.
 *
 * The class owns pixel memory, supports loading and saving image files, and
 * provides helper methods used by the filter pipeline.
 */
class Image {
private:
    /// Image width in pixels.
    int width;
    /// Image height in pixels.
    int height;
    /// Number of channels per pixel, for example 1, 3, or 4.
    int channels;
    /// Raw interleaved pixel buffer in row-major order.
    std::vector<unsigned char> data;

public:
    /**
     * @brief Construct an empty image.
     */
    Image();
    /**
     * @brief Construct a blank image of the requested size.
     * @param w Image width in pixels.
     * @param h Image height in pixels.
     * @param c Number of channels per pixel.
     */
    Image(int w, int h, int c);
    /**
     * @brief Construct an image by loading it from disk.
     * @param filepath Path to the source image file.
     */
    Image(const std::string& filepath);
    ~Image() = default;

    /**
     * @brief Load image data from a file.
     * @param filepath Path to the source image file.
     * @return True on success, false if loading fails.
     */
    bool load(const std::string& filepath);
    /**
     * @brief Save image data to a file.
     * @param filepath Output path, written using stb_image_write.
     * @return True on success, false if writing fails.
     */
    bool save(const std::string& filepath) const;

    /// @return Image width in pixels.
    int getWidth() const { return width; }
    /// @return Image height in pixels.
    int getHeight() const { return height; }
    /// @return Number of channels per pixel.
    int getChannels() const { return channels; }
    /// @return Read-only pointer to the raw pixel buffer.
    const unsigned char* getData() const { return data.data(); }
    /// @return Writable pointer to the raw pixel buffer for performance-critical code.
    unsigned char* getDataUnsafe() { return data.data(); }

    /**
     * @brief Read a pixel component without bounds checking.
     * @param x X-coordinate in pixels.
     * @param y Y-coordinate in pixels.
     * @param channel Channel index within the pixel.
     * @return Requested channel value.
     */
    inline unsigned char getPixelSimple(int x, int y, int channel) const {
        return data[(y * width + x) * channels + channel];
    } 

    /**
     * @brief Read a pixel component using clamped border handling.
     * @param x X-coordinate in pixels.
     * @param y Y-coordinate in pixels.
     * @param channel Channel index within the pixel.
     * @return Requested channel value after clamping coordinates.
     */
    inline unsigned char getPixel(int x, int y, int channel) const {
        x = std::clamp(x, 0, width - 1);  
        y = std::clamp(y, 0, height - 1);
        return data[(y * width + x) * channels + channel];
    }

    /**
     * @brief Write one channel of one pixel if the coordinates are in range.
     * @param x X-coordinate in pixels.
     * @param y Y-coordinate in pixels.
     * @param channel Channel index within the pixel.
     * @param value New 8-bit value.
     */
    inline void setPixel(int x, int y, int channel, unsigned char value) {
        if (x < 0 || x >= width || y < 0 || y >= height || channel < 0 || channel >= channels) return;
        data[(y * width + x) * channels + channel] = value;
    }
    
    /**
     * @brief Convert the stored data to a single-channel grayscale image.
     *
     * Alpha is discarded if present.
     */
    void toGrayscale(); 

    /**
     * @brief Convert RGB image data to HSV triplets.
     * @return Separate floating-point buffer storing H, S, and V for each pixel.
     */
    std::vector<float> toHSV() const;
    /**
     * @brief Convert RGB image data to HSL triplets.
     * @return Separate floating-point buffer storing H, S, and L for each pixel.
     */
    std::vector<float> toHSL() const;

    /**
     * @brief Replace RGB data with values converted back from HSV.
     * @param hsvData Flattened H, S, V triplets for every pixel.
     */
    void updateFromHSV(const std::vector<float>& hsvData);
    /**
     * @brief Replace RGB data with values converted back from HSL.
     * @param hslData Flattened H, S, L triplets for every pixel.
     */
    void updateFromHSL(const std::vector<float>& hslData);
};

#endif // IMAGE_H
