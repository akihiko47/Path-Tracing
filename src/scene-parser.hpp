#pragma once

#include "yaml-cpp/yaml.h"

#include <string>
#include <unordered_map>
#include <map>

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
		SceneParser(const std::string &filename) { PopulateScene(filename); }

		~SceneParser() {
			Cleanup();
		}

		const Camera   *GetCamera() const { return &m_camera; }
		const Hittable *GetScene()  const { return &m_scene; }

	private:

		void Cleanup() {
			for (std::unordered_map<std::string, Texture*>::iterator itr = m_textures.begin(); itr != m_textures.end(); itr++) {
				delete (itr->second);
			}
			m_textures.clear();

			for (std::unordered_map<std::string, Material*>::iterator itr = m_materials.begin(); itr != m_materials.end(); itr++) {
				delete (itr->second);
			}
			m_materials.clear();

			for (Hittable *obj : m_objects) {
				delete obj;
			}
			m_objects.clear();
		}

		// get scene description from file
		void PopulateScene(const std::string &filename) {
			
			YAML::Node file = OpenFile(filename);

			ParseCamera(file["camera"]);

			// place parsed textures in unordered map
			// (we need to get textures by name to create materials)
			YAML::Node textures = file["textures"];
			for (uint32_t i = 0, ie = textures.size(); i != ie; ++i) {
				m_textures[textures[i].Tag()] = ParseTexture(textures[i]);
			}

			// place parsed materials in unordered map
			// (we need to get materials by name to create objects)
			YAML::Node materials = file["materials"];
			for (uint32_t i = 0, ie = materials.size(); i != ie; ++i) {
				m_materials[materials[i].Tag()] = ParseMaterial(materials[i]);
			}

			// objects are stored in array
			for (YAML::Node object : file["objects"]) {
				for (YAML::const_iterator it = object.begin(); it != object.end(); ++it) {
					YAML::Node objectProperties = it->second;
					m_objects.push_back(ParseObject(objectProperties));
				}
				
			}
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

		void ParseCamera(const YAML::Node &camera) {
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

		Hittable* ParseObject(const YAML::Node &object) {
			Hittable *result;

			const std::string type = object["type"].as<std::string>();
			if (type == "sphere") {
				result = new Sphere(
					object["position"].as<glm::vec3>(),
					object["radius"].as<float>(),
					m_materials[object["material"].as<std::string>()]
				);

				if (result == nullptr) {
					std::cerr << "incorrect object data" << "\n";
					exit(1);
				}
			} else {
				std::cerr << "incorrect object type - " << object["type"].as<std::string>() << "\n";
				exit(1);
			}

			return result;
		}

		Material* ParseMaterial(YAML::Node material) {
			return nullptr;
		}

		Texture* ParseTexture(YAML::Node texture) {
			return nullptr;
		}

		std::unordered_map<std::string, Texture*> m_textures;
		std::unordered_map<std::string, Material*> m_materials;
		std::vector<Hittable*> m_objects;

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