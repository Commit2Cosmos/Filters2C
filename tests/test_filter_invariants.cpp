#include <cassert>
#include <iostream>

#include "BoxBlurFilter.h"
#include "Brightness.h"
#include "EmbossFilter.h"
#include "GaussianBlurFilter.h"
#include "GrayScale.h"
#include "HistogramEqualisation.h"
#include "Image.h"
#include "MedianBlurFilter.h"
#include "SaltAndPepper.h"
#include "SharpenFilter.h"
#include "Threshold.h"

namespace {

// Build deterministic inputs so every filter is checked on the same data.
Image makePatternImage(int width, int height, int channels)
{
    Image image(width, height, channels);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                const unsigned char value =
                    static_cast<unsigned char>((x * 17 + y * 31 + c * 53) % 256);
                image.setPixel(x, y, c, value);
            }
        }
    }

    return image;
}

// These helpers capture the basic invariants most filters should preserve.
void assertImageShapeUnchanged(const Image& before, const Image& after)
{
    assert(before.getWidth() == after.getWidth());
    assert(before.getHeight() == after.getHeight());
    assert(before.getChannels() == after.getChannels());
}

void assertImageValuesInRange(const Image& image)
{
    for (int y = 0; y < image.getHeight(); ++y) {
        for (int x = 0; x < image.getWidth(); ++x) {
            for (int c = 0; c < image.getChannels(); ++c) {
                const int value = image.getPixel(x, y, c);
                assert(value >= 0);
                assert(value <= 255);
            }
        }
    }
}

bool imagesEqual(const Image& lhs, const Image& rhs)
{
    if (lhs.getWidth() != rhs.getWidth() ||
        lhs.getHeight() != rhs.getHeight() ||
        lhs.getChannels() != rhs.getChannels()) {
        return false;
    }

    for (int y = 0; y < lhs.getHeight(); ++y) {
        for (int x = 0; x < lhs.getWidth(); ++x) {
            for (int c = 0; c < lhs.getChannels(); ++c) {
                if (lhs.getPixel(x, y, c) != rhs.getPixel(x, y, c)) {
                    return false;
                }
            }
        }
    }

    return true;
}

// Most 2D filters should preserve image shape and keep values in byte range.
template <typename FilterType>
void assertImageFilterPreservesShapeAndRange(FilterType& filter, const Image& source)
{
    Image result = source;
    filter.apply(result);

    assertImageShapeUnchanged(source, result);
    assertImageValuesInRange(result);
}

// Deterministic filters should give identical output on repeated runs.
template <typename FilterType>
void assertImageFilterIsDeterministic(FilterType& filter, const Image& source)
{
    Image first = source;
    Image second = source;

    filter.apply(first);
    filter.apply(second);

    assert(imagesEqual(first, second));
}

// Broad smoke test for common image filters on a non-trivial RGB input.
void testGeneralImageFilterInvariants()
{
    Image rgb = makePatternImage(17, 13, 3);

    Brightness brightness(25);
    assertImageFilterPreservesShapeAndRange(brightness, rgb);

    GrayScale grayscale;
    assertImageFilterPreservesShapeAndRange(grayscale, rgb);

    HistogramEqualisation equalisation;
    assertImageFilterPreservesShapeAndRange(equalisation, rgb);

    Threshold threshold;
    assertImageFilterPreservesShapeAndRange(threshold, rgb);

    SaltAndPepper saltAndPepper(0.15);
    assertImageFilterPreservesShapeAndRange(saltAndPepper, rgb);

    BoxBlurFilter boxBlur(3);
    assertImageFilterPreservesShapeAndRange(boxBlur, rgb);

    GaussianBlurFilter gaussianBlur(3, 2.0);
    assertImageFilterPreservesShapeAndRange(gaussianBlur, rgb);

    MedianBlurFilter medianBlur(3);
    assertImageFilterPreservesShapeAndRange(medianBlur, rgb);

    SharpenFilter sharpen;
    assertImageFilterPreservesShapeAndRange(sharpen, rgb);

    EmbossFilter emboss;
    assertImageFilterPreservesShapeAndRange(emboss, rgb);
}

// Most filters should behave sensibly on both grayscale and RGB images.
void testImageFilterChannelCompatibility()
{
    Image gray = makePatternImage(17, 13, 1);
    Image rgb = makePatternImage(17, 13, 3);

    Brightness brightness(25);
    assertImageFilterPreservesShapeAndRange(brightness, gray);
    assertImageFilterPreservesShapeAndRange(brightness, rgb);

    GrayScale grayscale;
    assertImageFilterPreservesShapeAndRange(grayscale, gray);
    assertImageFilterPreservesShapeAndRange(grayscale, rgb);

    HistogramEqualisation equalisation;
    assertImageFilterPreservesShapeAndRange(equalisation, gray);
    assertImageFilterPreservesShapeAndRange(equalisation, rgb);

    Threshold threshold;
    assertImageFilterPreservesShapeAndRange(threshold, gray);
    assertImageFilterPreservesShapeAndRange(threshold, rgb);

    SaltAndPepper saltAndPepper(0.15);
    assertImageFilterPreservesShapeAndRange(saltAndPepper, gray);
    assertImageFilterPreservesShapeAndRange(saltAndPepper, rgb);

    BoxBlurFilter boxBlur(3);
    assertImageFilterPreservesShapeAndRange(boxBlur, gray);
    assertImageFilterPreservesShapeAndRange(boxBlur, rgb);

    GaussianBlurFilter gaussianBlur(3, 2.0);
    assertImageFilterPreservesShapeAndRange(gaussianBlur, gray);
    assertImageFilterPreservesShapeAndRange(gaussianBlur, rgb);

    MedianBlurFilter medianBlur(3);
    assertImageFilterPreservesShapeAndRange(medianBlur, gray);
    assertImageFilterPreservesShapeAndRange(medianBlur, rgb);

    SharpenFilter sharpen;
    assertImageFilterPreservesShapeAndRange(sharpen, gray);
    assertImageFilterPreservesShapeAndRange(sharpen, rgb);

    EmbossFilter emboss;
    assertImageFilterPreservesShapeAndRange(emboss, gray);
    assertImageFilterPreservesShapeAndRange(emboss, rgb);
}

// Randomised filters are excluded here on purpose.
void testDeterministicImageFilters()
{
    Image rgb = makePatternImage(17, 13, 3);

    Brightness brightness(25);
    assertImageFilterIsDeterministic(brightness, rgb);

    GrayScale grayscale;
    assertImageFilterIsDeterministic(grayscale, rgb);

    HistogramEqualisation equalisation;
    assertImageFilterIsDeterministic(equalisation, rgb);

    Threshold threshold;
    assertImageFilterIsDeterministic(threshold, rgb);

    BoxBlurFilter boxBlur(3);
    assertImageFilterIsDeterministic(boxBlur, rgb);

    GaussianBlurFilter gaussianBlur(3, 2.0);
    assertImageFilterIsDeterministic(gaussianBlur, rgb);

    MedianBlurFilter medianBlur(3);
    assertImageFilterIsDeterministic(medianBlur, rgb);

    SharpenFilter sharpen;
    assertImageFilterIsDeterministic(sharpen, rgb);

    EmbossFilter emboss;
    assertImageFilterIsDeterministic(emboss, rgb);
}

} // namespace

int main()
{
    std::cout << "Running Filter Invariant Tests..." << std::endl;

    testGeneralImageFilterInvariants();
    testImageFilterChannelCompatibility();
    testDeterministicImageFilters();

    return 0;
}
