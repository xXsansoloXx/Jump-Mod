#include "PCH.h"  // Skyrim engine classes
#include "Hooks.h"  // Header for this file
#include "Settings.h"  // Settings header for accessing mod settings
#include "Main.h"  // Main header for mod initialization and logging
#include "Addresses.h"
#include <xbyak/xbyak.h>     // Assembly generation (likely only used here, maybe not in PCH)

namespace Hooks {
    //-----------------------------------------------------------------------------------
    // Hook Installation Functions (using Xbyak for Assembly Generation)
    //-----------------------------------------------------------------------------------
    // These functions create small pieces of assembly code (using the Xbyak library)
    // that act as intermediaries (detours) for the original game functions.
    // They call our C++ functions and then jump back to the original game code.
    // Commonlib's trampoline handles the actual injection of these detours.

    /**
     * @brief Installs the hook for the jump state check.
     * Redirects execution to `FinalizeAirJumpState`.
     */
    void InstallJumpStateCheckHook() {
        logger::info("InstallJumpStateCheckHook: Entering.");
        // Get trampoline for code allocation and hooking
        auto& trampoline = SKSE::GetTrampoline();
        logger::trace("InstallJumpStateCheckHook: Got trampoline.");

        // Define the assembly code structure using Xbyak.
        struct HookCode : Xbyak::CodeGenerator {
            HookCode(void* buf)
                : Xbyak::CodeGenerator(4096, buf)  // Allocate 4KB buffer for code
            {
                logger::trace("InstallJumpStateCheckHook: HookCode generator starting.");
                // --- Assembly Code ---
                // The original code at jumpStateCheckAddress expects the character controller pointer
                // in the RSI register. We need it in RCX for our C++ function call (standard x64 calling convention).
                mov(rcx, rsi);  // Move the pointer from RSI to RCX
                logger::trace("InstallJumpStateCheckHook: Moved RSI to RCX.");

                // Jump to our C++ function. Xbyak handles the call instruction generation.
                // The return value (the potentially modified state) will be in EAX/RAX.
                jmp(ptr[rip]);  // Jump to the address stored immediately after this instruction.
                dq((std::uintptr_t)Main::FinalizeAirJumpState);  // The address of our C++ function.
                logger::trace("InstallJumpStateCheckHook: Added jump to FinalizeAirJumpState.");
                // --- End Assembly ---
                logger::trace("InstallJumpStateCheckHook: HookCode generator finished.");
            }
        };

        // Allocate memory for the hook code using SKSE's trampoline.
        logger::trace("InstallJumpStateCheckHook: Allocating code buffer.");
        void* codeBuffer = trampoline.allocate<HookCode>();
        logger::trace("InstallJumpStateCheckHook: Code buffer allocated.");
        HookCode hook(codeBuffer);  // Generate the assembly code into the buffer.
        logger::trace("InstallJumpStateCheckHook: HookCode generated.");

        // Write a 5-byte CALL instruction at the target game address (`jumpStateCheckAddress`),
        // redirecting the game's execution flow to our generated hook code.
        // Use Commonlib's Write5Call via the trampoline.
        logger::trace("InstallJumpStateCheckHook: Writing call hook.");
        trampoline.write_call<5>(jumpStateCheckAddress.address(), (std::uintptr_t)hook.getCode());
        logger::trace("InstallJumpStateCheckHook: Call hook written.");
        logger::info("Installed hook at JumpStateCheckAddress (0x{:p})",
                     (void*)jumpStateCheckAddress.address());  // Existing log
        logger::info("InstallJumpStateCheckHook: Exiting.");
    }

    /**
     * @brief Installs the hook for jump initiation.
     * Redirects execution to `CheckAndPermitAirJump`.
     */
    void InstallInitiateJumpHook() {
        logger::info("InstallInitiateJumpHook: Entering.");
        // Get trampoline
        auto& trampoline = SKSE::GetTrampoline();
        logger::trace("InstallInitiateJumpHook: Got trampoline.");

        // Define the assembly code structure.
        struct HookCode : Xbyak::CodeGenerator {
            HookCode(void* buf, std::uintptr_t retAddr) : Xbyak::CodeGenerator(4096, buf) {
                logger::trace("InstallInitiateJumpHook: HookCode generator starting.");
                // --- Assembly Code ---
                // The original code passes the character controller pointer in RAX.
                // Move it to RCX for our C++ function call.
                mov(rcx, rax);
                logger::trace("InstallInitiateJumpHook: Moved RAX to RCX.");

                // Call our C++ function CheckAndPermitAirJump.
                // Use MOV RAX, address; CALL RAX for flexibility.
                mov(rax, (std::uintptr_t)Main::CheckAndPermitAirJump);
                call(rax);  // Call the function. Return value (bool) will be in AL (lowest byte of RAX).
                logger::trace("InstallInitiateJumpHook: Called CheckAndPermitAirJump.");

                // Test the boolean result in AL. If CheckAndPermitAirJump returned false (AL == 0),
                // the Zero Flag (ZF) will be set by the TEST instruction.
                test(al, al);
                logger::trace("InstallInitiateJumpHook: Tested function result.");

                // Jump back into the original Skyrim code. The specific jump target address
                // (`retAddr`) is determined by reverse engineering and skips
                // the original jump permission check that our function replaced.
                jmp(ptr[rip]);
                dq(retAddr);  // Address to jump back to in Skyrim's code.
                logger::trace("InstallInitiateJumpHook: Added jump back to original code.");
                // --- End Assembly ---
                logger::trace("InstallInitiateJumpHook: HookCode generator finished.");
            }
        };

        // Allocate memory and generate code.
        // The return address is initiateJumpAddress + 0xC.
        std::uintptr_t returnAddress = initiateJumpAddress + 0xC;
        logger::trace("InstallInitiateJumpHook: Calculated return address.");
        logger::trace("InstallInitiateJumpHook: Allocating code buffer.");
        void* codeBuffer = trampoline.allocate<HookCode>();
        logger::trace("InstallInitiateJumpHook: Code buffer allocated.");
        HookCode hook(codeBuffer, returnAddress);  // Generate the assembly code into the buffer.
        logger::trace("InstallInitiateJumpHook: HookCode generated.");

        // Write a 6-byte JMP instruction at the target game address (`initiateJumpAddress`),
        // redirecting execution to our hook code. (Using Write6Branch).
        logger::trace("InstallInitiateJumpHook: Writing branch hook.");
        trampoline.write_branch<6>(initiateJumpAddress, (std::uintptr_t)hook.getCode());
        logger::trace("InstallInitiateJumpHook: Branch hook written.");
        logger::info("Installed hook at InitiateJumpAddress (0x{:p})", (void*)initiateJumpAddress);  // Existing log
        logger::info("InstallInitiateJumpHook: Exiting.");
    }

    /**
     * @brief Installs the hook for the sprint jump check.
     * Sets the `isPlayerSprinting` flag and optionally blocks sprint jumping.
     */
    void InstallSprintJumpCheckHook() {
        logger::info("InstallSprintJumpCheckHook: Entering.");
        // Get trampoline
        auto& trampoline = SKSE::GetTrampoline();
        logger::trace("InstallSprintJumpCheckHook: Got trampoline.");

        // Define the assembly code structure.
        struct HookCode : Xbyak::CodeGenerator {
            HookCode(void* buf, std::uintptr_t playerPtrAddr, std::uintptr_t sprintFunc1, std::uintptr_t sprintFunc2,
                     std::uintptr_t continueAddr, std::uintptr_t blockAddr)
                : Xbyak::CodeGenerator(4096, buf) {
                Xbyak::Label IsSprintingPath, PlayerNotSprintingPath, BlockJumpFinal,
                    ContinueJumpFinal;  // Removed ConfigCheckPath as it's integrated

                // Check first sprinting condition
                mov(rcx, playerPtrAddr);
                mov(rcx, ptr[rcx]);
                mov(rax, sprintFunc1);
                call(rax);
                test(al, al);
                jne(IsSprintingPath);  // If sprinting, go to IsSprintingPath

                // Check second sprinting condition
                mov(rcx, playerPtrAddr);
                mov(rcx, ptr[rcx]);
                mov(edx, 0x100);
                add(rcx, 0xB8);
                mov(rax, sprintFunc2);
                call(rax);
                test(al, al);
                jne(IsSprintingPath);  // If sprinting, go to IsSprintingPath

                // --- Player is NOT sprinting ---
                // L(PlayerNotSprintingPath); // This label isn't strictly needed if it's just fall-through
                mov(rcx, (std::uintptr_t)&isPlayerSprinting);
                mov(byte[rcx], 0);       // isPlayerSprinting = false
                jmp(ContinueJumpFinal);  // Directly go to continue game logic

                // --- Player IS sprinting ---
                L(IsSprintingPath);  // Use L() for Xbyak label definition
                mov(rcx, (std::uintptr_t)&isPlayerSprinting);
                mov(byte[rcx], 1);  // isPlayerSprinting = true
                // Now check config
                mov(rax, (std::uintptr_t)Settings::General::SprintJumpToggle);
                cmp(byte[rax], 0);   // if SprintJumpToggle == false (0)
                je(BlockJumpFinal);  // then jump to block the sprint jump
                // Else (SprintJumpToggle is true), fall through to continue

                // --- Sprint jump allowed by config (or player not sprinting via the jmp above) ---
                L(ContinueJumpFinal);  // Use L() for Xbyak label definition
                jmp(ptr[rip]);
                dq(continueAddr);  // Jump to game's continue path

                // --- Sprint jump blocked by config ---
                L(BlockJumpFinal);  // Use L() for Xbyak label definition
                jmp(ptr[rip]);
                dq(blockAddr);  // Jump to game's block path
            }
        };

        // Allocate memory and generate code.
        // Calculate return addresses based on sprintJumpCheckAddress.
        std::uintptr_t continueAddress = sprintJumpCheckAddress + 0x2C;
        std::uintptr_t blockAddress = sprintJumpCheckAddress + 0x3D;
        logger::trace("InstallSprintJumpCheckHook: Calculated return addresses.");

        // Pass the *address* of the playerCharacterPtr variable to the HookCode constructor.
        std::uintptr_t playerPtrAddress = (std::uintptr_t)&playerCharacterPtr;
        logger::trace("InstallSprintJumpCheckHook: Got player pointer variable address.");
        logger::trace("InstallSprintJumpCheckHook: Allocating code buffer.");
        void* codeBuffer = trampoline.allocate<HookCode>();
        logger::trace("InstallSprintJumpCheckHook: Code buffer allocated at: {:p}", codeBuffer);
        if (!codeBuffer) {
            logger::error("InstallSprintJumpCheckHook: Failed to allocate code buffer from trampoline!");
            // Handle error, perhaps return or throw
            return;  // Or some other error handling
        }
        // Construct the HookCode object with all required arguments
        HookCode hook(
            codeBuffer,
            playerPtrAddress,                   // Address of your global playerCharacterPtr Relocation object
            isPlayerSprintingFunctionAddress1,  // Resolved address of the first game sprint function
            isPlayerSprintingFunctionAddress2,  // Resolved address of the second game sprint function
            continueAddress,                    // Calculated: sprintJumpCheckAddress + 0x2C
            blockAddress);  // Calculated: sprintJumpCheckAddress + 0x3D logger::trace("InstallSprintJumpCheckHook:
                            // HookCode generated. Code at: {:p}", (void*)hook.getCode());
        if (!hook.getCode()) {
            logger::error("InstallSprintJumpCheckHook: hook.getCode() returned NULL!");
            return;  // Or some other error handling
        }
        logger::trace("InstallSprintJumpCheckHook: Code buffer allocated.");

        logger::trace("InstallSprintJumpCheckHook: HookCode generated.");

        // Write a 6-byte JMP instruction at the target game address (`sprintJumpCheckAddress`).
        logger::trace("InstallSprintJumpCheckHook: Writing branch hook.");
        // In InstallSprintJumpCheckHook, before the write_branch call:
        logger::info("InstallSprintJumpCheckHook: Attempting to write branch at sprintJumpCheckAddress: {:p}",
                     (void*)sprintJumpCheckAddress);
        if (!sprintJumpCheckAddress) {
            logger::error("InstallSprintJumpCheckHook: sprintJumpCheckAddress is NULL! Aborting hook.");
            return;
        }

        trampoline.write_branch<6>(sprintJumpCheckAddress, (std::uintptr_t)hook.getCode());

        logger::trace("InstallSprintJumpCheckHook: Branch hook written.");
        // logger::info("Installed hook at SprintJumpCheckAddress (0x{:p})", (void*)sprintJumpCheckAddress.address());
        // // Existing log
        logger::info("InstallSprintJumpCheckHook: Exiting.");
    }

    //-----------------------------------------------------------------------------------
    // Initialization Functions
    //-----------------------------------------------------------------------------------

    /**
     * @brief Resolves all required memory addresses using Commonlib's Address Library integration.
     * @return True if all addresses were found successfully, false otherwise.
     */
    bool ResolveAddresses() {
        logger::info("Resolving game addresses...");

        if (!jumpHandlerBaseAddress.address()) {
            logger::error("Failed to find jumpHandlerBaseAddress (ID 42423)!");
            return false;
        }
        logger::info("jumpHandlerBaseAddress: 0x{:X}", jumpHandlerBaseAddress.address());

        isPlayerSprintingFunctionAddress1 = Utils::ReadRelativeDisplacement(jumpHandlerBaseAddress.address() + 0xE1);
        if (!isPlayerSprintingFunctionAddress1) {
            logger::error("Failed: isPlayerSprintingFunctionAddress1 from base + 0xE1 (0x{:X})",
                          jumpHandlerBaseAddress.address() + 0xE1);
            return false;
        }
        logger::info("OK: isPlayerSprintingFunctionAddress1: 0x{:X}", isPlayerSprintingFunctionAddress1);

        isPlayerSprintingFunctionAddress2 = Utils::ReadRelativeDisplacement(jumpHandlerBaseAddress.address() + 0xFD);
        if (!isPlayerSprintingFunctionAddress2) {
            logger::error("Failed: isPlayerSprintingFunctionAddress2 from base + 0xFD (0x{:X})",
                          jumpHandlerBaseAddress.address() + 0xFD);
            return false;
        }
        logger::info("OK: isPlayerSprintingFunctionAddress2: 0x{:X}", isPlayerSprintingFunctionAddress2);

        sprintJumpCheckAddress = jumpHandlerBaseAddress.address() + 0xDA;
        logger::info("Derived: sprintJumpCheckAddress: 0x{:X}", sprintJumpCheckAddress);
        if (!sprintJumpCheckAddress) {  // This check might be redundant if jumpHandlerBaseAddress is already non-zero,
                                        // but good for sanity
            logger::error("Failed: sprintJumpCheckAddress is NULL or invalid after calculation!");
            return false;
        }

        if (!jumpStateCheckAddress.address()) {
            logger::error("Failed to find jumpStateCheckAddress (ID 78275, Offset 0xBA3)!");
            return false;
        }
        logger::info("jumpStateCheckAddress: 0x{:X}", jumpStateCheckAddress.address());

        initiateJumpAddress = jumpHandlerBaseAddress.address() + 0xBC;
        logger::info("initiateJumpAddress: 0x{:X}", initiateJumpAddress);

        originalJumpHeightFunctionAddress = Utils::ReadRelativeDisplacement(jumpHeightCalculationAddress.address());
        if (!originalJumpHeightFunctionAddress) {
            logger::error("Failed to find originalJumpHeightFunctionAddress from jumpHeightCalculationAddress: 0x{:X}!",
                          jumpHeightCalculationAddress.address());
            return false;
        }
        logger::info("originalJumpHeightFunctionAddress: 0x{:X}", originalJumpHeightFunctionAddress);

        // Check movementSpeedUpdateAddress directly
        if (!movementSpeedUpdateAddress.address()) {
            logger::error("Failed: movementSpeedUpdateAddress (ID 38019, Offset 0xA1)!");
            return false;
        }
        logger::info("OK: movementSpeedUpdateAddress: 0x{:X}", movementSpeedUpdateAddress.address());

        // Check originalMovementSpeedFunctionAddress (depends on the above)
        originalMovementSpeedFunctionAddress = Utils::ReadRelativeDisplacement(movementSpeedUpdateAddress.address());
        if (!originalMovementSpeedFunctionAddress) {
            logger::error("Failed: originalMovementSpeedFunctionAddress from 0x{:X}",
                          movementSpeedUpdateAddress.address());
            return false;
        }
        logger::info("OK: originalMovementSpeedFunctionAddress: 0x{:X}", originalMovementSpeedFunctionAddress);

        // ... (check lookupFormByIDFunction, playerCharacterPtr) ...

        logger::info("All required addresses resolved successfully.");
        return true;
    }

    /**
     * @brief Installs all necessary hooks and patches.
     * @return True if all hooks and patches were installed successfully, false otherwise.
     */
    bool InstallHooks() {
        logger::info("Applying hooks and patches...");  // Existing log
        logger::info("InstallHooks: Entering.");

        // Patch 1: Apply a simple byte patch.
        // 0xEB is the opcode for an unconditional short JMP. This likely bypasses a small piece of original code.
        // Using a direct pointer cast write.
        /** logger::info("InstallHooks: Applying unknown patch.");
        *reinterpret_cast<std::uint8_t*>(unknownPatchAddress.address()) = 0xEB;
        logger::info("InstallHooks: Unknown patch applied.");
        logger::info("Applied patch at UnknownPatchAddress (0x{:p})", (void*)unknownPatchAddress.address()); // Existing
        log
      */

        // Hook 1: Install hook for jump state checking.
        logger::info("InstallHooks: Calling InstallJumpStateCheckHook.");
        InstallJumpStateCheckHook();
        logger::info("InstallHooks: InstallJumpStateCheckHook finished.");

        // Hook 2: Install hook for jump initiation.
        logger::info("InstallHooks: Calling InstallInitiateJumpHook.");
        InstallInitiateJumpHook();
        logger::info("InstallHooks: InstallInitiateJumpHook finished.");

        // Hook 3: Install hook for sprint jump checking.
        logger::info("InstallHooks: Calling InstallSprintJumpCheckHook.");
        InstallSprintJumpCheckHook();
        logger::info("InstallHooks: InstallSprintJumpCheckHook finished.");

        // Hook 4: Hook the call to the original jump height calculation function.
        // Redirect it to our custom calculation function using a 5-byte CALL hook via trampoline.
        logger::info("InstallHooks: Installing JumpHeightCalculation hook.");
        SKSE::GetTrampoline().write_call<5>(jumpHeightCalculationAddress.address(),
                                            (std::uintptr_t)Main::CalculateJumpHeightMultiplier);
        logger::info("InstallHooks: JumpHeightCalculation hook installed.");
        logger::info("Installed hook at JumpHeightCalculationAddress (0x{:p})",
                     (void*)jumpHeightCalculationAddress.address());  // Existing log

        // Hook 5: Hook the call to the original movement speed update function.
        // Redirect it to our custom sprint speed maintenance function via trampoline.
        logger::info("InstallHooks: Installing MovementSpeedUpdate hook.");
        SKSE::GetTrampoline().write_call<5>(movementSpeedUpdateAddress.address(),
                                            (std::uintptr_t)Main::MaintainSprintSpeedDuringJump);
        logger::info("InstallHooks: MovementSpeedUpdate hook installed.");
        logger::info("Installed hook at MovementSpeedUpdateAddress (0x{:p})",
                     (void*)movementSpeedUpdateAddress.address());  // Existing log

        logger::info("All hooks and patches applied successfully.");  // Existing log
        logger::info("InstallHooks: Exiting.");
        return true;  // Assuming writes succeeded
    }
}