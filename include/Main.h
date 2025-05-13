#pragma once
#include "Utils.h"
namespace Main {
    /**
     * @brief Main initialization function for the Jumping mod logic.
     * Called from the SKSE plugin entry point after addresses are resolved and trampolines are ready.
     */
     void Initialize();  // The entry point called by SKSE_Load
     
     // Hook callback functions
     bool CheckAndPermitAirJump(void* charController);
     std::uint32_t FinalizeAirJumpState(void* charController);
     float CalculateJumpHeightMultiplier(RE::Actor* jumpingActor);
     void MaintainSprintSpeedDuringJump(RE::Actor* actor, void* movementData);
     
     // Utility functions if they are specific to Main's logic
     std::uintptr_t Utils::ReadRelativeDisplacement(
         std::uintptr_t instructionAddress);  // Or make this global if used elsewhere
     
     /*// Global mod state variables (if you want to namespace them)
     extern int remainingAirJumps; // Example if you move them
     extern bool isPerformingAirJump;
     extern void* lastJumpCharacterController;
     extern bool isPlayerSprinting;
     extern float currentFallTime;
     extern bool isFalling;*/
}  // namespace Main
