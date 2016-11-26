#include "pwn_cy.hpp"

namespace pwn_utils {

	key_iterator::key_iterator(int32_t size)
		: size(size), index(0) {
		maxIndex = 1;
		for (int j = 0; j < size; j++) {
			maxIndex *= 26;
		}
	}

	bool key_iterator::nextKey(char* key) {
		if (index >= maxIndex) return false;
		//GEN KEY WITH CURRENT INDEX//
		int32_t runningValue = index;
		for(int32_t i = 0; i < size; i++) {
			key[i] = 'A' + runningValue % 26;
			runningValue = runningValue / 26;
		}
		index++;
		return true;
	}

	void key_iterator::reset() {
		index = 0;
	}

}