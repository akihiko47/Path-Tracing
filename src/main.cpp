#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream> // output
#include <iomanip>  // formatting output

#include "ray.h"

// image
const uint32_t imageWidth  = 256;
const uint32_t imageHeight = 144;
const uint8_t  numChannels = 3;

bool HitSphere(const glm::vec3 &p, float radius, const Ray &r) {
    glm::vec3 no = p - r.GetOrigin();
    float a = glm::dot(r.GetDirection(), r.GetDirection());
    float b = 2.0 * glm::dot(r.GetDirection(), no);
    float c = glm::dot(no, no) - radius * radius;
    float D = b * b - 4 * a * c;
    return D >= 0;
}

void SavePngImage(uint8_t *image, const std::string &name) {
    std::string filePath = name + ".png";
    #ifdef OUTPUT_DIR
        const char* outputDir = OUTPUT_DIR;
        filePath = std::string(outputDir) + "/" + name + ".png";
    #endif
    int res = stbi_write_png(filePath.c_str(), imageWidth, imageHeight, numChannels, image, imageWidth * numChannels);
    if (res == 0) {
        std::cerr << "Error while saving image: " << filePath << "\n";
    }
}

void SetPixelColor(uint8_t *image, uint32_t px, uint32_t py, glm::vec3 color) {
    int ir = int(255.999 * color.r);
    int ig = int(255.999 * color.g);
    int ib = int(255.999 * color.b);

    image[numChannels * (py * imageWidth + px)]     = ir;
    image[numChannels * (py * imageWidth + px) + 1] = ig;
    image[numChannels * (py * imageWidth + px) + 2] = ib;
}

glm::vec3 RayColor(const Ray &r) {
    if (HitSphere(glm::vec3(0, 0, -1), 0.5, r)) {
        return glm::vec3(1, 0, 0);
    }

    glm::vec3 dir = glm::normalize(r.GetDirection());
    float t = 0.5f * (dir.y + 1.0f);
    return glm::mix(glm::vec3(1.0), glm::vec3(0.5, 0.7, 1.0), t);
}

int main() {

    // camera
    float focalLength = 1.0;
    float viewportHeight = 2.0;
    float viewportWidth = viewportHeight * (float(imageWidth) / imageHeight);
    glm::vec3 cameraPos = glm::vec3(0);

    // viewport vectors
    glm::vec3 viewportU = glm::vec3(viewportWidth, 0, 0);
    glm::vec3 viewportV = glm::vec3(0, -viewportHeight, 0);
    glm::vec3 pixelDeltaU = viewportU / float(imageWidth);
    glm::vec3 pixelDeltaV = viewportV / float(imageHeight);
    glm::vec3 viewportUpperLeft = cameraPos - glm::vec3(0, 0, focalLength) - viewportU / 2.0f - viewportV / 2.0f;
    glm::vec3 pixel00Pos = viewportUpperLeft + 0.5f * (pixelDeltaU + pixelDeltaV);
    
    uint8_t *img = new uint8_t[imageWidth * imageHeight * numChannels];

    std::clog << "Starting to render...\n" << std::flush;

    for (uint32_t j = 0; j < imageHeight; j++) {
        std::clog << std::setprecision(2) << "\tProgress: " << int((float(j) / float(imageHeight)) * 100) << "%" << '\n' << std::flush;
        for (uint32_t i = 0; i < imageWidth; i++) {
            float screenU = float(i) / (imageWidth - 1);
            float screenV = float(j) / (imageHeight - 1);

            glm::vec3 pixelPos = pixel00Pos + (float(i) * pixelDeltaU) + (float(j) * pixelDeltaV);
            glm::vec3 rd = pixelPos - cameraPos;
            glm::vec3 ro = cameraPos;

            Ray r = Ray(ro, rd);
            
            glm::vec3 pixelColor = RayColor(r);

            SetPixelColor(img, i, j, pixelColor);
        }
    }

    std::clog << "Done! Saving image.\n";
    SavePngImage(img, "test");

    delete[] img;

    return 0;
}