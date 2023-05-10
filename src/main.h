#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <extensions_2.h>
#include <MinHook.h>
#include <chrono>



// GLOBALS AND FUNCTION DEFINITIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const auto mod = (uint32_t)GetModuleHandleA("GeometryDash.exe");

int frame = 0;
uint64_t prevtime = 0;
bool countFrames = true;
bool isRecording = false;
bool isClicking = 0;
bool prevState = 0;

inline void setAttemptsZero(MegaHackExt::Button* obj);

inline bool(__thiscall* playerDestroyed)(void* a, void* b);
inline void(__thiscall* update)(void* self, void* dt);
inline void(__thiscall* restartLevel)(void* a);



// MAIN FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



inline bool WPM(bool state, DWORD addr, std::vector<BYTE> bytes) {

	WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(mod + addr), bytes.data(), bytes.size(), nullptr);

	return !state;
}

namespace Attempts {

	inline int* totalAttemptsPointer() {

		const auto a = *reinterpret_cast<int*>(mod + 0x3222D0);
		const auto b = *reinterpret_cast<int*>(a + 0x164);
		const auto c = *reinterpret_cast<int*>(b + 0x22C);
		const auto d = *reinterpret_cast<int*>(c + 0x114);

		const auto total_attempts = reinterpret_cast<int*>(d + 0x210);

		return total_attempts;
	}

	inline int totalAttempts() {

		return *totalAttemptsPointer();
	}

	inline int currentAttempts() {

		const auto a = *reinterpret_cast<int*>(mod + 0x3222D0);
		const auto b = *reinterpret_cast<int*>(a + 0x164);

		const auto current_attempts = *reinterpret_cast<int*>(b + 0x4a8);

		return current_attempts;
	}

	inline void MH_CALL fixAttempts(MegaHackExt::Button* obj) {

		int* attemptPtr = totalAttemptsPointer();

		*attemptPtr = static_cast<int>(0);
	}
}

inline void sendMouseInput(bool down, bool button)
{
	POINT pos{}; GetCursorPos(&pos);

	down ? (button ? SendMessage(GetForegroundWindow(), WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pos.x, pos.y)) :
		SendMessage(GetForegroundWindow(), WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(pos.x, pos.y))) :
		(button ? SendMessage(GetForegroundWindow(), WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(pos.x, pos.y)) :
			SendMessage(GetForegroundWindow(), WM_RBUTTONUP, MK_RBUTTON, MAKELPARAM(pos.x, pos.y)));
}

inline void startConsole() {
	BOOL attached;
	attached = AllocConsole() && SetConsoleTitleW(L":3");
	freopen_s(reinterpret_cast<FILE**>stdin, "CONIN$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>stdout, "CONOUT$", "w", stdout);

	printf("Recording Started\n");
}

inline void unloadConsole() {

	printf("Recording Stopped. You can now close this window.\n");
	auto console_window = GetConsoleWindow();
	FreeConsole();
	PostMessageA(console_window, WM_QUIT, 0, 0);
}



// HOOKS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void __fastcall updateHook(void* self, void*, void* dt) {

	if (countFrames) {

		uint64_t now = duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		uint64_t delta = now - prevtime;

		frame++;

		if (isRecording) {

			if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0) { isClicking = true; }
			else { isClicking = false; }

			if (prevState != isClicking) { std::cout << "[" << frame << ", " << isClicking << "]," << std::endl; }

			prevState = isClicking;
		}

		//std::cout << "Frame:  " << frame << "      Delta:  " << delta << std::endl;

		prevtime = now;
	}

	update(self, dt);
}

void __fastcall deathHook(void* a, void* b) {

	countFrames = false;

	printf("----- DEATH ON FRAME %i -----\n", frame);
	playerDestroyed(a, b);
}

void __fastcall restartLevelHook(void* a, void*) {

	frame = 0;
	countFrames = true;

	restartLevel(a);
}



// HACK AND OFFSET STRUCTS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



struct Offsets {

	uint32_t frameUpdate = (mod + 0x2029C0);
	uint32_t onDeath = (mod + 0x1EFAA0);
	uint32_t restartLevel = (mod + 0x20BF00);
};

struct NoClip {
	std::vector<BYTE> on = { 0xE9, 0x79, 0x06, 0x00, 0x00 };
	std::vector<BYTE> off = { 0x6A, 0x14, 0x8B, 0xCB, 0xFF };
	uint32_t addr = 0x20A23C;
};

struct FreezePlayer {
	std::vector<BYTE> on = { 0x90, 0x90, 0x90 };
	std::vector<BYTE> off = { 0x50, 0xFF, 0xD6 };
	uint32_t addr = 0x203519;
};

struct JumpHack {
	std::vector<BYTE> on = { 0x01 };
	std::vector<BYTE> off = { 0x00 };
	uint32_t addr1 = 0x1E9141;
	uint32_t addr2 = 0x1E9498;
};

struct InstantComplete {
	std::vector<BYTE> on = { 0xC7, 0x81, 0x7C, 0x06, 0x00, 0x00, 0x20, 0xBC, 0xBE, 0x4C, 0x90, 0x90, 0x90, 0x90, 0x90 };
	std::vector<BYTE> off = { 0x8B, 0x35, 0xE0, 0x23, 0xB3, 0x00, 0x81, 0xC1, 0x7C, 0x06, 0x00, 0x00, 0x50, 0xFF, 0xD6 };
	uint32_t addr = 0x20350D;
};



// STRUCT OBJECT INITIALIZERS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



const NoClip noclip;
const FreezePlayer playerFreeze;
const JumpHack jumpHack;
const InstantComplete instantComplete;
const Offsets offsets;



// MEGA HACK STUFF
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



inline void initWindow() {

	MegaHackExt::Window* window = MegaHackExt::Window::Create("SpaceUK Sim");

	MegaHackExt::CheckBox* noclip_checkbox = MegaHackExt::CheckBox::Create("Aqui es de noclip");
	noclip_checkbox->setCallback([](MegaHackExt::CheckBox* obj, bool noclipState = false) {

		if (noclipState) {
			noclipState = WPM(noclipState, noclip.addr, noclip.on);
		}
		else {
			noclipState = WPM(noclipState, noclip.addr, noclip.off);
		}
		});
	window->add(noclip_checkbox);

	MegaHackExt::CheckBox* jump_checkbox = MegaHackExt::CheckBox::Create("Le jump hack");
	jump_checkbox->setCallback([](MegaHackExt::CheckBox* obj, bool jumpState = false) {

		if (jumpState) {
			jumpState = WPM(jumpState, jumpHack.addr1, jumpHack.on);
			jumpState = WPM(jumpState, jumpHack.addr2, jumpHack.on);
		}
		else {
			jumpState = WPM(jumpState, jumpHack.addr1, jumpHack.off);
			jumpState = WPM(jumpState, jumpHack.addr2, jumpHack.off);
		}
		});
	window->add(jump_checkbox);

	MegaHackExt::CheckBox* freeze_checkbox = MegaHackExt::CheckBox::Create("stop moving");
	freeze_checkbox->setCallback([](MegaHackExt::CheckBox* obj, bool playerFrozen = false) {

		if (playerFrozen) {
			playerFrozen = WPM(playerFrozen, playerFreeze.addr, playerFreeze.on);
		}
		else {
			playerFrozen = WPM(playerFrozen, playerFreeze.addr, playerFreeze.off);
		}
		});
	window->add(freeze_checkbox);

	MegaHackExt::CheckBox* instantComplete_checkbox = MegaHackExt::CheckBox::Create("insta Beat");
	instantComplete_checkbox->setCallback([](MegaHackExt::CheckBox* obj, bool instantCompleteState = false) {

		if (instantCompleteState) {
			instantCompleteState = WPM(instantCompleteState, instantComplete.addr, instantComplete.on);
		}
		else {
			instantCompleteState = WPM(instantCompleteState, instantComplete.addr, instantComplete.off);
		}
		});
	window->add(instantComplete_checkbox);

	MegaHackExt::Button* attempts_zero_button = MegaHackExt::Button::Create("Fix attempt Count");
	attempts_zero_button->setCallback(Attempts::fixAttempts);

	window->add(attempts_zero_button);

	MegaHackExt::CheckBox* record_checkbox = MegaHackExt::CheckBox::Create("record macro");
	record_checkbox->setCallback([](MegaHackExt::CheckBox* obj, bool a) {

		isRecording = !isRecording;

		if (isRecording) { startConsole(); }
		else { unloadConsole(); }

		});
	window->add(record_checkbox);

	MegaHackExt::Client::commit(window);
}