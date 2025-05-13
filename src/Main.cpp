#include "PCH.h" // <--- Add this FIRST
#include "Main.h"
#include "Hooks.h"
#include "Addresses.h"
#include "Settings.h"



namespace Main
{
    class Main {
        public:
            static void Initialize();
        private:
    };

    // Game Engine Classes (using RE namespace) (Should be covered by RE/Skyrim.h in PCH)
    using PlayerCharacter = RE::PlayerCharacter;
    using Actor = RE::Actor;
    using TESForm = RE::TESForm;
    using BGSPerk = RE::BGSPerk;
    


    /**
     * @brief The main initialization function for the mod logic.
     * Called from the SKSE plugin entry point after game data is loaded.
     */
    void Initialize() {
        logger::info("BetterJumpingSE Refactored Initializing...");  // Existing log
        logger::info("Main::Initialize: Entering.");

        logger::info("Main::Initialize: Allocating trampoline.");
        // Allocate trampoline space early. Commonlib handles getting the SKSE trampoline.
        // Requesting reasonable sizes for branch and local (code generation) trampolines.
        // Combined size might need adjustment based on total hooks from all mods.
        SKSE::GetTrampoline().create(1024 * 64);  // Example size: 64KB
        logger::info("Main::Initialize: Trampoline allocated.");

        logger::info("Main::Initialize: Calling ResolveAddresses.");
        // Resolve necessary game addresses using Address Library.
        if (!Hooks::ResolveAddresses()) {
            logger::critical("Failed to resolve game addresses. Mod will not be functional.");  // Existing log
            logger::info("Main::Initialize: ResolveAddresses failed. Aborting initialization.");
            // Addresses are critical, stop initialization if they fail.
            return;
        }
        logger::info("Main::Initialize: ResolveAddresses succeeded.");

        logger::info("Main::Initialize: Calling InstallHooks.");
        // Install memory hooks and patches.
        if (!Hooks::InstallHooks()) {
            logger::critical(
                "Failed to install hooks/patches. Mod may be unstable or not functional.");  // Existing log
            logger::info("Main::Initialize: InstallHooks failed. Continuing initialization with potential issues.");
            // Hooking might fail for various reasons (e.g., address conflicts with other mods),
            // but it might not be fatal, so we continue but log the failure.
        }
        logger::info("Main::Initialize: InstallHooks finished.");

        logger::info("BetterJumpingSE Refactored Initialization finished.");  // Existing log
        logger::info("Main::Initialize: Exiting.");
    }
    //-----------------------------------------------------------------------------------
    // Perk Check Function
    //-----------------------------------------------------------------------------------
    /**
     * @brief Checks if the player character has the perk specified in the Settings.
     * @return True if no perk is required or if the player has the required perk, false otherwise.
     */
    /* bool DoesPlayerHaveRequiredPerk() {
        logger::trace("DoesPlayerHaveRequiredPerk: Entering.");
        // Check if perk requirement is toggled off via config pointer.
        bool perkRequirementEnabled = Settings::Advanced::PerkRequirementToggle;
        logger::trace("DoesPlayerHaveRequiredPerk: Read perk requirement toggle.");
    
        if (!perkRequirementEnabled) {
            logger::trace("DoesPlayerHaveRequiredPerk: Perk requirement disabled.");
            logger::trace("DoesPlayerHaveRequiredPerk: Exiting, returning true.");
            return true;
        }
        logger::trace("DoesPlayerHaveRequiredPerk: Perk requirement enabled.");
    
        int requiredPerkID = Settings::Advanced::RequiredPerkIDValue;
        logger::trace("DoesPlayerHaveRequiredPerk: Read required perk ID.");
    
    
        // If no perk ID is configured (ID is 0), then no check is needed.
        if (requiredPerkID == 0)
        {
            logger::trace("DoesPlayerHaveRequiredPerk: Required perk ID is 0, no perk needed.");
            logger::trace("DoesPlayerHaveRequiredPerk: Exiting, returning true.");
            return true; // Perk requirement met (none required).
        }
        logger::trace("DoesPlayerHaveRequiredPerk: Required perk ID is non-zero.");
    
        // Get the player character object from the global pointer.
        RE::PlayerCharacter* player = playerCharacterPtr.get();
        logger::trace("DoesPlayerHaveRequiredPerk: Got player character pointer.");
        if (!player) {
            logger::warn("PerkCheck: Could not get PlayerCharacter pointer!"); // Existing log
            logger::trace("DoesPlayerHaveRequiredPerk: Exiting, returning false (null player).");
            return false; // Cannot check perk if player pointer is invalid.
        }
    
        // Use the game's LookupByID function to find the TESForm associated with the configured perk ID.
        // Using the relocated function pointer.
        // Cast the ID to std::uint32_t to resolve the overload ambiguity.
        logger::trace("DoesPlayerHaveRequiredPerk: Attempting to lookup perk form by ID.");
        RE::TESForm* perkForm = lookupFormByIDFunction(static_cast<std::uint32_t>(requiredPerkID));
        logger::trace("DoesPlayerHaveRequiredPerk: Finished perk form lookup.");
    
        // Check if the FormID actually corresponds to a valid TESForm.
        if (perkForm == nullptr)
        {
            // Only warn once per perk ID? Or rely on user to fix config.
            // This might spam logs if an invalid ID is used, but useful for debugging.
            logger::warn("PerkCheck: Perk with FormID 0x{:X} not found!", requiredPerkID); // Existing log
            logger::trace("DoesPlayerHaveRequiredPerk: Exiting, returning false (perk form not found).");
            return false; // Perk not found, requirement not met.
        }
        logger::trace("DoesPlayerHaveRequiredPerk: Perk form found.");
    
        // Verify that the found TESForm is actually a Perk (BGSPerk).
        // Using RE::FormType enum instead of raw byte check.
        logger::trace("DoesPlayerHaveRequiredPerk: Checking perk form type.");
        if (perkForm->GetFormType() != RE::FormType::Perk)
        {
            logger::warn("PerkCheck: FormID 0x{:X} is not a Perk! It is type {}", requiredPerkID, static_cast<int>(perkForm->GetFormType())); // Existing log
            logger::trace("DoesPlayerHaveRequiredPerk: Exiting, returning false (form is not a perk).");
            return false; // Found object is not a perk, requirement not met.
        }
        logger::trace("DoesPlayerHaveRequiredPerk: Form type is Perk.");
    
        // Call the game's HasPerk function using the relocated function pointer.
        RE::BGSPerk* perk = static_cast<RE::BGSPerk*>(perkForm);
        logger::trace("DoesPlayerHaveRequiredPerk: Calling game's HasPerk function.");
        bool hasPerk = hasPerkFunction(player, perk);
        logger::trace("DoesPlayerHaveRequiredPerk: HasPerk function returned.");
    
        logger::trace("DoesPlayerHaveRequiredPerk: Exiting, returning HasPerk result.");
        return hasPerk; // Return the result of the HasPerk check.
    }*/
    
    //-----------------------------------------------------------------------------------
    // Air Jump Permission Hook Function 1 (Jump Initiation)
    //-----------------------------------------------------------------------------------
    /**
     * @brief Determines if a jump (ground or air) should be allowed when the jump action starts.
     * Called by the hook installed at `initiateJumpAddress`.
     * @param charController Pointer to the character controller object managing the actor's physics state.
     * @return True if the jump is allowed (either ground or air), false otherwise.
     */
    bool CheckAndPermitAirJump(void* charController)
    {
        logger::trace("CheckAndPermitAirJump: Entering hook.");
        // Check if player jump modifications are enabled via config pointer.
        bool playerToggleEnabled = Settings::General::PlayerToggle;
        logger::trace("CheckAndPermitAirJump: Read player toggle.");
    
        if (!playerToggleEnabled) {
            logger::trace("CheckAndPermitAirJump: Player toggle disabled.");
            // If disabled, allow only ground jumps by resetting state and returning true.
            // The original game code handles ground jump checks before this hook.
            remainingAirJumps = 0;
            isPerformingAirJump = false;
            lastJumpCharacterController = nullptr;
            currentFallTime = 0.0f;
            isFalling = false;
            logger::trace("CheckAndPermitAirJump: Player toggle disabled, state reset. Exiting, returning true.");
            return true;
        }
        logger::trace("CheckAndPermitAirJump: Player toggle enabled.");
    
    
        // Store the character controller pointer for use in FinalizeAirJumpState.
        lastJumpCharacterController = charController;
        logger::trace("CheckAndPermitAirJump: Stored character controller pointer.");
        currentFallTime = 0.0f; // Reset fall time on jump initiation
        isFalling = false;
        logger::trace("CheckAndPermitAirJump: Reset fall time and falling state.");
    
    
        // Check if the character is currently on the ground.
        // Access the isGrounded flag (offset 0x218 in char controller)
        logger::trace("CheckAndPermitAirJump: Checking if character is grounded.");
        bool isGrounded = (*(std::uint32_t*)((std::uintptr_t)charController + 0x218) & 0x400) != 0;
    
        if (isGrounded)
        {
            logger::trace("CheckAndPermitAirJump: Character is grounded.");
            // Player is on the ground, initiating a normal jump.
            // Reset the air jump counter based on Settings and perk check.
            // Max Jumps setting includes the ground jump (so N jumps total means N-1 air jumps).
            int maxJumps = Settings::General::MaxJumps;
            logger::trace("CheckAndPermitAirJump: Read MaxJumps setting.");
    
            if (maxJumps < 0) maxJumps = -1; // Treat negative as infinite
            //logger::trace("CheckAndPermitAirJump: Checking perk requirement for ground jump.");
            remainingAirJumps = maxJumps;  // DoesPlayerHaveRequiredPerk() ? (maxJumps == -1 ? -1 : maxJumps - 1) : 0;
            logger::trace("CheckAndPermitAirJump: Set remaining air jumps.");
            isPerformingAirJump = false; // Ensure air jump flag is off for ground jumps.
            logger::trace("CheckAndPermitAirJump: Reset isPerformingAirJump flag.");
            logger::trace("Ground jump initiated. Air jumps available: {}", remainingAirJumps == -1 ? "Infinite" : std::to_string(remainingAirJumps)); // Existing log
            logger::trace("CheckAndPermitAirJump: Exiting, allowing ground jump.");
            return true; // Allow the ground jump.
        }
        else
        {
            logger::trace("CheckAndPermitAirJump: Character is NOT grounded.");
            // Player is in the air, attempting an air jump.
            // Check if air jumps are allowed (-1 means infinite) based on remaining count.
            logger::trace("CheckAndPermitAirJump: Checking if air jumps are allowed.");
            if (remainingAirJumps == 0)
            {
                logger::trace("Air jump blocked: No jumps remaining."); // Existing log
                logger::trace("CheckAndPermitAirJump: Exiting, blocking air jump.");
                return false; // No air jumps left, block the jump.
            }
            logger::trace("CheckAndPermitAirJump: Air jumps are allowed.");
    
    
            // Decrement the counter if not infinite.
            if (remainingAirJumps > 0)
            {
                logger::trace("CheckAndPermitAirJump: Decrementing remaining air jumps.");
                remainingAirJumps--;
            }
            logger::trace("CheckAndPermitAirJump: Remaining air jumps checked/decremented.");
    
            // Set the flag indicating an air jump is happening.
            isPerformingAirJump = true;
            logger::trace("CheckAndPermitAirJump: Set isPerformingAirJump flag.");
            logger::trace("Air jump initiated. Jumps remaining: {}", remainingAirJumps == -1 ? "Infinite" : std::to_string(remainingAirJumps)); // Existing log
            logger::trace("CheckAndPermitAirJump: Exiting, allowing air jump.");
            return true; // Allow the air jump.
        }
        logger::trace("CheckAndPermitAirJump: Exiting.");
    }
    
    //-----------------------------------------------------------------------------------
    // Air Jump State Update Hook Function
    //-----------------------------------------------------------------------------------
    /**
     * @brief Modifies the character's movement state shortly after a jump starts.
     * Crucially, if an air jump was just performed, it changes the state
     * from "falling" back to "jumping" to apply upward force again. Also tracks fall time.
     * Called by the hook installed at `jumpStateCheckAddress`.
     * @param charController Pointer to the character controller object.
     * @return The (potentially modified) character movement state.
     */
    std::uint32_t FinalizeAirJumpState(void* charController) {
        //logger::trace("FinalizeAirJumpState: Entered. charController: {:p}", charController);
        std::uint32_t currentState = *(std::uint32_t*)((std::uintptr_t)charController + 0x1E0 + 0x20);
        logger::trace("FinalizeAirJumpState: Current state: {}", currentState);
    
        // Track fall time
        if (currentState == 2) {  // Falling state
            if (!isFalling) {
                isFalling = true;
                currentFallTime = 0.0f;
                //logger::trace("FinalizeAirJumpState: Entered Falling state (2), started fall timer.");
            } else {
                currentFallTime = *(float*)((std::uintptr_t)charController + 0x244);
                // Optionally log if fall time is significant or changes frequently
                // logger::trace("FinalizeAirJumpState: Updated fall time: {:.2f}", currentFallTime);
            }
        } else {
            /* if (isFalling) {  // Log only if there was a change from falling
                logger::trace("FinalizeAirJumpState: Exited Falling state (was true, now false). Reset fall timer.");
            }*/
            isFalling = false;
            currentFallTime = 0.0f;
        }
    
        // If state is landing (3), reset air jumps
        if (currentState == 3) {
            //logger::info("FinalizeAirJumpState: Landing (3) detected. Resetting air jumps. Returning original state.");
            remainingAirJumps = 0;
            isPerformingAirJump = false;
            lastJumpCharacterController = nullptr;
            return currentState;  // Don't modify landing state
        }
    
        // We only care about modifying the state if it's currently "falling" (state 2).
        if (currentState != 2) {
            // This path is hit very frequently, so trace might still be too much.
            // Consider removing this log entirely if it's too noisy and not providing new info.
            // logger::trace("FinalizeAirJumpState: Not Falling (2). Returning original state: {}", currentState);
            return currentState;  // Not falling, do nothing to state.
        }
        // From here, currentState IS 2 (Falling)
    
        // Check if an air jump was just initiated in CheckAndPermitAirJump.
        if (!isPerformingAirJump) {
            // logger::trace("FinalizeAirJumpState: State is Falling (2) but isPerformingAirJump is false. Returning
            // original state.");
            return currentState;  // Not performing an air jump, do nothing to state.
        }
        // From here, an air jump was flagged
    
        // Safety check: Ensure this is the same character controller that initiated the jump.
        if (lastJumpCharacterController != charController) {
            logger::warn(
                "FinalizeAirJumpState: Mismatched charController during air jump! Aborting state change. Expected: {:p}, "
                "Got: {:p}",
                lastJumpCharacterController, charController);
            isPerformingAirJump = false;
            lastJumpCharacterController = nullptr;
            return currentState;  // Mismatch, do nothing.
        }
    
        // Check fall time limit if configured and enabled.
        bool midairWindowEnabled = Settings::Advanced::MidairWindowToggle;
        if (midairWindowEnabled) {
            float maxFallTime = Settings::Advanced::MidairWindowTime;
            if (maxFallTime > 0.0f && currentFallTime >= maxFallTime) {
                logger::info(
                    "FinalizeAirJumpState: Air jump state change BLOCKED: Fall time limit exceeded ({:.2f} >= {:.2f})",
                    currentFallTime, maxFallTime);
                isPerformingAirJump = false;
                lastJumpCharacterController = nullptr;
                return currentState;  // Exceeded fall time, do not change state back to jumping.
            }
        }
    
        // If all checks pass, it means an air jump should occur.
        isPerformingAirJump = false;            // Reset the flag
        lastJumpCharacterController = nullptr;  // Clear pointer after use
        logger::info("FinalizeAirJumpState: Finalizing air jump! Changing state from 2 (Falling) to 1 (Jumping).");
        return 1;  // Return the modified state (Jumping).
    }
    
    //-----------------------------------------------------------------------------------
    // Jump Height Calculation Hook Function
    //-----------------------------------------------------------------------------------
    /**
     * @brief Replaces the game's jump height calculation to apply custom multipliers.
     * Called by the hook installed at `jumpHeightCalculationAddress`.
     * @param jumpingActor Pointer to the Actor performing the jump.
     * @return The calculated jump height multiplier.
     */
    float CalculateJumpHeightMultiplier(RE::Actor* jumpingActor)
    {
        logger::trace("CalculateJumpHeightMultiplier: Entering hook.");
        // Get the base jump height from the original game function first.
        logger::trace("CalculateJumpHeightMultiplier: Calling original jump height function.");
        float baseJumpHeight = ((float(*)(RE::Actor*))originalJumpHeightFunctionAddress)(jumpingActor);
        logger::trace("CalculateJumpHeightMultiplier: Original jump height function returned.");
    
    
        // Check if player jump modifications are enabled via config pointer.
        logger::trace("CalculateJumpHeightMultiplier: Checking player toggle.");
        bool playerToggleEnabled = Settings::General::PlayerToggle;
        logger::trace("CalculateJumpHeightMultiplier: Player toggle read.");
    
        if (!playerToggleEnabled) {
            logger::trace("CalculateJumpHeightMultiplier: Player toggle disabled.");
            // If disabled, return the original calculated base height.
            logger::trace("CalculateJumpHeightMultiplier: Exiting, returning base jump height.");
            return baseJumpHeight;
        }
        logger::trace("CalculateJumpHeightMultiplier: Player toggle enabled.");
    
    
        // Get multipliers from Settings. Use 1.0f if pointers are null or multipliers are disabled.
        logger::trace("CalculateJumpHeightMultiplier: Checking jump multipliers toggle.");
        bool jumpMultipliersEnabled = Settings::Advanced::JumpMultiplierToggle;
        logger::trace("CalculateJumpHeightMultiplier: Jump multipliers toggle read.");
    
        float groundMult = Settings::Advanced::GroundJumpMultiplier;
        float airMult = Settings::Advanced::AirJumpMultiplier;
        logger::trace("CalculateJumpHeightMultiplier: Read ground and air multipliers.");
    
    
        float sprintMult = Settings::General::SprintJumpMultiplier;
        bool sprintJumpToggleEnabled = Settings::General::SprintJumpToggle;
        logger::trace("CalculateJumpHeightMultiplier: Read sprint multiplier and toggle.");
    
        // 1. Start with the base jump height.
        float finalMultiplier = baseJumpHeight;
        logger::trace("CalculateJumpHeightMultiplier: Starting with base jump height.");
    
    
        // 2. Apply the ground jump multiplier (applies as a base unless multipliers are off).
        if (jumpMultipliersEnabled) {
            logger::trace("CalculateJumpHeightMultiplier: Jump multipliers enabled, applying ground multiplier.");
            finalMultiplier *= groundMult;
            logger::trace("CalculateJumpHeightMultiplier: Ground multiplier applied.");
            logger::trace("CalculateJumpHeightMultiplier: Checking if currently performing air jump.");
            if (isPerformingAirJump)
            {
                logger::trace("CalculateJumpHeightMultiplier: Is performing air jump.");
                logger::trace("CalculateJumpHeightMultiplier: Jump multipliers enabled, applying air multiplier.");
                finalMultiplier *= airMult;
                logger::trace("CalculateJumpHeightMultiplier: Air multiplier applied.");
            } else {
            logger::trace("CalculateJumpHeightMultiplier: Not performing air jump.");
            }
        }

        // 4. Apply the sprint jump multiplier if currently sprinting AND sprint jumping is allowed by config.
        // Use the flag updated by the sprint hook AND the config toggle.
        logger::trace("CalculateJumpHeightMultiplier: Checking if currently sprinting.");
        logger::info(
            "CalcJumpHeight: isGrounded={}, isPerformingAirJump={}, isPlayerSprinting(global)={}, "
            "gameReportsSprinting(actor+0xBDD)={}",
            (*(std::uint32_t*)((std::uintptr_t)jumpingActor->GetCharController() + 0x218) & 0x400) !=
                0,  // Re-check isGrounded if possible
            isPerformingAirJump, isPlayerSprinting, (*(std::uint8_t*)((std::uintptr_t)jumpingActor + 0xBDD) & 1) != 0);
        if (isPlayerSprinting) {
            logger::trace("CalculateJumpHeightMultiplier: Is currently sprinting.");
            logger::trace("CalculateJumpHeightMultiplier: Checking sprint jump toggle.");
            if (sprintJumpToggleEnabled)
            {
                logger::trace("CalculateJumpHeightMultiplier: Sprint jump enabled, applying sprint multiplier.");
                finalMultiplier *= sprintMult;
                logger::trace("CalculateJumpHeightMultiplier: Sprint multiplier applied.");
            } else {
                logger::trace("CalculateJumpHeightMultiplier: Sprint jump disabled by config, NOT applying sprint multiplier.");
            }
        } else {
            logger::trace("CalculateJumpHeightMultiplier: Not currently sprinting.");
        }
    
    
        // 5. Return the final calculated multiplier.
        logger::trace("CalculateJumpHeightMultiplier: Exiting, returning final multiplier. {}", finalMultiplier);
        return finalMultiplier;
    }
    
    
    //-----------------------------------------------------------------------------------
    // Sprint Speed Maintenance Hook Function
    //-----------------------------------------------------------------------------------
    /**
     * @brief Attempts to preserve the player's sprint speed when jumping while sprinting.
     * Called by the hook installed at `movementSpeedUpdateAddress`.
     * @param actor Pointer to the Actor whose speed is being updated.
     * @param movementData Pointer to a structure containing movement data (e.g., speed vector).
     */
    void MaintainSprintSpeedDuringJump(RE::Actor* actor, void* movementData)
    {
        logger::trace("MaintainSprintSpeedDuringJump: Entering hook.");
        // First, call the original game function this hook replaced.
        // This ensures the default speed calculations still happen.
        logger::trace("MaintainSprintSpeedDuringJump: Calling original movement speed function.");
        ((void(*)(RE::Actor*, void*))originalMovementSpeedFunctionAddress)(actor, movementData);
        logger::trace("MaintainSprintSpeedDuringJump: Original function returned.");
    
        // Only apply this logic to the player character.
        RE::PlayerCharacter* player = playerCharacterPtr.get();
        logger::trace("MaintainSprintSpeedDuringJump: Got player character pointer.");
         if (!player)
        {
            logger::trace("MaintainSprintSpeedDuringJump: Player Actor is not valid.");
            logger::trace("MaintainSprintSpeedDuringJump: Invalid Actor, Exiting.");
            return; // Not the player, do nothing extra.
        }
         if (actor != player)
        {
            logger::trace("MaintainSprintSpeedDuringJump: Actor is not the player.");
            logger::trace("MaintainSprintSpeedDuringJump: Exiting.");
            return; // Not the player, do nothing extra.
        }
        logger::trace("MaintainSprintSpeedDuringJump: Actor is the player.");
    
    
        // Check if player jump modifications are enabled AND sprint jumping is allowed.
        logger::trace("MaintainSprintSpeedDuringJump: Checking mod features and sprint jump toggles.");
        bool modFeaturesEnabled = Settings::General::PlayerToggle;
        bool sprintJumpAllowed = Settings::General::SprintJumpToggle;
        logger::trace("MaintainSprintSpeedDuringJump: Toggles read.");
    
         // If either condition is false, and we weren't already in the middle of overriding, exit.
        // Static flag persists across calls.
        static bool overrideSpeedFlag = false;
        logger::trace("MaintainSprintSpeedDuringJump: overrideSpeedFlag value read.");
    
    
        if (!modFeaturesEnabled || !sprintJumpAllowed) {
            logger::trace("MaintainSprintSpeedDuringJump: Mod features or sprint jump disabled.");
            // If features/sprint jump are disabled, stop any ongoing override
            // and let the game handle speed normally.
            if (overrideSpeedFlag) {
                 logger::trace("MaintainSprintSpeedDuringJump: Resetting overrideSpeedFlag.");
                 overrideSpeedFlag = false;
                 logger::trace("MaintainSprintSpeedDuringJump: overrideSpeedFlag is now false.");
            } else {
                logger::trace("MaintainSprintSpeedDuringJump: overrideSpeedFlag was already false.");
            }
            // We already called the original function, so targetSpeed should be set correctly by it.
            // No need to force *targetSpeed = *fromSpeed; here unless we need to explicitly undo
            // any potential weirdness, which the original call usually handles.
            logger::trace("MaintainSprintSpeedDuringJump: Exiting (features disabled).");
            return;
        }
        logger::trace("MaintainSprintSpeedDuringJump: Mod features and sprint jump enabled.");
    
    
        // --- Accessing nested game data structures ---
        logger::trace("MaintainSprintSpeedDuringJump: Attempting to access nested game structures.");
        // Using RE namespace types where available, otherwise relying on offsets.
        // Note: RE::Actor doesn't expose these easily, still need raw offsets.
    
        // Get ProcessManager from Actor (offset 0xF0)
        // RE::ActorProcessManager* processManager = actor->processManager; // Use RE type if available
         std::uintptr_t processManager = *(std::uintptr_t*)((std::uintptr_t)actor + 0xF0);
        logger::trace("MaintainSprintSpeedDuringJump: Got process manager pointer.");
        if (!processManager) {
            logger::trace("MaintainSprintSpeedDuringJump: Process manager is null.");
            overrideSpeedFlag = false; // Reset if we lose process manager
            logger::trace("MaintainSprintSpeedDuringJump: overrideSpeedFlag reset due to null process manager.");
            logger::trace("MaintainSprintSpeedDuringJump: Exiting (null process manager).");
            return; // Safety check
        }
        logger::trace("MaintainSprintSpeedDuringJump: Process manager is valid.");
    
        // Get MiddleHighProcess from ProcessManager (offset 0x8)
        std::uintptr_t middleProcess = *(std::uintptr_t*)((std::uintptr_t)processManager + 0x8);
        logger::trace("MaintainSprintSpeedDuringJump: Got middle process pointer.");
        if (!middleProcess) {
            logger::trace("MaintainSprintSpeedDuringJump: Middle process is null.");
             overrideSpeedFlag = false; // Reset if we lose middle process
             logger::trace("MaintainSprintSpeedDuringJump: overrideSpeedFlag reset due to null middle process.");
            logger::trace("MaintainSprintSpeedDuringJump: Exiting (null middle process).");
            return; // Safety check
        }
        logger::trace("MaintainSprintSpeedDuringJump: Middle process is valid.");
    
    
        // Get HighProcess from ProcessManager (offset 0x10)
        std::uintptr_t highProcess = *(std::uintptr_t*)((std::uintptr_t)processManager + 0x10);
        logger::trace("MaintainSprintSpeedDuringJump: Got high process pointer.");
        if (!highProcess) {
            logger::trace("MaintainSprintSpeedDuringJump: High process is null.");
             overrideSpeedFlag = false; // Reset if we lose high process
             logger::trace("MaintainSprintSpeedDuringJump: overrideSpeedFlag reset due to null high process.");
            logger::trace("MaintainSprintSpeedDuringJump: Exiting (null high process).");
            return; // Safety check
        }
        logger::trace("MaintainSprintSpeedDuringJump: High process is valid.");
    
    
        // Get CharacterController from MiddleHighProcess (offset 0x250)
        std::uintptr_t charController = *(std::uintptr_t*)((std::uintptr_t)middleProcess + 0x250);
        logger::trace("MaintainSprintSpeedDuringJump: Got character controller pointer.");
        if (!charController) {
            logger::trace("MaintainSprintSpeedDuringJump: Character controller is null.");
             overrideSpeedFlag = false; // Reset if we lose char controller
             logger::trace("MaintainSprintSpeedDuringJump: overrideSpeedFlag reset due to null character controller.");
            logger::trace("MaintainSprintSpeedDuringJump: Exiting (null character controller).");
            return; // Safety check
        }
        logger::trace("MaintainSprintSpeedDuringJump: Character controller is valid.");
        // --- End of data structure navigation ---
        logger::trace("MaintainSprintSpeedDuringJump: Successfully accessed nested game structures.");
    
    
        // Get pointers to the relevant speed values.
        // 'fromSpeed' is the speed calculated by the game for the current frame (in movementData).
        // 'targetSpeed' is the speed value stored in the HighProcess that the game *uses*.
        logger::trace("MaintainSprintSpeedDuringJump: Getting speed pointers.");
        float* fromSpeed = (float*)((std::uintptr_t)movementData + 0x1C); // Offset 0x1C within movement data struct
        float* targetSpeed = (float*)((std::uintptr_t)highProcess + 0xF0 + 0x1C); // Offset 0xF0 + 0x1C within HighProcess struct
        logger::trace("MaintainSprintSpeedDuringJump: Got speed pointers.");
    
        // Check if the player *is currently* considered sprinting by the game engine.
        // Reads a byte flag at offset 0xBDD within the Actor structure.
        logger::trace("MaintainSprintSpeedDuringJump: Checking game's sprint flag.");
        bool gameReportsSprinting = (*(std::uint8_t*)((std::uintptr_t)actor + 0xBDD) & 1) != 0;
        logger::trace("MaintainSprintSpeedDuringJump: Game's sprint flag read.");
    
        // Read the character's current movement state (same as in FinalizeAirJumpState).
        logger::trace("MaintainSprintSpeedDuringJump: Reading current character state.");
        std::uint32_t currentState = *(std::uint32_t*)((std::uintptr_t)charController + 0x1E0 + 0x20);
        logger::trace("MaintainSprintSpeedDuringJump: Current character state read.");
    
        static float savedSprintSpeed = 0.0f;
        logger::trace("MaintainSprintSpeedDuringJump: savedSprintSpeed value read.");
    
        // Check if the player is sprinting OR if we are already in the process of overriding speed.
        // We only apply the override logic if mod features and sprint jump are allowed (checked above).
        logger::trace("MaintainSprintSpeedDuringJump: Checking if game reports sprinting or override flag is set.");
        if (gameReportsSprinting || overrideSpeedFlag)
        {
            // Static variable to store the sprint speed from the last grounded frame.
            // 'static' means its value persists across multiple calls to this function.
            logger::trace("MaintainSprintSpeedDuringJump: Game reports sprinting OR override flag is set.");
            overrideSpeedFlag = false; // Assume we won't need to override next frame unless conditions are met again.
            logger::trace("MaintainSprintSpeedDuringJump: overrideSpeedFlag reset.");
    
            logger::trace("MaintainSprintSpeedDuringJump: Checking character state for speed logic.");
            if (currentState == 0) // State 0: On ground (Idle/Walking/Running/Sprinting)
            {
                logger::trace("MaintainSprintSpeedDuringJump: State is On Ground (0).");
                // While sprinting on the ground, save the current speed.
                logger::trace("MaintainSprintSpeedDuringJump: Saving current speed.");
                savedSprintSpeed = *fromSpeed;
                logger::trace("MaintainSprintSpeedDuringJump: Speed saved.");
            }
            else if (currentState == 1 || currentState == 2) // State 1: Jumping, State 2: Falling
            {
                logger::trace("MaintainSprintSpeedDuringJump: State is Airborne (1 or 2).");
                // While jumping or falling after sprinting, force the speed.
                logger::trace("MaintainSprintSpeedDuringJump: Setting overrideSpeedFlag for next frame.");
                overrideSpeedFlag = true; // Set flag to continue overriding next frame if still airborne.
                logger::trace("MaintainSprintSpeedDuringJump: overrideSpeedFlag is now true.");
                logger::trace("MaintainSprintSpeedDuringJump: Overriding target speed with saved sprint speed.");
                *targetSpeed = savedSprintSpeed; // Override the game's target speed.
                logger::trace("MaintainSprintSpeedDuringJump: Overriding from speed with saved sprint speed.");
                *fromSpeed = savedSprintSpeed;   // Also override the 'from' speed for compatibility with other mods (e.g., Bug Fixes SSE).
                logger::trace("MaintainSprintSpeedDuringJump: Speeds overridden.");
            }
            else // Other states (e.g., landing)
            {
                logger::trace("MaintainSprintSpeedDuringJump: State is Other (not 0, 1, or 2).");
                // In other states, just let the game handle the speed normally.
                // This happens automatically if overrideSpeedFlag is false.
                logger::trace("MaintainSprintSpeedDuringJump: Syncing target speed to from speed.");
                *targetSpeed = *fromSpeed; // Ensure they are in sync
                logger::trace("MaintainSprintSpeedDuringJump: Speeds synced.");
            }
        }
        else // Not sprinting and not currently overriding speed
        {
             logger::trace("MaintainSprintSpeedDuringJump: Game does NOT report sprinting AND override flag is false.");
             // Ensure the override flag is off and let the game handle speed normally.
             /* if (overrideSpeedFlag) {
                logger::trace("MaintainSprintSpeedDuringJump: Resetting overrideSpeedFlag.");
                 overrideSpeedFlag = false;
                 logger::trace("MaintainSprintSpeedDuringJump: overrideSpeedFlag is now false.");
            } else {
                logger::trace("MaintainSprintSpeedDuringJump: overrideSpeedFlag was already false.");
            }*/
            logger::trace("MaintainSprintSpeedDuringJump: Syncing target speed to from speed.");
            *targetSpeed = *fromSpeed; // Ensure they are in sync
            logger::trace("MaintainSprintSpeedDuringJump: Speeds synced.");
        }
        logger::trace("MaintainSprintSpeedDuringJump: Exiting.");
    }
}