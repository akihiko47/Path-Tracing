#pragma once

#include <iostream> // output
#include <iomanip>  // formatting output

#include "hittable.hpp"
#include "image.hpp"


namespace art {
	class Camera {
	public:

		Camera() : m_pos(glm::vec3(0.0)) {}

		void Render(Image &image, const Hittable &scene) {
			// camera
			float focalLength = 1.0;
			float viewportHeight = 2.0;
			float viewportWidth = viewportHeight * (float(image.GetWidth()) / image.GetHeight());
			
			// viewport vectors
			glm::vec3 viewportU = glm::vec3(viewportWidth, 0, 0);
			glm::vec3 viewportV = glm::vec3(0, -viewportHeight, 0);
			glm::vec3 pixelDeltaU = viewportU / float(image.GetWidth());
			glm::vec3 pixelDeltaV = viewportV / float(image.GetHeight());
			glm::vec3 viewportUpperLeft = m_pos - glm::vec3(0, 0, focalLength) - viewportU / 2.0f - viewportV / 2.0f;
			glm::vec3 pixel00Pos = viewportUpperLeft + 0.5f * (pixelDeltaU + pixelDeltaV);

			std::clog << "Starting to render...\n" << std::flush;

			for (uint32_t j = 0; j < image.GetHeight(); j++) {
				std::clog << std::setprecision(2) << "\tProgress: " << int((float(j) / float(image.GetHeight())) * 100) << "%" << '\n' << std::flush;
				for (uint32_t i = 0; i < image.GetWidth(); i++) {
					float screenU = float(i) / (image.GetWidth() - 1);
					float screenV = float(j) / (image.GetHeight() - 1);

					glm::vec3 pixelPos = pixel00Pos + (float(i) * pixelDeltaU) + (float(j) * pixelDeltaV);
					glm::vec3 rd = pixelPos - m_pos;
					glm::vec3 ro = m_pos;

					art::Ray r = art::Ray(ro, rd);

					glm::vec3 pixelColor = RayColor(r, scene);

					image.SetPixelColor(i, j, pixelColor);
				}
			}
		}

	private:
		glm::vec3 RayColor(const art::Ray &r, const art::Hittable &scene) const {
			art::HitInfo info;
			if (scene.Hit(r, art::Interval(0, art::infinity), info)) {
				return 0.5f * (info.N + 1.0f);
			}

			glm::vec3 dir = glm::normalize(r.GetDirection());
			float a = 0.5f * (dir.y + 1.0f);
			return glm::mix(glm::vec3(1.0), glm::vec3(0.5, 0.7, 1.0), a);
		}

		glm::vec3 m_pos;
	};
}
