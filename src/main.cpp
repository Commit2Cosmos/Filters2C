#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

#include "Image.h"
#include "Volume.h"
#include "GrayScale.h"
#include "Brightness.h"
#include "HistogramEqualisation.h"
#include "BoxBlurFilter.h"
#include "GaussianBlurFilter.h"
#include "MedianBlurFilter.h"
#include "SharpenFilter.h"
#include "EmbossFilter.h"
#include "SobelFilter.h"
#include "PrewittFilter.h"
#include "ScharrFilter.h"
#include "RobertsCrossFilter.h"
#include "SaltAndPepper.h"
#include "Threshold.h"
#include "Projection.h"
#include "Slice.h"

namespace fs = std::filesystem;

// ── Helpers ───────────────────────────────────────────────────────────────────

static void printUsage(const char* prog) {
    std::cerr
        << "Usage:\n"
        << "  " << prog << " -i <input> [filters...] <output>\n"
        << "  " << prog << " -d <volume> [-f <n>] [-l <n>] [-x <ext>] [-r <type> <size> [<stdev>]] (-s|-p) ... <output>\n"
        << "\nImage filters:\n"
        << "  -g, --greyscale\n"
        << "  -b, --brightness <value|auto>\n"
        << "  -h, --histogram [HSV|HSL]          (default: HSV)\n"
        << "  -r, --blur <Gaussian|Box|Median> <size> [<stdev>]\n"
        << "  -e, --edge <Sobel|Prewitt|Scharr|RobertsCross>\n"
        << "  -L, --sharpen\n"
        << "  -m, --emboss <strength> <NW|NE|SE|SW> [HSV|HSL]\n"
        << "  -n, --saltpepper <amount>\n"
        << "  -t, --threshold <value> [HSV|HSL]  (default: HSV)\n"
        << "\nVolume options:\n"
        << "  -f, --first <index>\n"
        << "  -l, --last  <index>\n"
        << "  -x, --extension <ext>              (default: png)\n"
        << "  -s, --slice <XZ|YZ> <index>\n"
        << "  -p, --projection <MIP|MinIP|meanAIP|medianAIP>\n";
}

static bool isColorspace(const std::string& s) {
    return s == "HSV" || s == "HSL";
}

// Peek whether the next token (if it exists and is not the output path) is a colorspace keyword.
static bool nextIsColorspace(const std::vector<std::string>& args, int i) {
    return i < static_cast<int>(args.size()) - 1 && isColorspace(args[i]);
}

// Build a sorted list of file paths matching <base><anything>.<ext>
static std::vector<std::string> buildVolumeFileList(
    const std::string& base, int first, int last, const std::string& ext)
{
    fs::path dir    = fs::path(base).parent_path();
    std::string pfx = fs::path(base).filename().string();
    std::string dotExt = "." + ext;

    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(dir)) {
        std::string fname = entry.path().filename().string();
        if (fname.rfind(pfx, 0) == 0 && entry.path().extension() == dotExt)
            files.push_back(entry.path().string());
    }
    std::sort(files.begin(), files.end());

    // Trim to [first, last] window if specified
    if (!files.empty()) {
        int lo = (first >= 0) ? std::min(first, static_cast<int>(files.size())) : 0;
        int hi = (last  >= 0) ? std::min(last + 1, static_cast<int>(files.size()))
                               : static_cast<int>(files.size());
        files = { files.begin() + lo, files.begin() + hi };
    }

    return files;
}

// ── Entry point ───────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    if (argc < 4) { printUsage(argv[0]); return 1; }

    const std::vector<std::string> args(argv + 1, argv + argc);
    const std::string outputPath = args.back();

    // ── State ─────────────────────────────────────────────────────────────────
    bool        isVolumeMode   = false;
    std::string inputImagePath;
    std::string volumeBase;
    int         firstIndex     = -1;
    int         lastIndex      = -1;
    std::string volumeExt      = "png";

    // Volume blur is stored and applied lazily when the volume is loaded.
    struct BlurSpec { std::string type; int size; double stdev = 1.0; };
    std::optional<BlurSpec> volumeBlur;

    Image img;
    bool  imageLoaded = false;

    // Lazy-load the 2-D input image on first filter access.
    auto ensureLoaded = [&]() {
        if (!imageLoaded) {
            if (!img.load(inputImagePath)) {
                std::cerr << "Error: cannot load image '" << inputImagePath << "'\n";
                std::exit(1);
            }
            imageLoaded = true;
        }
    };

    // Load the volume (and optionally blur it) right before slice/projection.
    auto loadVolume = [&]() -> Volume {
        auto files = buildVolumeFileList(volumeBase, firstIndex, lastIndex, volumeExt);
        if (files.empty()) {
            std::cerr << "Error: no volume files found at '" << volumeBase << "'\n";
            std::exit(1);
        }
        Volume vol;
        if (!vol.loadFromFiles(files)) {
            std::cerr << "Error: failed to load volume from '" << volumeBase << "'\n";
            std::exit(1);
        }

        if (volumeBlur) {
            if (volumeBlur->type == "Gaussian") {
                GaussianBlurFilter f(volumeBlur->size, volumeBlur->stdev);
                f.apply(vol);
            } else if (volumeBlur->type == "Median") {
                MedianBlurFilter f(volumeBlur->size);
                f.apply(vol);
            } else {
                std::cerr << "Error: unknown volume blur type '" << volumeBlur->type << "'\n";
                std::exit(1);
            }
        }

        return vol;
    };

    // ── Argument parsing loop ─────────────────────────────────────────────────
    for (int i = 0; i < static_cast<int>(args.size()) - 1; ++i) {
        const std::string& a = args[i];

        // ── Input source ──────────────────────────────────────────────────────
        if (a == "-i") {
            inputImagePath = args[++i];
            isVolumeMode   = false;

        } else if (a == "-d") {
            volumeBase   = args[++i];
            isVolumeMode = true;

        // ── Volume reading options ─────────────────────────────────────────
        } else if (a == "-f" || a == "--first") {
            firstIndex = std::stoi(args[++i]);

        } else if (a == "-l" || a == "--last") {
            lastIndex = std::stoi(args[++i]);

        } else if (a == "-x" || a == "--extension") {
            volumeExt = args[++i];

        // ── Greyscale ─────────────────────────────────────────────────────
        } else if (a == "-g" || a == "--greyscale") {
            if (!isVolumeMode) {
                ensureLoaded();
                GrayScale f;
                f.apply(img);
            }

        // ── Brightness ────────────────────────────────────────────────────
        } else if (a == "-b" || a == "--brightness") {
            std::string val = args[++i];
            if (!isVolumeMode) {
                ensureLoaded();
                Brightness f(val);
                f.apply(img);
            }

        // ── Histogram Equalisation ─────────────────────────────────────────
        } else if (a == "-h" || a == "--histogram") {
            std::string colorspace = "HSV";
            if (nextIsColorspace(args, i + 1)) colorspace = args[++i];
            if (!isVolumeMode) {
                ensureLoaded();
                HistogramEqualisation f(colorspace);
                f.apply(img);
            }

        // ── Blur (image and volume) ────────────────────────────────────────
        } else if (a == "-r" || a == "--blur") {
            std::string type = args[++i];
            int         size = std::stoi(args[++i]);

            if (isVolumeMode) {
                BlurSpec spec{ type, size };
                if (type == "Gaussian") spec.stdev = std::stod(args[++i]);
                volumeBlur = spec;
            } else {
                ensureLoaded();
                if (type == "Gaussian") {
                    double stdev = std::stod(args[++i]);
                    GaussianBlurFilter f(size, stdev); f.apply(img);
                } else if (type == "Box") {
                    BoxBlurFilter f(size); f.apply(img);
                } else if (type == "Median") {
                    MedianBlurFilter f(size); f.apply(img);
                } else {
                    std::cerr << "Error: unknown blur type '" << type << "'\n"; return 1;
                }
            }

        // ── Edge detection ─────────────────────────────────────────────────
        } else if (a == "-e" || a == "--edge") {
            std::string method = args[++i];
            if (!isVolumeMode) {
                ensureLoaded();
                if      (method == "Sobel")        { SobelFilter f;        f.apply(img); }
                else if (method == "Prewitt")       { PrewittFilter f;      f.apply(img); }
                else if (method == "Scharr")        { ScharrFilter f;       f.apply(img); }
                else if (method == "RobertsCross")  { RobertsCrossFilter f; f.apply(img); }
                else { std::cerr << "Error: unknown edge method '" << method << "'\n"; return 1; }
            }

        // ── Sharpen ────────────────────────────────────────────────────────
        } else if (a == "-L" || a == "--sharpen") {
            if (!isVolumeMode) {
                ensureLoaded();
                SharpenFilter f;
                f.apply(img);
            }

        // ── Emboss ─────────────────────────────────────────────────────────
        } else if (a == "-m" || a == "--emboss") {
            float strength   = std::stof(args[++i]);
            if (strength < 0.0f || strength > 10.0f) {
                throw std::invalid_argument("Emboss strength must be in range [0, 10]");
            }
            std::string direction = args[++i];
            std::string colorspace = "HSV";
            if (nextIsColorspace(args, i + 1)) colorspace = args[++i];
            if (!isVolumeMode) {
                ensureLoaded();
                EmbossFilter f(strength,
                               EmbossFilter::parseDirection(direction),
                               EmbossFilter::parseColorSpace(colorspace));
                f.apply(img);
            }

        // ── Salt and Pepper ────────────────────────────────────────────────
        } else if (a == "-n" || a == "--saltpepper") {
            double amount = std::stod(args[++i]);
            if (!isVolumeMode) {
                ensureLoaded();
                SaltAndPepper f(amount);
                f.apply(img);
            }

        // ── Threshold ──────────────────────────────────────────────────────
        } else if (a == "-t" || a == "--threshold") {
            int         value      = std::stoi(args[++i]);
            std::string colorspace = "HSV";
            if (nextIsColorspace(args, i + 1)) colorspace = args[++i];
            if (!isVolumeMode) {
                ensureLoaded();
                Threshold f(value, colorspace);
                f.apply(img);
            }

        // ── Volume Slice ───────────────────────────────────────────────────
        } else if (a == "-s" || a == "--slice") {
            std::string plane = args[++i];
            int         index = std::stoi(args[++i]);
            if (isVolumeMode) {
                Volume vol = loadVolume();
                if      (plane == "XZ") img = Slice::extractXZ(vol, index);
                else if (plane == "YZ") img = Slice::extractYZ(vol, index);
                else { std::cerr << "Error: unknown slice plane '" << plane << "'\n"; return 1; }
                imageLoaded = true;
            }

        // ── Volume Projection ──────────────────────────────────────────────
        } else if (a == "-p" || a == "--projection") {
            std::string type = args[++i];
            if (isVolumeMode) {
                Volume vol = loadVolume();
                Projection proj;
                if      (type == "MIP")        img = proj.applyMaxIP   (vol, firstIndex, lastIndex);
                else if (type == "MinIP")       img = proj.applyMinIP   (vol, firstIndex, lastIndex);
                else if (type == "meanAIP")     img = proj.applyMeanIP  (vol, firstIndex, lastIndex);
                else if (type == "medianAIP")   img = proj.applyMedianIP(vol, firstIndex, lastIndex);
                else { std::cerr << "Error: unknown projection type '" << type << "'\n"; return 1; }
                imageLoaded = true;
            }

        // ── Unknown ────────────────────────────────────────────────────────
        } else {
            std::cerr << "Error: unknown argument '" << a << "'\n";
            return 1;
        }
    }

    // Image mode with no filters: just load and copy.
    if (!isVolumeMode) ensureLoaded();

    if (!imageLoaded) {
        std::cerr << "Error: no output image produced (missing -s or -p for volume mode?)\n";
        return 1;
    }

    if (!img.save(outputPath)) {
        std::cerr << "Error: cannot save output to '" << outputPath << "'\n";
        return 1;
    }

    return 0;
}
