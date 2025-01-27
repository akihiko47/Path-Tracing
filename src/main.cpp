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

    // this parser is owner of everyting in scene (textures, materials, objects) as well as render image
    // all the variables below are refs to instances owned by scene parser
    // if this object gets destroyed before the end of rendering then program will have undefined behavior !
    art::Timer timer{"Scene file parsing"};
    art::SceneParser parser{std::string(argv[1])};
    timer.Stop();

    // get refs (these objects are owned by parser)
    const art::Camera &camera      = parser.GetCamera();
          art::Image  &renderImage = parser.GetImage();
    const art::Scene  &scene       = parser.GetScene();
    const std::string &outputName  = parser.GetOutputFileName();

    // scopes are created for scoped timers
    {
        art::Timer timer{"Rendering"};
        camera.Render(renderImage, scene);
    }

    {
        art::Timer timer{"Saving"};
        renderImage.SaveAsPng(outputName);
    }

    // only here it is safe to delete scene parser
    // but this will break other variables
    // so please don't do it

    return 0;
}