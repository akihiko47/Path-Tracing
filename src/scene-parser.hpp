#pragma once

#include "yaml-cpp/yaml.h"

#include <string>

#include "hittable.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "scene.hpp"

namespace art {

	// this class is owner of the scene
	// it must allocate and deallocate textures, materials and objects
	class SceneParser final {
	public:
		SceneParser() { PopulateScene(); }
		SceneParser(const std::string &filename) { PopulateScene(filename); }

		~SceneParser() {
			Cleanup();
		}

		const Camera   *GetCamera() const { return &m_camera; }
		const Hittable *GetScene()  const { return &m_scene; }

	private:

		void Cleanup() {
			for (Texture *t : m_textures) {
				delete t;
			}
			m_textures.clear();

			for (Material *m : m_materials) {
				delete m;
			}
			m_materials.clear();

			for (Hittable *o : m_objects) {
				delete o;
			}
			m_objects.clear();
		}

		// manually populate scene from code
		void PopulateScene() {
			m_textures.push_back(new ImageTexture("earth.png"));

			m_textures.push_back(new SolidColorTexture(glm::vec3(0.2)));
			m_textures.push_back(new SolidColorTexture(glm::vec3(0.6)));
			m_textures.push_back(new CheckerTexture(0.5, m_textures[1], m_textures[2]));

			m_materials.push_back(new Metal(m_textures[0], 1.0));
			m_materials.push_back(new Lambertian(m_textures[3]));

			m_objects.push_back(new Sphere(glm::vec3(0), 2.0, m_materials[0]));
			m_objects.push_back(new Sphere(glm::vec3(0.0, -1002, -1.0), 1000.0, m_materials[1]));

			m_scene = Scene();
			for (Hittable *object : m_objects) {
				m_scene.AddObject(object);
			}

			m_camera = Camera{100, 10, glm::vec3(7, 7, 10), glm::vec3(0, 0, 0), 30, 0, 15};
		}

		// get scene description from file
		void PopulateScene(const std::string &filename) {
			
			YAML::Node file = OpenFile(filename);
			YAML::Node camera = file["camera"];

			m_camera = Camera(
				camera["samples"].as<int>(),
				camera["bounces"].as<int>(),
				camera["position"].as<glm::vec3>(),
				camera["look at"].as<glm::vec3>(),
				camera["fov"].as<int>(),
				camera["defocus angle"].as<int>(),
				camera["focus distance"].as<int>()
			);

		}

		YAML::Node OpenFile(const std::string &filename) {
			std::string filePath = filename;
			std::filesystem::path namePath(filename);  // need this to check if path is relative

			// if path is relative then seek inside scenes folder (setup by cmake)
			// otherwise use absolute path specified by user
			if (namePath.is_relative()) {
				#ifdef SCENE_DIR
					const char* sceneDir = SCENE_DIR;
					filePath = std::string(sceneDir) + "/" + filePath + ".yaml";
				#endif
			}

			std::cout << "parsing scene " << filePath << "\n";
			return YAML::LoadFile(filePath);
		}

		std::vector<Texture*> m_textures;
		std::vector<Hittable*> m_objects;
		std::vector<Material*> m_materials;

		Camera m_camera;
		Scene  m_scene;
	};

}


namespace YAML {
	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3) {
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
}