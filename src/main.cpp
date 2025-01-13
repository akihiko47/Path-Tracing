#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream> // output
#include <iomanip>  // formatting output


const uint32_t image_width  = 256;
const uint32_t image_height = 144;
const uint8_t  num_channels = 3;

void save_png_image(uint8_t *image, const std::string &name) {
    std::string filePath = name + ".png";
    #ifdef OUTPUT_DIR
        const char* outputDir = OUTPUT_DIR;
        filePath = std::string(outputDir) + "/" + name + ".png";
    #endif
    int res = stbi_write_png(filePath.c_str(), image_width, image_height, num_channels, image, image_width * num_channels);
    if (res == 0) {
        std::cerr << "Error while saving image: " << filePath << "\n";
    }
}

int main() {
    
    uint8_t *img = new uint8_t[image_width * image_height * num_channels];

    std::clog << "Starting to render...\n" << std::flush;

    for (int j = 0; j < image_height; j++) {
        std::clog << std::setprecision(2) << "\tProgress: " << int((float(j) / float(image_height)) * 100) << "%" << '\n' << std::flush;
        for (int i = 0; i < image_width; i++) {
            double r = double(i) / (image_width - 1);
            double g = double(j) / (image_height - 1);
            double b = 0;

            int ir = int(255.999 * r);
            int ig = int(255.999 * g);
            int ib = int(255.999 * b);

            img[num_channels * (j * image_width + i)] = ir;
            img[num_channels * (j * image_width + i) + 1] = ig;
            img[num_channels * (j * image_width + i) + 2] = ib;
        }
    }
    std::clog << "Done! Saving image.\n";
    save_png_image(img, "test");

    delete[] img;

    return 0;
}