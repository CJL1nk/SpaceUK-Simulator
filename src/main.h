#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <extensions_2.h>
#include <MinHook.h>
#include <chrono>
#include <fstream>
#include <sstream>
#include <filesystem>



// GLOBALS AND FUNCTION DEFINITIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



const auto mod = (uint32_t)GetModuleHandleA("GeometryDash.exe");

int frame = 0;
uint64_t prevtime = 0;
bool countFrames = true;
bool isRecording = false;
bool isPlaying = false;
bool isClicking = false;
bool prevState = false;

int macroIndex = 0;

inline void setAttemptsZero(MegaHackExt::Button* obj);

inline bool(__thiscall* playerDestroyed)(void* a, void* b);
inline void(__thiscall* update)(void* self, void* dt);
inline void(__thiscall* restartLevel)(void* a);
inline bool(__thiscall* initLevel)(void* a, void* c);
inline void(__thiscall* onLevelComplete)(void* a);

inline void loadMacro(unsigned int levelID);
void sendMouseInput(bool down, bool button);

inline void saveMacro();

namespace fs = std::filesystem;


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

inline void startConsole() {
	BOOL attached;
	attached = AllocConsole() && SetConsoleTitleW(L":3");
	freopen_s(reinterpret_cast<FILE**>stdin, "CONIN$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>stdout, "CONOUT$", "w", stdout);
}

inline void unloadConsole() {
	system("pause\n");
	auto console_window = GetConsoleWindow();
	FreeConsole();
	PostMessageA(console_window, WM_QUIT, 0, 0);
}

inline unsigned int getLevelID() {

	const auto a = *reinterpret_cast<uint32_t*>(mod + 0x3222D0);
	const auto b = *reinterpret_cast<uint32_t*>(a + 0x2A0);

	//if (!b) { printf("LevelID not found\n"); }
	return b;
}



// HOOKS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void __fastcall deathHook(void* a, void* b) {

	countFrames = false;

	if (isPlaying) {
		sendMouseInput(false, true);
	}

	if (isRecording) {
		sendMouseInput(false, true);
		saveMacro();
	}

	//printf("----- DEATH ON FRAME %i -----\n", frame);
	playerDestroyed(a, b);
}

void __fastcall restartLevelHook(void* a, void*) {

	frame = 0;
	macroIndex = 0;

	countFrames = true;

	restartLevel(a);
}

void __fastcall initLevelHook(void* a, void* b, void* c) {

	loadMacro(getLevelID());
	//printf("Level Loaded\n");

	initLevel(a, c);
}

void __fastcall onLevelCompleteHook(void* a) {

	saveMacro();

	onLevelComplete(a);
}



// HACK AND OFFSET STRUCTS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



struct Input {
	int frame;
	bool click;
};

struct Offsets {
	uint32_t frameUpdate = (mod + 0x2029C0);
	uint32_t onDeath = (mod + 0x1EFAA0);
	uint32_t restartLevel = (mod + 0x20BF00);
	uint32_t initLevel = (mod + 0x01FB780);
	uint32_t onLevelComplete = (mod + 0x1FD3D0);
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

std::vector<Input> inputs;



// MACRO STUFF
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void sendMouseInput(bool down, bool button) {
	POINT pos{}; GetCursorPos(&pos);

	down ? (button ? SendMessage(GetForegroundWindow(), WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pos.x, pos.y)) :
		SendMessage(GetForegroundWindow(), WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(pos.x, pos.y))) :
		(button ? SendMessage(GetForegroundWindow(), WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(pos.x, pos.y)) :
			SendMessage(GetForegroundWindow(), WM_RBUTTONUP, MK_RBUTTON, MAKELPARAM(pos.x, pos.y)));
}

inline void saveMacro() {

	std::ofstream outputFile("SpaceUK Macros\\" + std::to_string(getLevelID()) + ".space");

	if (outputFile.is_open()) {
		for (const auto& input : inputs) {
			outputFile << input.frame << ";" << input.click << "\n";
		}
		outputFile.close();
		//std::cout << "Save successful." << std::endl;
	}

	inputs.clear();
}

inline void loadMacro(unsigned int levelID) {

	inputs.clear();

	std::ifstream file("SpaceUK Macros\\" + std::to_string(levelID) + ".space");

	if (!file) { printf("No file with level ID found\n"); }
	else {

		std::string line;

		while (std::getline(file, line)) {

			Input input;
			std::stringstream line_ss(line);
			std::string token;

			std::getline(line_ss, token, ';');
			input.frame = std::stoi(token);

			std::getline(line_ss, token, ';');
			input.click = (std::stoi(token) != 0);

			inputs.push_back(input);
		}

		file.close();
		//printf("Macro Loaded\n");
	}
}

void __fastcall updateHook(void* self, void*, void* dt) {

	if (countFrames) {

		uint64_t now = duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		uint64_t delta = now - prevtime;

		frame++;

		if (isRecording) {

			if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0) { isClicking = true; }
			else { isClicking = false; }

			if (prevState != isClicking) { inputs.push_back({ frame, isClicking }); }

			prevState = isClicking;
		}

		if (isPlaying) {
			if (frame == inputs[macroIndex].frame) {
				//std::cout << "Sending mouse input on frame " << frame << std::endl;
				sendMouseInput(inputs[macroIndex].click, true);
				macroIndex++;
			}
		}

		//std::cout << "Frame:  " << frame << "      Delta:  " << delta << std::endl;

		prevtime = now;
	}

	update(self, dt);
}

void MH_CALL saveMacroMH(MegaHackExt::Button* obj) {
	saveMacro();
}



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

	MegaHackExt::CheckBox* record_checkbox = MegaHackExt::CheckBox::Create("record");
	record_checkbox->setCallback([](MegaHackExt::CheckBox* obj, bool a) {

		isRecording = !isRecording;

		if (isRecording) {
			inputs.clear();
			//printf("Recording Started\n");
		}
		//else { printf("Recording Stopped\n"); }

		});

	MegaHackExt::Button* save_button = MegaHackExt::Button::Create("save Macro");
	save_button->setCallback(saveMacroMH);

	window->add(MegaHackExt::HorizontalLayout::Create(record_checkbox, save_button));

	MegaHackExt::CheckBox* replay_checkbox = MegaHackExt::CheckBox::Create("Replay macro");
	replay_checkbox->setCallback([](MegaHackExt::CheckBox* obj, bool a) {

		isPlaying = !isPlaying;
		});

	window->add(replay_checkbox);

	MegaHackExt::Client::commit(window);
}