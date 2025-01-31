#pragma once

#include <iostream> // output
#include <iomanip>  // formatting output
#include <vector>
#include <thread>
#include <execution>

#include "hittable.hpp"
#include "image.hpp"
#include "utils.hpp"
#include "material.hpp"
#include "scene.hpp"

namespace art {
	class Camera final {
	public:

		Camera() : 
			m_nSamples(10), 
			m_maxDepth(10),
			m_pos(glm::vec3(0.0)),
			m_lookAt(glm::vec3(0, 0, -1)),
			m_fov(45),
			m_defocusAngle(0),
			m_focusDist(1) {}

		Camera(uint32_t nSamples, uint32_t maxDepth, glm::vec3 Pos, glm::vec3 lookAt, float fov, float defocusAngle, float focusDist) :
			m_nSamples(nSamples), 
			m_maxDepth(maxDepth), 
			m_pos(Pos),
			m_lookAt(lookAt),
			m_fov(fov),
			m_defocusAngle(defocusAngle),
			m_focusDist(focusDist) {}

		void Render(Image &image, Scene &scene) const {
			// camera
			float h = std::tan(glm::radians(m_fov) / 2);
			float viewportHeight = 2 * h * m_focusDist;
			float viewportWidth = viewportHeight * (float(image.GetWidth()) / image.GetHeight());

			// vieport basis
			glm::vec3 u, v, w;
			w = glm::normalize(m_pos - m_lookAt);
			u = glm::normalize(glm::cross(glm::vec3(0, 1, 0), w));
			v = glm::cross(w, u);
			
			// viewport vectors
			glm::vec3 viewportU = viewportWidth * u;
			glm::vec3 viewportV = viewportHeight * -v;
			m_pixelDeltaU = viewportU / float(image.GetWidth());
			m_pixelDeltaV = viewportV / float(image.GetHeight());
			glm::vec3 viewportUpperLeft = m_pos - (m_focusDist * w) - viewportU / 2.0f - viewportV / 2.0f;
			m_pixel00Pos = viewportUpperLeft + 0.5f * (m_pixelDeltaU + m_pixelDeltaV);

			// defocus disk (we shoot rays from here)
			float defocusRadius = m_focusDist * std::tan(glm::radians(m_defocusAngle / 2));
			m_defocusU = u * defocusRadius;
			m_defocusV = v * defocusRadius;

			// stratification
			stratNumRow = static_cast<int>(std::sqrt(m_nSamples));
			stratRegionEdgeLength = 1.0 / stratNumRow;

			// multi-threading
			uint32_t height = image.GetHeight();
			m_iteratorV.resize(height);
			for (uint32_t i = 0; i < height; i++) {
				m_iteratorV[i] = i;
			}

			// pre compute
			float pixel_scale = 1.0f / (stratNumRow * stratNumRow);

			// main loop
			std::cout << "Starting to render...\n" << std::flush;

#define MT 1
#if MT
			std::for_each(std::execution::par, m_iteratorV.begin(), m_iteratorV.end(),
				[&](uint32_t j) {

					for (uint32_t i = 0, ie = image.GetWidth(); i != ie; ++i) {

						glm::vec3 pixelColor(0);
						for (uint32_t s_j = 0; s_j < stratNumRow; s_j++) {
							for (uint32_t s_i = 0; s_i < stratNumRow; s_i++) {
								art::Ray r = GetRay(i, j, s_i, s_j);
								pixelColor += RayColor(r, m_maxDepth, scene);
							}
						}

						image.SetPixelColor(i, j, pixelColor * pixel_scale);
					}

					std::stringstream msg;
					msg << "\tRow " << j << "/" << height << " done\n";
					std::cout << msg.str();

				}
			);
#else
			for (uint32_t j = 0, je = image.GetHeight(); j != je; ++j) {
				std::cout << std::setprecision(2) << "\tProgress: " << int((float(j) / float(image.GetHeight())) * 100) << "%" << '\n' << std::flush;
				for (uint32_t i = 0, ie = image.GetWidth(); i != ie; ++i) {

					glm::vec3 pixelColor(0);
					for (uint32_t s_j = 0; s_j < stratNumRow; s_j++) {
						for (uint32_t s_i = 0; s_i < stratNumRow; s_i++) {
							art::Ray r = GetRay(i, j, s_i, s_j);
							pixelColor += RayColor(r, m_maxDepth, scene);
						}
					}

					image.SetPixelColor(i, j, pixelColor * pixel_scale);
				}
			}
#endif
		}

	private:
		glm::vec3 RayColor(const art::Ray &r, int currDepth, const art::Scene &scene) const {
			if (currDepth <= 0) {  // reached bounce limit
				return glm::vec3(0.0);
			}

			// return background (skybox) if no hit
			art::HitInfo info;
			if (!scene.Hit(r, art::Interval(0.001, art::infinity), info)) {
				currDepth = 0;
				return scene.SampleSkybox(r.GetDirection());
			}

			Ray rayOut;
			glm::vec3 attenuation;
			glm::vec3 emitted = info.mat->Emission(info.u, info.v, info.p);

			if (!info.mat->Scatter(r, info, attenuation, rayOut)) {
				return emitted;
			}

			glm::vec3 scattered = attenuation * RayColor(rayOut, currDepth - 1, scene);

			return emitted + scattered;
		}

		Ray GetRay(uint32_t i, uint32_t j, uint32_t s_i, uint32_t s_j) const {
			glm::vec2 offset = art::RandomInStratifiedSquare(s_i, s_j, stratRegionEdgeLength);
			glm::vec3 pixelPos = m_pixel00Pos + ((i + offset.x) * m_pixelDeltaU) + ((j + offset.y) * m_pixelDeltaV);		

			glm::vec3 ro;
			if (m_defocusAngle <= 0) {
				// no defocus blur
				ro = m_pos;
			} else {
				// apply defocus blur
				glm::vec3 p = RandomOnDisk();
				ro = m_pos + (p.x * m_defocusU) + (p.y * m_defocusV);
			}
			glm::vec3 rd = glm::normalize(pixelPos - ro);

			return Ray(ro, rd);
		}

	private:
		uint32_t  m_nSamples;
		uint32_t  m_maxDepth;
		glm::vec3 m_pos;
		glm::vec3 m_lookAt;
		float     m_fov;
		float     m_defocusAngle;
		float     m_focusDist;
		
		// these fields can be changed in Render() method 
		// (semantic constancy)
		mutable glm::vec3 m_pixel00Pos;
		mutable glm::vec3 m_pixelDeltaU;
		mutable glm::vec3 m_pixelDeltaV;
		mutable glm::vec3 m_defocusU;
		mutable glm::vec3 m_defocusV;

		// stratification
		mutable int stratNumRow;              // number of stratification regions in pixel row
		mutable float stratRegionEdgeLength;  // edge length of each stratification region

		// multi-threading
		mutable std::vector<uint32_t> m_iteratorV;
	};
}
