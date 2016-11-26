#pragma once
#include <string>
#include <vector>

//EVERYTHING ASSUMES NO SPACES + ALL CAPITAL LETTERS
namespace ngram_stats {
	extern float monograms[26];
	extern float bigrams[26 * 26];
	extern float trigrams[26 * 26 * 26];
	extern float quadgrams[26 * 26 * 26 * 26];

	void loadStats();

	float scoreMonogram(std::string str);
	float scoreBigram(std::string str);
	float scoreTrigram(std::string str);
	float scoreQuadgram(std::string str);
};
namespace pwn_utils {
	struct key_iterator {
	private:
		int64_t index;
		int32_t size;
		int64_t maxIndex;
	public:
		key_iterator(int32_t size);
		bool nextKey(char* key);
		void reset();
	};

	struct order_iterator {
	private:
		int64_t index;
		int32_t size;
		int64_t maxIndex;
	public:
		order_iterator(int32_t size);
		bool nextOrder(int32_t* key);
		void reset(int32_t* key);
	};

	class best_n_cache {
	private:
		int32_t alloc_size;
	public:
		struct best_kv {
			float score = -std::numeric_limits<float>::max();
			std::string string;
		};
	private:
		std::vector<best_kv> vals;
		float worst_score;
	public:
		best_n_cache(int32_t nval);
		void addToCache(float score, std::string str);
		const std::vector<best_kv>& getVec();
		best_kv getBest();
		void kill();
		void load(best_n_cache* pntr);
	};
}

namespace pwn_cipher {
	void pwn_vigenere(std::string str);
	void pwn_keyword(std::string str);
	void pwn_affine(std::string str);
	void pwn_transposition(std::string str);
	void pwn_columnar(std::string str);
	void pwn_amsco(std::string str);
}