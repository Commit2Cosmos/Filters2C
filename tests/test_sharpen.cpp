// SharpenTest.cpp
// Unit tests for SharpenFilter.

#include "SharpenFilter.h"
#include "Image.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

// ---------------------------------------------------------------------------
// Minimal test harness
// ---------------------------------------------------------------------------
static int g_failCount = 0;

#define CHECK(cond, msg)                                                      \
    do {                                                                      \
        if (!(cond)) {                                                        \
            std::fprintf(stderr, "FAIL [%s:%d]: %s\n",                       \
                         __FILE__, __LINE__, msg);                            \
            ++g_failCount;                                                    \
        }                                                                     \
    } while (0)

#define CHECK_FLOAT_EQ(a, b, msg)                                             \
    do {                                                                      \
        float _a = static_cast<float>(a);                                     \
        float _b = static_cast<float>(b);                                     \
        if (std::fabs(_a - _b) > 1e-4f) {                                    \
            std::fprintf(stderr,                                              \
                "FAIL [%s:%d]: %s (got %.4f, expected %.4f)\n",               \
                __FILE__, __LINE__, msg, _a, _b);                             \
            ++g_failCount;                                                    \
        }                                                                     \
    } while (0)

// ---------------------------------------------------------------------------
// Test-access shim.
// Inherits SharpenFilter to legally expose protected convolveChannel/kernels_.
// ---------------------------------------------------------------------------
class SharpenFilterTest : public SharpenFilter {
public:
    // Runs the single convolution kernel stored in kernels_[0].
    std::vector<float> runConvolveChannel(const Image& img, int ch) const {
        return convolveChannel(img, ch, kernels_[0]);
    }
};

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Build a 3×3 single-channel Image from a flat 9-element row-major array.
static Image make3x3(const uint8_t px[9]) {
    Image img(3, 3, 1);
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            img.setPixel(x, y, 0, px[y * 3 + x]);
    return img;
}

// Fill every pixel in every channel with a constant value.
static void fillImage(Image& img, uint8_t v) {
    for (int c = 0; c < img.getChannels(); ++c)
        for (int y = 0; y < img.getHeight(); ++y)
            for (int x = 0; x < img.getWidth(); ++x)
                img.setPixel(x, y, c, v);
}

// ---------------------------------------------------------------------------
// Analytical dot-product of the sharpening kernel.
// ---------------------------------------------------------------------------
static float dotSharp(const float p[3][3]) {
    return           (-1.f * p[0][1])
         + (-1.f * p[1][0]) + (5.f * p[1][1]) + (-1.f * p[1][2])
         +           (-1.f * p[2][1]);
}

// ---------------------------------------------------------------------------
// Section 1 – Analytical dot-product validation (no Image required)
// ---------------------------------------------------------------------------

void testDotSharp_FlatPatch() {
    // A constant field: I_sharp must equal I_original.
    float p[3][3] = {
        { 100.f, 100.f, 100.f },
        { 100.f, 100.f, 100.f },
        { 100.f, 100.f, 100.f }
    };
    CHECK_FLOAT_EQ(dotSharp(p), 100.f,
                   "dotSharp: flat patch => 100 (identity preserved)");
}

void testDotSharp_LinearRamp() {
    // A linear surface has zero Laplacian (second derivative = 0),
    // so I_sharp = I at interior pixels.  Centre value is 4.
    //
    //  0  1  2
    //  3  4  5   →  dotSharp = -1 + (-3 + 20 + -5) + -7 = 4
    //  6  7  8
    float p[3][3] = {
        { 0.f, 1.f, 2.f },
        { 3.f, 4.f, 5.f },
        { 6.f, 7.f, 8.f }
    };
    CHECK_FLOAT_EQ(dotSharp(p), 4.f,
                   "dotSharp: linear ramp centre => 4 (zero Laplacian)");
}

void testDotSharp_CentreHeavyPatch() {
    // Prominent centre surrounded by uniform neighbours.
    //
    //  10  20  10
    //  20  50  20   →  G = 4*50 − 4*20 = 120
    //  10  20  10       I_sharp = 50 + 120 = 170
    //
    //  dotSharp = -20 + (-20 + 250 + -20) + -20 = 170
    float p[3][3] = {
        { 10.f, 20.f, 10.f },
        { 20.f, 50.f, 20.f },
        { 10.f, 20.f, 10.f }
    };
    CHECK_FLOAT_EQ(dotSharp(p), 170.f,
                   "dotSharp: centre-heavy patch => 170");
}

void testDotSharp_IsolatedBrightPixel_PreClamp() {
    // Bright pixel on a black background: G = 4*200 = 800.
    // I_sharp = 200 + 800 = 1000  (raw, before clamp).
    float p[3][3] = {
        {   0.f,   0.f,   0.f },
        {   0.f, 200.f,   0.f },
        {   0.f,   0.f,   0.f }
    };
    CHECK_FLOAT_EQ(dotSharp(p), 1000.f,
                   "dotSharp: isolated bright pixel => 1000 (pre-clamp)");
}

void testDotSharp_DarkCentreWithBrightNeighbours_PreClamp() {
    // Inverted spike: G = 4*0 − 4*255 = −1020.
    // I_sharp = 0 − 1020 = −1020 (raw, before clamp).
    float p[3][3] = {
        {   0.f, 255.f,   0.f },
        { 255.f,   0.f, 255.f },
        {   0.f, 255.f,   0.f }
    };
    CHECK_FLOAT_EQ(dotSharp(p), -1020.f,
                   "dotSharp: dark centre with bright neighbours => -1020 (pre-clamp)");
}

void testDotSharp_RotationalSymmetry() {
    // The kernel is rotationally symmetric. Rotating the patch 90°
    // must return the same centre response.
    float orig[3][3] = {
        { 10.f, 30.f, 10.f },
        { 20.f, 50.f, 40.f },
        { 10.f, 60.f, 10.f }
    };
    float rotated[3][3] = {
        { 10.f, 20.f, 10.f },
        { 30.f, 50.f, 30.f },
        { 10.f, 40.f, 10.f }
    };
    CHECK_FLOAT_EQ(dotSharp(orig), dotSharp(rotated),
                   "dotSharp: 4-fold symmetric kernel => same response on rotated patch");
}

// ---------------------------------------------------------------------------
// Section 2 – convolveChannel via the test shim (float output, pre-clamp)
// ---------------------------------------------------------------------------

void testConvolve_FlatImage_ReturnsOriginalValues() {
    // Every pixel of a flat image must be returned as-is.
    Image img(5, 5, 1);
    fillImage(img, 80);

    SharpenFilterTest f;
    auto out = f.runConvolveChannel(img, 0);

    CHECK(out.size() == 25u, "convolveChannel flat: output size == 25");
    for (size_t i = 0; i < out.size(); ++i) {
        char msg[64];
        std::snprintf(msg, sizeof(msg),
                      "convolveChannel flat: pixel %zu => 80.f", i);
        CHECK_FLOAT_EQ(out[i], 80.f, msg);
    }
}

void testConvolve_LinearRamp_InteriorPixelsUnchanged() {
    // Interior pixels of a linear ramp have G = 0, so output equals input.
    // Boundary pixels are skipped.
    const int W = 5, H = 5;
    Image img(W, H, 1);
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            img.setPixel(x, y, 0, static_cast<uint8_t>(x + y));

    SharpenFilterTest f;
    auto out = f.runConvolveChannel(img, 0);

    for (int y = 1; y < H - 1; ++y) {
        for (int x = 1; x < W - 1; ++x) {
            float expected = static_cast<float>(x + y);
            char msg[64];
            std::snprintf(msg, sizeof(msg),
                          "convolveChannel linear ramp: interior (%d,%d) => %.0f",
                          x, y, expected);
            CHECK_FLOAT_EQ(out[y * W + x], expected, msg);
        }
    }
}

void testConvolve_OutputSize() {
    // Output vector must always equal width * height.
    Image img(7, 4, 1);
    fillImage(img, 50);
    SharpenFilterTest f;
    auto out = f.runConvolveChannel(img, 0);
    CHECK(out.size() == 28u, "convolveChannel: output size == width * height");
}

void testConvolve_KnownCentrePatch() {
    // 3×3 image; centre (1,1) has all real neighbours — no padding involved.
    //
    //  10  20  10
    //  20  50  20   →  expected 170.f  (see Section 1)
    //  10  20  10
    const uint8_t px[9] = { 10, 20, 10,
                             20, 50, 20,
                             10, 20, 10 };
    Image img = make3x3(px);

    SharpenFilterTest f;
    auto out = f.runConvolveChannel(img, 0);

    CHECK_FLOAT_EQ(out[1 * 3 + 1], 170.f,
                   "convolveChannel: centre-heavy 3×3 centre == 170.f");
}

void testConvolve_LinearRampCentreValue() {
    // 3×3 linear ramp; centre (1,1) must return 4.f (G = 0 ⇒ I_sharp = I).
    const uint8_t px[9] = { 0, 1, 2,
                             3, 4, 5,
                             6, 7, 8 };
    Image img = make3x3(px);

    SharpenFilterTest f;
    auto out = f.runConvolveChannel(img, 0);

    CHECK_FLOAT_EQ(out[1 * 3 + 1], 4.f,
                   "convolveChannel: linear ramp 3×3 centre == 4.f");
}

// ---------------------------------------------------------------------------
// Section 3 – Full apply pipeline (uint8_t output, clamping in effect)
// ---------------------------------------------------------------------------

void testApply_FlatImage_Unchanged() {
    // A uniform image has no edges; every pixel must survive apply identically.
    const uint8_t v = 120;
    Image img(5, 5, 3);
    fillImage(img, v);

    SharpenFilter f;
    f.apply(img);

    for (int c = 0; c < 3; ++c)
        for (int y = 0; y < 5; ++y)
            for (int x = 0; x < 5; ++x) {
                char msg[64];
                std::snprintf(msg, sizeof(msg),
                              "apply flat: (%d,%d,ch%d) == 120", x, y, c);
                CHECK(img.getPixel(x, y, c) == v, msg);
            }
}

void testApply_LinearRamp_InteriorUnchanged() {
    // Interior pixels of a linear ramp must be unmodified after apply.
    const int W = 5, H = 5;
    Image img(W, H, 1);
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            img.setPixel(x, y, 0, static_cast<uint8_t>(x + y));

    // Snapshot interior before apply.
    std::vector<uint8_t> before;
    before.reserve((W - 2) * (H - 2));
    for (int y = 1; y < H - 1; ++y)
        for (int x = 1; x < W - 1; ++x)
            before.push_back(img.getPixel(x, y, 0));

    SharpenFilter f;
    f.apply(img);

    int k = 0;
    for (int y = 1; y < H - 1; ++y) {
        for (int x = 1; x < W - 1; ++x) {
            char msg[64];
            std::snprintf(msg, sizeof(msg),
                          "apply linear ramp: interior (%d,%d) unchanged", x, y);
            CHECK(img.getPixel(x, y, 0) == before[k++], msg);
        }
    }
}

void testApply_KnownCentrePatch() {
    // Analytically derived: centre of the centre-heavy patch must be 170.
    const uint8_t px[9] = { 10, 20, 10,
                             20, 50, 20,
                             10, 20, 10 };
    Image img = make3x3(px);

    SharpenFilter f;
    f.apply(img);

    CHECK(img.getPixel(1, 1, 0) == 170,
          "apply: centre-heavy 3×3 centre == 170");
}

void testApply_UpperClamp() {
    // Isolated bright pixel: I_sharp = 1000, must be clamped to 255.
    //
    //   0    0    0
    //   0  200    0
    //   0    0    0
    const uint8_t px[9] = {   0,   0,   0,
                               0, 200,   0,
                               0,   0,   0 };
    Image img = make3x3(px);

    SharpenFilter f;
    f.apply(img);

    CHECK(img.getPixel(1, 1, 0) == 255,
          "apply upper clamp: isolated bright pixel => 255");
    // Verify the clamp does not overflow into neighbouring pixels.
    CHECK(img.getPixel(0, 1, 0) <= 255,
          "apply upper clamp: neighbour pixel within [0,255]");
}

void testApply_LowerClamp() {
    // Dark centre, bright NSEW neighbours: I_sharp = -1020, must clamp to 0.
    //
    //    0  255    0
    //  255    0  255
    //    0  255    0
    const uint8_t px[9] = {   0, 255,   0,
                             255,   0, 255,
                               0, 255,   0 };
    Image img = make3x3(px);

    SharpenFilter f;
    f.apply(img);

    CHECK(img.getPixel(1, 1, 0) == 0,
          "apply lower clamp: dark centre with bright neighbours => 0");
}

void testApply_ClampDoesNotAffectNeighbours() {
    // A large positive G at the centre must not bleed into adjacent pixels.
    const uint8_t px[9] = {   0,   0,   0,
                               0, 200,   0,
                               0,   0,   0 };
    Image img = make3x3(px);

    SharpenFilter f;
    f.apply(img);

    // NSEW neighbours: G = -1*200 → I_sharp = 0 + (-200) = -200, clamped to 0.
    CHECK(img.getPixel(1, 0, 0) == 0,
          "apply clamp: north neighbour of bright spike => 0");
    CHECK(img.getPixel(0, 1, 0) == 0,
          "apply clamp: west neighbour of bright spike => 0");
    CHECK(img.getPixel(2, 1, 0) == 0,
          "apply clamp: east neighbour of bright spike => 0");
    CHECK(img.getPixel(1, 2, 0) == 0,
          "apply clamp: south neighbour of bright spike => 0");
}

void testApply_MultiChannel_IndependentChannels() {
    // Each channel carries a different flat value; sharpening must not
    // mix channels.
    Image img(3, 3, 3);
    const uint8_t vals[3] = { 80, 120, 200 };
    for (int c = 0; c < 3; ++c)
        for (int y = 0; y < 3; ++y)
            for (int x = 0; x < 3; ++x)
                img.setPixel(x, y, c, vals[c]);

    SharpenFilter f;
    f.apply(img);

    for (int c = 0; c < 3; ++c) {
        char msg[64];
        std::snprintf(msg, sizeof(msg),
                      "apply multichannel: flat channel %d == %d", c, vals[c]);
        CHECK(img.getPixel(1, 1, c) == vals[c], msg);
    }
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    // Section 1: analytical dot-product
    testDotSharp_FlatPatch();
    testDotSharp_LinearRamp();
    testDotSharp_CentreHeavyPatch();
    testDotSharp_IsolatedBrightPixel_PreClamp();
    testDotSharp_DarkCentreWithBrightNeighbours_PreClamp();
    testDotSharp_RotationalSymmetry();

    // Section 2: convolveChannel via shim
    testConvolve_FlatImage_ReturnsOriginalValues();
    testConvolve_LinearRamp_InteriorPixelsUnchanged();
    testConvolve_OutputSize();
    testConvolve_KnownCentrePatch();
    testConvolve_LinearRampCentreValue();

    // Section 3: full apply pipeline
    testApply_FlatImage_Unchanged();
    testApply_LinearRamp_InteriorUnchanged();
    testApply_KnownCentrePatch();
    testApply_UpperClamp();
    testApply_LowerClamp();
    testApply_ClampDoesNotAffectNeighbours();
    testApply_MultiChannel_IndependentChannels();

    if (g_failCount > 0) {
        std::fprintf(stderr, "\n%d test(s) FAILED.\n", g_failCount);
        return EXIT_FAILURE;
    }
    std::fprintf(stdout, "All tests passed.\n");
    return EXIT_SUCCESS;
}
