#include <iostream>

#pragma once
namespace utils {
	void print_hex(unsigned char cmd[], size_t len);
	int fcopy(unsigned char res[], const char path[], size_t offset);
}