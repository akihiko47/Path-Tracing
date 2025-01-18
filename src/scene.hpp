#pragma once

#include "hittable.hpp"
#include "material.hpp"
#include "texture.hpp"

namespace art {

	class Scene : public Hittable {
	public:
		Scene() { PopulateScene(); }
		Scene(const std::string &fileName) { PopulateScene(fileName); }

		~Scene() {
			for (Material *material : m_materials) {
				delete material;
			}
			for (Hittable *object : m_objects) {
				delete object;
			}
		}

		void AddObject(Hittable *obj) {
			m_objects.push_back(obj);
		}

		bool Hit(const Ray& r, Interval tSpan, HitInfo& hitInfo) const override {
			HitInfo tempInfo;
			bool hit = false;
			float tClosest = tSpan.GetMax();

			for (Hittable *object : m_objects) {
				if (object->Hit(r, Interval(tSpan.GetMin(), tClosest), tempInfo)) {
					hit = true;
					tClosest = tempInfo.t;
					hitInfo = tempInfo;
				}
			}

			return hit;
		}

	private:
		void PopulateScene() {
			/*m_textures.push_back(new SolidColorTexture(glm::vec3(0.6, 0.6, 0.6)));
			m_textures.push_back(new SolidColorTexture(glm::vec3(0.2, 0.2, 0.2)));
			m_textures.push_back(new SolidColorTexture(glm::vec3(0.6, 0.0, 0.0)));
			m_textures.push_back(new CheckerTexture(1.0, m_textures[0], m_textures[1]));

			m_materials.push_back(new Lambertian(m_textures[3]));
			m_materials.push_back(new Metal(glm::vec3(0.1, 0.1, 0.1), 1.0));
			m_materials.push_back(new Metal(glm::vec3(0.8, 0.6, 0.2), 1.0));
			m_materials.push_back(new Lambertian(m_textures[2]));
			m_materials.push_back(new Dielectric(1.5, glm::vec3(0.2, 0.2, 0.9), 1.0));

			m_objects.push_back(new Sphere(glm::vec3(0.0, -1000.5, -1.0), 1000.0, m_materials[0]));

			for (int i = 0; i < 20; i++) {
				m_objects.push_back(new Sphere(glm::vec3(-1.0, 0.0, -i * 3), 0.5, m_materials[(i + 2) % 3 + 2]));
				m_objects.push_back(new Sphere(glm::vec3( 1.0, 0.0, -i * 3), 0.5, m_materials[(i + 2) % 3 + 2]));
			}*/

			m_textures.push_back(new ImageTexture("earth.png"));

			m_textures.push_back(new SolidColorTexture(glm::vec3(0.2)));
			m_textures.push_back(new SolidColorTexture(glm::vec3(0.6)));
			m_textures.push_back(new CheckerTexture(0.5, m_textures[1], m_textures[2]));

			m_materials.push_back(new Metal(m_textures[0], 1.0));
			m_materials.push_back(new Lambertian(m_textures[3]));

			m_objects.push_back(new Sphere(glm::vec3(0), 2.0, m_materials[0]));
			m_objects.push_back(new Sphere(glm::vec3(0.0, -1002, -1.0), 1000.0, m_materials[1]));
		}

		void PopulateScene(const std::string &fileName) {
			
		}

		std::vector<Texture*> m_textures;
		std::vector<Hittable*> m_objects;
		std::vector<Material*> m_materials;
	};
}