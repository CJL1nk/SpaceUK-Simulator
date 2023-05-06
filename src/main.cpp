#include "main.h"

void unload(LPVOID instance) {
    auto console_window = GetConsoleWindow();
    FreeConsole();
    PostMessageA(console_window, WM_QUIT, 0, 0);
    FreeLibraryAndExitThread((HMODULE)instance, 0);
}

void main_thread(void* instance) {

    BOOL noclip_state = false;
    BOOL pressing_noclip = false;

    BOOL player_frozen = false;
    BOOL pressing_freeze = false;

    BOOL jump_state = false;
    BOOL pressing_jump = false;

    // setup console
    BOOL attached;
    attached = AllocConsole() && SetConsoleTitleW(L"CJ_Link's Epic Awesome Cheat Menu");
    freopen_s(reinterpret_cast<FILE**>stdin, "CONIN$", "r", stdin);
    freopen_s(reinterpret_cast<FILE**>stdout, "CONOUT$", "w", stdout);

    if (!attached) {
        unload(instance);
    }

    menu();

    while (!GetAsyncKeyState(VK_END)) {

        if (GetAsyncKeyState(0x31) != pressing_noclip) {
            if (!pressing_noclip) {

                if (!noclip_state) {

                    noclip_state = WPM(noclip_state, noclip.addr, noclip.on, noclip.byte_size);
                    std::cout << " Noclip turned on." << std::endl << std::endl;
                }
                else {
                    noclip_state = WPM(noclip_state, noclip.addr, noclip.off, noclip.byte_size);
                    std::cout << " Noclip turned off." << std::endl << std::endl;
                }
                pressing_noclip = true;
            }
            else {
                pressing_noclip = false;
            }
        }

        if (GetAsyncKeyState(0x32) != pressing_freeze) {
            if (!pressing_freeze) {

                if (!player_frozen) {

                    player_frozen = WPM(player_frozen, playerFreeze.addr, playerFreeze.on, playerFreeze.byte_size);
                    std::cout << " Player frozen." << std::endl << std::endl;
                }
                else {
                    player_frozen = WPM(player_frozen, playerFreeze.addr, playerFreeze.off, playerFreeze.byte_size);
                    std::cout << " Player unfrozen." << std::endl << std::endl;
                }
                pressing_freeze = true;
            }
            else {
                pressing_freeze = false;
            }
        }

        if (GetAsyncKeyState(0x33) != pressing_jump) {
            if (!pressing_jump) {

                if (!jump_state) {

                    jump_state = WPM(jump_state, jumpHack.addr1, jumpHack.on, jumpHack.byte_size);
                    WPM(jump_state, jumpHack.addr2, jumpHack.on, jumpHack.byte_size);
                    std::cout << " Jump hack enabled." << std::endl << std::endl;
                }
                else {
                    jump_state = WPM(jump_state, jumpHack.addr1, jumpHack.off, jumpHack.byte_size);
                    WPM(jump_state, jumpHack.addr2, jumpHack.off, jumpHack.byte_size);
                    std::cout << " Jump hack disabled." << std::endl << std::endl;
                }
                pressing_jump = true;
            }
            else {
                pressing_jump = false;
            }
        }
        Sleep(200);
    }

    unload(instance);
}

BOOL APIENTRY DllMain(LPVOID instance, DWORD reason_for_call, LPVOID reservation) {

	if (reason_for_call != DLL_PROCESS_ATTACH) { return false; }

    CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)main_thread, instance, 0, nullptr));

    return true;
} 

