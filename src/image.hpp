#pragma once

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "glm/glm.hpp"

#include <iostream> 

namespace art {
    class Image {
    public:
        Image() : m_width(64), m_height(64), m_numChannels(3) {
            m_data = new uint8_t[m_width * m_height * m_numChannels];
        }
        Image(uint32_t width, uint32_t height, uint8_t nChannels = 3) : m_width(width), m_height(height), m_numChannels(nChannels) {
            m_data = new uint8_t[m_width * m_height * m_numChannels];
        }
        ~Image() { delete m_data; }


        void SetPixelColor(uint32_t px, uint32_t py, glm::vec3 color) {
            color = LinearToGamma(color);

            int ir = int(255.999 * color.r);
            int ig = int(255.999 * color.g);
            int ib = int(255.999 * color.b);

            m_data[m_numChannels * (py * m_width + px)]     = ir;
            m_data[m_numChannels * (py * m_width + px) + 1] = ig;
            m_data[m_numChannels * (py * m_width + px) + 2] = ib;
        }

        glm::vec3 LinearToGamma(glm::vec3 linearColor) {
            if (linearColor.r > 0) {
                linearColor.r = std::sqrt(linearColor.r);
            }
            if (linearColor.g > 0) {
                linearColor.g = std::sqrt(linearColor.g);
            }
            if (linearColor.b > 0) {
                linearColor.b = std::sqrt(linearColor.b);
            }
            return linearColor;
        }

        void SaveAsPng(const std::string &name) {
            std::string filePath = name + ".png";
            #ifdef OUTPUT_DIR
                const char* outputDir = OUTPUT_DIR;
                filePath = std::string(outputDir) + "/" + name + ".png";
            #endif
            int res = stbi_write_png(filePath.c_str(), m_width, m_height, m_numChannels, m_data, m_width * m_numChannels);
            if (res == 0) {
                std::cerr << "Error while saving image: " << filePath << "\n";
            }
        }

        uint32_t GetWidth()       const { return m_width; }
        uint32_t GetHeight()      const { return m_height; }
        uint32_t GetNumChannels() const { return m_numChannels; }

    private:
        uint8_t *m_data;

        uint32_t m_width;
        uint32_t m_height;
        uint8_t  m_numChannels;
    };
}
