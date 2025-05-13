#include "PCH.h"
#include "Utils.h"

namespace Utils {
    //-----------------------------------------------------------------------------------
    // Utility Function - Read Relative Displacement
    //-----------------------------------------------------------------------------------
    /**
     * @brief Calculates the absolute target address of a relative CALL or JMP instruction (E8/E9 opcode).
     * Relative instructions store the offset from the *next* instruction's address.
     * @param instructionAddress The memory address of the CALL/JMP instruction itself.
     * @return The absolute memory address the instruction jumps/calls to.
     */
    std::uintptr_t ReadRelativeDisplacement(std::uintptr_t instructionAddress) {
        logger::trace("ReadRelativeDisplacement: Entering.");
        // The relative offset is stored as a 32-bit signed integer (int32_t)
        // starting 1 byte after the opcode (E8/E9).
        std::int32_t relativeOffset = *reinterpret_cast<std::int32_t*>(instructionAddress + 1);
        logger::trace("ReadRelativeDisplacement: Read relative offset.");

        // The absolute address is calculated relative to the address of the *next* instruction.
        // A 5-byte CALL/JMP instruction occupies addresses: Opcode, OffsetByte1, OffsetByte2, OffsetByte3, OffsetByte4.
        // The next instruction starts at instructionAddress + 5.
        std::uintptr_t nextInstructionAddress = instructionAddress + 5;
        logger::trace("ReadRelativeDisplacement: Calculated next instruction address.");

        // Absolute Address = Address of Next Instruction + Relative Offset
        std::uintptr_t targetAddress = nextInstructionAddress + relativeOffset;
        logger::trace("ReadRelativeDisplacement: Calculated target address.");

        logger::trace("ReadRelativeDisplacement: Exiting.");
        return targetAddress;
    }
}
