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

		const Camera      &GetCamera()         const { return m_camera; }
		const Hittable    &GetScene()          const { return m_scene; }
		      Image       &GetImage()                { return *m_renderImage; }  // not const because we need to render to it
		const std::string &GetOutputFileName() const { return m_outputFileName; }

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

			delete m_renderImage;
		}

		// get scene description from file
		void PopulateScene(const std::string &filename) {
			
			m_file = OpenFile(filename);

			ParseCamera(m_file["camera"]);
			ParseOutput(m_file["output"]);

			// objects are stored in array
			YAML::Node objects = m_file["objects"];
			for (YAML::const_iterator it = objects.begin(); it != objects.end(); ++it) {
				YAML::Node objectProperties = it->second;
				m_objects.push_back(ParseObject(objectProperties));
			}

			// add objects to scene
			for (Hittable *obj : m_objects) {
				m_scene.AddObject(obj);
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
				camera["fov"].as<float>(),
				camera["defocus angle"].as<float>(),
				camera["focus distance"].as<float>(),
				camera["background"].as<glm::vec3>()
			);
		}

		void ParseOutput(const YAML::Node &image) {
			m_renderImage = new Image(
				image["width"].as<int>(),
				image["height"].as<int>()
			);
			m_outputFileName = m_file["output"]["file name"].as<std::string>();
		}

		Hittable* ParseObject(const YAML::Node &object) {
			Hittable *result;

			const std::string type = object["type"].as<std::string>();
			if (type == "sphere") {
				result = new Sphere(
					object["position"].as<glm::vec3>(),
					object["radius"].as<float>(),
					ParseMaterial(object["material"].as<std::string>())
				);
			} else if (type == "quad") {
				result = new Quad(
					object["q"].as<glm::vec3>(),
					object["u"].as<glm::vec3>(),
					object["v"].as<glm::vec3>(),
					ParseMaterial(object["material"].as<std::string>())
				);
			} else {
				std::cerr << "incorrect object type - " << object["type"].as<std::string>() << "\n";
				exit(1);
			}

			if (result == nullptr) {
				std::cerr << "incorrect object data" << "\n";
				exit(1);
			}

			return result;
		}

		Material* ParseMaterial(std::string materialName) {
			Material *result;

			// check if material with that name already in map
			if (m_materials.find(materialName) != m_materials.end()) {
				std::cout << "material" << materialName << "already exists\n";
				return m_materials[materialName];
			}

			YAML::Node materials = m_file["materials"];
			YAML::Node material = materials[materialName];

			std::string type = material["type"].as<std::string>();

			if (type == "plastic") {
				if (material["albedo"].IsSequence()) {
					result = new Lambertian(
						material["albedo"].as<glm::vec3>()
					);
				} else {
					result = new Lambertian(
						ParseTexture(material["albedo"].as<std::string>())
					);
				}
			} else if (type == "metal") {
				if (material["albedo"].IsSequence()) {
					result = new Metal(
						material["albedo"].as<glm::vec3>(),
						material["smoothness"].as<float>()
					);
				} else {
					result = new Metal(
						ParseTexture(material["albedo"].as<std::string>()),
						material["smoothness"].as<float>()
					);
				}
			} else if (type == "glass") {
				result = new Dielectric(
					material["refraction index"].as<float>(),
					material["albedo"].as<glm::vec3>(),
					material["smoothness"].as<float>()
				);
			} else if (type == "light") {
				if (material["albedo"].IsSequence()) {
					result = new DiffuseLight(
						material["albedo"].as<glm::vec3>()
					);
				} else {
					result = new DiffuseLight(
						ParseTexture(material["albedo"].as<std::string>())
					);
				}
			} else {
				std::cerr << "incorrect material type - " << material["type"].as<std::string>() << "\n";
				exit(1);
			}

			m_materials[materialName] = result;
			return result;
		}

		Texture* ParseTexture(std::string textureName) {
			Texture *result;

			// ckeck if there are any textures in file
			if (!m_file["textures"]) {
				return nullptr;
			}

			// check if material with that name already in map
			if (m_textures.find(textureName) != m_textures.end()) {
				std::cout << "texture" << textureName << "already exists\n";
				return m_textures[textureName];
			}

			YAML::Node textures = m_file["textures"];
			YAML::Node texture = textures[textureName];

			std::string type = texture["type"].as<std::string>();

			if (type == "color") {
				result = new SolidColorTexture(
					texture["color"].as<glm::vec3>()
				);
			} else if (type == "image") {
				result = new ImageTexture(
					texture["file name"].as<std::string>()
				);
			} else if (type == "checker") {
				result = new CheckerTexture(
					texture["scale"].as<float>(),
					ParseTexture(texture["texture 1"].as<std::string>()),
					ParseTexture(texture["texture 2"].as<std::string>())
				);
			} else {
				std::cerr << "incorrect texture type - " << texture["type"].as<std::string>() << "\n";
				exit(1);
			}

			m_textures[textureName] = result;
			return result;
		}

		YAML::Node m_file;

		std::unordered_map<std::string, Texture*> m_textures;
		std::unordered_map<std::string, Material*> m_materials;
		std::vector<Hittable*> m_objects;

		std::string m_outputFileName;
		Image      *m_renderImage;
		Camera      m_camera;
		Scene       m_scene;
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