#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "ray.hpp"
#include "hittable.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "scene.hpp"
#include "timer.hpp"


int main() {
    art::Image image(512, 288);
    art::Scene scene{};
    art::Camera camera(100, 10, glm::vec3(0, 0.7, 5), glm::vec3(0, 0, -50), 30, 0, 15);

    {
        art::Timer timer{"Rendering"};
        camera.Render(image, scene);
    }

    {
        art::Timer timer{"Saving"};
        image.SaveAsPng("test");
    }
    

    return 0;
}