/*
Kruskal
Xinying Yang ada-xy325
Oliver Whitehead ada-ow25
Anton Belov ada-ab2125 (ada-85a5785b61)
Hao Liu ada-hl4425
Jiayu Wu ada-jw1125
Yuhang Dai ada-yd1125
Julia Williams ada-jjw25
*/

#include "SobelFilter.h"

SobelFilter::SobelFilter()
    : ConvFilter(
        // Gx
        {{ -1, 0, 1 },
         { -2, 0, 2 },
         { -1, 0, 1 }},
        // Gy
        {{ -1, -2, -1 },
         {  0,  0,  0 },
         {  1,  2,  1 }})
{}

// Sobel: Gx positive weights = 1+2+1 = 4
static constexpr float SOBEL_AXIS_MAX = 4.f * 255.f;

void SobelFilter::apply(Image& image) {
    if (image.getChannels() > 1) image.toGrayscale();
    auto results = computeResults(image);
    float theoreticalMax = getTheoreticalMax(SOBEL_AXIS_MAX);
    normalise(results, theoreticalMax);
    writeResults(image, results);
}
