#pragma once

namespace Hooks {
    // Should not need to include PCH.h here if .cpp files do.
    // Declare functions defined in Hooks.cpp
    void InstallJumpStateCheckHook();
    void InstallInitiateJumpHook();
    void InstallSprintJumpCheckHook();
    // Maybe ResolveAddresses and InstallHooks if they are now part of this "module"
    bool ResolveAddresses();
    bool InstallHooks();
}