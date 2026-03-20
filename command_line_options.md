# Command line options

Your program should accept the command line options detailed below. The first option should always be `-i <input_image>` or `-d <data_volume>` to 
specify whether the program is going to be processing an image or volume, and to specify where to find the input. The last option should be the output image name (this does not require a flag). The program should then process the input image or volume according to the options provided, and save the output image to the specified location.

Note that each option (other than the -i/-d flags and the output name) can use either the long or short flag format: e.g. `--greyscale` or `-g`.

## Image Processing Options

- Input Image: `-i <input_image>`
- Output Image: `<output_image>` (must have extension .png or .jpg)

### Filters
- Greyscale: `--greyscale` or `-g`
- Brightness: `--brightness <value>` or `-b <value>`
  - If `<value>` is an integer (in the range -255 to 255), add that offset to all pixel intensities.
  - If `<value>` is set to `auto`, your code should perform an automatic brightness filter by setting the average value of all pixels (accross all chanels) to 128.
- Histogram Equalisation: `--histogram <type>` or `-h <type>` (e.g., HSV, HSL)
  - Note that `<type>` is an optional parameter. For greyscale images, it should have no effect. For RGB impages, it determines whether to use the intensity from the HSV or HSL colourspace. The default, if not provided, should be HSV.
- Blur: `--blur <type> <size> [<stdev>]` or `-r <type> <size> [<stdev>]` (e.g., Gaussian 5 2.0, Box 7, Median 3; note `<stdev>` is only required for Gaussian)
- Edge Detection: `--edge <type>` or `-e <type>` (e.g., Sobel, Prewitt, Scharr, RobertsCross)
- Laplacian Sharpening: `--sharpen` or `-L`
- Emboss: `--emboss <strength> <direction> <type>` or `-m <strength> <direction> <type>`
  - `<strength>` is a positive float, in the range 0–10.
  - `<direction>` is one of `NW`, `NE`, `SE`, or `SW`
  - Note that `<type>` is an optional parameter. For greyscale images, it should have no effect. For RGB impages, it determines whether to use the intensity from the HSV or HSL colourspace. The default, if not provided, should be HSV.
- Salt and Pepper Noise: `--saltpepper <amount>` or `-n <amount>`
- Threshold: `--threshold <value> <type>` or `-t <value> <type>` (e.g., 128 HSV, 64 HSL)
  - Note that `<type>` is an optional parameter. For greyscale images, it should have no effect. For RGB impages, it determines whether to use the intensity from the HSV or HSL colourspace. The default, if not provided, should be HSV.

You can specify one or multiple filters, by chaining the options together (e.g. `-g -r Median 3` will convert to greyscale and then apply a median blur filter).

## Volume Processing Options

- Input Volume: `-d <data_volume>`
  - Note that `<data_volume>` should include the path and the common beginning of the filename of the images of the volume, without the numerical part of the filename.
  - For example, `Scans/TestVolume/vol`, or `Scans/fracture/granite1_`
- Output Image: `<output_image>` (must have extension .png or .jpg)

### Volume Reading
- First Index: `--first <index>` or `-f <index>` (optional)
- Last Index: `--last <index>` or `-l <index>` (optional)
- Extension: `--extension <ext>` or `-x <ext>` (optional; default is png).

If first and last index are not defined, all images in volume should be read.

### Volume Blur Filter
- Blur: `--blur <type> <size> [<stdev>]` or `-r <type> <size> [<stdev>]` (e.g., `Gaussian 3 2.0, Median 3`; note `<stdev>` is only required for Gaussian)

Note that the blur filter is optional in volume processing; if it is spcified, the subsequent slice or projection will be applied to the blurred volume, otherwise it will be applied to the original volume.

### Volume Processing Options
- Slice: `--slice <plane> <constant>` or `-s <plane> <constant>` (e.g., `XZ 16`, `YZ 16`)
- Projection: `--projection <type>` or `-p <type>` (e.g., MIP, MinIP, meanAIP, medianAIP)

Note that it is necessary to specify one of these processing options (Slice or Projection), as this defines the output image from the command.

## Example Commands

- Brightness: `./APImageFiltersKruskal -i input.png -b 100 output.png`
- Greyscale: `./APImageFiltersKruskal -i input.png -g output.png`
- Histogram Equalisation: `./APImageFiltersKruskal -i input.png -h HSV output.png`
- Blur (Gaussian): `./APImageFiltersKruskal -i input.png -r Gaussian 5 2.0 output.png`
- Edge Detection (Sobel): `./APImageFiltersKruskal -i input.png --edge Sobel output.png`
- Laplacian Sharpening: `./APImageFiltersKruskal -i input.png --sharpen output.png`
- Emboss: `./APImageFiltersKruskal -i input.png --emboss 2.5 NE HSV output.png`
- Salt and Pepper Noise: `./APImageFiltersKruskal -i input.png --saltpepper 5 output.png`
- Threshold: `./APImageFiltersKruskal -i input.png --threshold 128 HSV output.png`
- Volume Slice (XZ): `./APImageFiltersKruskal -d volume -s XZ 16 output.png`
- Volume Projection (MIP): `./APImageFiltersKruskal -d volume -p MIP output.png`
