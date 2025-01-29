#pragma once

#include "image.hpp"

namespace art {

	float Normpdf(float x, float sigma) {
		return 0.39894 * exp(-0.5 * x * x / (sigma * sigma)) / sigma;
	}

	float Normpdf3(glm::vec3 v, float sigma) {
		return 0.39894 * exp(-0.5 * glm::dot(v, v) / (sigma * sigma)) / sigma;
	}

	std::unique_ptr<Image> BilateralFilter(const Image &img, uint32_t msize, float sigma, float bsigma) {
		// shadertoy.com/view/4dfGDH

		//declare stuff
		const int kSize = (msize - 1) / 2;
		std::vector<float> kernel;
		kernel.resize(msize);

		// create new image
		std::unique_ptr<Image> newImg = std::make_unique<Image>(img.GetWidth(), img.GetHeight(), img.GetNumChannels());

		//create the 1-D kernel
		for (int32_t j = 0; j <= kSize; ++j) {
			kernel[kSize + j] = kernel[kSize - j] = Normpdf(float(j), sigma);
		}

		for (int32_t py = 0; py != img.GetHeight(); ++py) {
			for (int32_t px = 0; px != img.GetWidth(); ++px) {

				//declare stuff
				glm::vec3 final_colour = glm::vec3(0.0);
				float Z = 0.0;

				glm::vec3 c = img.GetPixelColor(px, py);
				glm::vec3 cc;
				float factor;
				float bZ = 1.0 / Normpdf(0.0, bsigma);
				//read out the texels
				for (int32_t i = -kSize; i <= kSize; ++i) {
					for (int32_t j = -kSize; j <= kSize; ++j) {
						cc = img.GetPixelColor(
							std::clamp(px + j, 0, static_cast<int>(img.GetWidth()) - 1),
							std::clamp(py + i, 0, static_cast<int>(img.GetHeight()) - 1)
						);
						factor = Normpdf3(cc - c, bsigma) * bZ * kernel[kSize + j] * kernel[kSize + i];
						Z += factor;
						final_colour += factor * cc;
					}
				}
				newImg->SetPixelColor(px, py, final_colour / Z, false);
			}
		}

		return newImg;
	}
}