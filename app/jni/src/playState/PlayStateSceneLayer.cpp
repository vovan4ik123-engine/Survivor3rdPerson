#include "PlayStateSceneLayer.h"
#include "EnumsAndVariables.h"
#include "Sounds.h"
#include "enemy/MovableEnemy.h"

namespace Survivor3rdPerson
{
    PlayStateSceneLayer::PlayStateSceneLayer(std::shared_ptr<PlayStateGUILayer> gui) : m_gui(std::move(gui))
    {
        m_ID = Beryll::LayerID::PLAY_SCENE;

        m_staticEnv.reserve(300);
        m_playerBullets.reserve(100);
        m_movableEnemiesToSort.reserve(1000);
        m_movableEnemiesOriginalOrder.reserve(1000);
        m_movableEnemiesToSpawn.reserve(500);
        m_animatedOrDynamicObjects.reserve(1500);
        m_animatedObjForShadowMap.reserve(1100);
        m_simpleObjForShadowMap.reserve(300);

        loadPlayer();
        loadEnv();
        loadEnemies();
        loadShadersAndLight();

        m_pathFinderEnemies = AStar(m_mapMinX, m_mapMaxX, m_mapMinZ, m_mapMaxZ, 10);
//        std::vector<glm::vec3> walls = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathEnemiesWalls.fbx");
//        for(const auto& wall : walls)
//        {
//            m_pathFinderEnemies.addWallPosition({(int)std::roundf(wall.x), (int)std::roundf(wall.z)});
//        }
//
//        BR_INFO("Map1 pathfinder walls: %d", walls.size());

        std::vector<glm::vec3> allowedPoints = BeryllUtils::Common::loadMeshVerticesToVector("models3D/map1/PathEnemiesAllowedPositions.fbx");
        m_pathAllowedPositionsXZ.reserve(allowedPoints.size());
        for(const auto& point : allowedPoints)
        {
            m_pathAllowedPositionsXZ.push_back({(int)std::roundf(point.x), (int)std::roundf(point.z)});
        }

        BR_INFO("Map1 pathfinder allowed points: %d", m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnEnemies.reserve(m_pathAllowedPositionsXZ.size());
        m_pointsToSpawnEnemiesOnPlayerMoveDir.reserve(m_pathAllowedPositionsXZ.size() / 2);

        m_skyBox = Beryll::Renderer::createSkyBox("skyboxes/nightClouds");

        Sounds::loadSounds();
        Sounds::reset();
        EnumsAndVars::reset();
        Beryll::TimeStep::fixateTime();
        Beryll::TextOnScene::setMaxCountToShow(15);

        //BR_INFO(" X:%f Y:%f Z:%f", .x, .y, .z);
        //BR_INFO("%s", "");
        //BR_INFO(" %f", );
        //BR_INFO(" %d", );
    }

    PlayStateSceneLayer::~PlayStateSceneLayer()
    {

    }

    void PlayStateSceneLayer::updateBeforePhysics()
    {
        EnumsAndVars::mapPlayTimeSec += Beryll::TimeStep::getTimeStepSec();
        Sounds::update();

        m_player->update();
        handleControls();
        checkMapBorders();
        updatePathfindingAndSpawnEnemies();

        for(auto& enemy : m_movableEnemiesOriginalOrder)
        {
            if(enemy->getIsEnabled())
                enemy->update(m_player->getOrigin());
        }
    }

    void PlayStateSceneLayer::updateAfterPhysics()
    {
        for(const std::shared_ptr<Beryll::SceneObject>& so : m_animatedOrDynamicObjects)
        {
            if(so->getIsEnabledUpdate())
            {
                so->updateAfterPhysics();

                if(so->getSceneObjectGroup() == Beryll::SceneObjectGroups::ENEMY)
                {
                    if(Beryll::Camera::getIsSeeObject(so->getOrigin()))
                        so->enableDraw();
                    else
                        so->disableDraw();
                }

                if(so->getOrigin().y < -50.0f)
                {
                    so->disableUpdate();
                    so->disableCollisionMesh();
                    so->disableDraw();
                }
            }
            else
            {
                so->disableDraw();
            }
        }

        handlePlayerAttacks();
        handleEnemiesAttacks();
        handleCamera();
    }

    void PlayStateSceneLayer::draw()
    {
        //BR_INFO("%s", "scene draw call");

        // 1. Draw into shadow map.
        glm::vec3 sunPos = m_player->getOrigin() +
                           (Beryll::Camera::getCameraFrontDirectionXZ() * 200.0f) +
                           (m_dirToSun * 200.0f);
        glm::mat4 lightProjection = glm::ortho(-250.0f, 250.0f, -250.0f, 250.0f, 5.0f, 400.0f);
        glm::mat4 lightView = glm::lookAt(sunPos, sunPos + m_sunLightDir, BeryllConstants::worldUp);

        m_sunLightVPMatrix = lightProjection * lightView;

        Beryll::Renderer::disableFaceCulling();
        m_shadowMap->drawIntoShadowMap(m_simpleObjForShadowMap, m_animatedObjForShadowMap, m_sunLightVPMatrix);
        Beryll::Renderer::enableFaceCulling();

        // 2. Draw scene.
        glm::mat4 modelMatrix{1.0f};

        m_animatedObjSunLight->bind();
        m_animatedObjSunLight->set3Float("sunLightDir", m_sunLightDir);
        m_animatedObjSunLight->set1Float("ambientLight", 0.7f);

        for(auto& animObj : m_movableEnemiesOriginalOrder)
        {
            if(animObj->getObj()->getIsEnabledDraw())
            {
                modelMatrix = animObj->getObj()->getModelMatrix();
                m_animatedObjSunLight->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(animObj->getObj(), modelMatrix, m_animatedObjSunLight);
            }
        }

        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadows->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadows->set1Float("ambientLight", 0.6f);
        m_simpleObjSunLightShadows->set1Float("sunLightStrength", 0.25f);
        m_simpleObjSunLightShadows->set1Float("specularLightStrength", 0.8f);
        m_simpleObjSunLightShadows->set1Float("alphaTransparency", 1.0f);

        modelMatrix = m_player->getModelMatrix();
        m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
        m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
        m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
        Beryll::Renderer::drawObject(m_player, modelMatrix, m_simpleObjSunLightShadows);

        for(auto& bullet : m_playerBullets)
        {
            if(bullet.getObj()->getIsEnabledDraw())
            {
                modelMatrix = bullet.getObj()->getModelMatrix();
                m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
                m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
                m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
                Beryll::Renderer::drawObject(bullet.getObj(), modelMatrix, m_simpleObjSunLightShadows);
            }
        }

        m_simpleObjSunLightShadows->set1Float("ambientLight", 0.45f);

        for(const auto& staticObj : m_staticEnv)
        {
            modelMatrix = staticObj->getModelMatrix();
            m_simpleObjSunLightShadows->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
            m_simpleObjSunLightShadows->setMatrix4x4Float("modelMatrix", modelMatrix);
            m_simpleObjSunLightShadows->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
            Beryll::Renderer::drawObject(staticObj, modelMatrix, m_simpleObjSunLightShadows);
        }

        m_simpleObjSunLightShadowsNormals->bind();
        m_simpleObjSunLightShadowsNormals->set3Float("sunLightDir", m_sunLightDir);
        m_simpleObjSunLightShadowsNormals->set3Float("cameraPos", Beryll::Camera::getCameraPos());
        m_simpleObjSunLightShadowsNormals->set1Float("ambientLight", 0.45f);
        m_simpleObjSunLightShadowsNormals->set1Float("specularLightStrength", 0.3f);

        for(const auto& normalMapObj : m_objWithNormalMap)
        {
            modelMatrix = normalMapObj->getModelMatrix();
            m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("MVPLightMatrix", m_sunLightVPMatrix * modelMatrix);
            m_simpleObjSunLightShadowsNormals->setMatrix4x4Float("modelMatrix", modelMatrix);
            m_simpleObjSunLightShadowsNormals->setMatrix3x3Float("normalMatrix", glm::mat3(modelMatrix));
            Beryll::Renderer::drawObject(normalMapObj, modelMatrix, m_simpleObjSunLightShadowsNormals);
        }

        m_bulletTrajectory.calculateAndDraw(EnumsAndVars::bulletMass,
                                            EnumsAndVars::bulletGravity,
                                            m_bulletStartPosition,
                                            m_bulletAngleRadians,
                                            m_bulletImpulseVector,
                                            glm::vec3(1.0f),
                                            m_sunLightDir);

        m_skyBox->draw();
        Beryll::TextOnScene::draw();
        //Beryll::ParticleSystem::draw();
    }

    void PlayStateSceneLayer::loadPlayer()
    {
        m_player = std::make_shared<Player>("models3D/player/Player.fbx",
                                            EnumsAndVars::playerMass,
                                            true,
                                            Beryll::CollisionFlags::DYNAMIC,
                                            Beryll::CollisionGroups::PLAYER,
                                            Beryll::CollisionGroups::STATIC_ENVIRONMENT | Beryll::CollisionGroups::JUMPPAD,
                                            Beryll::SceneObjectGroups::PLAYER,
                                            EnumsAndVars::playerStartHP);

        m_player->setOrigin(glm::vec3(-650.0f, m_player->getFromOriginToBottom(), -530.0f));
        m_player->getController().moveSpeed = 50.0f;
        m_player->setGravity(EnumsAndVars::playerGravity);
        m_player->setAngularFactor(glm::vec3(0.0f));
        m_player->setLinearFactor(glm::vec3(1.0f, 1.0f, 1.0f));

        m_animatedOrDynamicObjects.push_back(m_player);
        m_simpleObjForShadowMap.push_back(m_player);

        for(int i = 0; i < 20; ++i)
        {
            PlayerBullet bullet("models3D/player/PlayerBullet.fbx",
                                EnumsAndVars::bulletMass,
                                true,
                                Beryll::CollisionFlags::DYNAMIC,
                                Beryll::CollisionGroups::PLAYER_BULLET,
                                Beryll::CollisionGroups::STATIC_ENVIRONMENT | Beryll::CollisionGroups::MOVABLE_ENEMY,
                                Beryll::SceneObjectGroups::BULLET);

            m_animatedOrDynamicObjects.push_back(bullet.getObj());
            m_playerBullets.push_back(bullet);
        }
    }

    void PlayStateSceneLayer::loadEnv()
    {
        const auto groundsNormalMap = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/GroundNormalMap.fbx",
                                                                            0.0f,
                                                                            false,
                                                                            Beryll::CollisionFlags::STATIC,
                                                                            Beryll::CollisionGroups::STATIC_ENVIRONMENT,
                                                                            Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::PLAYER_BULLET |
                                                                            Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                            Beryll::SceneObjectGroups::STATIC_ENVIRONMENT);

        for(const auto& obj : groundsNormalMap)
        {
            m_objWithNormalMap.push_back(obj);
        }

        const auto staticEnv = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/StaticEnv.fbx",
                                                                                        0.0f,
                                                                                        false,
                                                                                        Beryll::CollisionFlags::STATIC,
                                                                                        Beryll::CollisionGroups::STATIC_ENVIRONMENT,
                                                                                        Beryll::CollisionGroups::PLAYER | Beryll::CollisionGroups::PLAYER_BULLET |
                                                                                        Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                                        Beryll::SceneObjectGroups::STATIC_ENVIRONMENT);

        for(const auto& obj : staticEnv)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }

        const auto jumpPads = Beryll::SimpleCollidingObject::loadManyModelsFromOneFile("models3D/map1/JumpPads.fbx",
                                                                                       0.0f,
                                                                                       false,
                                                                                       Beryll::CollisionFlags::STATIC,
                                                                                       Beryll::CollisionGroups::JUMPPAD,
                                                                                       Beryll::CollisionGroups::PLAYER,
                                                                                       Beryll::SceneObjectGroups::JUMPPAD);

        for(const auto& obj : jumpPads)
        {
            m_staticEnv.push_back(obj);
        }

        const auto envNoColliders1 = Beryll::SimpleObject::loadManyModelsFromOneFile("models3D/map1/EnvNoColliders.fbx", Beryll::SceneObjectGroups::STATIC_ENVIRONMENT);

        for(const auto& obj : envNoColliders1)
        {
            m_staticEnv.push_back(obj);
            m_simpleObjForShadowMap.push_back(obj);
        }
    }

    void PlayStateSceneLayer::loadEnemies()
    {
        m_enemiesFirstID = BeryllUtils::Common::getLastGeneratedID() + 1;

        for(int i = 0; i < 300; ++i)
        {
            auto skeleton = std::make_shared<MovableEnemy>("models3D/enemies/SkeletonSword.fbx",
                                                                                    0.0f,
                                                                                    false,
                                                                                    Beryll::CollisionFlags::STATIC,
                                                                                    Beryll::CollisionGroups::MOVABLE_ENEMY,
                                                                                    Beryll::CollisionGroups::PLAYER_BULLET,
                                                                                    Beryll::SceneObjectGroups::ENEMY,
                                                                                    1.0f);

            skeleton->getObj()->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, true, true);
            skeleton->getObj()->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            skeleton->unitType = UnitType::ENEMY_1;
            skeleton->attackSound = SoundType::NONE;
            skeleton->attackHitSound = SoundType::NONE;
            skeleton->dieSound = SoundType::NONE;

            skeleton->damage = 1.0f;
            skeleton->attackDistance = 30.0f;
            skeleton->timeBetweenAttacks = 1.5f + Beryll::RandomGenerator::getFloat() * 0.5f;

            skeleton->experienceWhenDie = 25;
            skeleton->getObj()->getController().moveSpeed = 40.0f;

            m_animatedOrDynamicObjects.push_back(skeleton->getObj());
            m_movableEnemiesToSort.push_back(skeleton);
            m_movableEnemiesOriginalOrder.push_back(skeleton);
            m_animatedObjForShadowMap.push_back(skeleton->getObj());
        }

        for(int i = 0; i < 250; ++i)
        {
            auto ghoul = std::make_shared<MovableEnemy>("models3D/enemies/GhoulBones.fbx",
                                                                                 0.0f,
                                                                                 false,
                                                                                 Beryll::CollisionFlags::STATIC,
                                                                                 Beryll::CollisionGroups::MOVABLE_ENEMY,
                                                                                 Beryll::CollisionGroups::PLAYER_BULLET,
                                                                                 Beryll::SceneObjectGroups::ENEMY,
                                                                                 1.0f);

            ghoul->getObj()->setCurrentAnimationByIndex(EnumsAndVars::AnimationIndexes::run, false, true, true);
            ghoul->getObj()->setDefaultAnimationByIndex(EnumsAndVars::AnimationIndexes::stand);
            ghoul->unitType = UnitType::ENEMY_2;
            ghoul->attackSound = SoundType::NONE;
            ghoul->attackHitSound = SoundType::NONE;
            ghoul->dieSound = SoundType::NONE;

            ghoul->damage = 1.0f;
            ghoul->attackDistance = 25.0f;
            ghoul->timeBetweenAttacks = 2.5f + Beryll::RandomGenerator::getFloat() * 0.5f;

            ghoul->experienceWhenDie = 25;
            ghoul->getObj()->getController().moveSpeed = 30.0f;

            m_animatedOrDynamicObjects.push_back(ghoul->getObj());
            m_movableEnemiesToSort.push_back(ghoul);
            m_movableEnemiesOriginalOrder.push_back(ghoul);
            m_animatedObjForShadowMap.push_back(ghoul->getObj());
        }
    }

    void PlayStateSceneLayer::loadShadersAndLight()
    {
        m_simpleObjSunLightShadows = Beryll::Renderer::createShader("shaders/GLES/SimpleObjectSunLightShadows.vert",
                                                                    "shaders/GLES/SimpleObjectSunLightShadows.frag");
        m_simpleObjSunLightShadows->bind();
        m_simpleObjSunLightShadows->activateDiffuseTextureMat1();
        m_simpleObjSunLightShadows->activateShadowMapTexture();

        m_simpleObjSunLightShadowsNormals = Beryll::Renderer::createShader("shaders/GLES/SimpleObjectSunLightShadowsNormals.vert",
                                                                           "shaders/GLES/SimpleObjectSunLightShadowsNormals.frag");
        m_simpleObjSunLightShadowsNormals->bind();
        m_simpleObjSunLightShadowsNormals->activateDiffuseTextureMat1();
        m_simpleObjSunLightShadowsNormals->activateNormalMapTextureMat1();
        m_simpleObjSunLightShadowsNormals->activateShadowMapTexture();

        m_animatedObjSunLight = Beryll::Renderer::createShader("shaders/GLES/AnimatedObjectSunLight.vert",
                                                               "shaders/GLES/AnimatedObjectSunLight.frag");
        m_animatedObjSunLight->bind();
        m_animatedObjSunLight->activateDiffuseTextureMat1();

        m_shadowMap = Beryll::Renderer::createShadowMap(3600, 3600);

        m_dirToSun = glm::normalize(glm::vec3(1.0f, 2.0f, -0.5f));
        m_sunLightDir = -m_dirToSun;
    }

    void PlayStateSceneLayer::handleControls()
    {
        std::vector<Beryll::Finger>& fingers = Beryll::EventHandler::getFingers();
        for(Beryll::Finger& f : fingers)
        {
            if(f.downEvent &&
               f.pixelsPos.x > 100.0f && f.normalizedPos.x < 0.4f &&
               f.pixelsPos.y > 100.0f && f.normalizedPos.y < 0.8f)
            {
                m_gui->playerJoystick->enable();
                m_gui->playerJoystick->setOrigin(f.normalizedPos);
                m_gui->playerJoystick->pressedFingerID = f.ID;
                f.downEvent = false;

                return; // From method.
            }
        }

        if(m_gui->playerJoystick->getIsTouched())
        {
            const glm::vec2 joyDir2D = m_gui->playerJoystick->getDirection();

            if(glm::any(glm::isnan(joyDir2D)) || glm::length(joyDir2D) < 0.001f)
                return;

            // Default face direction is {1.0f, 0.0f, 0.0f}.
            // fromDefaultDirToCamera = rotation between camera and default face direction.
            const glm::quat fromDefaultDirToCamera = glm::rotation(glm::vec3{1.0f, 0.0f, 0.0f}, Beryll::Camera::getCameraFrontDirectionXZ());
            // Move on XZ plane.
            glm::vec3 moveDir = fromDefaultDirToCamera * glm::vec4(joyDir2D.y, 0.0f, joyDir2D.x, 1.0f);

            m_player->getController().moveToDirection(moveDir, false, false);
        }
        else
        {
            m_gui->playerJoystick->disable();
            return;
        }
    }

    void PlayStateSceneLayer::handleCamera()
    {
        const std::vector<Beryll::Finger>& fingers = Beryll::EventHandler::getFingers();
        for(const Beryll::Finger& f : fingers)
        {
            if(f.normalizedPos.x < 0.5f)
                continue;

            shootBullet();

            if(f.downEvent)
            {
                m_lastFingerMovePosX = f.normalizedPos.x;
                m_lastFingerMovePosY = f.normalizedPos.y;
                break;
            }
            else
            {
                float deltaX = (f.normalizedPos.x - m_lastFingerMovePosX) * EnumsAndVars::SettingsMenu::cameraHorizontalSpeed;
                const float deltaXInOneSecAbs = std::fabs(deltaX * (1.0f / Beryll::TimeStep::getTimeStepSec()));

                if(deltaXInOneSecAbs > EnumsAndVars::SettingsMenu::cameraSpeedThresholdToAccelerate)
                {
                    const float accelFactor = std::powf((deltaXInOneSecAbs - EnumsAndVars::SettingsMenu::cameraSpeedThresholdToAccelerate), 1.1f) * 0.001f;
                    deltaX = deltaX + deltaX * accelFactor;
                }

                float deltaY = (f.normalizedPos.y - m_lastFingerMovePosY) * EnumsAndVars::SettingsMenu::cameraVerticalSpeed;

                m_lastFingerMovePosX = f.normalizedPos.x;
                m_lastFingerMovePosY = f.normalizedPos.y;

                m_eyesLookAngleXZ += deltaX;
                m_eyesLookAngleY += deltaY;
                if(m_eyesLookAngleY > 35.0f) m_eyesLookAngleY = 35.0f; // Eye up.
                if(m_eyesLookAngleY < -75.0f) m_eyesLookAngleY = -75.0f; // Eye down.
                //BR_INFO("m_eyesLookAngleXZ %f m_eyesLookAngleY %f", m_eyesLookAngleXZ, m_eyesLookAngleY);
                break;
            }
        }

        // Euler angles.
        float m_eyesLookAngleXZRadians = glm::radians(m_eyesLookAngleXZ);
        float m_eyesLookAngleYRadians = glm::radians(m_eyesLookAngleY);
        m_cameraOffset.x = glm::cos(m_eyesLookAngleXZRadians) * glm::cos(m_eyesLookAngleYRadians);
        m_cameraOffset.y = glm::sin(m_eyesLookAngleYRadians);
        m_cameraOffset.z = glm::sin(m_eyesLookAngleXZRadians) * glm::cos(m_eyesLookAngleYRadians);
        m_cameraOffset = glm::normalize(m_cameraOffset);

        m_cameraFront = m_player->getOrigin();
        m_cameraFront.y += 12.0f;
        Beryll::Camera::setCameraPos(m_cameraFront - m_cameraOffset * m_cameraDistance);
        Beryll::Camera::setCameraFrontPos(m_cameraFront);
        Beryll::Camera::updateCameraVectors();

        m_player->rotateToDirection(Beryll::Camera::getCameraFrontDirectionXZ(), true);

        // Move camera a bit to left side(player's character will not directly on the middle of the screen).
        // That allow player see all trajectory including part going down.
        Beryll::Camera::setCameraPos(Beryll::Camera::getCameraPos() + Beryll::Camera::getCameraLeftXYZ() * 8.0f);
        Beryll::Camera::setCameraFrontPos(Beryll::Camera::getCameraFrontPos() + Beryll::Camera::getCameraLeftXYZ() * 7.0f);

        // Update shoot dir after camera.
        float angleBetweenWorldUpAndCameraBack = BeryllUtils::Common::getAngleInRadians(BeryllConstants::worldUp, Beryll::Camera::getCameraBackDirectionXYZ());
        m_bulletAngleRadians = angleBetweenWorldUpAndCameraBack - glm::half_pi<float>() + 0.29f; // + 0...f direct trajectory more up.

        m_bulletImpulseVector = m_player->getFaceDirXZ();
        m_bulletImpulseVector.y = glm::tan(m_bulletAngleRadians);
        m_bulletImpulseVector = glm::normalize(m_bulletImpulseVector);
        m_bulletImpulseVector *= EnumsAndVars::bulletMass;
        m_bulletImpulseVector *= 400.0f;

        m_bulletStartPosition = m_player->getOrigin() + m_player->getFaceDirXZ() * 4.0f;
        m_bulletStartPosition.y += 4.0f;
    }

    void PlayStateSceneLayer::shootBullet()
    {
        if(EnumsAndVars::shotTimeSec + EnumsAndVars::shotDelaySec < EnumsAndVars::mapPlayTimeSec)
        {
            if(m_currentBulletIndex >= m_playerBullets.size())
                m_currentBulletIndex = 0;

            m_playerBullets[m_currentBulletIndex].shoot(m_bulletStartPosition, m_bulletImpulseVector);

            ++m_currentBulletIndex;
            EnumsAndVars::shotTimeSec = EnumsAndVars::mapPlayTimeSec;
        }
    }

    void PlayStateSceneLayer::checkMapBorders()
    {
        glm::vec3 origin = m_player->getOrigin();
        bool resetOrigin = false;

        if(origin.x < m_mapMinX)
        {
            resetOrigin = true;
            origin.x = m_mapMinX;
        }
        else if(origin.x > m_mapMaxX)
        {
            resetOrigin = true;
            origin.x = m_mapMaxX;
        }

        if(origin.z < m_mapMinZ)
        {
            resetOrigin = true;
            origin.z = m_mapMinZ;
        }
        else if(origin.z > m_mapMaxZ)
        {
            resetOrigin = true;
            origin.z = m_mapMaxZ;
        }

        if(resetOrigin)
        {
            m_player->setOrigin(origin, false);
        }
    }

    void PlayStateSceneLayer::updatePathfindingAndSpawnEnemies()
    {
        m_pointsToSpawnEnemies.clear();
        m_pointsToSpawnEnemiesOnPlayerMoveDir.clear();

        // Find closest point to player.
        glm::vec2 playerPosXZ{m_player->getOrigin().x, m_player->getOrigin().z};
        float distanceToClosestPoint = std::numeric_limits<float>::max();
        float distanceToCurrent = 0.0f;

        for(const glm::ivec2& point : m_pathAllowedPositionsXZ)
        {
            distanceToCurrent = glm::distance(playerPosXZ, glm::vec2(float(point.x), float(point.y)));

            if(distanceToCurrent < distanceToClosestPoint)
            {
                distanceToClosestPoint = distanceToCurrent;
                m_playerClosestAllowedPos = point;
            }
        }

        // Then divide spawn enemies + path finding to many frames.
        // In first frame:
        // 1. Clear blocked positions.
        // 2. Find allowed points to spawn enemies.
        // 3. Spawn enemies.
        if(m_pathFindingIteration == 0)
        {
            ++m_pathFindingIteration;
            // 1.
            m_pathFinderEnemies.clearBlockedPositions();

            // 2.
            glm::vec2 playerMoveDirXZ{m_player->getController().getMoveDir().x, m_player->getController().getMoveDir().z};
            playerMoveDirXZ = glm::normalize(playerMoveDirXZ);
            for(const glm::ivec2& point : m_pathAllowedPositionsXZ)
            {
                distanceToCurrent = glm::distance(playerPosXZ, glm::vec2(float(point.x), float(point.y)));

                if(distanceToCurrent > EnumsAndVars::enemiesMinDistanceToSpawn && distanceToCurrent < EnumsAndVars::enemiesMaxDistanceToSpawn)
                {
                    // We can spawn enemy at this point.
                    m_pointsToSpawnEnemies.push_back(point);

                    if(m_player->getController().getIsMoving())
                    {
                        glm::vec2 pointDirXZ = glm::vec2(float(point.x), float(point.y)) - playerPosXZ;
                        if(BeryllUtils::Common::getAngleInRadians(playerMoveDirXZ, glm::normalize(pointDirXZ)) < 2.0f)
                            m_pointsToSpawnEnemiesOnPlayerMoveDir.push_back(point);
                    }
                }
            }

            BR_ASSERT((!m_pointsToSpawnEnemies.empty()), "%s", "m_allowedPointsToSpawnEnemies empty.");

            // 3.
            spawnEnemies();
        }
        // In second frame:
        // 1. Update paths for enemies. For EnumsAndVars::enemiesMaxPathfindingInOneFrame enemies in one frame.
        else if(m_pathFindingIteration == 1)
        {
            int enemiesUpdated = 0;
            int& i = EnumsAndVars::enemiesCurrentPathfindingIndex;
            for( ; i < m_movableEnemiesOriginalOrder.size(); ++i)
            {
                if(enemiesUpdated >= EnumsAndVars::enemiesMaxPathfindingInOneFrame)
                    break;

                if(m_movableEnemiesOriginalOrder[i]->getIsEnabled() && m_movableEnemiesOriginalOrder[i]->unitState != UnitState::DYING)
                {
                    m_movableEnemiesOriginalOrder[i]->setPathArray(m_pathFinderEnemies.findPath(m_movableEnemiesOriginalOrder[i]->getCurrentPointToMove2DInt(), m_playerClosestAllowedPos, 7), 0);
                    m_pathFinderEnemies.addBlockedPosition(m_movableEnemiesOriginalOrder[i]->getCurrentPointToMove2DInt());
                    ++enemiesUpdated;
                }
            }

            if(EnumsAndVars::enemiesCurrentPathfindingIndex >= m_movableEnemiesOriginalOrder.size())
            {
                // All enemies were updated.
                //BR_INFO("Path for all enemies updated. Last index: %d", EnumsAndVariables::enemiesCurrentPathfindingIndex);
                EnumsAndVars::enemiesCurrentPathfindingIndex = 0;
                m_pathFindingIteration = 0; // Start from start again in next frame.
            }
        }
    }

    void PlayStateSceneLayer::spawnEnemies()
    {
        // Prepare waves.
        if(m_prepareWave1 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave1Time)
        {
            m_prepareWave1 = false;

            int skeletonCount = 0;
            int ghoulCount = 0;
            for(auto& enemy : m_movableEnemiesOriginalOrder)
            {
                if(skeletonCount < 100 && enemy->unitType == UnitType::ENEMY_1)
                {
                    enemy->isCanBeSpawned = true;
                    ++skeletonCount;
                }

                if(ghoulCount < 75 && enemy->unitType == UnitType::ENEMY_2)
                {
                    enemy->isCanBeSpawned = true;
                    ++ghoulCount;
                }
            }

            BR_INFO("Prepare wave 1. Max enemies: %d", skeletonCount + ghoulCount);
        }
        if(m_prepareWave2 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave2Time)
        {
            m_prepareWave2 = false;

            int skeletonCount = 0;
            int ghoulCount = 0;
            for(auto& enemy : m_movableEnemiesOriginalOrder)
            {
                if(skeletonCount < 200 && enemy->unitType == UnitType::ENEMY_1)
                {
                    enemy->isCanBeSpawned = true;
                    ++skeletonCount;
                }

                if(ghoulCount < 150 && enemy->unitType == UnitType::ENEMY_2)
                {
                    enemy->isCanBeSpawned = true;
                    ++ghoulCount;
                }
            }

            BR_INFO("Prepare wave 2. Max enemies: %d", skeletonCount + ghoulCount);
        }
        if(m_prepareWave3 && EnumsAndVars::mapPlayTimeSec > m_enemiesWave3Time)
        {
            m_prepareWave3 = false;

            int skeletonCount = 0;
            int ghoulCount = 0;
            for(auto& enemy : m_movableEnemiesOriginalOrder)
            {
                if(skeletonCount < 300 && enemy->unitType == UnitType::ENEMY_1)
                {
                    enemy->isCanBeSpawned = true;
                    ++skeletonCount;
                }

                if(ghoulCount < 225 && enemy->unitType == UnitType::ENEMY_2)
                {
                    enemy->isCanBeSpawned = true;
                    ++ghoulCount;
                }
            }

            BR_INFO("Prepare wave 3. Max enemies: %d", skeletonCount + ghoulCount);
        }

        if(m_pointsToSpawnEnemies.empty())
            return;

        std::sort(m_movableEnemiesToSort.begin(), m_movableEnemiesToSort.end(), [&](const std::shared_ptr<MovableEnemy>& e1, const std::shared_ptr<MovableEnemy>& e2)
        {
            return (glm::distance(m_player->getOrigin(), e1->getObj()->getOrigin()) > glm::distance(m_player->getOrigin(), e2->getObj()->getOrigin()));
        });

        // Spawn enemies.
        m_movableEnemiesToSpawn.clear();
        int mostFarCountRespawned = 0;
        const int maxAllowedCountToRespawn = int(BaseEnemy::getActiveCount() / 8); // Respawn max ~12% of all active enemies.
        for(const auto& enemy : m_movableEnemiesToSort)
        {
            // Always spawn disabled enemies that can be spawned. Usually after was killed.
            if(!enemy->getIsEnabled() && enemy->isCanBeSpawned)
                m_movableEnemiesToSpawn.push_back(enemy);

            // Then respawn enabled enemies if need.
            if(enemy->getIsEnabled() && enemy->unitState == UnitState::MOVE && enemy->spawnTime + 5.0f < EnumsAndVars::mapPlayTimeSec)
            {
                // Always respawn some most far enabled enemies from player.
                if(mostFarCountRespawned < maxAllowedCountToRespawn)
                {
                    ++mostFarCountRespawned;
                    m_movableEnemiesToSpawn.push_back(enemy);
                }
                else // Continue check distance threshold to respawn.
                {
                    if(glm::distance(m_player->getOrigin(), enemy->getObj()->getOrigin()) > EnumsAndVars::enemiesDistanceRespawnAfter)
                        m_movableEnemiesToSpawn.push_back(enemy);
                }
            }
        }

        //int spawnedCount = 0;
        for(auto& enemyToSpawn : m_movableEnemiesToSpawn)
        {
            if(!enemyToSpawn->getIsEnabled())
                enemyToSpawn->enableEnemy();

            //++spawnedCount;
            glm::ivec2 spawnPoint2D{0};
            if(m_player->getController().getIsMoving() && m_pointsToSpawnEnemiesOnPlayerMoveDir.size() >= 10 && Beryll::RandomGenerator::getFloat() > 0.5f)
            {
                // Spawn close to player move dir.
                spawnPoint2D = m_pointsToSpawnEnemiesOnPlayerMoveDir[Beryll::RandomGenerator::getInt(m_pointsToSpawnEnemiesOnPlayerMoveDir.size() - 1)];
            }
            else
            {
                // Spawn at random point around player.
                spawnPoint2D = m_pointsToSpawnEnemies[Beryll::RandomGenerator::getInt(m_pointsToSpawnEnemies.size() - 1)];
            }

            enemyToSpawn->setPathArray(m_pathFinderEnemies.findPath(spawnPoint2D, m_playerClosestAllowedPos, 7), 1);
            m_pathFinderEnemies.addBlockedPosition(enemyToSpawn->getCurrentPointToMove2DInt());

            glm::vec3 spawnPoint3D{spawnPoint2D.x, 0.0f, spawnPoint2D.y};
            glm::vec3 rayFrom{spawnPoint3D.x, 400.0f, spawnPoint3D.z};
            glm::vec3 rayTo{spawnPoint3D.x, -400.0f, spawnPoint3D.z};
            Beryll::RayClosestHit rayHit = Beryll::Physics::castRayClosestHit(rayFrom, rayTo,
                                                                               Beryll::CollisionGroups::RAY_FOR_ENVIRONMENT,
                                                                               Beryll::CollisionGroups::STATIC_ENVIRONMENT);
            if(rayHit)
                spawnPoint3D.y = rayHit.hitPoint.y + enemyToSpawn->getObj()->getFromOriginToBottom();
            else
                spawnPoint3D.y = enemyToSpawn->getObj()->getFromOriginToBottom();

            enemyToSpawn->getObj()->setOrigin(spawnPoint3D);
            enemyToSpawn->spawnTime = EnumsAndVars::mapPlayTimeSec;
        }

        //BR_INFO("spawned: %d", spawnedCount);
        //BR_INFO("BaseEnemy::getActiveCount(): %d", BaseEnemy::getActiveCount());
    }

    void PlayStateSceneLayer::handlePlayerAttacks()
    {
        for(auto& bullet : m_playerBullets)
        {
            if(bullet.getIsEnabled())
            {
                int collisionID = Beryll::Physics::getAnyCollisionForID(bullet.getObjID());
                if(collisionID >= m_enemiesFirstID && (collisionID - m_enemiesFirstID) < m_movableEnemiesOriginalOrder.size())
                {
                    m_movableEnemiesOriginalOrder[collisionID - m_enemiesFirstID]->takeDamage(1.0f);

                    Sounds::playSoundEffect(SoundType::BULLET_HIT);
                    // Damage on screen.
                    int number = Beryll::RandomGenerator::getInt(1000) + 1;
                    float numberHeight = std::max(2.5f, glm::distance(Beryll::Camera::getCameraPos(), bullet.getObj()->getOrigin()) * 0.03f);
                    if(Beryll::RandomGenerator::getFloat() < 0.1f)
                    {
                        number *= 10;
                        numberHeight *= 3.0f;
                    }
                    Beryll::TextOnScene::addNumbersToShow(number, numberHeight, 0.5f, bullet.getObj()->getOrigin() + glm::vec3(0.0f, 10.0f, 0.0f),
                                                          glm::vec3{Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f,
                                                                    Beryll::RandomGenerator::getFloat() * 3.0f + 2.0f,
                                                                    Beryll::RandomGenerator::getFloat() * 10.0f - 5.0f},
                                                          50.0f);

                    EnumsAndVars::playerTotalDamage += number;
                }
            }
        }
    }

    void PlayStateSceneLayer::handleEnemiesAttacks()
    {
        for(auto& enemy : m_movableEnemiesOriginalOrder)
        {
            if(enemy->getIsEnabled() && enemy->unitState == UnitState::CAN_ATTACK)
            {
                m_player->takeDamage(1.0f);
                enemy->attack(m_player->getOrigin());
            }
        }
    }
}
