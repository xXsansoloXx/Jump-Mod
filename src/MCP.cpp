#include "PCH.h"

#include "MCP.h"
#include "Settings.h"
#include "SKSEMCP/SKSEMenuFramework.hpp"

namespace MCP {
    void Register() {
        if (!SKSEMenuFramework::IsInstalled()) {
            return;
        }
        SKSEMenuFramework::SetSection("Better Jumping SE");
        SKSEMenuFramework::AddSectionItem("General Settings", General::Render);
        SKSEMenuFramework::AddSectionItem("Advanced Settings", Advanced::Render);
        SKSEMenuFramework::AddSectionItem("Maintenance Settings", Maintenance::Render);
    }

    void __stdcall General::Render() {
        ImGui::Checkbox("Mod Enabled", &Settings::General::ModToggle);

        ImGui::Checkbox("Player Jump Modifications", &Settings::General::PlayerToggle);
        ImGui::Checkbox("Mount Jump Modifications", &Settings::General::MountToggle);
        ImGui::SliderInt("Max Jumps", &Settings::General::MaxJumps, -1, 100);
        ImGui::Checkbox("Sprint Jump Toggle", &Settings::General::SprintJumpToggle);
        ImGui::SliderFloat("Sprint Jump Multiplier", &Settings::General::SprintJumpMultiplier, 0.1f, 100.0f);
    }

    void __stdcall Advanced::Render() {
        ImGui::Checkbox("Perk Requirement", &Settings::Advanced::PerkRequirementToggle);
        // ImGui::ImFormatString("Required Perk", &Settings::Advanced::RequiredPerk);

        ImGui::Checkbox("Midair Jump Window Toggle", &Settings::Advanced::MidairWindowToggle);
        ImGui::SliderFloat("Midair Jump Window Time", &Settings::Advanced::MidairWindowTime, 0.1f, 100.0f);

        ImGui::Checkbox("Jump Multipliers Toggle", &Settings::Advanced::JumpMultiplierToggle);
        ImGui::SliderFloat("Ground Jump Multiplier", &Settings::Advanced::GroundJumpMultiplier, 0.1f, 100.0f);
        ImGui::SliderFloat("Air Jump Multiplier", &Settings::Advanced::AirJumpMultiplier, 0.1f, 100.0f);
    }

    void __stdcall Maintenance::Render() {
        if (ImGui::Button("Save Settings")) {
            Settings::SaveSettings();
        }
        if (ImGui::Button("Reset Settings")) {
            Settings::ResetSettings();
        }
    }
}