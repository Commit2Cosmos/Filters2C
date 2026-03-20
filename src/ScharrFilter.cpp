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

#include "ScharrFilter.h"

ScharrFilter::ScharrFilter()
    : ConvFilter(
        // Gx
        {{  -3, 0,  3 },
         { -10, 0, 10 },
         {  -3, 0,  3 }},
        // Gy
        {{ -3, -10, -3 },
         {  0,   0,  0 },
         {  3,  10,  3 }})
{}

static constexpr float SCHARR_AXIS_MAX = 16.f * 255.f;

void ScharrFilter::apply(Image& image) {
    if (image.getChannels() > 1) image.toGrayscale();
    auto results = computeResults(image);
    float theoreticalMax = getTheoreticalMax(SCHARR_AXIS_MAX);
    normalise(results, theoreticalMax);
    writeResults(image, results);
}
