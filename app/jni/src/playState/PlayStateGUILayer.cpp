#include "PlayStateGUILayer.h"
#include "EnumsAndVariables.h"
#include "GameStateHelper.h"

namespace Survivor3rdPerson
{
    PlayStateGUILayer::PlayStateGUILayer()
    {
        m_ID = Beryll::LayerID::PLAY_GUI;

        const float screenAR = Beryll::Window::getInstance()->getScreenAspectRatio();

        if(m_statisticsShow)
        {
            m_statistics1 = Beryll::Renderer::createGUIText("", glm::vec3{0.06f, 0.06f, 0.06f}, glm::vec3{1.0f, 96.3f, 0.0f}, 3.0f);
            m_guiObjects.push_back(m_statistics1);
            m_statistics2 = Beryll::Renderer::createGUIText("", glm::vec3{0.06f, 0.06f, 0.06f}, glm::vec3{0.1f, 92.3f, 0.0f}, 3.0f);
            m_guiObjects.push_back(m_statistics2);
            m_statistics3 = Beryll::Renderer::createGUIText("", glm::vec3{0.06f, 0.06f, 0.06f}, glm::vec3{0.1f, 88.3f, 0.0f}, 3.0f);
            m_guiObjects.push_back(m_statistics3);
        }

        playerJoystick = std::make_shared<Beryll::Joystick>("GUI/playState/Joystick.png","",
                                                            glm::vec3{0.0f, 0.0f, 0.1f}, glm::vec2{120.0f / screenAR, 120.0f}, false);
        m_guiObjects.push_back(playerJoystick);
        playerJoystick->disable();

        // Sometime one random texture disappears after touch Oo. That is fix for this. Invisible button will affected by this bug.
        auto fixBug = std::make_shared<Beryll::ButtonWithTexture>("GUI/FullTransparent.png", "",
                                                                  glm::vec3{-50.0f, 50.0f, 0.2f}, glm::vec2{10.0f, 10.0f});
        m_guiObjects.push_back(fixBug);
        checkBoxBallGun = std::make_shared<Beryll::CheckBox>("GUI/CheckBoxUnMarked.png", "GUI/CheckBoxMarked.png",
                                                             glm::vec3{0.0f, 0.0f, 0.2f}, glm::vec2{10.0f / screenAR, 10.0f});
        m_guiObjects.push_back(checkBoxBallGun);
        std::shared_ptr<Beryll::GUIText> textBallGun = Beryll::Renderer::createGUIText("Ball Gun", glm::vec3{0.35f, 0.8f, 0.0f}, glm::vec3{4.5f, 3.5f, 0.2f}, 4.0f);
        m_guiObjects.push_back(textBallGun);

        checkBoxShotGun = std::make_shared<Beryll::CheckBox>("GUI/CheckBoxUnMarked.png", "GUI/CheckBoxMarked.png",
                                                             glm::vec3{0.0f, 11.0f, 0.2f}, glm::vec2{10.0f / screenAR, 10.0f});
        m_guiObjects.push_back(checkBoxShotGun);
        std::shared_ptr<Beryll::GUIText> textShotGun = Beryll::Renderer::createGUIText("Shot Gun", glm::vec3{0.35f, 0.8f, 0.0f}, glm::vec3{4.5f, 14.5f, 0.2f}, 4.0f);
        m_guiObjects.push_back(textShotGun);

        checkBoxLaserGun = std::make_shared<Beryll::CheckBox>("GUI/CheckBoxUnMarked.png", "GUI/CheckBoxMarked.png",
                                                              glm::vec3{0.0f, 22.0f, 0.2f}, glm::vec2{10.0f / screenAR, 10.0f});
        m_guiObjects.push_back(checkBoxLaserGun);
        std::shared_ptr<Beryll::GUIText> textLaserGun = Beryll::Renderer::createGUIText("Laser", glm::vec3{0.35f, 0.8f, 0.0f}, glm::vec3{4.5f, 25.5f, 0.2f}, 4.0f);
        m_guiObjects.push_back(textLaserGun);

        checkBoxSword = std::make_shared<Beryll::CheckBox>("GUI/CheckBoxUnMarked.png", "GUI/CheckBoxMarked.png",
                                                           glm::vec3{0.0f, 33.0f, 0.2f}, glm::vec2{10.0f / screenAR, 10.0f});
        m_guiObjects.push_back(checkBoxSword);
        std::shared_ptr<Beryll::GUIText> textSword = Beryll::Renderer::createGUIText("Sword", glm::vec3{0.35f, 0.8f, 0.0f}, glm::vec3{4.5f, 36.5f, 0.2f}, 4.0f);
        m_guiObjects.push_back(textSword);

        // Sort to update nearest objects first. But draw should starts from farest object(in reverse order).
        std::sort(m_guiObjects.begin(), m_guiObjects.end(), [](std::shared_ptr<Beryll::GUIObject> o1, std::shared_ptr<Beryll::GUIObject> o2)
        {
            return (o1->getPositionNormalized().z > o2->getPositionNormalized().z);
        });
    }

    PlayStateGUILayer::~PlayStateGUILayer()
    {

    }

    void PlayStateGUILayer::updateBeforePhysics()
    {
        for(const std::shared_ptr<Beryll::GUIObject>& go : m_guiObjects)
        {
            if(go->getIsEnabled())
            {
                go->updateBeforePhysics();
            }
        }

        if(m_statisticsShow && Beryll::TimeStep::getMilliSecFromStart() > m_statisticsUpdateTime + 200) // Update every 200 ms.
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1);
            stream << "Frame: " << Beryll::GameLoop::getFrameTime() << "  FPS: " << Beryll::GameLoop::getFPS();
            m_statistics1->text = stream.str();

            stream.str(""); // Way to clear std::stringstream.
            stream << std::fixed << std::setprecision(1);
            stream << "Phys: " << Beryll::Physics::getSimulationTime();
            stream << "  Logic: " << (Beryll::GameLoop::getCPUTime() - Beryll::Physics::getSimulationTime());
            stream << "  GPU: " << Beryll::GameLoop::getGPUTime();
            m_statistics2->text = stream.str();

            stream.str("");
            stream << "Time: " << int(EnumsAndVars::mapPlayTimeSec / 60.0f) << ":" << int(std::fmod(EnumsAndVars::mapPlayTimeSec, 60.0f));
            m_statistics3->text = stream.str();

            m_statisticsUpdateTime = Beryll::TimeStep::getMilliSecFromStart();
        }
    }

    void PlayStateGUILayer::updateAfterPhysics()
    {
        // Dont need update.
    }

    void PlayStateGUILayer::draw()
    {
        for(auto it = m_guiObjects.rbegin(); it != m_guiObjects.rend(); ++it)
        {
            if((*it)->getIsEnabled())
            {
                (*it)->draw();
            }
        }
    }
}
