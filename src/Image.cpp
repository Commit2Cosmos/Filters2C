#include "Image.h"
#include <iostream>
#include <algorithm> // For std::clamp
#include <cmath>
#include "stb_image.h"
#include "stb_image_write.h"

// Constructors
Image::Image() : width(0), height(0), channels(0) {}

Image::Image(int w, int h, int c) : width(w), height(h), channels(c) {
    data.resize(w * h * c, 0); // Initialise with black pixels
}

Image::Image(const std::string& filepath) {
    load(filepath);
}


// File I/O
bool Image::load(const std::string& filepath) {
    unsigned char* raw_data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    if (!raw_data) {
        std::cerr << "Failed to load image: " << filepath << std::endl;
        return false;
    }
    // Copy data to vector to manage memory safely
    data.assign(raw_data, raw_data + (width * height * channels));
    stbi_image_free(raw_data);
    return true;
}


bool Image::save(const std::string& filepath) const {
    // Assuming PNG format. The third-party stb library handles the encoding.
    int stride = width * channels;
    int success = stbi_write_png(filepath.c_str(), width, height, channels, data.data(), stride);
    return success != 0;
}


// Pixel Access
unsigned char Image::getPixel(int x, int y, int channel) const {
    // if out of bounds clamp to edge pixel
    x = std::clamp(x, 0, width - 1);  
    y = std::clamp(y, 0, height - 1);
    
    int index = (y * width + x) * channels + channel;
    return data[index];
}

void Image::setPixel(int x, int y, int channel, unsigned char value) {
    if (x < 0 || x >= width || y < 0 || y >= height || channel < 0 || channel >= channels) {
        return; // Ignore out-of-bounds 
    }
    int index = (y * width + x) * channels + channel;
    data[index] = value;
}


void Image::toGrayscale() {
    if (channels == 1) return; // Already grayscale

    std::vector<unsigned char> newData(width * height);
    
    for (int i = 0; i < width * height; ++i) {
        int idx = i * channels;
        // Apply the required luminance formula
        float y_val = 0.2126f * data[idx] + 0.7152f * data[idx + 1] + 0.0722f * data[idx + 2];
        newData[i] = static_cast<unsigned char>(std::clamp(y_val, 0.0f, 255.0f));
    }
    
    // Replace old data and drop the alpha channel if it existed
    data = std::move(newData);
    channels = 1; 
}


std::vector<float> Image::toHSV() const {
    // store 3 floats per pixel (H, S, V)
    std::vector<float> hsv(width * height * 3);
    if (channels < 3) return hsv; // Fallback for grayscale

    for (int i = 0; i < width * height; ++i) {
        int idx = i * channels;

        // Normalise RGB values between 0.0 and 1.0
        float r = data[idx] / 255.0f;
        float g = data[idx + 1] / 255.0f;
        float b = data[idx + 2] / 255.0f;

        float cmax = std::max({r, g, b});
        float cmin = std::min({r, g, b});
        float delta = cmax - cmin;

        float h = 0.0f, s = 0.0f, v = cmax;

        if (delta > 0.0f) {
            if (cmax == r) h = 60.0f * std::fmod(((g - b) / delta), 6.0f);
            else if (cmax == g) h = 60.0f * (((b - r) / delta) + 2.0f);
            else if (cmax == b) h = 60.0f * (((r - g) / delta) + 4.0f);
            if (h < 0.0f) h += 360.0f; // Ensure hue is positive
            s = delta / cmax;
        }

        int hsv_idx = i * 3;
        hsv[hsv_idx] = h;
        hsv[hsv_idx + 1] = s;
        hsv[hsv_idx + 2] = v;
    }
    return hsv;
}


std::vector<float> Image::toHSL() const {
    // store 3 floats per pixel (H, S, L)
    std::vector<float> hsl(width * height * 3, 0.0f);
    
    if (channels < 3) return hsl; 

    for (int i = 0; i < width * height; ++i) {
        int idx = i * channels;
        
        // Normalise RGB values between 0.0 and 1.0
        float r = data[idx] / 255.0f;
        float g = data[idx + 1] / 255.0f;
        float b = data[idx + 2] / 255.0f;

        float cmax = std::max({r, g, b});
        float cmin = std::min({r, g, b});
        float delta = cmax - cmin;

        float h = 0.0f, s = 0.0f;
        
        // Lightness is the average of the largest and smallest color components
        float l = (cmax + cmin) / 2.0f;

        if (delta > 0.0f) {
            // Saturation calculation is different from HSV!
            s = delta / (1.0f - std::abs(2.0f * l - 1.0f));

            // Hue calculation remains the same as HSV
            if (cmax == r) { h = 60.0f * std::fmod(((g - b) / delta), 6.0f); } 
            else if (cmax == g) { h = 60.0f * (((b - r) / delta) + 2.0f); } 
            else if (cmax == b) { h = 60.0f * (((r - g) / delta) + 4.0f);}
            if (h < 0.0f) h += 360.0f; // Ensure hue is positive
        }

        int hsl_idx = i * 3;
        hsl[hsl_idx] = h;
        hsl[hsl_idx + 1] = s;
        hsl[hsl_idx + 2] = l;
    }
    
    return hsl;
}


void Image::updateFromHSV(const std::vector<float>& hsvData) {
    // Safety check: ensure we have an RGB image and the correct amount of HSV data
    if (channels < 3) return; // Safety check

    for (int i = 0; i < width * height; ++i) {
        int hsv_idx = i * 3;
        float h = hsvData[hsv_idx];
        float s = hsvData[hsv_idx + 1];
        float v = hsvData[hsv_idx + 2];

        float c = v * s;
        float x_val = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = v - c;

        float r = 0.0f, g = 0.0f, b = 0.0f;

        // Determine RGB components based on the 60-degree sector of the hue wheel
        if (h >= 0 && h < 60) { r = c; g = x_val; b = 0; }
        else if (h >= 60 && h < 120) { r = x_val; g = c; b = 0; }
        else if (h >= 120 && h < 180) { r = 0; g = c; b = x_val; }
        else if (h >= 180 && h < 240) { r = 0; g = x_val; b = c; }
        else if (h >= 240 && h < 300) { r = x_val; g = 0; b = c; }
        else if (h >= 300 && h < 360) { r = c; g = 0; b = x_val; }

        int idx = i * channels;
        data[idx] = static_cast<unsigned char>((r + m) * 255.0f);
        data[idx + 1] = static_cast<unsigned char>((g + m) * 255.0f);
        data[idx + 2] = static_cast<unsigned char>((b + m) * 255.0f);
        // Alpha channel remains untouched if it exists (channels == 4)
    }
}


void Image::updateFromHSL(const std::vector<float>& hslData) {
    // Safety check: ensure we have an RGB image and the correct amount of HSL data
    if (channels < 3 || hslData.size() != width * height * 3) return;

    for (int i = 0; i < width * height; ++i) {
        int hsl_idx = i * 3;
        float h = hslData[hsl_idx];
        float s = hslData[hsl_idx + 1];
        float l = hslData[hsl_idx + 2];

        // Reverse the math to get Chroma (C) and the intermediate value (X)
        float c = (1.0f - std::abs(2.0f * l - 1.0f)) * s;
        float x_val = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = l - c / 2.0f;

        float r = 0.0f, g = 0.0f, b = 0.0f;

        // Determine RGB components based on the 60-degree sector of the hue wheel
        if (h >= 0.0f && h < 60.0f) { r = c; g = x_val; b = 0.0f; }
        else if (h >= 60.0f && h < 120.0f) { r = x_val; g = c; b = 0.0f; }
        else if (h >= 120.0f && h < 180.0f) { r = 0.0f; g = c; b = x_val; }
        else if (h >= 180.0f && h < 240.0f) { r = 0.0f; g = x_val; b = c; }
        else if (h >= 240.0f && h < 300.0f) { r = x_val; g = 0.0f; b = c; }
        else if (h >= 300.0f && h < 360.0f) { r = c; g = 0.0f; b = x_val; }

        int idx = i * channels;
        
        // Add m, scale back to 255, and safely cast to unsigned char using std::clamp 
        // to prevent unexpected floating-point overflows or underflows.
        data[idx] = static_cast<unsigned char>(std::clamp((r + m) * 255.0f, 0.0f, 255.0f));
        data[idx + 1] = static_cast<unsigned char>(std::clamp((g + m) * 255.0f, 0.0f, 255.0f));
        data[idx + 2] = static_cast<unsigned char>(std::clamp((b + m) * 255.0f, 0.0f, 255.0f));
        // Alpha channel remains untouched if it exists (channels == 4)

    }
}