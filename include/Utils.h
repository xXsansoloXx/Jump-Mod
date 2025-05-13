#pragma once
#include "PCH.h"  // Include PCH here so REL::Relocation and RE types are known

namespace Utils {
    /**
     * @brief Calculates the absolute target address of a relative CALL or JMP instruction (E8/E9 opcode).
     * Relative instructions store the offset from the *next* instruction's address.
     * @param instructionAddress The memory address of the CALL/JMP instruction itself.
     * @return The absolute memory address the instruction jumps/calls to.
     */
    std::uintptr_t ReadRelativeDisplacement(std::uintptr_t instructionAddress);
}