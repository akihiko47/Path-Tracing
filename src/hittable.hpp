#pragma once

#include <vector>

#include "ray.hpp"

namespace art {
	float infinity = std::numeric_limits<float>::infinity();

	class Material;  // to solve circular dependency
	struct HitInfo {
		glm::vec3 p;
		glm::vec3 N;
		Material *mat;
		float t;
		bool frontFace;

		void SetFaceNormal(const Ray &r, const glm::vec3 &outNormal) {
			frontFace = glm::dot(r.GetDirection(), outNormal) < 0;
			N = frontFace ? outNormal : -outNormal;
		}
	};


	class Interval {
	public:
		Interval() : m_min(+infinity), m_max(-infinity) {}
		Interval(float min, float max) : m_min(min), m_max(max) {}

		float Size() { return m_max - m_min; }
		bool  Contains(float x)  const { return m_min <= x && x <= m_max; }
		bool  Surrounds(float x) const { return m_min < x && x < m_max; }

		float GetMin() const { return m_min; }
		float GetMax() const { return m_max; }

		static const Interval empty;
		static const Interval full;
	private:
		float m_min;
		float m_max;
	};

	const Interval Interval::empty = Interval(+infinity, -infinity);
	const Interval Interval::full = Interval(-infinity, +infinity);


	class Hittable {
	public:
		virtual ~Hittable() = default;

		virtual bool Hit(const Ray& r, Interval tSpan, HitInfo& hitInfo) const = 0;
	};


	class Sphere : public Hittable {
	public:
		Sphere(const glm::vec3 &center, float radius, Material *mat) : m_center(center), m_radius(std::fmax(0, radius)), m_mat(mat) {}

		bool Hit(const Ray& r, Interval tSpan, HitInfo& hitInfo) const override {
			glm::vec3 no = m_center - r.GetOrigin();
			float a = glm::dot(r.GetDirection(), r.GetDirection());
			float h = glm::dot(r.GetDirection(), no);
			float c = glm::dot(no, no) - m_radius * m_radius;
			float D = h * h - a * c;

			if (D < 0) {
				return false;
			}

			float sqrtD = std::sqrt(D);

			float t = (h - sqrtD) / a;
			if (!tSpan.Surrounds(t)) {
				float t = (h + sqrtD) / a;
				if (!tSpan.Surrounds(t)) {
					return false;
				}
			}

			hitInfo.t = t;
			hitInfo.p = r.At(t);
			glm::vec3 outN = (hitInfo.p - m_center) / m_radius;
			hitInfo.SetFaceNormal(r, outN);
			hitInfo.mat = m_mat;

			return true;
		}

	private:
		glm::vec3 m_center;
		float     m_radius;
		Material *m_mat;
	};


	class Scene : public Hittable {
	public:
		Scene() { PopulateScene(); }

		~Scene() {
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
			
		}

		std::vector<Hittable*> m_objects;
	};
}
