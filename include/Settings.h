#pragma once

#include "SimpleIni.h"

namespace Settings {

    inline const char* setting_path{"Data/SKSE/Plugins/jumpingmod.ini"};  // Remember to change this

    void LoadSettings();
    void SaveSettings();
    void ResetSettings();

    namespace General {
        inline bool ModToggle = true;
        inline bool PlayerToggle = true;
        inline bool MountToggle = true;

        inline int MaxJumps = 1;

        inline bool SprintJumpToggle = true;
        inline float SprintJumpMultiplier = 1.0f;
    }

    namespace Advanced {
        inline bool PerkRequirementToggle = false;
        inline int RequiredPerkIDValue = 0;

        inline bool MidairWindowToggle = true;
        inline float MidairWindowTime = 1.0f;

        inline bool JumpMultiplierToggle = true;
        inline float GroundJumpMultiplier = 1.0f;
        inline float AirJumpMultiplier = 1.0f;
    }

    namespace Maintenance {

    }
};