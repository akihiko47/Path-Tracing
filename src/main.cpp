#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "timer.hpp"
#include "scene-parser.hpp"
#include "image-filters.hpp"


void ShowTutorial() {
    std::cout << "\n=== Red Eye Path Tracer ===\n\n";
    std::cout << "Usage:   ./RedEye <scene-name>\n";
    std::cout << "Example: ./RedEye example\n\n";
    std::cout << "Usage:   ./RedEye filter <image-name> <kernel-size> <sigma> <b-sigma>\n";
    std::cout << "Example: ./RedEye filter example-scene.png 15 10 0.1\n\n";
    std::cout << "None: default search directories are 'project-root/scenes/' and 'project-root/output/'\n";
    std::cout << "(but you can specify absolute path to yaml scene file or image)\n";
    std::cout << "Resulting image will be saved in 'project-root/output/'\n\n";
    std::cout << "For more info see README\n";
    std::cout << "\n===========================\n\n";
}

void FilterImage(const std::string &fileName, uint32_t msize, float sigma, float bsigma) {
    art::Timer timer{"Filtering"};

    // read image and filter it
    art::Image img = art::Image(fileName);
    std::cout << "image loaded, filtering\n";
    std::unique_ptr<art::Image> filteredImage = art::BilateralFilter(img, msize, sigma, bsigma);

    // save filtered image
    std::cout << "image filtered, saving\n";
    std::string newName = std::filesystem::path(fileName).replace_extension().filename().string() + "-filtered";
    filteredImage->SaveAsPng(newName);

    std::cout << "filtered image saved as: " << newName << ".png" << "\n";
}


int main(int argc, char *argv[]) {

    // filtering mode
    if (argc == 6) {
        if (std::string(argv[1]) != "filter") {
            ShowTutorial();
            return 0;
        }
        
        FilterImage(std::string(argv[2]), std::stoi(argv[3]), std::stof(argv[4]), std::stof(argv[5]));
        return 0;
    }

    if (argc != 2) {
        ShowTutorial();
        return 0;
    }


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