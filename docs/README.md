# Performance Analysis — Edge Detection Filters

Benchmark results for the four edge detection filters implemented in this project: Sobel, Prewitt, Scharr, and RobertsCross.

Each measurement is the median of 5 runs on synthetic RGB images generated at resolutions from 64×64 to 2048×2048. Benchmarks were run using the `benchmark.py` script in the project root.

---

## Image Resolution vs Processing Time

![Image Resolution vs Processing Time](plot_size_vs_time.png)

*Figure 1: Processing time across image resolutions. At small resolutions, process initialisation overhead (~5–8 ms) dominates, masking differences between filters. Computation cost becomes measurable at 512×512 and above.*

---

## Filter Speed Comparison (512×512)

![Filter Speed Comparison](plot_filter_comparison.png)

*Figure 2: Median processing time for each filter on a 512×512 image (n=5, error bars show standard deviation). All filters complete within 10 ms; Prewitt is marginally fastest at 7.82 ms.*

---

## Linear Scaling Validation

![Linear Scaling](plot_linear_scaling.png)

*Figure 3: Processing time versus total pixel count. The dashed O(n) reference line is fit on resolutions ≥256×256. Results are consistent with linear time complexity O(n) in image area.*
