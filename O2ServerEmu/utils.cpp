#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <istream>
#include <windows.h>

#include "utils.h"
#define MAX_PRINT 100

namespace utils {
	void print_hex(unsigned char cmd[], std::size_t len) {
		int floor_len = len > MAX_PRINT ? MAX_PRINT : len;
		std::cout << " ";
		for (int i = 0; i < floor_len; i++) {
			unsigned char byte = cmd[i];
			std::cout << " " << std::hex << (int)byte;
		}
		if (len > MAX_PRINT) std::cout << "...";
		std::cout << std::endl;
	}

	int fcopy(unsigned char res[], const char path[], size_t offset) {
		std::ifstream f;

		// Set exceptions to be thrown on failure
		f.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			f.open(path, std::ios::binary | std::ios::ate);
		}
		catch (std::system_error& e) {
			std::cerr << e.code().message() << std::endl;
		}

		f.seekg(offset, std::ios::beg);
		std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(f), {});
		buffer.reserve(10284);

		std::copy(buffer.begin(), buffer.end(), res);

		return buffer.size();
	}
}