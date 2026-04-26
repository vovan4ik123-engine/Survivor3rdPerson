#include "EngineHeaders.h"
#include "GameStateHelper.h"

// For static model in blender 36.6k triangles = 1mb FBX file size.

// Release instruction:
// 1. Reset global vars in EnumsAndVars.h
// 2. New version in AndroidManifest.xml
// 3. New version in build.gradle
// 4. Uncomment -DCMAKE_BUILD_TYPE=Release in build.gradle
// 5. Comment add_definitions(-DBR_DEBUG) in game CMake
// 6. Comment add_definitions(-DBR_DEBUG) in engine CMake
// 7. Real ad units in AdsManager.java
// 8. Build variants -> release
// 9. Clean, refresh, build

int main(int argc, char* argv[])
{
    BR_INFO("%s", "main() started.");

    Beryll::GameLoop::create(Beryll::ScreenOrientation::MOBILE_LANDSCAPE_AND_FLIPPED);

    Beryll::GameLoop::setFPSLimit(60.0f);

    Beryll::Camera::setCameraFov(45.0f);
    Beryll::Camera::setProjectionNearClipPlane(5.0f);
    Beryll::Camera::setProjectionFarClipPlane(1500.0f);
    Beryll::Camera::setObjectsViewDistance(1500.0f);

    //Beryll::Physics::setResolution(2);
    //Beryll::Physics::setMinAcceptableFPS(20.0f);

    Beryll::Renderer::enableFaceCulling();

    Survivor3rdPerson::GameStateHelper::pushPlayState();

    Beryll::GameLoop::run();

    BR_INFO("%s", "main() finished.");
    return 0;
}
