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
    std::cout << "Example: ./PathTracing cornell-box\n\n";
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

    art::SceneParser parser{std::string(argv[1])};

    {
        art::Timer timer{"Rendering"};
        parser.GetCamera().Render(parser.GetImage(), parser.GetScene());
    }

    {
        art::Timer timer{"Saving"};
        parser.GetImage().SaveAsPng(parser.GetOutputFileName());
    }

    return 0;
}