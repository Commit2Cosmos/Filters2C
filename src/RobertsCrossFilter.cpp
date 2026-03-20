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

#include "RobertsCrossFilter.h"

RobertsCrossFilter::RobertsCrossFilter()
    : ConvFilter(
        // G1
        {{ 1,  0 },
         { 0, -1 }},
        // G2
        {{  0, 1 },
         { -1, 0 }})
{}

static constexpr float ROBERTS_AXIS_MAX = 1.f * 255.f;

void RobertsCrossFilter::apply(Image& image) {
    if (image.getChannels() > 1) image.toGrayscale();
    auto results = computeResults(image);
    float theoreticalMax = getTheoreticalMax(ROBERTS_AXIS_MAX);
    normalise(results, theoreticalMax);
    writeResults(image, results);
}
