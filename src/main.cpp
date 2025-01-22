#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "timer.hpp"
#include "scene-parser.hpp"


int main() {
    art::SceneParser parser{"cornell-box"};

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