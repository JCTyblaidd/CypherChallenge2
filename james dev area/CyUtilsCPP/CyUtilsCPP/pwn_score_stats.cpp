#include "pwn_cy.hpp"
#include "MainFramework.hpp"

namespace ngram_stats {
	float scoreMonogram(std::string str) {
		float SCORE = 0;
		int32_t maxIndex = str.length();
		for (int32_t index = 0; index < maxIndex; index++) {
			char c = str[index];
			int32_t VAL_T = c - 'A';//Limit A -> Z; 0 -> 25
			SCORE += monograms[VAL_T];
		}
		return SCORE;
	}
	float scoreBigram(std::string str) {
		float SCORE = 0;
		int32_t maxIndex = str.length() - 1;
		for (int32_t index = 0; index < maxIndex; index++) {
			char c1 = str[index];
			char c2 = str[index + 1];
			int32_t VAL1 = c1 - 'A';
			int32_t VAL2 = c2 - 'A';
			int32_t VAL_T = (VAL1*26) + VAL2;
			SCORE += bigrams[VAL_T];
		}
		return SCORE;
	}
	float scoreTrigram(std::string str) {
		float SCORE = 0;
		int32_t maxIndex = str.length() - 2;
		for (int32_t index = 0; index < maxIndex; index++) {
			char c1 = str[index];
			char c2 = str[index + 1];
			char c3 = str[index + 2];
			int32_t VAL1 = c1 - 'A';
			int32_t VAL2 = c2 - 'A';
			int32_t VAL3 = c3 - 'A';
			int32_t VAL_T = (VAL1 * 26 * 26) + (VAL2 * 26) + VAL3;
			SCORE += trigrams[VAL_T];
		}
		return SCORE;
	}
	float scoreQuadgram(std::string str) {
		float SCORE = 0;
		int32_t maxIndex = str.length() - 3;
		for (int32_t index = 0; index < maxIndex; index++) {
			char c1 = str[index];
			char c2 = str[index + 1];
			char c3 = str[index + 2];
			char c4 = str[index + 3];
			int32_t VAL1 = c1 - 'A';
			int32_t VAL2 = c2 - 'A';
			int32_t VAL3 = c3 - 'A';
			int32_t VAL4 = c4 - 'A';
			int32_t VAL_T = (VAL1 * 26 * 26 * 26) + (VAL2 * 26 * 26) + (VAL3 * 26) + VAL4;
			SCORE += quadgrams[VAL_T];
		}
		return SCORE;
	}
}