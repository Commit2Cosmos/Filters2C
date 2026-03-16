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

    // Convert image to HSL
    std::vector<float> hslData = image.toHSL();

    // Coursework slide says threshold = 127
    // Since L is stored in [0,1], convert 127 to [0,1]
    const float threshold = 127.0f / 255.0f;

    for (int i = 0; i < width * height; ++i)
    {
        int idx = i * 3;

        // hslData[idx]     = H
        // hslData[idx + 1] = S
        // hslData[idx + 2] = L
        float& h = hslData[idx];
        float& s = hslData[idx + 1];
        float& l = hslData[idx + 2];

        if (l > threshold)
            l = 1.0f;
        else
            l = 0.0f;
    }

    // Convert modified HSL back to RGB
    image.updateFromHSL(hslData);
}