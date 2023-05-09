#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "extensions_2.h"

const auto mod = (uint32_t)GetModuleHandleA("GeometryDash.exe");

inline bool WPM(bool state, DWORD addr, std::vector<BYTE> bytes) {

	WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(mod + addr), bytes.data(), bytes.size(), nullptr);

	return !state;
}