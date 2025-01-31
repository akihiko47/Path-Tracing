#pragma once

#include "hittable.hpp"


namespace art {

	class Scene : public Hittable {
	public:
		Scene() : m_skyboxTextureIndex(-1), m_skyboxColor(glm::vec3(0)) {}

		void AddObject(std::unique_ptr<Hittable> obj) {
			m_objects.push_back(std::move(obj));
		}

		void AddMaterial(std::unique_ptr<Material> mat) {
			m_materials.push_back(std::move(mat));
		}

		void AddTexture(std::unique_ptr<Texture> tex) {
			m_textures.push_back(std::move(tex));
		}

		void AddSkybox(std::unique_ptr<Texture> tex) {
			m_textures.push_back(std::move(tex));
			m_skyboxTextureIndex = m_textures.size() - 1;
		}

		void AddSkybox(const glm::vec3 &color) {
			m_skyboxColor = color;
		}

		glm::vec3 SampleSkybox(glm::vec3 dir) const {
			return (m_skyboxTextureIndex != -1) ? m_textures[m_skyboxTextureIndex]->Sample(0, 0, glm::vec3(0)) : m_skyboxColor;
		}

		bool Hit(const Ray& r, Interval tSpan, HitInfo& hitInfo) const override {
			HitInfo tempInfo;
			bool hit = false;
			float tClosest = tSpan.GetMax();

			for (const auto &object : m_objects) {
				if (object->Hit(r, Interval(tSpan.GetMin(), tClosest), tempInfo)) {
					hit = true;
					tClosest = tempInfo.t;
					hitInfo = tempInfo;
				}
			}

			return hit;
		}

	private:
		std::vector<std::unique_ptr<Hittable>> m_objects;
		std::vector<std::unique_ptr<Material>> m_materials;
		std::vector<std::unique_ptr<Texture>>  m_textures;

		glm::vec3 m_skyboxColor;
		int m_skyboxTextureIndex;  // if this index is -1 then skybox is solid color
	};

}