#pragma once

#include "glm/glm.hpp"
#include "image.hpp"

namespace art {
	class Texture {
	public:
		virtual ~Texture() = default;

		virtual glm::vec3 Sample(float u, float v, const glm::vec3 &p, const glm::vec3 &dir) const { return glm::vec3(1, 0, 1); }
	};


	class SolidColorTexture : public Texture {
	public:
		SolidColorTexture(const glm::vec3 &albedo) : m_albedo(albedo) {}

		glm::vec3 Sample(float u, float v, const glm::vec3 &p, const glm::vec3 &dir) const {
			return m_albedo;
		}

	private:
		glm::vec3 m_albedo;
	};


	class CheckerTexture : public Texture {
	public:
		CheckerTexture(float scale, const Texture *evenTexture, const Texture *oddTexture) :
			m_scale(scale),
			m_evenTexture(evenTexture),
			m_oddTexture(oddTexture) {}

		glm::vec3 Sample(float u, float v, const glm::vec3 &p, const glm::vec3 &dir) const {
			int x = static_cast<int>(std::floor(m_scale * p.x));
			int y = static_cast<int>(std::floor(m_scale * p.y));
			int z = static_cast<int>(std::floor(m_scale * p.z));

			if ((x + y + z) % 2 == 0) {
				return m_evenTexture->Sample(u, v, p, glm::vec3(0));
			} else {
				return m_oddTexture->Sample(u, v, p, glm::vec3(0));
			}
		}

	private:
		float          m_scale;
		const Texture *m_evenTexture;
		const Texture *m_oddTexture;
	};


	class ImageTexture : public Texture {
	public:
		ImageTexture(const std::string &filename) :
			m_image(filename) {}

		glm::vec3 Sample(float u, float v, const glm::vec3 &p, const glm::vec3 &dir) const {
			float ip;  // dummy parameter for integer part

			// getting fractional part of uv (for repeating)
			u = std::modf(u, &ip);
			v = 1.0 - std::modf(v, &ip);  // reversing y

			uint32_t px = u * (m_image.GetWidth() - 1);
			uint32_t py = v * (m_image.GetHeight() - 1);
			return m_image.GetPixelColor(px, py);
		}

	private:
		const Image m_image;
	};

	class CubemapTexture : public Texture {
	public:
		CubemapTexture(std::vector<std::string> faces) {
			std::cout << "loading cubemap with images:\n";
			if (faces.size() != 6) {
				std::cerr << "error! not enough images for cubemap\n";
				exit(1);
			}

			m_images.reserve(6);
			for (int i = 0; i != faces.size(); ++i) {
				m_images.emplace_back(std::make_unique<Image>(faces[i]));
			}
		}

		glm::vec3 Sample(float u, float v, const glm::vec3 &p, const glm::vec3 &dir) const {
			glm::vec3 normDir = glm::normalize(dir);
			int faceIndex = GetFaceIndex(normDir);
			glm::vec2 uv = GetUVOnFace(faceIndex, normDir);

			uint32_t px = uv.x * static_cast<float>(m_images[faceIndex]->GetWidth() - 1);
			uint32_t py = uv.y * static_cast<float>(m_images[faceIndex]->GetHeight() - 1);
			return m_images[faceIndex]->GetPixelColor(px, py);
		}

	private:
		int GetFaceIndex(const glm::vec3 &dir) const {
			glm::vec3 absDir = glm::abs(dir);
			float maxComponent = std::fmax(absDir.x, std::fmax(absDir.y, absDir.z));

			if (maxComponent == absDir.x) {
				return dir.x > 0 ? 0 : 1; // +X or -X
			} else if (maxComponent == absDir.y) {
				return dir.y > 0 ? 2 : 3; // +Y or -Y
			} else {
				return dir.z > 0 ? 4 : 5; // +Z or -Z
			}
		}

		glm::vec2 GetUVOnFace(int faceIndex, const glm::vec3 &dir) const {
			glm::vec3 absDir = glm::abs(dir);
			float maxComponent = std::fmax(absDir.x, std::fmax(absDir.y, absDir.z));

			glm::vec2 uv;
			switch (faceIndex) {
				case 0: // +X
					uv = glm::vec2(-dir.z / maxComponent, -dir.y / maxComponent);
					break;
				case 1: // -X
					uv = glm::vec2(dir.z / maxComponent, -dir.y / maxComponent);
					break;
				case 2: // +Y
					uv = glm::vec2(dir.x / maxComponent, dir.z / maxComponent);
					break;
				case 3: // -Y
					uv = glm::vec2(dir.x / maxComponent, -dir.z / maxComponent);
					break;
				case 4: // +Z
					uv = glm::vec2(dir.x / maxComponent, -dir.y / maxComponent);
					break;
				case 5: // -Z
					uv = glm::vec2(-dir.x / maxComponent, -dir.y / maxComponent);
					break;
			}


			uv = uv * 0.5f + 0.5f;
			return uv;
		}

		std::vector<std::unique_ptr<Image>> m_images;
	};
}

