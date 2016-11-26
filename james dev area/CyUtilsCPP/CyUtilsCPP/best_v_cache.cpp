#include "pwn_cy.hpp"
#include "MainFramework.hpp"

namespace pwn_utils {
	best_n_cache::best_n_cache(int32_t nval) : alloc_size(nval) {
		vals.reserve(nval);
		worst_score = -std::numeric_limits<float>::max();
	}
	void best_n_cache::addToCache(float score, std::string str) {
		best_kv v;
		v.score = score;
		v.string = str;
		if (vals.size() < alloc_size) {
			vals.push_back(v);
			if (score > worst_score) {
				worst_score = score;
			}
		}
		else {
			if (score <= worst_score) return;
			//Else Remove Someone//
			int32_t index = 0;
			for (; index < alloc_size; index++) {
				if (vals[index].score == worst_score) {
					vals[index] = v;
					worst_score = score;
					return;
				}
			}
			//UNKNOWN
			WARN("ODD ERROR ADDING TO N CACHE");
		}
	}
	const std::vector<best_n_cache::best_kv>& best_n_cache::getVec() {
		return this->vals;
	}

	best_n_cache::best_kv best_n_cache::getBest() {
		int32_t index = 0;
		float highest_value = -std::numeric_limits<float>::max();
		for (; index < alloc_size; index++) {
			if (vals[index].score > highest_value) {
				highest_value = vals[index].score;
			}
		}
		index = 0;
		for (; index < alloc_size; index++) {
			if (vals[index].score == highest_value) {
				return vals[index];
			}
		}
		best_kv a;
		a.score = -std::numeric_limits<float>::max();
		a.string = "IMPORTANTTHISISANERRORYOUFAIL";
		return a;
	}

	void best_n_cache::kill() {
		vals.clear();
	}

	void best_n_cache::load(best_n_cache* ptr) {
		vals = ptr->vals;
		worst_score = ptr->worst_score;
	}
}