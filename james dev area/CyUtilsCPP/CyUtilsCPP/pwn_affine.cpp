#include "pwn_cy.hpp"
#include "MainFramework.hpp"
namespace pwn_cipher {
	std::string decrypt_affine(std::string cipherstr,int32_t A, int32_t B) {
		std::string res;
		res.resize(cipherstr.length());
		char Encryption[26];
		char Decrytion[26];
		for (int32_t i = 0; i < 26; i++) {
			int32_t temp = ((A * i) + B);
			temp = temp % 26;
			Encryption[i] = 'A' + temp;
		}
		for (int32_t i = 0; i < 26; i++) {
			char Val = 'A' + i;
			char Index = Encryption[i] - 'A';
			Decrytion[Index] = Val;
		}
		for (int32_t i = 0; i < cipherstr.length(); i++) {
			res[i] = Decrytion[cipherstr[i] - 'A'];
		}
		return res;
	}


	void pwn_affine(std::string cipherstr) {
		int32_t valid_values[12] = {1,3,5,7,9,11,15,17,19,21,23,25};
		int32_t best_a = 0, best_b = 0;
		float best_score = -std::numeric_limits<float>::max();
		for (int32_t a_index = 0; a_index < 12; a_index++) {
			int32_t a_value = valid_values[a_index];
			for (int32_t b_value = 0; b_value < 16; b_value++) {
				float score = ngram_stats::scoreQuadgram(decrypt_affine(cipherstr,a_value,b_value));
				if (score > best_score) {
					best_score = score;
					best_a = a_value;
					best_b = b_value;
				}
			}
		}
		INFO("BEST AFFINE[ITERALL,] = " + std::to_string(best_a) + "x + " + std::to_string(best_b));
		INFO("Decryption:");
		INFO(decrypt_affine(cipherstr, best_a, best_b));
	}
}