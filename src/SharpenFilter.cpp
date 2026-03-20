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

#include "SharpenFilter.h"

SharpenFilter::SharpenFilter()
    : ConvFilter(
        // Combined kernel: computes (I_original + G)
        {{  0, -1,  0 },
         { -1,  5, -1 },
         {  0, -1,  0 }}
    )
{}
