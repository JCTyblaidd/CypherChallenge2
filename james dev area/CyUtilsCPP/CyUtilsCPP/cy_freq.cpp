#pragma once
#include "MainFramework.hpp"
#include "cy_header.h"


void runFreqAnalysis(std::string str) {
	uint COUNTS[26];
	memset(COUNTS, 0, 26 * sizeof(uint));
	uint TOTAL = 0;
	for (char c : str) {
		COUNTS[c - 'A']++;
		TOTAL++;
	}
	float PERCS[26];
	for (uint Val = 0; Val < 26; Val++) {
		PERCS[Val] = float(COUNTS[Val]) / float(TOTAL);
	}
	for (uint Val = 0; Val < 26; Val++) {
		String Txt;
		Txt += (Val + 'A');
		Txt += " => ";
		Txt += std::to_string(COUNTS[Val]);
		Txt += " ==> ";
		Txt += std::to_string(PERCS[Val]);
		Txt += "% ";
		INFO(Txt);
	}
}