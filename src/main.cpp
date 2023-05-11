#include "main.h"

void main_thread(void* instance) {

    //startConsole();

    MH_Initialize();
    MH_CreateHook(reinterpret_cast<void*>(offsets.frameUpdate), updateHook, reinterpret_cast<void**>(&update));

    MH_CreateHook(reinterpret_cast<void*>(offsets.onDeath), deathHook, reinterpret_cast<void**>(&playerDestroyed));

    MH_CreateHook(reinterpret_cast<void*>(offsets.restartLevel), restartLevelHook, reinterpret_cast<void**>(&restartLevel));

    MH_CreateHook(reinterpret_cast<void*>(offsets.initLevel), initLevelHook, reinterpret_cast<void**>(&initLevel));

    MH_CreateHook(reinterpret_cast<void*>(offsets.onLevelComplete), onLevelCompleteHook, reinterpret_cast<void**>(&onLevelComplete));
    MH_EnableHook(MH_ALL_HOOKS);

    if (!fs::exists("SpaceUK Macros")) { fs::create_directory("SpaceUK Macros"); }

    initWindow();
}

BOOL APIENTRY DllMain(void* instance, DWORD reason_for_call, void* reservation) {

    if (reason_for_call != DLL_PROCESS_ATTACH) { return false; }

    CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)main_thread, instance, 0, nullptr));

    return true;
}

