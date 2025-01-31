#pragma once

#include <iostream> 
#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#endif

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "stb/stb_image.h"

#include "glm/glm.hpp"


namespace art {
    class Image final {
    public:
        Image() : m_height(0), m_width(0), m_numChannels(0), m_data(nullptr), m_stbImpl(false) { }

        Image(uint32_t width, uint32_t height, uint8_t nChannels = 3) : m_width(width), m_height(height), m_numChannels(nChannels), m_stbImpl(false) {
            m_data = new uint8_t[m_width * m_height * m_numChannels];
        }

        Image(const std::string &filename) : m_stbImpl(true) {
            LoadFromFile(filename); 
        }

        ~Image() { 
            if (m_stbImpl) {
                stbi_image_free(m_data);
            } else {
                delete[] m_data;
            }
        }

        const uint32_t GetWidth()       const { return m_width; }
        const uint32_t GetHeight()      const { return m_height; }
        const uint32_t GetNumChannels() const { return m_numChannels; }

        glm::vec3 GetPixelColor(uint32_t px, uint32_t py) const {
            // cant sample out of bounds
            if (!(0 <= px && px <= (m_width - 1)) || !(0 <= py && py <= (m_height - 1))) {
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

        void SetPixelColor(uint32_t px, uint32_t py, glm::vec3 color, bool gammaCorrection = true) {
            px = std::clamp(px, uint32_t(0), GetWidth());
            py = std::clamp(py, uint32_t(0), GetHeight());

            if (gammaCorrection) {
                color = LinearToGamma(color);
            }

            int ir = int(255.999 * std::clamp(color.r, 0.0f, 1.0f));
            int ig = int(255.999 * std::clamp(color.g, 0.0f, 1.0f));
            int ib = int(255.999 * std::clamp(color.b, 0.0f, 1.0f));

            m_data[m_numChannels * (py * m_width + px)]     = ir;
            m_data[m_numChannels * (py * m_width + px) + 1] = ig;
            m_data[m_numChannels * (py * m_width + px) + 2] = ib;
        }

        void SaveAsPng(const std::string &name) const {
            // saving to output directory specified by cmake
            std::string filePath = name;
            #ifdef OUTPUT_DIR
                const char* outputDir = OUTPUT_DIR;
                filePath = std::string(outputDir) + "/" + name;
            #endif
            filePath = std::filesystem::path(filePath).replace_extension("png").string();

            std::cout << "saving image: " << filePath << "\n";

            int res = stbi_write_png(filePath.c_str(), m_width, m_height, m_numChannels, m_data, m_width * m_numChannels);

#ifdef _WIN32
            // show image in viewer
            ShellExecute(NULL, "open", filePath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#endif
        }

        void LoadFromFile(const std::string &filename) {
            std::filesystem::path namePath(filename);  // need this to check if path is relative

            // if path is relative then seek inside textures and output folder (setup by cmake)
            // otherwise use absolute path specified by user
            std::string finalFilePath;
            if (namePath.is_relative()) {
                #ifdef TEXTURE_DIR
                    const char* textureDir = TEXTURE_DIR;
                    finalFilePath = std::string(textureDir) + "/" + filename;
                #endif

                // if image is not in texture directory, look in output directory (for filtering)
                if (!std::filesystem::exists(finalFilePath)) {
                    #ifdef OUTPUT_DIR
                        const char* outputDir = OUTPUT_DIR;
                        finalFilePath = std::string(outputDir) + "/" + filename;
                    #endif
                }
            } else {
                finalFilePath = filename;
            }

            if (!std::filesystem::exists(finalFilePath)) {
                std::cerr << "error! no such image: " << finalFilePath;
                exit(1);
            }

            std::cout << "loading image " << finalFilePath << "\n";

            m_numChannels = 3;
            int texWidth, texHeight, texChannels;
            m_data = stbi_load(finalFilePath.c_str(), &texWidth, &texHeight, &texChannels, m_numChannels);
            
            if (m_data == NULL) {
                std::cerr << "failed to load image!";
                exit(1);
            }

            m_width = texWidth;
            m_height = texHeight;
        }

        glm::vec3 LinearToGamma(glm::vec3 linearColor) const {
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

    private:
        uint8_t *m_data;

        uint32_t m_width;
        uint32_t m_height;
        uint8_t  m_numChannels;

        bool     m_stbImpl;
    };
}
