#pragma once
#include "MainFramework.hpp"
#include "cy_header.h"


uint calcELim(const std::string& str) {
	uint COUNT = 0;
	for (char c : str) {
		if (c == 'E') {
			COUNT++;
		}
	}
	return COUNT;
}

void runCeasar(std::string str) {
	const char* Pntr = str.c_str();
	const uint Len = str.size();
	char* Result = new char[Len];
	String Data[26];
	for (uint Offset = 0; Offset < 26; Offset++) {
		for (uint Index = 0; Index < Len; Index++) {
			char v = Pntr[Index];
			v = v + Offset;
			if (v > 'Z') {
				v -= 26;
			}
			Result[Index] = v;
		}
		Data[Offset] = String(Result);
	}
	//Choose Best Value//
	uint BestIndex = 0;
	uint BestValue = 0;
	for (uint Offset = 0; Offset < 26; Offset++) {
		uint VAL = calcELim(Data[Offset]);
		if (VAL > BestValue) {
			BestValue = VAL;
			BestIndex = Offset;
		}
	}
	INFO("Best Guess:");
	WARN(Data[BestIndex] + String("\n\n"));
	INFO("All Guesses: ");
	for (uint Offset = 0; Offset < 26; Offset++) {
		INFO(Data[Offset]);
	}
}