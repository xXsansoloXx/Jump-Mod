#pragma once

#include "PCH.h"  // Include PCH here so REL::Relocation and RE types are known

//-----------------------------------------------------------------------------------
// Global Variables - Addresses - DECLARATIONS
//-----------------------------------------------------------------------------------

// Addresses resolved directly by Address Library (ID or ID + Offset)
extern REL::Relocation<std::uintptr_t> jumpStateCheckAddress;
extern REL::Relocation<std::uintptr_t> jumpHandlerBaseAddress;
extern REL::Relocation<std::uintptr_t> jumpHeightCalculationAddress;
extern REL::Relocation<std::uintptr_t> movementSpeedUpdateAddress;

// Essential game functions/data (resolved by Address Library)
extern REL::Relocation<RE::TESForm* (*)(RE::FormID)> lookupFormByIDFunction;
extern REL::Relocation<RE::PlayerCharacter*> playerCharacterPtr;
// extern REL::Relocation<decltype(&RE::Actor::HasPerk)> hasPerkFunction; // Keep commented if not used

// Addresses calculated at runtime (based on the above or via displacement)
extern std::uintptr_t initiateJumpAddress;
extern std::uintptr_t originalJumpHeightFunctionAddress;
extern std::uintptr_t sprintJumpCheckAddress;
extern std::uintptr_t isPlayerSprintingFunctionAddress1;
extern std::uintptr_t isPlayerSprintingFunctionAddress2;
extern std::uintptr_t originalMovementSpeedFunctionAddress;

// Mod state variable DEFINITIONS
extern int remainingAirJumps;
extern bool isPerformingAirJump;
extern void* lastJumpCharacterController;
extern bool isPlayerSprinting;
extern float currentFallTime;
extern bool isFalling;