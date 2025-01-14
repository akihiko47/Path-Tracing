#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "ray.hpp"
#include "hittable.hpp"
#include "camera.hpp"
#include "material.hpp"


int main() {
    art::Image image(1920, 1080);
    art::Scene scene;
    art::Camera camera(glm::vec3(0), 500, 15);

    art::Lambertian *matGround = new art::Lambertian(glm::vec3(0.4, 0.4, 0.4));
    art::Lambertian *matCenter = new art::Lambertian(glm::vec3(0.8, 0.0, 0.0));
    art::Metal *matLeft  = new art::Metal(glm::vec3(0.8, 0.8, 0.8));
    art::Metal *matRight = new art::Metal(glm::vec3(0.8, 0.6, 0.2));

    scene.AddObject(new art::Sphere(glm::vec3( 0.0,    0.0, -1.0),   0.5, matCenter));
    scene.AddObject(new art::Sphere(glm::vec3( 0.0, -100.5, -1.0), 100.0, matGround));
    scene.AddObject(new art::Sphere(glm::vec3(-1.0,    0.0, -1.0),   0.5, matLeft));
    scene.AddObject(new art::Sphere(glm::vec3( 1.0,    0.0, -1.0),   0.5, matRight));

    camera.Render(image, scene);
    
    std::clog << "Done! Saving image.\n";
    image.SaveAsPng("test");

    return 0;
}