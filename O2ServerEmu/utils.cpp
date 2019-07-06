#include <iostream>
#include <fstream>

#include "utils.h"

namespace utils {
	void print_hex(unsigned char cmd[], std::size_t len) {
		std::cout << " ";
		for (int i = 0; i < len; i++) {
			unsigned char byte = cmd[i];
			std::cout << " " << std::hex << (int)byte;
		}
		std::cout << std::endl;
	}

	int fcopy(unsigned char *res, const char path[]) {
		std::ifstream file;
		file.exceptions(std::ifstream::badbit);
		try {
			file.open(path);
		}
		catch (const std::ifstream::failure& e) {
			std::cout << "Error opening " << path << std::endl;
			abort();
		}

		std::string contents((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

		int size = contents.size();
		memcpy(res, contents.c_str(), size);
		file.close();

		return size;
	}
}