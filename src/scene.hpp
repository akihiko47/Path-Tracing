#pragma once

#include "hittable.hpp"


namespace art {

	class Scene : public Hittable {
	public:
		Scene() {}

		void AddObject(std::unique_ptr<Hittable> obj) {
			m_objects.push_back(std::move(obj));
		}

		void AddMaterial(std::unique_ptr<Material> mat) {
			m_materials.push_back(std::move(mat));
		}

		void AddTexture(std::unique_ptr<Texture> tex) {
			m_textures.push_back(std::move(tex));
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
	};

}