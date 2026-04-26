#pragma once

#include "EngineHeaders.h"

namespace Survivor3rdPerson
{
    enum class SoundType
    {
        NONE,
        BULLET_HIT,
        JUMPPAD
    };

    class Sounds
    {
    public:
        Sounds() = delete;
        ~Sounds() = delete;

        static void reset();

        static void loadSounds();
        static void update();
        static void playSoundEffect(SoundType type);

    private:
        static bool m_loaded;
        static int m_numberOfCurrentlyPlayingWAV;

        // Sounds.
        static std::string m_bulletHit1;
        static std::string m_bulletHit2;
        static float m_bulletHitTime;
        static float m_bulletHitDelay;
        static std::string m_jumppad;
    };
}
