#include "Threshold.h"
#include "Image.h"
#include <vector>

void Threshold::apply(Image& image)
{
    int width = image.getWidth();
    int height = image.getHeight();
    int channels = image.getChannels();

    const int thresholdInt = 127;

    // Case 1: grayscale image
    if (channels == 1)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int pixel = image.getPixel(x, y, 0);
                pixel = (pixel > thresholdInt) ? 255 : 0;
                image.setPixel(x, y, 0, pixel);
            }
        }
        return;
    }

    // Case 2: RGB image
    if (channels >= 3)
    {
        std::vector<float> hslData = image.toHSL();
        const float threshold = 127.0f / 255.0f;

        for (int i = 0; i < width * height; i++)
        {
            int idx = i * 3;
            float& s = hslData[idx + 1];
            float& l = hslData[idx + 2];

            l = (l > threshold) ? 1.0f : 0.0f;
            s = 0.0f; // remove colour so output becomes black/white
        }

        image.updateFromHSL(hslData);
    }
}