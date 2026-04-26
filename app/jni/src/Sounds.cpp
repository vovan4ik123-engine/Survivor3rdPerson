#include "Sounds.h"
#include "EnumsAndVariables.h"

namespace Survivor3rdPerson
{
    bool Sounds::m_loaded = false;
    int Sounds::m_numberOfCurrentlyPlayingWAV = 0;

    // Sounds.
    std::string Sounds::m_bulletHit1 = "sounds/BulletHit1.wav";
    std::string Sounds::m_bulletHit2 = "sounds/BulletHit2.wav";
    float Sounds::m_bulletHitTime = 0.0f;
    float Sounds::m_bulletHitDelay = 0.12f;
    std::string Sounds::m_jumppad = "sounds/Jumppad.wav";

    void Sounds::reset()
    {
        m_bulletHitTime = 0.0f;
    }

    void Sounds::loadSounds()
    {
        if(m_loaded)  { return; }

        Beryll::SoundsManager::loadWAV(m_bulletHit1, 20);
        Beryll::SoundsManager::loadWAV(m_bulletHit2, 20);
        Beryll::SoundsManager::loadWAV(m_jumppad, 80);

        m_loaded = true;
    }

    void Sounds::update()
    {
        m_numberOfCurrentlyPlayingWAV = Beryll::SoundsManager::getNumberOfWAVCurrentlyPlaying();
        //BR_INFO("m_numberOfCurrentlyPlayingWAV: %d", m_numberOfCurrentlyPlayingWAV);
    }

    void Sounds::playSoundEffect(SoundType type)
    {
        if(type == SoundType::NONE || m_numberOfCurrentlyPlayingWAV >= 8)
            return;

        if(type == SoundType::BULLET_HIT)
        {
            if(m_bulletHitTime + m_bulletHitDelay < EnumsAndVars::mapPlayTimeSec)
            {
                m_bulletHitTime = EnumsAndVars::mapPlayTimeSec;

                float randomValue = Beryll::RandomGenerator::getFloat();
                if(randomValue < 0.5f)
                    Beryll::SoundsManager::playWAV(m_bulletHit1);
                else
                    Beryll::SoundsManager::playWAV(m_bulletHit2);
            }
        }
        else if(type == SoundType::JUMPPAD)
        {
            Beryll::SoundsManager::playWAV(m_jumppad);
        }
    }
}
