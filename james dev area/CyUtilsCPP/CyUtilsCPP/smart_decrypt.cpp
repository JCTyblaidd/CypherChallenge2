#include "MainFramework.hpp"
#include "cy_header.h"
#include <ctype.h>


char shift(char input, int shift) {
	constexpr char MAX = 'Z';
	char TEMP = toupper(input);
	TEMP += shift;
	if (TEMP > MAX) {
		TEMP -= 26;
	}
	return TEMP;
}



void smartCeasar(std::string str) {
	std::string solutions[26];
	int len = str.size();
	
}

void smartAffine(std::string str) {

}

void smartKeyword(std::string str) {

}

void smartDecrypt(std::string str) {
	smartCeasar(str);
	smartAffine(str);
	smartKeyword(str);
}