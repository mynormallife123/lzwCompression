#ifndef LZW_COMPRESS_H
#define LZW_COMPRESS_H
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <iostream>
#include <fstream>
#include <cerrno>
#include <utility>
#include <unordered_map>

const int OPEN_SOURCE_FILE_FAILED = -1;
const int OPEN_ENCODED_FILE_FAILED = -2;
const int COMPRESS_SUCCESSFULLY = 0;
const int DECOMPRESS_SUCCESSFULLY = 0;
typedef std::string sequence;
typedef uint16_t code;

class LZWCompressor {
private:
    std::ofstream encodedFile, decodedFile;
    uint32_t bitBuffer = 0; // Buffer to store bits
    int bitCount = 0; // Number of bits in buffer

    // Write a 12-bit code to the output file
    void writeToEncodedFile(code c);

    // Flush remaining bits in buffer to the output file
    void flushBuffer();

    // Read a 12-bit code from the input file
    bool readCode(std::ifstream& sourceFile, code& c);

public:
    // Compresses the input file to the output file using LZW with 12-bit codes
    // Returns true on success, false on failure (e.g., file I/O errors)
    int lzwCompress(const std::string& sourceFileName, const std::string& encodedFileName);

    // Decompresses the input file to the output file using LZW with 12-bit codes
    // Returns true on success, false on failure (e.g., file I/O errors)
    int lzwDecompress(const std::string& sourceFileName, const std::string& decodedFileName);
};

#endif // LZW_COMPRESS_H