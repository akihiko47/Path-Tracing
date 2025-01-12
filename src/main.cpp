#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include <iostream>


const uint32_t image_width = 64;
const uint32_t image_height = 64;
const uint8_t num_channels = 3;

int main() {
    
    uint8_t img[image_width * image_height * num_channels];

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            double r = double(i) / (image_width - 1);
            double g = double(j) / (image_height - 1);
            double b = 0;

            int ir = int(255.999 * r);
            int ig = int(255.999 * g);
            int ib = int(255.999 * b);

            std::cout << ir << ' ' << ig << ' ' << ib << '\n';

            img[num_channels * (j * image_height + i)] = ir;
            img[num_channels * (j * image_height + i) + 1] = ig;
            img[num_channels * (j * image_height + i) + 2] = ib;
        }
    }

    stbi_write_png("test.png", image_width, image_height, num_channels, img, image_width * num_channels);

    return 0;
}