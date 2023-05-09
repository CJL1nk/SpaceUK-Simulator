#pragma once

#include "funcs.h"

struct NoClip {
	std::vector<BYTE> on = { 0xE9, 0x79, 0x06, 0x00, 0x00 };
	std::vector<BYTE> off = { 0x6A, 0x14, 0x8B, 0xCB, 0xFF };
	DWORD addr = 0x20A23C;
};

struct FreezePlayer {
	std::vector<BYTE> on = { 0x90, 0x90, 0x90 };
	std::vector<BYTE> off = { 0x50, 0xFF, 0xD6 };
	DWORD addr = 0x203519;
};

struct JumpHack {
	std::vector<BYTE> on = { 0x01 };
	std::vector<BYTE> off = { 0x00 };
	DWORD addr1 = 0x1E9141;
	DWORD addr2 = 0x1E9498;
};

const NoClip noclip;
const FreezePlayer playerFreeze;
const JumpHack jumpHack;