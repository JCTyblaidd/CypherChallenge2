#pragma once
#include "MainFramework.hpp"
#include "cy_header.h"

void runCeasar(std::string str) {
	const char* Pntr = str.c_str();
	const uint Len = str.size();
	char* Result = new char[Len];
	for (uint Offset = 0; Offset < 26; Offset++) {
		for (uint Index = 0; Index < Len; Index++) {
			char v = Pntr[Index];
			v = v + Offset;
			if (v > 'Z') {
				v -= 26;
			}
			Result[Index] = v;
		}
		INFO(String(Result));
	}
}