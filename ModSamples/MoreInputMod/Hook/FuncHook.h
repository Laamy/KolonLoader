#ifndef FUNC_HOOK_H
#define FUNC_HOOK_H

class FuncHook {
public:
    virtual bool Initialize() = 0;
};

namespace HookComps {
    void* __o__Options;
    void* __o__Keypress;
    void* __o__Zoom;
}

// include hooks here
#include "Hooks/KeymapHook.h"
#include "Hooks/ZoomHook.h"
#include "Hooks/BaseOptionsHook.h"

void InitHooks() {
    static FuncHook* hooks[] = {
        &KeymapHook::Get(),
        &BaseOptionsHook::Get(),
        &ZoomHook::Get()
    };

    for (std::size_t i = 0; i < std::size(hooks); ++i)
    {
        if (not hooks[i]->Initialize())
        {
            //error handling
        }
    }
}

#endif // !FUNC_HOOK_H
