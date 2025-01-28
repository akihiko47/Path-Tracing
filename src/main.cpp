#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "timer.hpp"
#include "scene-parser.hpp"


void ShowTutorial() {
    std::cout << "\n=== Akihiko Path Tracer ===\n\n";
    std::cout << "Usage:   ./PathTracing <scene-name>\n";
    std::cout << "Example: ./PathTracing example\n\n";
    std::cout << "None: default search directory is 'project-root/scenes/'\n";
    std::cout << "(but you can specify absolute path to yaml scene file)\n";
    std::cout << "Resulting image will be saved in 'project-root/output/'\n";
    std::cout << "\n===========================\n\n";
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        ShowTutorial();
        return 0;
    }

    // this object is responsible only for parsing
    // it returns needed variables by value
    // except for image, it is smart pointer so you don't need to delete it
    art::SceneParser parser{std::string(argv[1])};

    art::Scene                  scene;
    art::Camera                 camera      = parser.GetCamera();
    std::unique_ptr<art::Image> renderImage = parser.GetImage();
    std::string                 outputName  = parser.GetOutputFileName();


    {  // scopes are created for scoped timers
        art::Timer timer{"Scene parsing"};
        parser.PopulateScene(scene);
    }
   
    {
        art::Timer timer{"Rendering"};
        camera.Render(*renderImage, scene);
    }

    {
        art::Timer timer{"Saving"};
        renderImage->SaveAsPng(outputName);
    }

    return 0;
}