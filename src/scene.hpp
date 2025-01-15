#pragma once

#include "hittable.hpp"
#include "material.hpp"

namespace art {

	class Scene : public Hittable {
	public:
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
		void PopulateScene(const std::string &fileName) {
			m_materials.push_back(new Lambertian(glm::vec3(0.4, 0.4, 0.4)));
			m_materials.push_back(new Lambertian(glm::vec3(0.8, 0.0, 0.0)));
			m_materials.push_back(new Metal(glm::vec3(0.8, 0.8, 0.8), 0.2));
			m_materials.push_back(new Dielectric(1.3, glm::vec3(0.2, 0.2, 0.9), 1.0));

			m_objects.push_back(new Sphere(glm::vec3( 0.0,    0.0, -1.0),   0.5, m_materials[1]));
			m_objects.push_back(new Sphere(glm::vec3( 0.0, -100.5, -1.0), 100.0, m_materials[0]));
			m_objects.push_back(new Sphere(glm::vec3(-1.0,    0.0, -1.0),   0.5, m_materials[3]));
			m_objects.push_back(new Sphere(glm::vec3( 1.0,    0.0, -1.0),   0.5, m_materials[2]));
		}

		std::vector<Hittable*> m_objects;
		std::vector<Material*> m_materials;
	};
}