#pragma once
#include "MainFramework.hpp"
#include "cy_header.h"


int32_t guessVignereLength(std::string str) {
	int32_t result = 1;




	INFO(std::string("key Length = ") += std::to_string(result));
	return 0;
}

int32_t guessCeasarInteleaved(std::string str,int32_t interleave) {
	return 0;
}

std::string decryptVignere(std::string str, int32_t repeat, int32_t* shifts) {
	return "";
}


void runVignere(std::string str) {
	int32_t repeat = guessVignereLength(str);
	int32_t* vals = new int32_t[repeat];
	for (int i = 0; i < repeat; i++) {
		vals[i] = guessCeasarInteleaved(str, i);
	}
	std::string RESULT = decryptVignere(str, repeat, vals);
	delete vals;
	INFO(RESULT);
}