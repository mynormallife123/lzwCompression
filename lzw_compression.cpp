#include "lzw_compression.h"

/*	* LZW compression receives a text file and produces a binary file that
	* containing the compressed content
	*/
void LZWCompressor::writeToEncodedFile(code c) {
	// Add 12 bits to the end of buffer
	bitBuffer = (bitBuffer << 12) | c;
	bitCount += 12;
	while (bitCount >= 8) {
		// Extract bits from the buffer
		uint8_t byte = (bitBuffer >> (bitCount - 8)) & 0xFF;
		encodedFile.put(static_cast<char>(byte));
		bitCount -= 8;
		bitBuffer &= (1ULL << bitCount) - 1;
	}
}

bool LZWCompressor::readCode(std::ifstream& sourceFile, code& c) {
	while (bitCount < 12) {
		char byte;
		if (!sourceFile.get(byte)) {
			return false; // End of file or error
		}
		bitBuffer = (bitBuffer << 8) | (static_cast<uint8_t>(byte));
		bitCount += 8;
	}
	c = (bitBuffer >> (bitCount - 12)) & 0xFFF; // Extract 12 bits
	bitCount -= 12;
	bitBuffer &= (1ULL << bitCount) - 1; // Clear used bits
	return true;
}

void LZWCompressor::flushBuffer() {
	if (bitCount > 0) {
		bitBuffer <<= (8 - bitCount);
		encodedFile.put(static_cast<char>(bitBuffer & 0xFF));
		bitBuffer = 0;
		bitCount = 0;
	}
}


int LZWCompressor::lzwCompress(const std::string& sourceFileName, const std::string& encodedFileName) {
	// Open files
	std::ifstream sourceFile(sourceFileName);
	encodedFile.open(encodedFileName, std::ios::binary);

	// Initialize the dictionary
	std::unordered_map<sequence, code> dictionary;
	for (int i = 0; i < 256; ++i) {
		dictionary[sequence(1, static_cast<char>(i))] = i;
	}
		// Check if file can be opened or not
	if (!sourceFile) {
		std::cout << "Failed to open source file: " << std::strerror(errno) << '\n';
		return OPEN_SOURCE_FILE_FAILED;
	}
	if (!encodedFile) {
		std::cout << "Failed to open encoded file: " << std::strerror(errno) << '\n';
		return OPEN_ENCODED_FILE_FAILED;
	}
	// Encode the file:
	sourceFile.seekg(0, std::ios::beg);
	sequence currentSequence;
	code nextCode = 256;
	char byte;
	while (sourceFile.get(byte)) {
		sequence newSequence = currentSequence + byte;
		if (dictionary.find(newSequence) != dictionary.end()) {
			// Sequence can not be found
			currentSequence = newSequence;
		}
		else {
			writeToEncodedFile(dictionary[currentSequence]);
			if (nextCode < 4096) {
				dictionary[newSequence] = nextCode++;
			}
			currentSequence = std::string(1, byte);
		}
	}
	// Output for last sequence
	if (!currentSequence.empty()) {
		writeToEncodedFile(dictionary[currentSequence]);
	}

	flushBuffer();

	// Close files
	sourceFile.close();
	encodedFile.close();
	return COMPRESS_SUCCESSFULLY;
}
	
int LZWCompressor::lzwDecompress(const std::string& sourceFileName, const std::string& decodedFileName) {
	// Open files
	std::ifstream sourceFile(sourceFileName, std::ios::binary);
	decodedFile.open(decodedFileName);
	code nextCode = 256;

	// Initialize dictionary with ASCII chars
	std::vector<sequence> dictionary(4096);
	for (int i = 0; i < 256; ++i) {
		dictionary[i] = sequence(1, static_cast<char>(i));
	}

	// Check if file can be opened or not
	if (!sourceFile) {
		std::cout << "Failed to open source file: " << std::strerror(errno) << '\n';
		return OPEN_SOURCE_FILE_FAILED;
	}
	if (!decodedFile) {
		std::cout << "Failed to open decoded file: " << std::strerror(errno) << '\n';
		return OPEN_ENCODED_FILE_FAILED;
	}
	// Read first code
	code c;
	if (!readCode(sourceFile, c) || c > 255) {
		sourceFile.close();
		decodedFile.close();
		return false; // Invalid first code
	}
	std::string currentSequence = dictionary[c];
	decodedFile.write(currentSequence.c_str(), currentSequence.size());
		// Process remaining codes
	while (readCode(sourceFile, c)) {
		std::string new_sequence;
		if (c < nextCode) {
			// Code exists in dictionary
			new_sequence = dictionary[c];
		}
		else if (c == nextCode) {
			// Special case: code not yet in dictionary
			new_sequence = currentSequence + currentSequence[0];
		}
		else {
			// Invalid code
			sourceFile.close();
			decodedFile.close();
			return false;
		}
		// Output the sequence
		decodedFile.write(new_sequence.c_str(), new_sequence.size());

		// Add new sequence to dictionary
		if (nextCode < 4096) {
			dictionary[nextCode] = currentSequence + new_sequence[0];
			nextCode++;
		}
		currentSequence = new_sequence;
	}
	// Close files
	sourceFile.close();
	decodedFile.close();
	return DECOMPRESS_SUCCESSFULLY;
};
