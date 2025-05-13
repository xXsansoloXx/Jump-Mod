#include "PCH.h"      // First
#include "Addresses.h"  // Include the new header with extern declarations (optional here, but good practice)
// Even though this file defines them, including Addresses.h ensures consistency
// and that the declarations match the definitions.

//-----------------------------------------------------------------------------------
// Global Variables - Addresses - DEFINITIONS
//-----------------------------------------------------------------------------------

REL::Relocation<std::uintptr_t> jumpStateCheckAddress(REL::ID(78275), 0xBA3);
REL::Relocation<std::uintptr_t> jumpHandlerBaseAddress(REL::ID(42423));
REL::Relocation<std::uintptr_t> jumpHeightCalculationAddress(REL::ID(37257), 0x17F);
REL::Relocation<std::uintptr_t> movementSpeedUpdateAddress(REL::ID(38019), 0xA1);

REL::Relocation<RE::TESForm* (*)(RE::FormID)> lookupFormByIDFunction(REL::ID(14617));
REL::Relocation<RE::PlayerCharacter*> playerCharacterPtr(REL::ID(403521));
// REL::Relocation<decltype(&RE::Actor::HasPerk)> hasPerkFunction(REL::ID(37698));

std::uintptr_t initiateJumpAddress = 0;  // Initialize to 0 or some invalid default
std::uintptr_t originalJumpHeightFunctionAddress = 0;
std::uintptr_t sprintJumpCheckAddress = 0;
std::uintptr_t isPlayerSprintingFunctionAddress1 = 0;
std::uintptr_t isPlayerSprintingFunctionAddress2 = 0;
std::uintptr_t originalMovementSpeedFunctionAddress = 0;

// Mod state variable DEFINITIONS
int remainingAirJumps = 0;
bool isPerformingAirJump = false;
void* lastJumpCharacterController = nullptr;
bool isPlayerSprinting = false;
float currentFallTime = 0.0f;
bool isFalling = false;