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

// Unit tests for EmbossFilter.

#include "EmbossFilter.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

static int g_failCount = 0;

// Exact integer/byte equality
#define CHECK_EQ(a, b, msg)                                                   \
    do {                                                                      \
        auto _a = (a);                                                        \
        auto _b = (b);                                                        \
        if (_a != _b) {                                                       \
            std::fprintf(stderr,                                              \
                "FAIL [%s:%d]: %s (got %d, expected %d)\n",                  \
                __FILE__, __LINE__, msg,                                      \
                static_cast<int>(_a), static_cast<int>(_b));                 \
            ++g_failCount;                                                    \
        }                                                                     \
    } while (0)

// Strict less-than
#define CHECK_LT(a, b, msg)                                                   \
    do {                                                                      \
        auto _a = (a);                                                        \
        auto _b = (b);                                                        \
        if (!(_a < _b)) {                                                     \
            std::fprintf(stderr,                                              \
                "FAIL [%s:%d]: %s (got %d, expected < %d)\n",                \
                __FILE__, __LINE__, msg,                                      \
                static_cast<int>(_a), static_cast<int>(_b));                 \
            ++g_failCount;                                                    \
        }                                                                     \
    } while (0)

// Strict greater-than
#define CHECK_GT(a, b, msg)                                                   \
    do {                                                                      \
        auto _a = (a);                                                        \
        auto _b = (b);                                                        \
        if (!(_a > _b)) {                                                     \
            std::fprintf(stderr,                                              \
                "FAIL [%s:%d]: %s (got %d, expected > %d)\n",                \
                __FILE__, __LINE__, msg,                                      \
                static_cast<int>(_a), static_cast<int>(_b));                 \
            ++g_failCount;                                                    \
        }                                                                     \
    } while (0)

// Boolean assertion
#define CHECK_TRUE(expr, msg)                                                 \
    do {                                                                      \
        if (!(expr)) {                                                        \
            std::fprintf(stderr,                                              \
                "FAIL [%s:%d]: %s\n",                                        \
                __FILE__, __LINE__, msg);                                     \
            ++g_failCount;                                                    \
        }                                                                     \
    } while (0)


// ---------------------------------------------------------------------------
// Test class for accessing rotateKernel method
// ---------------------------------------------------------------------------
class EmbossFilterTest {
public:
    static Kernel rotateKernel(const Kernel& k, int turns) {
        return EmbossFilter::rotateKernel(k, turns);
    }
};

// ---------------------------------------------------------------------------
// Helper: check every element of a 3×3 Kernel against expected values
// ---------------------------------------------------------------------------
static void checkKernel(const Kernel& k, const float expected[3][3],
                        const char* label)
{
    // Optional but recommended: ensure the kernel is actually 3x3
    // CHECK_EQ(k.width, 3, "Kernel width must be 3");
    // CHECK_EQ(k.height, 3, "Kernel height must be 3");

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            char msg[64];
            std::snprintf(msg, sizeof(msg), "%s [%d][%d]", label, i, j);
            
            // k.data is 1D, so use [row * width + col]
            // expected is still 2D, so keep [i][j]
            CHECK_EQ(k.data[i * k.width + j], expected[i][j], msg);
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
    CHECK_EQ(dotNW(patch), 0.f, "dotNW: flat patch => 0");
}

void testApply_FlatImage_OutputIs128() {
    // All pixels identical → G*I = 0 everywhere → output = 0 + 128 = 128.
    Image img(5, 5, 1);
    for (int y = 0; y < 5; ++y)
        for (int x = 0; x < 5; ++x)
            img.setPixel(x, y, 0, 100);

    EmbossFilter f(1.0f, EmbossFilter::Direction::NW, EmbossFilter::ColorSpace::Grayscale);
    f.apply(img);

    for (int y = 0; y < 5; ++y)
        for (int x = 0; x < 5; ++x)
            CHECK_EQ(img.getPixel(x, y, 0), 128, "flat image: every pixel must be 128");
}

void testApply_NegativeResponse_PixelBelow128() {
    // Top-left = 25, everything else = 0.
    // Expected: -2*25 = -50 -> clamp(-50 + 128, 0, 255) = 78
    Image img(3, 3, 1);
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            img.setPixel(x, y, 0, 0);
    img.setPixel(0, 0, 0, 25);   // top-left only

    EmbossFilter f(1.0f, EmbossFilter::Direction::NW, EmbossFilter::ColorSpace::Grayscale);
    f.apply(img);

    unsigned char centre = img.getPixel(1, 1, 0);
    CHECK_EQ(centre, 78, "negative response must give 78, not 128");
}

void testApply_PositiveResponse_PixelAbove128() {
    // Bottom-right = 25, everything else = 0.
    // dotNW = +2*25 = +50 -> clamp(50 + 128, 0, 255) = 178
    Image img(3, 3, 1);
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            img.setPixel(x, y, 0, 0);
    img.setPixel(2, 2, 0, 25);   // bottom-right only

    EmbossFilter f(1.0f, EmbossFilter::Direction::NW, EmbossFilter::ColorSpace::Grayscale);
    f.apply(img);

    unsigned char centre = img.getPixel(1, 1, 0);
    CHECK_EQ(centre, 178, "positive response must give 178");
}

void testApply_StrengthScalesAroundBias() {
    // dotNW = -50 at centre
    // With strength = 2: clamp(2*-50 + 128, 0, 255) = clamp(28, 0, 255) = 28
    Image img(3, 3, 1);
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            img.setPixel(x, y, 0, 0);
    img.setPixel(0, 0, 0, 25);

    EmbossFilter f(2.0f, EmbossFilter::Direction::NW, EmbossFilter::ColorSpace::Grayscale);
    f.apply(img);

    CHECK_EQ(img.getPixel(1, 1, 0), 28, "strength=2: centre must be 28");
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

    testKernelDotProduct_FlatPatch();
    testApply_FlatImage_OutputIs128();
    testApply_NegativeResponse_PixelBelow128();
    testApply_PositiveResponse_PixelAbove128();
    testApply_StrengthScalesAroundBias();

    if (g_failCount > 0) {
        std::fprintf(stderr, "\n%d test(s) FAILED.\n", g_failCount);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}