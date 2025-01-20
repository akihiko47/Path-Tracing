#pragma once

#include "hittable.hpp"


namespace art {

	class Scene : public Hittable {
	public:
		Scene() {}

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
		std::vector<Hittable*> m_objects;
	};

}