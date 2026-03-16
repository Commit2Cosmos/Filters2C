#include "Threshold.h"
#include "Image.h"
#include <vector>

void Threshold::apply(Image& image)
{
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    // Threshold in HSV/HSL only makes sense for RGB images
    if (channels < 3)
        return;

    // Convert image to HSV
    std::vector<float> hsvData = image.toHSV();

    // Coursework slide says threshold = 127
    // Since V is stored in [0,1], convert 127 to [0,1]
    const float threshold = 127.0f / 255.0f;

    for (int i = 0; i < width * height; ++i)
    {
        int idx = i * 3;

        // hsvData[idx]     = H
        // hsvData[idx + 1] = S
        // hsvData[idx + 2] = V
        float& v = hsvData[idx + 2];

        if (v > threshold)
            v = 1.0f;
        else
            v = 0.0f;
    }

    // Convert modified HSV back to RGB
    image.updateFromHSV(hsvData);
}