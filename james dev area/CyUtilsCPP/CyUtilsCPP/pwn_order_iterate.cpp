#include "pwn_cy.hpp"

namespace pwn_utils {
	order_iterator::order_iterator(int32_t size) {
		index = 0;
		maxIndex = 1;
		for (int32_t i = 1; i <= size; i++) {
			maxIndex *= i;
		}
		this->size = size;
	}
	bool order_iterator::nextOrder(int32_t* key) {
		if (index >= maxIndex) return false;
		int32_t swap1 = index % size;
		int32_t swap2 = (index + 1) % size;
		int32_t temp = key[swap1];
		key[swap1] = key[swap2];
		key[swap2] = temp;
		index++;
		return true;
	}
	void order_iterator::reset(int32_t* key) {
		index = 0;
		for (int32_t i = 0; i < size; i++) {
			key[i] = i;
		}
	}
}