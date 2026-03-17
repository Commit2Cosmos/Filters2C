// EmbossTest.cpp
// Unit tests for EmbossFilter.

#include "EmbossFilter.h"

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
// Single test-access shim.
// ---------------------------------------------------------------------------
class EmbossFilterTest {
public:
    static Kernel rotateKernel(const Kernel& k, int turns) {
        return EmbossFilter::rotateKernel(k, turns);
    }

    static std::vector<float> combineResponses(
        const EmbossFilter& f,
        const std::vector<float>& x,
        const std::vector<float>& y = {})
    {
        return f.combineResponses(x, y);
    }
};

// ---------------------------------------------------------------------------
// Helper: check every element of a 3×3 Kernel against expected values
// ---------------------------------------------------------------------------
static void checkKernel(const Kernel& k, const float expected[3][3],
                        const char* label)
{
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            char msg[64];
            std::snprintf(msg, sizeof(msg), "%s [%d][%d]", label, i, j);
            CHECK_FLOAT_EQ(k[i][j], expected[i][j], msg);
        }
    }
}

// ---------------------------------------------------------------------------
// rotateKernel tests
// ---------------------------------------------------------------------------

static const Kernel BASE_NW_KERNEL = {{
    { -2.f, -1.f, 0.f },
    { -1.f,  0.f, 1.f },
    {  0.f,  1.f, 2.f }
}};

static const float EXPECTED_NW[3][3] = {
    { -2.f, -1.f,  0.f },
    { -1.f,  0.f,  1.f },
    {  0.f,  1.f,  2.f }
};
static const float EXPECTED_NE[3][3] = {
    {  0.f, -1.f, -2.f },
    {  1.f,  0.f, -1.f },
    {  2.f,  1.f,  0.f }
};
static const float EXPECTED_SE[3][3] = {
    {  2.f,  1.f,  0.f },
    {  1.f,  0.f, -1.f },
    {  0.f, -1.f, -2.f }
};
static const float EXPECTED_SW[3][3] = {
    {  0.f,  1.f,  2.f },
    { -1.f,  0.f,  1.f },
    { -2.f, -1.f,  0.f }
};

void testRotateKernel_ZeroTurns() {
    Kernel r = EmbossFilterTest::rotateKernel(BASE_NW_KERNEL, 0);
    checkKernel(r, EXPECTED_NW, "rotateKernel(0 turns)");
}
void testRotateKernel_OneTurn() {
    Kernel r = EmbossFilterTest::rotateKernel(BASE_NW_KERNEL, 1);
    checkKernel(r, EXPECTED_NE, "rotateKernel(1 turn / NE)");
}
void testRotateKernel_TwoTurns() {
    Kernel r = EmbossFilterTest::rotateKernel(BASE_NW_KERNEL, 2);
    checkKernel(r, EXPECTED_SE, "rotateKernel(2 turns / SE)");
}
void testRotateKernel_ThreeTurns() {
    Kernel r = EmbossFilterTest::rotateKernel(BASE_NW_KERNEL, 3);
    checkKernel(r, EXPECTED_SW, "rotateKernel(3 turns / SW)");
}
void testRotateKernel_FourTurnsIsIdentity() {
    Kernel r = EmbossFilterTest::rotateKernel(BASE_NW_KERNEL, 4);
    checkKernel(r, EXPECTED_NW, "rotateKernel(4 turns == identity)");
}

// ---------------------------------------------------------------------------
// combineResponses tests
// ---------------------------------------------------------------------------

void testCombineResponses_PositiveResponse() {
    EmbossFilter f(1.0f, EmbossFilter::NW);
    auto result = EmbossFilterTest::combineResponses(f, { 50.f, 100.f, 127.f });
    CHECK_FLOAT_EQ(result[0], 178.f, "combineResponses:  50 + 128 == 178");
    CHECK_FLOAT_EQ(result[1], 228.f, "combineResponses: 100 + 128 == 228");
    CHECK_FLOAT_EQ(result[2], 255.f, "combineResponses: 127 + 128 clamped to 255");
}

void testCombineResponses_Clamping() {
    EmbossFilter f(1.0f, EmbossFilter::NW);
    auto result = EmbossFilterTest::combineResponses(f, { 200.f, -200.f });
    CHECK(result[0] <= 255.f, "combineResponses: upper clamp <= 255");
    CHECK(result[1] >= 0.f,   "combineResponses: lower clamp >= 0");
}

void testCombineResponses_MultiplePixels() {
    EmbossFilter f(1.0f, EmbossFilter::NW);
    auto result = EmbossFilterTest::combineResponses(f, { -10.f, 0.f, 10.f });
    CHECK(result.size() == 3,  "combineResponses: output size matches input");
    CHECK_FLOAT_EQ(result[0], 118.f, "combineResponses multi: -10 + 128 == 118");
    CHECK_FLOAT_EQ(result[1], 128.f, "combineResponses multi:   0 + 128 == 128");
    CHECK_FLOAT_EQ(result[2], 138.f, "combineResponses multi:  10 + 128 == 138");
}

// ---------------------------------------------------------------------------
// Analytical kernel convolution helper
// ---------------------------------------------------------------------------

// Explicit dot product of the NW kernel with a 3×3 patch.
static float dotNW(const float p[3][3]) {
    return (-2.f * p[0][0]) + (-1.f * p[0][1]) + ( 0.f * p[0][2])
         + (-1.f * p[1][0]) + ( 0.f * p[1][1]) + ( 1.f * p[1][2])
         + ( 0.f * p[2][0]) + ( 1.f * p[2][1]) + ( 2.f * p[2][2]);
}

void testKernelDotProduct_FlatPatch() {
    // Uniform patch — antisymmetric kernel must produce zero.
    float patch[3][3] = {
        { 100.f, 100.f, 100.f },
        { 100.f, 100.f, 100.f },
        { 100.f, 100.f, 100.f }
    };
    CHECK_FLOAT_EQ(dotNW(patch), 0.f, "dotNW: flat patch => 0");
}

// ---------------------------------------------------------------------------
// Analytical 3×3 image tests
// ---------------------------------------------------------------------------
//
// Synthetic image — a linear ramp:
//
//   0  1  2
//   3  4  5
//   6  7  8
//
// NW kernel dot-product at the centre pixel (1,1):
//
//   (-2×0)+(-1×1)+(0×2) + (-1×3)+(0×4)+(1×5) + (0×6)+(1×7)+(2×8)
//   =  0  −  1  + 0    +  −3  + 0  + 5     +   0  + 7  + 16   = 24
//
// After the +128 bias:  24 + 128 = 152.
//
// The SE kernel is the 180° rotation of NW, so its dot-product on the
// same patch is exactly −24, giving −24 + 128 = 104.  This pair
// validates rotational antisymmetry through the full pipeline.

static const float RAMP_3X3[3][3] = {
    { 0.f, 1.f, 2.f },
    { 3.f, 4.f, 5.f },
    { 6.f, 7.f, 8.f }
};

void testAnalytical3x3Image_NW() {
    const float response = dotNW(RAMP_3X3);
    CHECK_FLOAT_EQ(response, 24.f, "analytical ramp: NW centre dot == 24");

    EmbossFilter f(1.0f, EmbossFilter::NW);
    auto out = EmbossFilterTest::combineResponses(f, { response });
    CHECK_FLOAT_EQ(out[0], 152.f, "analytical ramp: NW emboss centre == 152");
}

void testAnalytical3x3Image_SE() {
    // SE is the point-reflection of NW; response on the ramp is −24.
    EmbossFilter f(1.0f, EmbossFilter::SE);
    auto out = EmbossFilterTest::combineResponses(f, { -24.f });
    CHECK_FLOAT_EQ(out[0], 104.f, "analytical ramp: SE emboss centre == 104");
}

void testAnalytical3x3Image_FlatImage() {
    // Zero contrast → zero convolution response → output is mid-grey (128).
    EmbossFilter f(1.0f, EmbossFilter::NW);
    auto out = EmbossFilterTest::combineResponses(f, { 0.f });
    CHECK_FLOAT_EQ(out[0], 128.f, "analytical flat: zero response => mid-grey 128");
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    testRotateKernel_ZeroTurns();
    testRotateKernel_OneTurn();
    testRotateKernel_TwoTurns();
    testRotateKernel_ThreeTurns();
    testRotateKernel_FourTurnsIsIdentity();

    testCombineResponses_PositiveResponse();
    testCombineResponses_Clamping();
    testCombineResponses_MultiplePixels();

    testKernelDotProduct_FlatPatch();

    testAnalytical3x3Image_NW();
    testAnalytical3x3Image_SE();
    testAnalytical3x3Image_FlatImage();

    if (g_failCount > 0) {
        std::fprintf(stderr, "\n%d test(s) FAILED.\n", g_failCount);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}