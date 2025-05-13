#pragma once

#include "Settings.h"

namespace MCP {
    void Register();
    namespace General {
        void __stdcall Render();
    }
    namespace Advanced {
        void __stdcall Render();
    }
    namespace Maintenance {
        void __stdcall Render();
    }
};