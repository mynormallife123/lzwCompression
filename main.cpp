#include "lzw_compression.h"
#include "lzw_decompression.h"


void initializeDictionary(std::unordered_map<sequence, code>& dictionary) {
	for (int i = 0; i < 256; ++i) {
		dictionary[sequence(1, static_cast<char>(i))] = i;
	}
}