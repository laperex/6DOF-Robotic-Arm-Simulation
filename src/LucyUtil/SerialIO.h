#pragma once

#include <stdint.h>
#include <stddef.h>
#include <tuple>

namespace util::serial {
	enum {
		BAUDRATE_110 = 110,
		BAUDRATE_300 = 300,
		BAUDRATE_600 = 600,
		BAUDRATE_1200 = 1200,
		BAUDRATE_2400 = 2400,
		BAUDRATE_4800 = 4800,
		BAUDRATE_9600 = 9600,
		BAUDRATE_14400 = 14400,
		BAUDRATE_19200 = 19200,
		BAUDRATE_38400 = 38400,
		BAUDRATE_56000 = 56000,
		BAUDRATE_57600 = 57600,
		BAUDRATE_115200 = 115200,
		BAUDRATE_128000 = 128000,
		BAUDRATE_256000 = 256000,
	};

	bool connect_port(const char* port, uint32_t baudrate);
	void disconnect_port();
	bool is_port_connected();

	void set_wait_time(uint32_t time);

	int read_bytes_from_port(uint8_t* data, uint32_t size);
    bool write_bytes_to_port(uint8_t* data, uint32_t size);

	// bool write_bytes_to_port(const char* filename, const uint8_t* data, uint32_t size);
	// bool write_string_to_port(const char* filename, const char* data);

	// std::pair<uint8_t*, uint32_t> read_bytes_from_port(const char* filename);
	// uint8_t* read_bytes_from_port(const char* filename, uint32_t& size);

	// std::pair<char*, uint32_t> read_string_from_port(const char* filename);
	// char* read_string_from_port(const char* filename, uint32_t& size);
}