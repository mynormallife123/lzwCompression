#ifndef LZW_COMPRESS_H
#define LZW_COMPRESS_H
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
public:
    // Compresses the input file to the output file using LZW with 12-bit codes
    // Returns true on success, false on failure (e.g., file I/O errors)
    int lzwCompress(const std::string& sourceFileName, const std::string& encodedFileName,
        std::unordered_map<sequence, code>& dictionary);

    int lzwDecompress(const std::string& sourceFileName, const std::string& decodedFileName,
        std::unordered_map<sequence, code> dictionary);
};

#endif // LZW_COMPRESS_H