#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "timer.hpp"
#include "scene-parser.hpp"


int main() {
    art::Image image{512, 288};
    art::SceneParser parser{"test"};

    {
        art::Timer timer{"Rendering"};
        parser.GetCamera()->Render(image, *parser.GetScene());
    }

    {
        art::Timer timer{"Saving"};
        image.SaveAsPng("test");
    }

    return 0;
}