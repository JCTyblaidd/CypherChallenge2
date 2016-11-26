#include "pwn_cy.hpp"
#include <cmath>
#include "MainFramework.hpp"
#include <fstream>
#include <string>

#define GRAMS_COMPRESSED_FORMAT 1
#define GRAMS_EXPORT_COMPRESSED 0

namespace ngram_stats{
	float monograms[26];
	float bigrams[26*26];
	float trigrams[26*26*26];
	float quadgrams[26*26*26*26];

	void loadStats() {
		try {
#if !GRAMS_COMPRESSED_FORMAT
			const float NON_VAL = std::log10f(0.01F);
			{
				constexpr int32_t LIMIT = 26;
				for (int32_t i = 0; i < LIMIT; i++) {
					monograms[i] = NON_VAL / LIMIT;
				}
				std::ifstream file_ngram(getFileLoc("stat_data/monogram_vals.txt"));
				std::string str;
				while (std::getline(file_ngram, str))
				{
					int index = str.find(' ');
					std::string prefix = str.substr(0, index);
					std::string postfix = str.substr(index+1);
					int32_t INDEX = prefix[0] - 'A';
					int64_t FREQ = std::stol(postfix);
					float VALUE = (float)std::log10(FREQ);
					VALUE /= LIMIT;
					monograms[INDEX] = VALUE;
				}
			}

			{
				constexpr int32_t LIMIT = 26 * 26;
				for (int32_t i = 0; i < LIMIT; i++) {
					bigrams[i] = NON_VAL / LIMIT;
				}
				std::ifstream file_ngram(getFileLoc("stat_data/bigram_vals.txt"));
				std::string str;
				while (std::getline(file_ngram, str))
				{
					int index = str.find(' ');
					std::string prefix = str.substr(0, index);
					std::string postfix = str.substr(index + 1);
					int32_t INDEX = (prefix[0] - 'A') * 26 + (prefix[1] - 'A');
					int64_t FREQ = std::stol(postfix);
					float VALUE = (float)std::log10(FREQ);
					VALUE /= LIMIT;
					bigrams[INDEX] = VALUE;
				}
			}

			{
				constexpr int32_t LIMIT = 26 * 26 * 26;
				for (int32_t i = 0; i < LIMIT; i++) {
					trigrams[i] = NON_VAL / LIMIT;
				}
				std::ifstream file_ngram(getFileLoc("stat_data/trigram_vals.txt"));
				std::string str;
				while (std::getline(file_ngram, str))
				{
					int index = str.find(' ');
					std::string prefix = str.substr(0, index);
					std::string postfix = str.substr(index + 1);
					int32_t INDEX = (prefix[0] - 'A') * (26 * 26) + (prefix[1] - 'A') * 26 + (prefix[2] - 'A');
					int64_t FREQ = std::stol(postfix);
					float VALUE = (float)std::log10(FREQ);
					VALUE /= LIMIT;
					trigrams[INDEX] = VALUE;
				}
			}

			{
				constexpr int32_t LIMIT = 26 * 26 * 26 * 26;
				for (int32_t i = 0; i < LIMIT; i++) {
					quadgrams[i] = NON_VAL / LIMIT;
				}
				std::ifstream file_ngram(getFileLoc("stat_data/quadgram_vals.txt"));
				std::string str;
				while (std::getline(file_ngram, str))
				{
					int index = str.find(' ');
					std::string prefix = str.substr(0, index);
					std::string postfix = str.substr(index + 1);
					int32_t INDEX = (prefix[0] - 'A') * (26 * 26 * 26) + (prefix[1] - 'A') * (26 * 26) + (prefix[2] - 'A') * 26 + (prefix[3] - 'A');
					int64_t FREQ = std::stol(postfix);
					float VALUE = (float)std::log10(FREQ);
					VALUE /= LIMIT;
					quadgrams[INDEX] = VALUE;
				}
			}
#else
			{
				constexpr int32_t LIMIT = 26;
				std::ifstream load_ngram(getFileLoc("stat_data/monogram_vals.dat"), std::ios::binary);
				load_ngram.read(reinterpret_cast<char*>(monograms), sizeof(float) * LIMIT);
			}
			{
				constexpr int32_t LIMIT = 26 * 26;
				std::ifstream load_ngram(getFileLoc("stat_data/bigram_vals.dat"), std::ios::binary);
				load_ngram.read(reinterpret_cast<char*>(bigrams), sizeof(float) * LIMIT);
			}
			{
				constexpr int32_t LIMIT = 26 * 26 * 26;
				std::ifstream load_ngram(getFileLoc("stat_data/trigram_vals.dat"), std::ios::binary);
				load_ngram.read(reinterpret_cast<char*>(trigrams), sizeof(float) * LIMIT);
			}
			{
				constexpr int32_t LIMIT = 26 * 26 * 26 * 26;
				std::ifstream load_ngram(getFileLoc("stat_data/quadgram_vals.dat"), std::ios::binary);
				load_ngram.read(reinterpret_cast<char*>(quadgrams), sizeof(float) * LIMIT);
			}
#endif
#if GRAMS_EXPORT_COMPRESSED && !GRAMS_COMPRESSED_FORMAT
			{
				std::ofstream store_ngram(getFileLoc("stat_data/monogram_vals.dat"),std::ios::binary);
				constexpr int32_t LIMIT = 26;
				store_ngram.write(reinterpret_cast<char*>(monograms),sizeof(float) * LIMIT);
			}
			{
				std::ofstream store_ngram(getFileLoc("stat_data/bigram_vals.dat"),std::ios::binary);
				constexpr int32_t LIMIT = 26 * 26;
				store_ngram.write(reinterpret_cast<char*>(bigrams), sizeof(float) * LIMIT);
			}
			{
				std::ofstream store_ngram(getFileLoc("stat_data/trigram_vals.dat"),std::ios::binary);
				constexpr int32_t LIMIT = 26 * 26 * 26;
				store_ngram.write(reinterpret_cast<char*>(trigrams), sizeof(float) * LIMIT);
			}
			{
				std::ofstream store_ngram(getFileLoc("stat_data/quadgram_vals.dat"),std::ios::binary);
				constexpr int32_t LIMIT = 26 * 26 * 26 * 26;
				store_ngram.write(reinterpret_cast<char*>(quadgrams), sizeof(float) * LIMIT);
			}
#endif
		}
		catch (std::exception ex) {
			SEVERE("Error Loading Stats");
			system("pause");
			delete WorkPool;
			delete LogHandle;
			exit(-1);
		}
	}

}