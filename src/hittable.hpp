#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "ray.hpp"

namespace art {
	const float infinity = std::numeric_limits<float>::infinity();
	const float pi = 3.1415926;

	class Material;  // to solve circular dependency
	struct HitInfo {
		glm::vec3 p;
		glm::vec3 N;
		Material *mat;
		float t;
		float u;
		float v;
		bool frontFace;

		void SetFaceNormal(const Ray &r, const glm::vec3 &outNormal) {
			frontFace = glm::dot(r.GetDirection(), outNormal) < 0;
			N = frontFace ? outNormal : -outNormal;
		}
	};


	class Interval final {
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
				t = (h + sqrtD) / a;
				if (!tSpan.Surrounds(t)) {
					return false;
				}
			}

			hitInfo.t = t;
			hitInfo.p = r.At(hitInfo.t);
			glm::vec3 outN = glm::normalize((hitInfo.p - m_center) / m_radius);
			hitInfo.SetFaceNormal(r, outN);
			GetSphereUV(outN, hitInfo.u, hitInfo.v);
			hitInfo.mat = m_mat;

			return true;
		}

		static void GetSphereUV(const glm::vec3 &p, float &u, float &v) {
			float theta = std::acos(-p.y);
			float phi = std::atan2(-p.z, p.x) + pi;

			u = phi / (2 * pi);
			v = theta / pi;
		}

	private:
		glm::vec3 m_center;
		float     m_radius;
		Material *m_mat;
	};
}
