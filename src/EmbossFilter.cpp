#include "EmbossFilter.h"
#include <algorithm>
#include <stdexcept>

static const Kernel BASE_NW = {
    { -2.f, -1.f,  0.f },
    { -1.f,  0.f,  1.f },
    {  0.f,  1.f,  2.f }
};

EmbossFilter::Direction EmbossFilter::parseDirection(const std::string& s) {
    if (s == "NW") return EmbossFilter::Direction::NW;
    if (s == "NE") return EmbossFilter::Direction::NE;
    if (s == "SE") return EmbossFilter::Direction::SE;
    if (s == "SW") return EmbossFilter::Direction::SW;
    throw std::invalid_argument("Unknown direction: " + s);
}

EmbossFilter::ColorSpace EmbossFilter::parseColorSpace(const std::string& s) {
    if (s == "Grayscale") return ColorSpace::Grayscale;
    if (s == "HSV") return ColorSpace::HSV;
    if (s == "HSL") return ColorSpace::HSL;
    throw std::invalid_argument("Unknown colorspace: " + s);
}


EmbossFilter::EmbossFilter(float strength, Direction dir, ColorSpace cs)
    : ConvFilter([&]{
        Kernel k = rotateKernel(BASE_NW, static_cast<int>(dir));
        
        for (auto& val : k.data) {
            val *= strength;
        }
        
        return k;
    }())
    , m_strength(strength)
    , m_colorspace(cs)
{}


Kernel EmbossFilter::rotateKernel(const Kernel& k, int turns) {
    Kernel k_mod = k;
    int n = k_mod.width;
    int w = n - 1;

    for (int t = 0; t < turns; t++) {
        for (int i = 0; i <= n / 2; i++) {
            for (int j = i; j < w - i; j++) {
                // Convert 2D [row][col] to 1D [row * width + col]
                float temp = k_mod.data[i * n + j];
                
                k_mod.data[i * n + j] = k_mod.data[(w - j) * n + i];
                k_mod.data[(w - j) * n + i] = k_mod.data[(w - i) * n + (w - j)];
                k_mod.data[(w - i) * n + (w - j)] = k_mod.data[j * n + (w - i)];
                k_mod.data[j * n + (w - i)] = temp;
            }
        }
    }
    return k_mod;
}


void EmbossFilter::apply(Image& img) {
    if (m_colorspace == ColorSpace::Grayscale || img.getChannels() < 3) {
        auto results = computeResults(img);
        for (auto& ch : results) {
            const int totalPixels = ch.size();
            float* chData = ch.data(); // Raw float pointer
            for (int i = 0; i < totalPixels; ++i) {
                chData[i] += 128.0f; 
            }
        }
        writeResults(img, results);
        return;
    }

    const int w = img.getWidth();
    const int h = img.getHeight();
    const int totalPixels = w * h;
    const bool useHSL = (m_colorspace == ColorSpace::HSL);

    // Convert RGB → HSV or HSL
    std::vector<float> csData = useHSL ? img.toHSL() : img.toHSV();

    // Extract intensity channel — V in HSV and L in HSL are both at index 2
    Image vImg(w, h, 1);
    unsigned char* vData = vImg.getDataUnsafe(); // Use non-const raw pointer getter
    const float* csPtr = csData.data();

    for (int i = 0; i < totalPixels; ++i) {
        float v = csPtr[i * 3 + 2]; 
        // Write directly to the 1D array instead of using setPixel
        vData[i] = static_cast<unsigned char>(std::clamp(v * 255.f, 0.f, 255.f));
    }

    // Filter the intensity channel
    auto results = computeResults(vImg);
    const float* resData = results[0].data(); // Get raw pointer of the 1st

    // Write biased result back into the float buffer
    for (int i = 0; i < totalPixels; ++i) {
        // Read from the flat results and apply bias
        float newV = (resData[i] + 128.0f) / 255.f;
        
        // Write directly back to the 3-channel vector
        csData[i * 3 + 2] = std::clamp(newV, 0.f, 1.f);
    }

    // Convert back to RGB
    if (useHSL)
        img.updateFromHSL(csData);
    else
        img.updateFromHSV(csData);
}