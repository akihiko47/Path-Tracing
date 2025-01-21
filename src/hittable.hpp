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

	class Quad : public Hittable {
	public:
		Quad(const glm::vec3 &Q, const glm::vec3 &u, const glm::vec3 &v, Material *mat) : m_Q(Q), m_u(u), m_v(v), m_mat(mat) {
			glm::vec3 n = glm::cross(u, v);
			m_N = glm::normalize(n);
			m_D = glm::dot(m_N, Q);
			m_w = n / glm::dot(n, n);
		}

		bool Hit(const Ray& r, Interval tSpan, HitInfo& hitInfo) const override {
			float denom = glm::dot(m_N, r.GetDirection());

			// if ray is parallel to plane
			if (std::fabs(denom) < 1e-8) {
				return false;
			}

			// if hit is outside of ray interval
			float t = (m_D - glm::dot(m_N, r.GetOrigin())) / denom;
			if (!tSpan.Contains(t)) {
				return false;
			}

			// point of intersection
			glm::vec3 p = r.At(t);

			// check if point is inside quad using planar coordinates
			glm::vec3 planarHitVec = p - m_Q;
			float alpha = glm::dot(m_w, glm::cross(planarHitVec, m_v));
			float beta = glm::dot(m_w, glm::cross(m_u, planarHitVec));

			if (!isInside(alpha, beta, hitInfo)) {
				return false;
			}

			// set rest of hit info
			hitInfo.t = t;
			hitInfo.p = p;
			hitInfo.mat = m_mat;
			hitInfo.SetFaceNormal(r, m_N);

			return true;
		}

		virtual bool isInside(float a, float b, HitInfo &info) const {
			Interval unitInt = Interval{0, 1};

			if (!unitInt.Contains(a) || !unitInt.Contains(b)) {
				return false;
			}

			info.u = a;
			info.v = b;
			return true;
		}

	private:
		glm::vec3 m_Q;
		glm::vec3 m_u;
		glm::vec3 m_v;
		Material *m_mat;

		glm::vec3 m_N, m_w;
		float m_D;
	};
}
