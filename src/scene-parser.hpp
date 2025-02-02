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

	class SceneParser final {
	public:
		SceneParser() = delete;
		SceneParser(const std::string &filename) : m_file(OpenFile(filename)) {}

		void PopulateScene(Scene &scene) {

			ParseSkybox(scene);

			// allocate objects and transfer ownership to scene
			ErrorCheck(m_file, "objects");
			YAML::Node objects = m_file["objects"];
			for (YAML::const_iterator it = objects.begin(); it != objects.end(); ++it) {
				YAML::Node objectProperties = it->second;
				ParseObject(objectProperties, scene);
			}
		}

		Camera GetCamera(Scene &scene) { 
			ErrorCheck(m_file, "camera");

			YAML::Node camera = m_file["camera"];
			ErrorCheck(camera, "samples");
			ErrorCheck(camera, "bounces");
			ErrorCheck(camera, "position");
			ErrorCheck(camera, "look at");

			return Camera(
				camera["samples"].as<int>(),
				camera["bounces"].as<int>(),
				camera["position"].as<glm::vec3>(),
				camera["look at"].as<glm::vec3>(),
				camera["fov"] ? camera["fov"].as<float>() : 45,
				camera["defocus angle"] ? camera["defocus angle"].as<float>() : 0,
				camera["focus distance"] ? camera["focus distance"].as<float>() : 1
			);
		}

		std::unique_ptr<Image> GetImage() { 
			ErrorCheck(m_file, "output");

			YAML::Node image = m_file["output"];
			ErrorCheck(image, "width");
			ErrorCheck(image, "height");

			return std::make_unique<Image>(
				image["width"].as<int>(),
				image["height"].as<int>()
			);
		} 

		const std::string GetOutputFileName() const { 
			ErrorCheck(m_file, "output");
			ErrorCheck(m_file["output"], "file name");
			return m_file["output"]["file name"].as<std::string>();
		}

	private:

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

			if (!std::filesystem::exists(filePath)) {
				std::cerr << "parsing error! No such file: " << filePath << "\n";
				exit(1);
			}

			std::cout << "parsing scene " << filePath << "\n";
			return YAML::LoadFile(filePath);
		}

		void ParseObject(const YAML::Node &object, Scene &scene) {

			const std::string type = object["type"].as<std::string>();

			if (type == "sphere") {
				ErrorCheck(object, "position");
				ErrorCheck(object, "radius");
				ErrorCheck(object, "material");

				scene.AddObject(std::make_unique<Sphere>(
					object["position"].as<glm::vec3>(),
					object["radius"].as<float>(),
					ParseMaterial(object["material"].as<std::string>(), scene)
				));
			} else if (type == "quad") {
				ErrorCheck(object, "q");
				ErrorCheck(object, "u");
				ErrorCheck(object, "v");

				scene.AddObject(std::make_unique<Quad>(
					object["q"].as<glm::vec3>(),
					object["u"].as<glm::vec3>(),
					object["v"].as<glm::vec3>(),
					ParseMaterial(object["material"].as<std::string>(), scene),
					object["one side"] ? object["one side"].as<bool>() : false
				));
			} else {
				std::cerr << "incorrect object type - " << object["type"].as<std::string>() << "\n";
				exit(1);
			}
		}

		IMaterial* ParseMaterial(std::string materialName, Scene &scene) {
			std::unique_ptr<IMaterial> result;

			// check if material with that name already created
			if (m_parsedMaterials.find(materialName) != m_parsedMaterials.end()) {
				return m_parsedMaterials[materialName];
			}

			ErrorCheck(m_file, "materials");
			YAML::Node materials = m_file["materials"];
			YAML::Node material = materials[materialName];

			std::string type = material["type"].as<std::string>();

			if (type == "plastic") {
				ErrorCheck(material, "albedo");

				if (material["albedo"].IsSequence()) {
					result = std::make_unique<Lambertian>(
						material["albedo"].as<glm::vec3>(),
						material["smoothness"] ? material["smoothness"].as<float>() : 0.0,
						material["specular probability"] ? material["specular probability"].as<float>() : 0.0,
						material["normal map"] ? ParseTexture(material["normal map"].as<std::string>(), scene) : nullptr,
						material["normal map strength"] ? material["normal map strength"].as<float>() : 1.0
					);
				} else {
					result = std::make_unique<Lambertian>(
						ParseTexture(material["albedo"].as<std::string>(), scene),
						material["smoothness"] ? material["smoothness"].as<float>() : 0.0,
						material["specular probability"] ? material["specular probability"].as<float>() : 0.0,
						material["normal map"] ? ParseTexture(material["normal map"].as<std::string>(), scene) : nullptr,
						material["normal map strength"] ? material["normal map strength"].as<float>() : 1.0
					);
				}
			} else if (type == "metal") {
				ErrorCheck(material, "albedo");
				ErrorCheck(material, "smoothness");

				if (material["albedo"].IsSequence()) {
					result = std::make_unique<Metal>(
						material["albedo"].as<glm::vec3>(),
						material["smoothness"].as<float>()
					);
				} else {
					result = std::make_unique<Metal>(
						ParseTexture(material["albedo"].as<std::string>(), scene),
						material["smoothness"].as<float>()
					);
				}
			} else if (type == "glass") {
				ErrorCheck(material, "refraction index");
				ErrorCheck(material, "albedo");
				ErrorCheck(material, "smoothness");

				result = std::make_unique<Dielectric>(
					material["refraction index"].as<float>(),
					material["albedo"].as<glm::vec3>(),
					material["smoothness"].as<float>()
				);
			} else if (type == "light") {
				ErrorCheck(material, "albedo");

				if (material["albedo"].IsSequence()) {
					result = std::make_unique<DiffuseLight>(
						material["albedo"].as<glm::vec3>()
					);
				} else {
					result = std::make_unique<DiffuseLight>(
						ParseTexture(material["albedo"].as<std::string>(), scene)
					);
				}
			} else {
				std::cerr << "incorrect material type - " << material["type"].as<std::string>() << "\n";
				exit(1);
			}

			IMaterial *ptr = result.get();
			m_parsedMaterials[materialName] = ptr;
			scene.AddMaterial(std::move(result));
			return ptr;
		}

		ITexture* ParseTexture(std::string textureName, Scene &scene, bool forSkybox = false) {
			std::unique_ptr<ITexture> result;

			// check if texture with that name already created
			if (m_parsedTextures.find(textureName) != m_parsedTextures.end()) {
				return m_parsedTextures[textureName];
			}

			ErrorCheck(m_file, "textures");
			YAML::Node textures = m_file["textures"];
			YAML::Node texture = textures[textureName];

			std::string type = texture["type"].as<std::string>();

			if (type == "albedo") {
				ErrorCheck(texture, "albedo");

				result = std::make_unique<SolidColorTexture>(
					texture["albedo"].as<glm::vec3>()
				);
			} else if (type == "image") {
				ErrorCheck(texture, "file name");

				result = std::make_unique<ImageTexture>(
					texture["file name"].as<std::string>(),
					texture["is hdr"] ? texture["is hdr"].as<bool>() : false,
					texture["hdr range"] ? texture["hdr range"].as<float>() : art::infinity
				);
			} else if (type == "checker") {
				ErrorCheck(texture, "texture 1");
				ErrorCheck(texture, "texture 2");

				result = std::make_unique<CheckerTexture>(
					texture["scale"].as<float>(),
					ParseTexture(texture["texture 1"].as<std::string>(), scene),
					ParseTexture(texture["texture 2"].as<std::string>(), scene)
				);
			} else if (type == "cubemap") {
				ErrorCheck(texture, "top");
				ErrorCheck(texture, "left");
				ErrorCheck(texture, "front");
				ErrorCheck(texture, "right");
				ErrorCheck(texture, "back");
				ErrorCheck(texture, "bottom");

				result = std::make_unique<CubemapTexture>(
					std::vector<std::string> {
						texture["right"].as<std::string>(),
						texture["left"].as<std::string>(),
						texture["top"].as<std::string>(),
						texture["bottom"].as<std::string>(),
						texture["front"].as<std::string>(),
						texture["back"].as<std::string>(),
					}
				);
			} else {
				std::cerr << "incorrect texture type - " << texture["type"].as<std::string>() << "\n";
				exit(1);
			}

			ITexture *ptr = result.get();
			m_parsedTextures[textureName] = ptr;
			if (!forSkybox) {
				scene.AddTexture(std::move(result));
			} else {
				scene.AddSkybox(std::move(result));
			}

			return ptr;
		}

		void ParseSkybox(Scene &scene) {
			if (m_file["skybox"]) {
				YAML::Node skybox = m_file["skybox"];

				if (skybox.IsSequence()) {
					scene.AddSkybox(skybox.as<glm::vec3>());
				} else {
					ParseTexture(skybox.as<std::string>(), scene, true);
				}
			}
		}

		void ErrorCheck(YAML::Node node, const std::string &value) const {
			if (!node[value]) {
				std::cerr << "parsing error!\n\n" << node << "\n\ndoes't contain: " << value << "\n";
				exit(1);
			}
		}

		YAML::Node m_file;

		// need these containers to check if parsed object already exists
		// Scene object is owner of these, so we dont delete them in destructor
		std::unordered_map<std::string, IMaterial*> m_parsedMaterials;
		std::unordered_map<std::string, ITexture*> m_parsedTextures;
	};

}

// operation to parce vec3 from YAML file
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