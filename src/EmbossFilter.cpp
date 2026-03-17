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
    if (s == "RGB") return ColorSpace::RGB;
    if (s == "Grayscale") return ColorSpace::Grayscale;
    if (s == "HSV") return ColorSpace::HSV;
    throw std::invalid_argument("Unknown colorspace: " + s);
}


EmbossFilter::EmbossFilter(float strength, Direction dir, ColorSpace cs)
    : ConvFilter(rotateKernel(BASE_NW, static_cast<int>(dir)))
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
    if (m_colorspace != ColorSpace::RGB || img.getChannels() < 3) {
        ConvFilter::apply(img);
        return;
    }

    const int w = img.getWidth();
    const int h = img.getHeight();

    // Convert RGB to HSV.
    std::vector<float> hsvData = img.toHSV();

    // Extract the V channel
    Image vImg(w, h, 1);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float v = hsvData[(y * w + x) * 3 + 2];          // V in [0, 1]
            vImg.setPixel(x, y, 0,
                static_cast<unsigned char>(std::clamp(v * 255.f, 0.f, 255.f)));
        }
    }

    // Run the kernel on the V channel
    ConvFilter::apply(vImg);

    // Write the processed V values back into the HSV buffer
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float newV = vImg.getPixel(x, y, 0) / 255.f;     // back to [0, 1]
            hsvData[(y * w + x) * 3 + 2] = newV;
        }
    }

    // Convert modified HSV back to RGB
    img.updateFromHSV(hsvData);
}

std::vector<float> EmbossFilter::combineResponses(
    const std::vector<float>& respX,
    const std::vector<float>& /*respY*/) const   // single-kernel filter
{
    std::vector<float> result(respX.size());
    for (size_t i = 0; i < respX.size(); ++i) {
        float val = respX[i] * m_strength + 128.f;
        result[i] = std::clamp(val, 0.f, 255.f);
    }
    return result;
}
