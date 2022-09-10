#include "FileIO.h"

bool util::write_bytes_to_file(const char* filename, const uint8_t* data, std::size_t size) {
	std::ofstream ofs{ filename };

	if (ofs.is_open()) {
		ofs.write((char*)data, size);
		ofs.close();

		return true;
	}

	return false;
}

bool util::write_string_to_file(const char* filename, const char* data) {
	return write_bytes_to_file(filename, (uint8_t*)data, strlen(data));
}

std::pair<uint8_t*, std::size_t> util::read_bytes_from_file(const char* filename) {
	std::size_t size;
	uint8_t* read_buffer = read_bytes_from_file(filename, size);
	return { read_buffer, size };
}

uint8_t* util::read_bytes_from_file(const char* filename, std::size_t& size) {
	struct stat stat_buf;
    int rc = stat(filename, &stat_buf);
    size = (rc == 0) ? stat_buf.st_size : -1;
	uint8_t* read_buffer = nullptr;

	if (size) {
		read_buffer = new uint8_t[size];

		std::ifstream ifs{ filename };

		if (ifs.is_open()) {
			ifs.read((char*)read_buffer, size);
			ifs.close();
		}
	}

	return read_buffer;
}

std::pair<char*, std::size_t> util::read_string_from_file(const char* filename) {
	std::size_t size;
	char* read_buffer = read_string_from_file(filename, size);

	return { read_buffer, size };
}

char* util::read_string_from_file(const char* filename, std::size_t& size) {
	char* read_buffer = (char*)read_bytes_from_file(filename, size);
	if (read_buffer != nullptr)
		read_buffer[size - 1] = '\0';
	return read_buffer;
}
