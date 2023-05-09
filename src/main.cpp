#include "main.h"

void main_thread(void* instance) {

    MegaHackExt::Window* window = MegaHackExt::Window::Create("SpaceUK Sim");

    MegaHackExt::CheckBox* noclip_checkbox = MegaHackExt::CheckBox::Create("Aqui es de noclip");
    noclip_checkbox->setCallback([](MegaHackExt::CheckBox* obj, bool noclip_state = false) {
        
        if (noclip_state) {
            noclip_state = WPM(noclip_state, noclip.addr, noclip.on);
        }
        else {
            noclip_state = WPM(noclip_state, noclip.addr, noclip.off);
        }
    });
    window->add(noclip_checkbox);

    MegaHackExt::CheckBox* jump_checkbox = MegaHackExt::CheckBox::Create("Le jump hack");
    jump_checkbox->setCallback([](MegaHackExt::CheckBox* obj, bool jump_state = false) {

        if (jump_state) {
            jump_state = WPM(jump_state, jumpHack.addr1, jumpHack.on);
            jump_state = WPM(jump_state, jumpHack.addr2, jumpHack.on);
        }
        else {
            jump_state = WPM(jump_state, jumpHack.addr1, jumpHack.off);
            jump_state = WPM(jump_state, jumpHack.addr2, jumpHack.off);
        }
    });
    window->add(jump_checkbox);

    MegaHackExt::CheckBox* freeze_checkbox = MegaHackExt::CheckBox::Create("stop moving");
    freeze_checkbox->setCallback([](MegaHackExt::CheckBox* obj, bool player_frozen = false) {

        if (player_frozen) {
            player_frozen = WPM(player_frozen, playerFreeze.addr, playerFreeze.on);
        }
        else {
            player_frozen = WPM(player_frozen, playerFreeze.addr, playerFreeze.off);
        }
    });
    window->add(freeze_checkbox);

    MegaHackExt::Client::commit(window);
}

BOOL APIENTRY DllMain(LPVOID instance, DWORD reason_for_call, LPVOID reservation) {

	if (reason_for_call != DLL_PROCESS_ATTACH) { return false; }

    CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)main_thread, instance, 0, nullptr));

    return true;
} 

