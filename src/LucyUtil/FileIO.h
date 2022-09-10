#pragma once

#include <fstream>
#include <filesystem>
#include <stdint.h>

namespace util {
	bool write_bytes_to_file(const char* filename, const uint8_t* data, std::size_t size);
	bool write_string_to_file(const char* filename, const char* data);

	std::pair<uint8_t*, std::size_t> read_bytes_from_file(const char* filename);
	uint8_t* read_bytes_from_file(const char* filename, std::size_t& size);

	std::pair<char*, std::size_t> read_string_from_file(const char* filename);
	char* read_string_from_file(const char* filename, std::size_t& size);

	// static void replace_first(std::string& s, std::string const& toReplace, std::string const& replaceWith) {
	// 	size_t pos = s.find(toReplace);
	// 	if (pos == std::string::npos) return;

	// 	s.replace(pos, toReplace.length(), replaceWith);
	// }
}