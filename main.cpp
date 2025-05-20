#include "lzw_compression.h"

int main() {
	std::string sourceFileName = "source.txt";
	std::string encodedFileName = "encoded.txt";
	std::string decodedFileName = "decoded.txt";

	LZWCompressor compressor;
	compressor.lzwCompress(sourceFileName, encodedFileName);
	compressor.lzwDecompress(encodedFileName, decodedFileName);

	return 0;
}
