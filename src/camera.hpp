#pragma once

#include <iostream> // output
#include <iomanip>  // formatting output

#include "hittable.hpp"
#include "image.hpp"
#include "utils.hpp"

#define BOUNCE_DEBUG 0

namespace art {
	class Camera {
	public:

		Camera() : m_pos(glm::vec3(0.0)), m_nSamples(10), m_maxDepth(10) {}
		Camera(glm::vec3 Pos, uint32_t nSamples, uint32_t maxDepth) : m_pos(Pos), m_nSamples(nSamples), m_maxDepth(maxDepth) {}

		void Render(Image &image, const Hittable &scene) {
			// camera
			float focalLength = 1.0;
			float viewportHeight = 2.0;
			float viewportWidth = viewportHeight * (float(image.GetWidth()) / image.GetHeight());
			
			// viewport vectors
			glm::vec3 viewportU = glm::vec3(viewportWidth, 0, 0);
			glm::vec3 viewportV = glm::vec3(0, -viewportHeight, 0);
			m_pixelDeltaU = viewportU / float(image.GetWidth());
			m_pixelDeltaV = viewportV / float(image.GetHeight());
			glm::vec3 viewportUpperLeft = m_pos - glm::vec3(0, 0, focalLength) - viewportU / 2.0f - viewportV / 2.0f;
			m_pixel00Pos = viewportUpperLeft + 0.5f * (m_pixelDeltaU + m_pixelDeltaV);

			// pre compute
			float pixel_scale = 1.0f / m_nSamples;

			std::clog << "Starting to render...\n" << std::flush;

			for (uint32_t j = 0; j < image.GetHeight(); j++) {
				std::clog << std::setprecision(2) << "\tProgress: " << int((float(j) / float(image.GetHeight())) * 100) << "%" << '\n' << std::flush;
				for (uint32_t i = 0; i < image.GetWidth(); i++) {

					glm::vec3 pixelColor(0);
					for (int sample = 0; sample < m_nSamples; sample++) {
						art::Ray r = GetRay(i, j);
						pixelColor += RayColor(r, m_maxDepth, scene);
					}
					image.SetPixelColor(i, j, pixelColor * pixel_scale);
				}
			}
		}

	private:
		glm::vec3 RayColor(const art::Ray &r, int currDepth, const art::Hittable &scene) const {
			if (currDepth <= 0) {
				if (BOUNCE_DEBUG) {
					return glm::vec3(1.0, 0.0, 0.0);
				}
				return glm::vec3(0.0);
			}

			art::HitInfo info;
			if (scene.Hit(r, art::Interval(0.001, art::infinity), info)) {
				glm::vec3 dir = info.N + RandomVec();
				if (BOUNCE_DEBUG) {
					return RayColor(Ray(info.p, dir), currDepth - 1, scene);
				}
				return 0.5f * RayColor(Ray(info.p, dir), currDepth - 1, scene);
			}

			// Ray out of scene
			if (BOUNCE_DEBUG) {
				return glm::vec3(1.0 - (float(currDepth) / m_maxDepth), 0.0, 0.0);
			}

			glm::vec3 dir = glm::normalize(r.GetDirection());
			float a = 0.5f * (dir.y + 1.0f);
			return glm::mix(glm::vec3(1.0), glm::vec3(0.5, 0.7, 1.0), a);
			
		}

		Ray GetRay(uint32_t i, uint32_t j) const {
			glm::vec2 offset = art::RandomInSquare();
			glm::vec3 pixelPos = m_pixel00Pos + ((i + offset.x) * m_pixelDeltaU) + ((j + offset.y) * m_pixelDeltaV);
			glm::vec3 rd = pixelPos - m_pos;
			glm::vec3 ro = m_pos;

			return Ray(ro, rd);
		}

	private:
		uint32_t  m_nSamples;
		uint32_t  m_maxDepth;
		glm::vec3 m_pos;
		
		glm::vec3 m_pixel00Pos;
		glm::vec3 m_pixelDeltaU;
		glm::vec3 m_pixelDeltaV;

	};
}
