__kernel void blur_filter(__global const uchar *input,
                          __global uchar *output,
                          const int filter_size,
                          const int width,
                          const int height) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    if (x >= width || y >= height) return;
    
    int sum = 0;
    int count = 0;
    const int radius = filter_size / 2;  // radius: 3x3:1, 5x5:2
    for (int ky = -radius; ky <= radius; ky++) {
        for (int kx = -radius; kx <= radius; kx++) {
            int nx = x + kx;
            int ny = y + ky;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                sum += input[ny * width + nx];
                count++;
            }
        }
    }
    output[y * width + x] = (uchar)(sum / count);
}
