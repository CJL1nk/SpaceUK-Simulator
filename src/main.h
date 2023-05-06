#pragma once

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include "funcs.h"

struct NoClip {

	std::string on = "\xE9\x79\x06\x00\x00";
	std::string off = "\x6A\x14\x8B\xCB\xFF";
	size_t byte_size = 5;
	DWORD addr = 0x20A23C;
};

struct FreezePlayer {

	std::string on = "\x90\x90\x90";
	std::string off = "\x50\xFF\xD6";
	size_t byte_size = 3;
	DWORD addr = 0x203519;
};

struct JumpHack {
	std::string on = "\x01";
	std::string off = "\x00";
	size_t byte_size = 1;
	DWORD addr1 = 0x1E9141;
	DWORD addr2 = 0x1E9498;
};

const NoClip noclip;
const FreezePlayer playerFreeze;
const JumpHack jumpHack;