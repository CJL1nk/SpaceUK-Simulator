#include "main.h"

void main_thread(void* instance) {

    MH_Initialize();
    MH_CreateHook(reinterpret_cast<void*>(offsets.frameUpdate), updateHook, reinterpret_cast<void**>(&update));

    MH_CreateHook(reinterpret_cast<void*>(offsets.onDeath), deathHook, reinterpret_cast<void**>(&playerDestroyed));

    MH_CreateHook(reinterpret_cast<void*>(offsets.restartLevel), restartLevelHook, reinterpret_cast<void**>(&restartLevel));
    MH_EnableHook(MH_ALL_HOOKS);

    initWindow();
}

BOOL APIENTRY DllMain(void* instance, DWORD reason_for_call, void* reservation) {

    if (reason_for_call != DLL_PROCESS_ATTACH) { return false; }

    CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)main_thread, instance, 0, nullptr));

    return true;
}

