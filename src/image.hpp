#pragma once

#include <iostream> 
#include <filesystem>

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "stb/stb_image.h"

#include "glm/glm.hpp"


namespace art {
    class Image final {
    public:
        Image() : m_width(64), m_height(64), m_numChannels(3) {
            m_data = new uint8_t[m_width * m_height * m_numChannels];
        }
        Image(uint32_t width, uint32_t height, uint8_t nChannels = 3) : m_width(width), m_height(height), m_numChannels(nChannels) {
            m_data = new uint8_t[m_width * m_height * m_numChannels];
        }
        Image(const std::string &filename) { LoadFromFile(filename); }
        ~Image() { delete m_data; }


        glm::vec3 GetPixelColor(uint32_t px, uint32_t py) const {
            // cant sample out of bounds
            if (!(0 <= px <= m_width) || !(0 <= px <= m_height)) {
                std::cerr << "cant get x = " << px << ", y = " << py << " from image with width = " << m_width << ", height = " << m_height << "\n";
                return glm::vec3(1, 0, 1);
            }

            // get pixel color in set of 3x[0, 1]
            glm::vec3 res(
                m_data[(py * m_width + px) * m_numChannels]     / 255.0f,
                m_data[(py * m_width + px) * m_numChannels + 1] / 255.0f,
                m_data[(py * m_width + px) * m_numChannels + 2] / 255.0f
            );
            return res;
        }

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

        void SaveAsPng(const std::string &name) const {
            // saving to output directory specified by cmake
            std::string filePath = name + ".png";
            #ifdef OUTPUT_DIR
                const char* outputDir = OUTPUT_DIR;
                filePath = std::string(outputDir) + "/" + name + ".png";
            #endif

            int res = stbi_write_png(filePath.c_str(), m_width, m_height, m_numChannels, m_data, m_width * m_numChannels);
            assert(res != 0);
        }

        void LoadFromFile(const std::string &filename) {
            m_numChannels = 3;

            std::string filePath = filename;
            std::filesystem::path namePath(filename);  // need this to check if path is relative

            // if path is relative then seek inside textures folder (setup by cmake)
            // otherwise use absolute path specified by user
            if (namePath.is_relative()) {
                #ifdef TEXTURE_DIR
                    const char* textureDir = TEXTURE_DIR;
                    filePath = std::string(textureDir) + "/" + filePath;
                #endif
            }

            std::cout << "loading image " << filePath << "\n";

            int texWidth, texHeight, texChannels;
            m_data = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, m_numChannels);
            
            assert(m_data != NULL);
            if (m_data == NULL) {
                std::cerr << "failed to load texture image!";
            }

            m_width = texWidth;
            m_height = texHeight;
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
