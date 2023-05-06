#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>

const auto mod = (uint32_t)GetModuleHandleA("GeometryDash.exe");

inline void menu() {

	int menuInput;

	std::cout << " [1] Noclip" << std::endl;
	std::cout << " [2] Freeze Player" << std::endl;
	std::cout << " [3] Jump Hack" << std::endl;

	std::cout << std::endl;
}

inline bool WPM(bool state, DWORD addr, std::string bytes, size_t size) {

	WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(mod + addr), bytes.c_str(), size, NULL);

	return !state;
}