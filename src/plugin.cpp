#include "PCH.h"

#include "plugin.h"
#include "Main.h"
#include "MCP.h"
#include "logger.h"
#include "Settings.h"

void OnMessage(SKSE::MessagingInterface::Message* message) {
    switch (message->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            Settings::LoadSettings();
            break;
        case SKSE::MessagingInterface::kPostLoad:
            Main::Initialize();
            break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SetupLog();
    logger::info("Plugin loaded");
    SKSE::Init(skse);
    MCP::Register();
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    return true;
}