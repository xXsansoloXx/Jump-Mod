#pragma once

#include <spdlog/sinks/basic_file_sink.h>
// CommonLibSSE NG Headers
#include <RE/Skyrim.h>        // Contains RE types like Actor, TESForm, PlayerCharacter etc.
#include <SKSE/SKSE.h>        // Core SKSE interface, logging macros (logger::), trampoline access etc.
#include <REL/Relocation.h>   // Address library access and relocation

// Standard Library Headers
// Add standard library headers you use often.
#include <cstdint>            // For std::uint8_t, std::uint32_t, std::uint64_t etc.
#include <string>             // If you use std::string frequently
#include <vector>             // If you use std::vector frequently

namespace logger = SKSE::log;
using namespace std::literals;