#include "lzw_compression.h"



/*	* LZW compression receives a text file and produces a binary file that
	* containing the compressed content
	*/
class LZWCompressor {
private:
	std::ofstream encodedFile, decodedFile;
	uint32_t bitBuffer = 0; // Buffer to store bits
	int bitCount = 0; // Number of bits in buffer

	void flushBuffer() {
		if (bitCount > 0) {
			bitBuffer <<= (8 - bitCount);
			encodedFile.put(static_cast<char>(bitBuffer & 0xFF));
			bitBuffer = 0;
			bitCount = 0;
		}
	}

	// Write 12 last bits to output file
	void writeToEncodedFile(code c) {
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

public:
	int lzwCompress(const std::string& sourceFileName, const std::string& encodedFileName,
		std::unordered_map<sequence, code> dictionary) {
		// Open files
		std::ifstream sourceFile(sourceFileName);
		encodedFile.open(encodedFileName, std::ios::binary);


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

		//
		flushBuffer();

		// Close files
		sourceFile.close();
		encodedFile.close();
		return COMPRESS_SUCCESSFULLY;
	}

	int lzwDecompress(const std::string& sourceFileName, const std::string& decodedFileName,
		std::unordered_map<sequence, code> dictionary) {
		// Open files
		std::ifstream sourceFile(sourceFileName, std::ios::binary);
		decodedFile.open(decodedFileName);


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
		if (!read_code(sourceFile, c) || c > 255) {
			sourceFile.close();
			decodedFile.close();
			return false; // Invalid first code
		}
		std::string current_sequence = dictionary[c];
		decodedFile.write(current_sequence.c_str(), current_sequence.size());

		// Process remaining codes
		while (read_code(in_file, c)) {
			std::string new_sequence;
			if (c < next_code) {
				// Code exists in dictionary
				new_sequence = dictionary[c];
			}
			else if (c == next_code) {
				// Special case: code not yet in dictionary
				new_sequence = current_sequence + current_sequence[0];
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
			if (next_code < 4096) {
				dictionary[next_code] = current_sequence + new_sequence[0];
				next_code++;
			}

			current_sequence = new_sequence;
		}

		// Close files
		sourceFile.close();
		decodedFile.close();
		return DECOMPRESS_SUCCESSFULLY;
	}
};