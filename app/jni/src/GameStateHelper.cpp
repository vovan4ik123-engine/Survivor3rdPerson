#include "GameStateHelper.h"
#include "playState/PlayStateGUILayer.h"
#include "playState/PlayStateSceneLayer.h"

namespace Survivor3rdPerson
{
    void GameStateHelper::pushPlayState()
    {
        std::shared_ptr<PlayStateGUILayer> GUILayer = std::make_shared<PlayStateGUILayer>();
        std::shared_ptr<PlayStateSceneLayer> sceneLayer = std::make_shared<PlayStateSceneLayer>(GUILayer);

        std::shared_ptr<Beryll::GameState> playState = std::make_shared<Beryll::GameState>();
        playState->ID = Beryll::GameStateID::PLAY;
        playState->layerStack.pushLayer(sceneLayer);
        playState->layerStack.pushOverlay(GUILayer);

        Beryll::GameStateMachine::pushState(playState);
    }

    void GameStateHelper::popState()
    {
        Beryll::GameStateMachine::popState();
    }
}
