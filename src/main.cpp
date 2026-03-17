#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "Image.h"
#include "Volume.h"
#include "Blur.h"
#include "SharpenFilter.h"
#include "EmbossFilter.h"
#include "SobelFilter.h"
#include "PrewittFilter.h"
#include "ScharrFilter.h"
#include "RobertsCrossFilter.h"


static void printUsage(const char* prog) {
    std::cerr
        << "Usage:\n"
        << "  " << prog << " -i <input.png> [filters...] <output.png>\n"
        << "  " << prog << " -d <volume_dir> [blur] [-f <first>] [-l <last>] (-s|-p) ... <output.png>\n"
        << "\nImage filters:\n"
        << "  -L, --sharpen                  Sharpen filter\n"
        << "  -m, --emboss <strength> <dir> <HSV|HSL>\n";
}

// Returns true if `s` can be parsed as a double (used for optional sigma).
static bool isNumber(const std::string& s) {
    try { std::stod(s); return true; }
    catch (...) { return false; }
}

int main(int argc, char* argv[]) {
    if (argc < 4) { printUsage(argv[0]); return 1; }

    const std::vector<std::string> args(argv + 1, argv + argc);
    // The last positional argument is always the output path.
    const std::string outputPath = args.back();

    bool isVolumeMode = false;
    std::string inputImagePath;
    std::string volumeDir;

    Image img;
    bool  imageLoaded = false;
    Blur  blurEngine;

    // Helper: ensure the 2-D image is loaded before applying a filter.
    auto ensureLoaded = [&]() {
        if (!imageLoaded) {
            if (!img.load(inputImagePath)) {
                std::cerr << "Error: cannot load image '" << inputImagePath << "'\n";
                std::exit(1);
            }
            imageLoaded = true;
        }
    };

    // ── Argument parsing loop ─────────────────────────────────────────────────
    for (int i = 0; i < static_cast<int>(args.size()) - 1; ++i) {
        const std::string& a = args[i];

        // ── Input source ───────────────────────────────────────────────────
        if (a == "-i") {
            inputImagePath = args[++i];
            isVolumeMode   = false;

        } else if (a == "-d") {
            volumeDir    = args[++i];
            isVolumeMode = true;

        // ── Sharpen ───────────────────────────────────────────────────────
        } else if (a == "-L" || a == "--sharpen") {
            if (!isVolumeMode) {
                ensureLoaded();
                SharpenFilter f;
                f.apply(img);
            }

        // ── Emboss ────────────────────────────────────────────────────────
        } else if (a == "-m" || a == "--emboss") {
            float       strength   = std::stof(args[++i]);
            std::string direction  = args[++i];
            std::string colorspace = args[++i];

            if (!isVolumeMode) {
                ensureLoaded();
                EmbossFilter f(
                    strength, 
                    EmbossFilter::parseDirection(direction),
                    EmbossFilter::parseColorSpace(colorspace));
                f.apply(img);
            }
        }

        if (isVolumeMode) {

        }

        return 0;
    }
}