#include "pwn_cy.hpp"
#include <type_traits>
#include "MainFramework.hpp"
#include <set>
#include <random>

namespace pwn_cipher {

	std::string decrypt_keyword(const std::string& cipher, const  char* const key, int32_t keyLen) {
		std::string res;
		res.resize(cipher.length());
		char Encryption[26];
		int32_t progress = 0;
		for (int32_t i = 0; i < keyLen; i++) {
			char c = key[i];
			bool ok = true;
			for (int32_t j = 0; j < i; j++) {
				if (key[j] == c) {
					ok = false;
					break;
				}
			}
			if (ok) {
				Encryption[progress] = c;
				progress++;
			}
		}
		for (int32_t i = progress; i < 26; i++) {
			char prev = Encryption[i - 1];
			while (true) {
				prev++;
				prev = 'A' + ((prev - 'A') % 26);
				bool ok = true;
				for (int32_t j = 0; j < i; j++) {
					if (Encryption[j] == prev) {
						ok = false;
					}
				}
				if (ok) {
					break;
				}
			}
			Encryption[i] = prev;
		}
		char Decryption[26];
		for (int32_t i = 0; i < 26; i++) {
			char enc = Encryption[i];
			int32_t index = enc - 'A';
			char res = i + 'A';
			Decryption[index] = res;
		}
		for (int32_t i = 0; i < cipher.length(); i++) {
			res[i] = Decryption[cipher[i] - 'A'];
		}
		return res;
	}


	void keyCopy(char* target, char* from) {
		for (int32_t i = 0; i < 20; i++) {
			target[i] = from[i];
		}
	}

	float SCORE_ALL(std::string str) {
		return ngram_stats::scoreQuadgram(str);// +ngram_stats::scoreTrigram(str) + ngram_stats::scoreBigram(str) + ngram_stats::scoreMonogram(str);
	}

	std::string try_all_keyword(std::string cipherstr, int32_t keyLen) {
		char runningKey[26];
		char bestKey[26];
		for (int32_t i = 0; i < 26; i++) {
			runningKey[i] = 'A';
			bestKey[i] = 'A';
		}
		float best_score = -std::numeric_limits<float>::max();
		float score;
		pwn_utils::key_iterator kiter(keyLen);
		while (kiter.nextKey(runningKey)) {
			float SCORE = SCORE_ALL(decrypt_keyword(cipherstr, runningKey, keyLen));
			if (SCORE > best_score) {
				best_score = SCORE;
				keyCopy(bestKey, runningKey);
			}
		}
		INFO("BEST KEY[ITERALL," + std::to_string(keyLen) + "] = " + std::string(bestKey, keyLen));
		return std::string(bestKey, 26);
	}

	std::string hill_climb_keyword(std::string cipherstr, int32_t keyLen) {
		char runningKey[26];
		char bestKey[26];
		std::default_random_engine engine;
		std::uniform_int_distribution<int32_t> locDist(0, keyLen - 1);
		std::uniform_int_distribution<int32_t> keyDist(0, 25);
		float bestScore = -std::numeric_limits<float>::max();
		for (int32_t i = 0; i < 26; i++) {
			char randInit = keyDist(engine) + 'A';
			runningKey[i] = randInit;
			bestKey[i] = randInit;
		}
		//SEED THE KEYWORD DECRYPTOR//
		//FIND MOST COMMON 3WORD --> ASSUME = THE//
		char best_key[3];
		int32_t bestFreq = 0;
		char common_key[3];
		int32_t common_freq;
		int32_t maxLen = cipherstr.length() - 2;
		pwn_utils::key_iterator kiter3(3);
		while (kiter3.nextKey(common_key)) {
			common_freq = 0;
			for (int32_t i = 0; i < maxLen; i++) {
				char char1 = cipherstr[i];
				char char2 = cipherstr[i + 1];
				char char3 = cipherstr[i + 2];
				if ((common_key[0] == char1) & (common_key[1] == char2) & (common_key[2] == char3)) {
					common_freq++;
				}
			}
			if (common_freq > bestFreq) {
				bestFreq = common_freq;
				best_key[0] = common_key[0];
				best_key[1] = common_key[1];
				best_key[2] = common_key[2];
			}
		}
		//PUSH RESULTS//
		runningKey['T' - 'A'] = best_key[0];
		bestKey['T' - 'A'] = best_key[0];
		runningKey['H' - 'A'] = best_key[1];
		bestKey['H' - 'A'] = best_key[1];
		runningKey['E' - 'A'] = best_key[2];
		bestKey['E' - 'A'] = best_key[2];
	

		bestScore = SCORE_ALL(decrypt_keyword(cipherstr, bestKey, keyLen));
		int32_t iterLimit = keyLen * 10000;
		for (int32_t i = 0; i < iterLimit; i++) {
			int32_t loc = locDist(engine);
			int32_t loc2 = locDist(engine);
			int32_t loc3 = locDist(engine);
			int32_t newV = keyDist(engine);
			int32_t newV2 = keyDist(engine);
			int32_t newV3 = keyDist(engine);
			keyCopy(runningKey, bestKey);
			runningKey[loc] = 'A' + newV;
			runningKey[loc2] = 'A' + newV2;
			runningKey[loc3] = 'A' + newV3;
			float SCORE = SCORE_ALL(decrypt_keyword(cipherstr, runningKey, keyLen));
			if (SCORE > bestScore) {
				bestScore = SCORE;
				keyCopy(bestKey, runningKey);
			}
		}
		INFO("BEST KEY[HCLIMB," + std::to_string(keyLen) + "] = " + std::string(bestKey, keyLen));
		return std::string(bestKey, keyLen);
	}


	struct mthreadKGlobalStruct {
		std::atomic<int32_t> countdown;
		std::string results[27];
		float scores[27];
		mthreadKGlobalStruct() {
			countdown = 11;
			for (int32_t i = 0; i < 27; i++) {
				results[i] = "";
				scores[i] = -std::numeric_limits<float>::max();
			}
		}
	};

	struct mthreadKWStruct {
		std::string str;
		int32_t len;
		mthreadKGlobalStruct* globalPntr;
	};
	
	void delegateMThreadKeyword(void* pntr) {
		mthreadKWStruct* obj = reinterpret_cast<mthreadKWStruct*>(pntr);
		std::string amazingKey;
		if (obj->len < 5) {
			amazingKey = try_all_keyword(obj->str, obj->len);
		}else {
			amazingKey = hill_climb_keyword(obj->str, obj->len);
		}
		int32_t index = obj->len - 1;
		obj->globalPntr->results[index] = amazingKey;
		obj->globalPntr->scores[index] = SCORE_ALL(decrypt_keyword(obj->str, amazingKey.c_str(), obj->len));
		obj->globalPntr->countdown--;
		delete obj;
	}


	void pwn_keyword(std::string cipherstr) {
		mthreadKGlobalStruct* gStruct = new mthreadKGlobalStruct();
		for (int32_t i = 1; i < 12; i++) {
			RunnableCache rCache;
			mthreadKWStruct* obj = new mthreadKWStruct();
			obj->str = cipherstr;
			obj->len = i;
			obj->globalPntr = gStruct;
			rCache.Data = obj;
			rCache.Func = delegateMThreadKeyword;
			WorkPool->pushJob(rCache);
		}
		while (gStruct->countdown > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
		//GET BEST KEY//
		float best_score = -std::numeric_limits<float>::max();
		int32_t best_index = 0;
		for (int32_t i = 0; i < 13; i++) {
			if (gStruct->scores[i] > best_score) {
				best_score = gStruct->scores[i];
				best_index = i;
			}
		}
		INFO("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
		INFO("Best Keyword Key = " + gStruct->results[best_index]);
		INFO("Decryption:");
		INFO(decrypt_keyword(cipherstr, gStruct->results[best_index].c_str(), gStruct->results[best_index].length()));
		delete gStruct;
	}
}