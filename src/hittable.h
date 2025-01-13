#pragma once

#include <vector>

#include "ray.h"

struct HitInfo {
	glm::vec3 p;
	glm::vec3 N;
	float t;
	bool frontFace;

	void SetFaceNormal(const Ray &r, const glm::vec3 &outNormal) {
		frontFace = glm::dot(r.GetDirection(), outNormal) < 0;
		N = frontFace ? outNormal : -outNormal;
	}
};


class Hittable {
public:
	virtual ~Hittable() = default;

	virtual bool Hit(const Ray& r, float tMin, float tMax, HitInfo& hitInfo) const = 0;
};


class Sphere : public Hittable {
public:
	Sphere(const glm::vec3 &center, float radius) : m_center(center), m_radius(std::fmax(0, radius)) {}

	bool Hit(const Ray& r, float tMin, float tMax, HitInfo& hitInfo) const override {
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
		if (t <= tMin || tMax <= t) {
			float t = (h + sqrtD) / a;
			if (t <= tMin || tMax <= t) {
				return false;
			}
		}

		hitInfo.t = t;
		hitInfo.p = r.At(t);
		glm::vec3 outN = (hitInfo.p - m_center) / m_radius;
		hitInfo.SetFaceNormal(r, outN);

		return true;
	}

private:
	glm::vec3 m_center;
	float     m_radius;
};


class Scene : public Hittable {
public:
	Scene() { 
		PopulateScene(); 
	}
	~Scene() {
		for (Hittable *object : objects) {
			delete object;
		}
	}

	bool Hit(const Ray& r, float tMin, float tMax, HitInfo& hitInfo) const override {
		HitInfo tempInfo;
		bool hit = false;
		float tClosest = tMax;

		for (Hittable *object : objects) {
			if (object->Hit(r, tMin, tClosest, tempInfo)) {
				hit = true;
				tClosest = tempInfo.t;
				hitInfo = tempInfo;
			}
		}

		return hit;
	}

private:
	void PopulateScene() {
		objects.push_back(new Sphere(glm::vec3(0, 0, -1), 0.5));
		objects.push_back(new Sphere(glm::vec3(0, -100.5, -1), 100));
	}

	std::vector<Hittable*> objects;
};