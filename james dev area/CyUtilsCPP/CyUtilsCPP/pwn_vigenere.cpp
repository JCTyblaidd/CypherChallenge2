#include "pwn_cy.hpp"
#include <type_traits>
#include "MainFramework.hpp"
#include <set>
#include <random>

namespace pwn_cipher {

	inline char shift(char val, char key) {
		std::string a;
		char k1 = val - 'A';
		char k2 = key - 'A';
		char k3 = k1 - k2;
		char k4 = (k3 + 26) % 26;
		char k5 = k4 + 'A';
		return k5;
	}

	std::string decrypt_vignere(const std::string& cipher,const  char* const key, int32_t keyLen) {
		std::string res;
		res = cipher;
		int32_t cipher_len = cipher.length();
		//Run Decrypt//
		for (int32_t kval = 0; kval < keyLen; kval++) {
			char kv = key[kval];
			for (int32_t index = kval; index < cipher_len; index += keyLen) {
				res[index] = shift(res[index], kv);
			}
		}
		return res;
	}

#if 0

#define VIGENERE_N 300

	struct Vig_State {
	public:
		char keyList[VIGENERE_N][20];
		float scores[VIGENERE_N];
		int32_t keyLengths[VIGENERE_N];
		void reset() {
			for (int32_t i = 0; i < VIGENERE_N; i++) {
				for (int32_t j = 0; j < VIGENERE_N; j++) {
					keyList[i][j] = 'A';
				}
				scores[i] = -std::numeric_limits<float>::max();
				keyLengths[i] = 0;
			}
		}
		Vig_State() {
			reset();
		}
	};

#endif

	void keyCopy(char* target, char* from);
#if 0
	void push_vig(Vig_State* vstate,char* runningKey,int32_t len,float score) {
		float lowest_score = std::numeric_limits<float>::max();
		int32_t lowest_index = -1000;
		for (int32_t i = 0; i < VIGENERE_N; i++) {
			if (vstate->scores[i] > 5) {//SHODDY FIX//
				lowest_score = -std::numeric_limits<float>::max();
				lowest_index = i;
				break;
			}
			if (lowest_score > vstate->scores[i]) {
				lowest_score = vstate->scores[i];
				lowest_index = i;
			}
		}
		if (score <= lowest_score) {
			return;
		}
		//PUSH//
		vstate->keyLengths[lowest_index] = len;
		vstate->scores[lowest_index] = score;
		keyCopy(&vstate->keyList[lowest_index][0], runningKey);
	}

	void consumeBestKey(Vig_State* vstate, char* runningKey) {
		float best_score = -std::numeric_limits<float>::max();
		int32_t best_index = -1000;
		for (int32_t i = 0; i < VIGENERE_N; i++) {
			if (best_score < vstate->scores[i]) {
				best_score = vstate->scores[i];
				best_index = i;
			}
		}
		vstate->scores[best_index] = -std::numeric_limits<float>::max();
		keyCopy(runningKey, vstate->keyList[best_index]);
	}

#endif
	/**
		THE MAIN SOLVING METHOD
	**/
	std::string hill_climb_vigenere(std::string cipherstr,int32_t keyLen) {
		char runningKey[20];
		char bestKey[20];
		std::default_random_engine engine;
		std::uniform_int_distribution<int32_t> locDist(0, keyLen - 1);
		std::uniform_int_distribution<int32_t> keyDist(0,25);
		float bestScore = -std::numeric_limits<float>::max();
		for (int32_t i = 0; i < 20; i++) {
			runningKey[i] = 'A';
			bestKey[i] = 'A';
		}
		bestScore = ngram_stats::scoreQuadgram(decrypt_vignere(cipherstr, bestKey, keyLen));
		int32_t iterLimit = keyLen * 1000;
		for (int32_t i = 0; i < iterLimit; i++) {
			int32_t loc = locDist(engine);
			int32_t newV = keyDist(engine);
			keyCopy(runningKey, bestKey);
			runningKey[loc] = 'A' + newV;
			float SCORE = ngram_stats::scoreQuadgram(decrypt_vignere(cipherstr, runningKey, keyLen));
			if (SCORE > bestScore) {
				bestScore = SCORE;
				keyCopy(bestKey, runningKey);
			}
		}
		INFO("BEST KEY[HCLIMB,"+std::to_string(keyLen)+"] = " + std::string(bestKey, keyLen));
		return std::string(bestKey, keyLen);
	}

#if 0
	//BAD//
	void fail_vigenere(std::string cipherstr) {
		//INFO(std::to_string(ngram_stats::scoreQuadgram(decrypt_vignere(cipherstr, "NSANSA", 6))));
		Vig_State state1;
		Vig_State state2;
		Vig_State* currentState = &state1;
		Vig_State* oldState = &state2;
		char runningKey[20];
		for (int32_t i = 0; i < 20; i++) {
			runningKey[i] = 'A';
		}
		//RUN ALGORITHM//
		for (int32_t klen = 6/*3*/; klen < 20; klen++) {
			pwn_utils::key_iterator kiter3(3);
			std::string result;
			while (kiter3.nextKey(runningKey)) {
				result = decrypt_vignere(cipherstr, runningKey, klen);
				float SCORE = ngram_stats::scoreTrigram(result);
				if (SCORE > 10) {
					std::string debug(runningKey, klen);
					INFO(debug);
				}
				push_vig(oldState, runningKey, 3,SCORE);
			}
			currentState->reset();

			//MAIN LOOP//
			for (int32_t i = 0; i < klen - 3; i++) {
				for (int32_t kchoice = 0; kchoice < VIGENERE_N; kchoice++) {
					pwn_utils::key_iterator kiter_inner(1);
					keyCopy(runningKey, oldState->keyList[kchoice]);
					int32_t iterate_index = oldState->keyLengths[kchoice];
					//iterate_index = iterate_index % 20;
					while (kiter_inner.nextKey(&runningKey[iterate_index])) {
						result = decrypt_vignere(cipherstr, runningKey, klen);
						float SCORE = ngram_stats::scoreTrigram(result);
						push_vig(currentState, runningKey, iterate_index + 1, SCORE);
					}
				}
				Vig_State* temp = oldState;
				oldState = currentState;
				temp->reset();
				currentState = temp;

			}

			//OUTPUT BEST w/ DECRYPT//
			consumeBestKey(oldState, &runningKey[0]);
			INFO("BEST KEY[" + std::to_string(klen) + "] = " + std::string(runningKey,klen));
			//OUTPUT BEST 5 KEYS//
			
		}
	}
#endif

	struct mthreadGlobalStruct {
		std::atomic<int32_t> countdown;
		std::string results[17];
		float scores[17];
		mthreadGlobalStruct() {
			countdown = 17;
			for (int32_t i = 0; i < 17; i++) {
				results[i] = "";
				scores[i] = -std::numeric_limits<float>::max();
			}
		}
	};

	struct mthreadVigStruct {
		std::string str;
		int32_t len;
		mthreadGlobalStruct* globalPntr;
	};

	void delegateMthreadVigenere(void* pntr) {
		mthreadVigStruct* obj = reinterpret_cast<mthreadVigStruct*>(pntr);
		std::string amazingKey = hill_climb_vigenere(obj->str, obj->len);
		int32_t index = obj->len - 3;
		obj->globalPntr->results[index] = amazingKey;
		obj->globalPntr->scores[index] = ngram_stats::scoreQuadgram(decrypt_vignere(obj->str,amazingKey.c_str(),amazingKey.length()));
		obj->globalPntr->countdown--;
		delete obj;
	}

	void pwn_vigenere(std::string cipherstr) {
		mthreadGlobalStruct* gStruct = new mthreadGlobalStruct();
		for (int32_t i = 3; i < 20; i++) {
			RunnableCache rCache;
			mthreadVigStruct* obj = new mthreadVigStruct();
			obj->str = cipherstr;
			obj->len = i;
			obj->globalPntr = gStruct;
			rCache.Data = obj;
			rCache.Func = delegateMthreadVigenere;
			WorkPool->pushJob(rCache);
		}
		while (gStruct->countdown > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
		//GET BEST KEY//
		float best_score = -std::numeric_limits<float>::max();
		int32_t best_index = 0;
		for (int32_t i = 0; i < 17; i++) {
			if (gStruct->scores[i] > best_score) {
				best_score = gStruct->scores[i];
				best_index = i;
			}
		}
		INFO("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
		INFO("Best Vigenere Key = " + gStruct->results[best_index]);
		INFO("Decryption:");
		INFO(decrypt_vignere(cipherstr, gStruct->results[best_index].c_str(), gStruct->results[best_index].length()));
		delete gStruct;

#if 0
		for (int32_t klen = 3; klen < 20; klen++) {
			pwn_utils::best_n_cache cache_first(VIGENERE_N);
			pwn_utils::key_iterator kiter3(3);
			std::string key3;
			key3.resize(klen+1);
			for (int32_t i = 0; i < klen; i++) {
				key3[i] = 'A';
			}
			key3[klen] = NULL;
			std::string str;
			while (kiter3.nextKey(&key3[0])) {
				str = decrypt_vignere(cipherstr, &key3[0], klen);
				std::string key_use;
				key_use = std::string(key3).substr(0, 3);
				cache_first.addToCache(ngram_stats::scoreTrigram(str), std::string(key_use));
			}
			pwn_utils::best_n_cache cache_main(VIGENERE_N);
			for (int32_t i = 0; i < klen - 3; i++) {
				for (int32_t kchoice = 0; kchoice < VIGENERE_N; kchoice++) {
					pwn_utils::key_iterator kiter_inner(1);
					char normal_k;
					while (kiter_inner.nextKey(&normal_k)) {
						std::string old_key = cache_first.getVec()[kchoice].string;
						old_key += normal_k;
						int32_t oldSize = old_key.length();
						old_key.resize(klen);
						for (int32_t i = oldSize; i < klen; i++) {
							old_key[i] = 'A';
						}
						int32_t keySize = (oldSize + 1) % klen;
						std::string key_use = old_key.substr(0, keySize);
						str = decrypt_vignere(cipherstr, &old_key[0], klen);
						cache_main.addToCache(ngram_stats::scoreQuadgram(str), std::string(key_use));
					}
				}
				cache_first.kill();
				cache_first.load(&cache_main);
				cache_main.kill();
			}
			//Output//

			INFO("Best Key = " + cache_main.getBest().string);
		}
#endif
	}
}