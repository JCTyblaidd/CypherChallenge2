#include "pwn_cy.hpp"
#include "MainFramework.hpp"
#include <random>

namespace pwn_cipher {

	std::string convert_t_key(std::vector<int32_t> vec) {
		std::string OUT;
		for (int32_t i = 0; i < vec.size(); i++) {
			OUT += std::to_string(vec[i]);
		}
		return OUT;
	}

	std::vector<int32_t> validFactorize(int32_t len) {
		std::vector<int32_t> allowed;
		for (int32_t i = 2; i < 25; i++) {
			if (len % i == 0) {
				allowed.push_back(i);
			}
		}
		return allowed;
	}

	typedef std::string(*decrpType)(std::string cipher, int32_t* ordering, int32_t repeatLen);

	std::string decryptColumnar(std::string cipher,int32_t* ordering, int32_t repeatLen) {
		std::string res;
		res.resize(cipher.length());
		int32_t altRepeat = cipher.length() / repeatLen;
		for (int32_t i = 0; i < cipher.length(); i++) {
			int32_t fromI = i / repeatLen;//Y
			int32_t progI = i % repeatLen;//X
			int32_t infoI = ordering[progI];//new X//
			int32_t target = (infoI * altRepeat) + fromI;
			res[i] = cipher[target];
		}
		return res;
	}
	std::string decryptTrans(std::string cipher,int32_t* ordering, int32_t repeatLen) {
		std::string res;
		int32_t* reverseOrder = (int32_t*)alloca(sizeof(int32_t) * repeatLen);
		for (int32_t i = 0; i < repeatLen; i++) {
			reverseOrder[ordering[i]] = i;
		}
		res.resize(cipher.length());
		for (int32_t i = 0; i < cipher.length(); i++) {
			int32_t fromA = i / repeatLen;
			int32_t fromB = i % repeatLen;
			int32_t search = reverseOrder[fromB];
			int32_t target = (fromA * repeatLen) + search;
			res[i] = cipher[target];
		}
		return res;
	}

	template<decrpType OP>
	std::vector<int32_t> default_trans_hill_climb(std::string cypher,int32_t len) {
		std::vector<int32_t> vec_best;
		std::vector<int32_t> vec_running;
		float best_score = -std::numeric_limits<float>::max();
		vec_best.resize(len);
		vec_running.resize(len);
		std::default_random_engine engine;
		std::uniform_int_distribution<int32_t> locRandom(0,len-1);
		for (int32_t i = 0; i < len; i++) {
			vec_running[i] = i;
		}
		int32_t maxIter = len * 10000;
		for (int32_t iter = 0; iter < maxIter; iter++) {
			int32_t loc1 = locRandom(engine);
			int32_t loc2 = locRandom(engine);
			int32_t temp = vec_running[loc1];
			vec_running[loc1] = vec_running[loc2];
			vec_running[loc2] = temp;
			float SCORE = ngram_stats::scoreQuadgram(OP(cypher, &vec_running[0], len));
			if (SCORE > best_score) {
				best_score = SCORE;
				memcpy(&vec_best[0], &vec_running[0], sizeof(int32_t) * len);
			}
		}
		INFO("Best Trans[HCLIMB," + std::to_string(len) + "] = " + convert_t_key(vec_best));
		return vec_best;
	}

	template<decrpType OP>
	std::vector<int32_t> default_trans_iter_all(std::string cypher, int32_t len) {
		std::vector<int32_t> vec_best;
		std::vector<int32_t> vec_running;
		float best_score = -std::numeric_limits<float>::max();
		vec_best.resize(len);
		vec_running.resize(len);
		pwn_utils::order_iterator oiter(len);
		oiter.reset(&vec_running[0]);
		while (oiter.nextOrder(&vec_running[0])) {
			float SCORE = ngram_stats::scoreQuadgram(OP(cypher, &vec_running[0], len));
			if (SCORE > best_score) {
				best_score = SCORE;
				memcpy(&vec_best[0], &vec_running[0], sizeof(int32_t) * len);
			}
		}
		INFO("Best Trans[ITERALL," + std::to_string(len) + "] = " + convert_t_key(vec_best));
		return vec_best;
	}

	struct globalTransTypeState {
		std::atomic<int32_t> countdown;
		std::vector<std::vector<int32_t>> best_vals;
		std::vector<float> best_scores;
	};

	struct globalTransOpTemp {
		globalTransTypeState* gPntr;
		int32_t len, index;
		std::string str;
	};

	template<decrpType OP>
	void execute(void* pntr) {
		globalTransOpTemp* Pntr = reinterpret_cast<globalTransOpTemp*>(pntr);
		std::vector<int32_t> amazingKey;
		if (Pntr->len < 5) {
			amazingKey = default_trans_iter_all<OP>(Pntr->str, Pntr->len);
		}else {
			amazingKey = default_trans_hill_climb<OP>(Pntr->str, Pntr->len);
		}
		Pntr->gPntr->best_scores[Pntr->index] = ngram_stats::scoreQuadgram(OP(Pntr->str, &amazingKey[0], Pntr->len));
		Pntr->gPntr->best_vals[Pntr->index] = amazingKey;
		Pntr->gPntr->countdown--;
		delete Pntr;
	}

	void pwn_transposition(std::string str) {
		int32_t len = str.length();
		auto validFactor = validFactorize(len);
		globalTransTypeState* gPntr = new globalTransTypeState();
		gPntr->countdown = validFactor.size();
		gPntr->best_vals.resize(validFactor.size());
		gPntr->best_scores.resize(validFactor.size());
		for (int32_t index = 0; index < validFactor.size(); index++) {
			RunnableCache runnable;
			runnable.Func = execute<decryptTrans>;
			globalTransOpTemp* dat = new globalTransOpTemp();
			dat->index = index;
			dat->len = validFactor[index];
			dat->str = str;
			dat->gPntr = gPntr;
			runnable.Data = dat;
			WorkPool->pushJob(runnable);
		}
		while (gPntr->countdown > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		int32_t bestIndex;
		float bestScore = -std::numeric_limits<float>::max();
		for (int32_t index = 0; index < validFactor.size(); index++) {
			if (gPntr->best_scores[index] > bestScore) {
				bestScore = gPntr->best_scores[index];
				bestIndex = index;
			}
		}
		INFO("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
		INFO("Best Key = " + convert_t_key(gPntr->best_vals[bestIndex]));
		INFO("Decrypt:");
		std::string decrypt = decryptTrans(str, &gPntr->best_vals[bestIndex][0], validFactor[bestIndex]);
		INFO(decrypt);
		//ADD SUPPORT FOR SNEAKY REVERSE//
		std::string altDecrypt;
		altDecrypt.resize(decrypt.size());
		int32_t size = validFactor[bestIndex];
		int32_t blockNum = decrypt.size() / size;
		for (int32_t i = 0; i < decrypt.size(); i++) {
			int32_t block = i / size;
			int32_t index = i % size;
			int32_t block2start = (blockNum - block - 1) * size;
			altDecrypt[i] = decrypt[block2start + index];
		}
		INFO("Alt Decrypt:");
		INFO(altDecrypt);
		delete gPntr;
	}
	void pwn_columnar(std::string str) {
		int32_t len = str.length();
		auto validFactor = validFactorize(len);
		globalTransTypeState* gPntr = new globalTransTypeState();
		gPntr->countdown = validFactor.size();
		gPntr->best_vals.resize(validFactor.size());
		gPntr->best_scores.resize(validFactor.size());
		for (int32_t index = 0; index < validFactor.size(); index++) {
			RunnableCache runnable;
			runnable.Func = execute<decryptColumnar>;
			globalTransOpTemp* dat = new globalTransOpTemp();
			dat->index = index;
			dat->len = validFactor[index];
			dat->str = str;
			dat->gPntr = gPntr;
			runnable.Data = dat;
			WorkPool->pushJob(runnable);
		}
		while (gPntr->countdown > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		int32_t bestIndex;
		float bestScore = -std::numeric_limits<float>::max();
		for (int32_t index = 0; index < validFactor.size(); index++) {
			if (gPntr->best_scores[index] > bestScore) {
				bestScore = gPntr->best_scores[index];
				bestIndex = index;
			}
		}
		INFO("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
		INFO("Best Key = " + convert_t_key(gPntr->best_vals[bestIndex]));
		INFO("Decrypt:");
		INFO(decryptColumnar(str, &gPntr->best_vals[bestIndex][0], validFactor[bestIndex]));
		delete gPntr;
	}
}