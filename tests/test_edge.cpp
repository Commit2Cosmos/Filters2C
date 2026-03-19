/**
 * Unit tests for edge detection filters (Sobel, Prewitt, Scharr, Roberts Cross).
 *
 * Two test modes:
 *   1) No arguments  — runs unit tests with a hand-calculated 3x3 matrix.
 *   2) With an image  — loads the image, applies all 4 filters, saves outputs.
 *
 * Compile (from project root):
 *   g++ -std=c++20 -O2 -Isrc tests/EdgeTest.cpp src/Image.cpp src/ConvFilter.cpp \
 *       src/SobelFilter.cpp src/PrewittFilter.cpp \
 *       src/ScharrFilter.cpp src/RobertsCrossFilter.cpp \
 *       -o test_edge -lm
 *
 * Run unit tests:     ./test_edge
 * Run image test:     ./test_edge Images/gracehopper.png
 */

#include "Image.h"
#include "SobelFilter.h"
#include "PrewittFilter.h"
#include "ScharrFilter.h"
#include "RobertsCrossFilter.h"

#include <iostream>
#include <string>
#include <cmath>
#include <vector>


// ── Helper: build a 3x3 grayscale Image from a flat array ────────────────────

static Image make3x3(const unsigned char pixels[9]) {
    Image img(3, 3, 1);
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            img.setPixel(x, y, 0, pixels[y * 3 + x]);
    return img;
}


// ── Helper: compare image against expected pixel values ──────────────────────

static bool checkPixels(const Image& img, const unsigned char expected[9],
                        const std::string& filterName, int tolerance = 1) {
    bool pass = true;

    // Print the full expected vs actual grid so the result is verifiable
    std::cout << "\n  [" << filterName << "] Expected vs Actual (3x3):" << std::endl;
    for (int y = 0; y < 3; ++y) {
        std::cout << "    ";
        for (int x = 0; x < 3; ++x) {
            int got = expected[y * 3 + x];
            std::cout << got;
            if (x < 2) std::cout << "\t";
        }
        std::cout << "\t|\t";
        for (int x = 0; x < 3; ++x) {
            int got = img.getPixel(x, y, 0);
            std::cout << got;
            if (x < 2) std::cout << "\t";
        }
        std::cout << std::endl;
    }

    // Check each pixel
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            int got = img.getPixel(x, y, 0);
            int exp = expected[y * 3 + x];
            if (std::abs(got - exp) > tolerance) {
                std::cerr << "  MISMATCH pixel(" << x << "," << y << "): "
                          << "expected " << exp << ", got " << got << std::endl;
                pass = false;
            }
        }
    }
    return pass;
}


// ══════════════════════════════════════════════════════════════════════════════
// Unit tests — hand-calculated expected outputs for a 3x3 input
// ══════════════════════════════════════════════════════════════════════════════
//
// Input (3x3 grayscale, extend/clamp padding):
//
//   100  150  200
//    50  100  150
//     0   50  100
//
// For each filter we computed Gx, Gy at every pixel (including boundary
// pixels that use clamped neighbours), then magnitude = sqrt(Gx² + Gy²),
// then normalised the entire output so max maps to 255.
//
// Normalisation: each filter uses a fixed theoretical maximum rather than
// the dynamic max of the output.  theoreticalMax = sqrt(2) * filterAxisMax,
// where filterAxisMax is the largest possible single-axis (Gx or Gy) response
// for a fully-saturated (255) input.  scale = 255 / theoreticalMax.
//
// ── Sobel ────────────────────────────────────────────────────────────────
//   Gx:              Gy:                Magnitude (raw):
//   200  400  200    -200 -200 -200     282.8  447.2  282.8
//   200  400  200    -400 -400 -400     447.2  565.7  447.2
//   200  400  200    -200 -200 -200     282.8  447.2  282.8
//
//   SOBEL_AXIS_MAX = 4*255 = 1020
//   theoreticalMax = sqrt(2) * 1020 = 1442.5,  scale = 255 / 1442.5 = 0.17678
//   Normalised (unsigned char, truncated):
//      50   79   50
//      79  100   79
//      50   79   50
//
// ── Prewitt ──────────────────────────────────────────────────────────────
//   Gx:              Gy:                Magnitude (raw):
//   150  300  150    -150 -150 -150     212.1  335.4  212.1
//   150  300  150    -300 -300 -300     335.4  424.3  335.4
//   150  300  150    -150 -150 -150     212.1  335.4  212.1
//
//   PREWITT_AXIS_MAX = 3*255 = 765
//   theoreticalMax = sqrt(2) * 765 = 1081.9,  scale = 255 / 1081.9 = 0.23570
//   Normalised:
//      49   79   49
//      79   99   79
//      49   79   49
//
// ── Scharr ───────────────────────────────────────────────────────────────
//   Gx:               Gy:                 Magnitude (raw):
//    800  1600  800   -800  -800  -800    1131.4  1788.9  1131.4
//    800  1600  800   -1600 -1600 -1600   1788.9  2262.7  1788.9
//    800  1600  800   -800  -800  -800    1131.4  1788.9  1131.4
//
//   SCHARR_AXIS_MAX = 16*255 = 4080
//   theoreticalMax = sqrt(2) * 4080 = 5770.0,  scale = 255 / 5770.0 = 0.04419
//   Normalised:
//      50   79   50
//      79  100   79
//      50   79   50
//
// ── Roberts Cross ────────────────────────────────────────────────────────
//   G1 (diagonal ↘):    G2 (diagonal ↙):   Magnitude (raw):
//     0   -50  -50        0   50   50         0.0   70.7   70.7
//    50     0    0       50  100  100        70.7  100.0  100.0
//    50     0    0       50  100  100        70.7  100.0  100.0
//
//   ROBERTS_AXIS_MAX = 1*255 = 255
//   theoreticalMax = sqrt(2) * 255 = 360.6,  scale = 255 / 360.6 = 0.70711
//   Normalised:
//       0   50   50
//      50   70   70
//      50   70   70
//
// ══════════════════════════════════════════════════════════════════════════════

static int runUnitTests() {
    // Input pixels: row-major order
    const unsigned char input[9] = {
        100, 150, 200,
         50, 100, 150,
          0,  50, 100,
    };

    int passed = 0, failed = 0;

    // ── Sobel ────────────────────────────────────────────────────────────
    {
        const unsigned char expected[9] = {
             50,  79,  50,
             79, 100,  79,
             50,  79,  50,
        };
        Image img = make3x3(input);
        SobelFilter sobel;
        sobel.apply(img);
        if (checkPixels(img, expected, "Sobel")) {
            ++passed;
        } else { ++failed; }
    }

    // ── Prewitt ──────────────────────────────────────────────────────────
    {
        const unsigned char expected[9] = {
             49,  79,  49,
             79,  99,  79,
             49,  79,  49,
        };
        Image img = make3x3(input);
        PrewittFilter prewitt;
        prewitt.apply(img);
        if (checkPixels(img, expected, "Prewitt")) {
            ++passed;
        } else { ++failed; }
    }

    // ── Scharr ───────────────────────────────────────────────────────────
    {
        const unsigned char expected[9] = {
             50,  79,  50,
             79, 100,  79,
             50,  79,  50,
        };
        Image img = make3x3(input);
        ScharrFilter scharr;
        scharr.apply(img);
        if (checkPixels(img, expected, "Scharr")) {
            ++passed;
        } else { ++failed; }
    }

    // ── Roberts Cross ────────────────────────────────────────────────────
    {
        const unsigned char expected[9] = {
              0,  50,  50,
             50,  70,  70,
             50,  70,  70,
        };
        Image img = make3x3(input);
        RobertsCrossFilter roberts;
        roberts.apply(img);
        if (checkPixels(img, expected, "Roberts Cross")) {
            ++passed;
        } else { ++failed; }
    }

    // ── Auto-grayscale test ──────────────────────────────────────────────
    // Verify that passing an RGB image produces the same result as
    // passing an already-grayscale image (filter should convert internally).
    {
        // Build a 3x3 RGB image where R=G=B=grey value (pure grey)
        Image rgb(3, 3, 3);
        for (int y = 0; y < 3; ++y)
            for (int x = 0; x < 3; ++x) {
                unsigned char v = input[y * 3 + x];
                rgb.setPixel(x, y, 0, v);
                rgb.setPixel(x, y, 1, v);
                rgb.setPixel(x, y, 2, v);
            }

        // Apply Sobel to the RGB image
        SobelFilter sobel;
        sobel.apply(rgb);

        // After apply, image should be 1 channel (converted internally)
        const unsigned char expected[9] = {
             50,  79,  50,
             79, 100,  79,
             50,  79,  50,
        };

        bool pass = (rgb.getChannels() == 1);
        if (!pass) {
            std::cerr << "  FAIL Auto-grayscale: channels = "
                      << rgb.getChannels() << ", expected 1" << std::endl;
        }
        pass = pass && checkPixels(rgb, expected, "Auto-grayscale (Sobel on RGB)");

        if (pass) {
            ++passed;
        } else { ++failed; }
    }

    std::cout << "\nResults: " << passed << " passed, "
              << failed << " failed" << std::endl;
    return failed > 0 ? 1 : 0;
}


// ══════════════════════════════════════════════════════════════════════════════
// Image output test — load a real image, apply filters, save PNGs
// ══════════════════════════════════════════════════════════════════════════════

static int runImageTest(const std::string& path) {
    Image original(path);
    if (original.getWidth() == 0) {
        std::cerr << "Failed to load: " << path << std::endl;
        return 1;
    }
    std::cout << "Loaded " << original.getWidth() << "x" << original.getHeight()
              << " (" << original.getChannels() << " channels)" << std::endl;

    struct TestCase { const char* name; Filter* filter; };

    SobelFilter        sobel;
    PrewittFilter      prewitt;
    ScharrFilter       scharr;
    RobertsCrossFilter roberts;

    TestCase tests[] = {
        {"sobel",   &sobel},
        {"prewitt", &prewitt},
        {"scharr",  &scharr},
        {"roberts", &roberts},
    };

    for (auto& t : tests) {
        Image img = original;
        t.filter->apply(img);
        std::string outfile = std::string("output_") + t.name + ".png";
        img.save(outfile);
        std::cout << "Saved " << outfile << std::endl;
    }

    return 0;
}


// ══════════════════════════════════════════════════════════════════════════════

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Running unit tests (no image argument)...\n" << std::endl;
        return runUnitTests();
    }
    return runImageTest(argv[1]);
}
