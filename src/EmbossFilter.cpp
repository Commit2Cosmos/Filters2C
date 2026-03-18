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
        for (auto& row : k)
            for (auto& val : row)
                val *= strength;
        return k;
      }())
    , m_strength(strength)
    , m_colorspace(cs)
{}


Kernel EmbossFilter::rotateKernel(const Kernel& k, int turns) {
    Kernel k_mod = k;
    int n = static_cast<int>(k_mod.size());
    int w = n - 1;

    for (int t = 0; t < turns; t++) {
        for (int i = 0; i <= n/2; i++) {
            for (int j = i; j < w - i; j++) {
                float temp = k_mod[i][j];
                k_mod[i][j] = k_mod[w-j][i];
                k_mod[w-j][i] = k_mod[w-i][w-j];
                k_mod[w-i][w-j] = k_mod[j][w-i];
                k_mod[j][w-i] = temp;
            }
        }
    }
    return k_mod;
}


void EmbossFilter::apply(Image& img) {
    if (m_colorspace == ColorSpace::Grayscale || img.getChannels() < 3) {
        auto results = computeResults(img);
        for (auto& ch : results)
            for (auto& v : ch)
                v += 128.0f;
        writeResults(img, results);
        return;
    }

    const int w = img.getWidth();
    const int h = img.getHeight();
    const bool useHSL = (m_colorspace == ColorSpace::HSL);

    // Convert RGB → HSV or HSL
    std::vector<float> csData = useHSL ? img.toHSL() : img.toHSV();

    // Extract intensity channel — V in HSV and L in HSL are both at index 2
    Image vImg(w, h, 1);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            float v = csData[(y * w + x) * 3 + 2];   // [0, 1]
            vImg.setPixel(x, y, 0,
                static_cast<unsigned char>(std::clamp(v * 255.f, 0.f, 255.f)));
        }

    // Filter the intensity channel
    auto results = computeResults(vImg);

    // Write biased result back into the float buffer
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            float newV = (results[0][y * w + x] + 128.0f) / 255.f;
            csData[(y * w + x) * 3 + 2] = std::clamp(newV, 0.f, 1.f);
        }

    // Convert back to RGB
    if (useHSL)
        img.updateFromHSL(csData);
    else
        img.updateFromHSV(csData);
}