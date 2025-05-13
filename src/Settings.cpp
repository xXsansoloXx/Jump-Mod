#include "Settings.h"

#include <filesystem>  // Include for std::filesystem::exists

#include "PCH.h"

namespace Settings {

    void LoadSettings() {
        CSimpleIniW ini;
        ini.SetUnicode();

        if (!std::filesystem::exists(setting_path)) {
            logger::info("No {} file found, creating new with default values", setting_path);
            SaveSettings();  // Save defaults if file doesn't exist
        } else {
            ini.LoadFile(setting_path);

            General::ModToggle = ini.GetBoolValue(L"General", L"ModToggle", General::ModToggle);
            General::PlayerToggle = ini.GetBoolValue(L"General", L"PlayerToggle", General::PlayerToggle);
            General::MountToggle = ini.GetBoolValue(L"General", L"MountToggle", General::MountToggle);

            // Corrected from GetIntValue to GetLongValue for int variable
            General::MaxJumps = static_cast<int>(ini.GetLongValue(L"General", L"MaxJumps", General::MaxJumps));

            General::SprintJumpToggle = ini.GetBoolValue(L"General", L"SprintJumpToggle", General::SprintJumpToggle);
            // GetDoubleValue is correct for float, added explicit cast for clarity
            General::SprintJumpMultiplier = static_cast<float>(
                ini.GetDoubleValue(L"General", L"SprintJumpMultiplier", General::SprintJumpMultiplier));

            Advanced::PerkRequirementToggle =
                ini.GetBoolValue(L"Advanced", L"PerkRequirementToggle", Advanced::PerkRequirementToggle);
            // Corrected from GetIntValue to GetLongValue for int variable
            Advanced::RequiredPerkIDValue =
                static_cast<int>(ini.GetLongValue(L"Advanced", L"RequiredPerkIDValue", Advanced::RequiredPerkIDValue));

            Advanced::MidairWindowToggle =
                ini.GetBoolValue(L"Advanced", L"MidairWindowToggle", Advanced::MidairWindowToggle);
            // GetDoubleValue is correct for float, added explicit cast
            Advanced::MidairWindowTime =
                static_cast<float>(ini.GetDoubleValue(L"Advanced", L"MidairWindowTime", Advanced::MidairWindowTime));

            Advanced::JumpMultiplierToggle =
                ini.GetBoolValue(L"Advanced", L"JumpMultiplierToggle", Advanced::JumpMultiplierToggle);
            // GetDoubleValue is correct for float, added explicit cast
            Advanced::GroundJumpMultiplier = static_cast<float>(ini.GetDoubleValue(
                L"Advanced", L"GroundJumpMultiplier", Advanced::GroundJumpMultiplier));  // Fixed namespace here too
            // GetDoubleValue is correct for float, added explicit cast
            Advanced::AirJumpMultiplier = static_cast<float>(ini.GetDoubleValue(
                L"Advanced", L"AirJumpMultiplier", Advanced::AirJumpMultiplier));  // Fixed namespace here too

            logger::info("Settings Loaded");
        }
    }

    void SaveSettings() {
        CSimpleIniW ini;
        ini.SetUnicode();
        ini.Reset();  // Ensure a clean INI object before setting values

        ini.SetBoolValue(L"General", L"ModToggle", General::ModToggle, L"; Main Mod Toggle");
        ini.SetBoolValue(L"General", L"PlayerToggle", General::PlayerToggle, L"; Main Player Toggle");
        ini.SetBoolValue(L"General", L"MountToggle", General::MountToggle, L"; Main Mount Toggle");

        // Corrected from SetDoubleValue to SetLongValue for int variable
        ini.SetLongValue(L"General", L"MaxJumps", General::MaxJumps,
                         L"; Max Amount of Jumps able to be made while midair");

        ini.SetBoolValue(L"General", L"SprintJumpToggle", General::SprintJumpToggle, L"; Sprint Jump Toggle");
        // SetDoubleValue is correct for float
        ini.SetDoubleValue(L"General", L"SprintJumpMultiplier", General::SprintJumpMultiplier,
                           L"; Multiplier applied to jump height when sprinting");

        ini.SetBoolValue(L"Advanced", L"PerkRequirementToggle", Advanced::PerkRequirementToggle,
                         L"; Perk Requirement Toggle");
        // Corrected from SetDoubleValue to SetLongValue for int variable
        ini.SetLongValue(L"Advanced", L"RequiredPerkIDValue", Advanced::RequiredPerkIDValue,
                         L"; Perk Required for Double Jumping");

        ini.SetBoolValue(L"Advanced", L"MidairWindowToggle", Advanced::MidairWindowToggle,
                         L"; Midair Jump Window Toggle");
        // SetDoubleValue is correct for float
        ini.SetDoubleValue(L"Advanced", L"MidairWindowTime", Advanced::MidairWindowTime,
                           L"; Allowed time window in between midair jumps");

        ini.SetBoolValue(L"Advanced", L"JumpMultiplierToggle", Advanced::JumpMultiplierToggle,
                         L"; Jump Multiplier(s) Toggle");
        // SetDoubleValue is correct for float
        ini.SetDoubleValue(L"Advanced", L"GroundJumpMultiplier", Advanced::GroundJumpMultiplier,
                           L"; Multiplier for jumping off of the ground");
        // SetDoubleValue is correct for float
        ini.SetDoubleValue(L"Advanced", L"AirJumpMultiplier", Advanced::AirJumpMultiplier,
                           L"; Multiplier for jumping mid air");

        ini.SaveFile(setting_path);

        logger::info("Settings Saved");
    }

    void ResetSettings() {
        // These assignments are already correct based on the variable types
        General::ModToggle = true;
        General::PlayerToggle = true;
        General::MountToggle = true;
        General::MaxJumps = 1;  // Correct for int
        General::SprintJumpToggle = true;
        General::SprintJumpMultiplier = 1.0f;  // Correct for float (using 'f' literal)

        Advanced::PerkRequirementToggle = false;
        Advanced::RequiredPerkIDValue = 0;  // Correct for int
        Advanced::MidairWindowToggle = true;
        Advanced::MidairWindowTime = 1.0f;  // Correct for float
        Advanced::JumpMultiplierToggle = true;
        Advanced::GroundJumpMultiplier = 1.0f;  // Correct for float
        Advanced::AirJumpMultiplier = 1.0f;     // Correct for float

        logger::info("Settings Reset");
    }
}