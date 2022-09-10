#include "SerialIO.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WAIT_TIME 2000

struct SerialIOContext {
	HANDLE handler;
	bool connected;
	COMSTAT status;
	DWORD errors;

	uint32_t wait_time = DEFAULT_WAIT_TIME;
};

static SerialIOContext* context = nullptr;

#define self (context)

bool util::serial::connect_port(const char* port, uint32_t baudrate) {
	if (self != nullptr)
		disconnect_port();

	self = new SerialIOContext{};

	self->connected = false;
	self->handler = CreateFileA(static_cast<LPCSTR>(port),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (self->handler == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			printf("ERROR: Handle was not attached. Reason: %s not available\n", port);
		} else {
			printf("ERROR!!!");
		}
	} else {
		DCB dcbSerialParameters = { 0 };

		if (!GetCommState(self->handler, &dcbSerialParameters)) {
			printf("failed to get current serial parameters");
		} else {
			dcbSerialParameters.BaudRate = baudrate;
			dcbSerialParameters.ByteSize = 8;
			dcbSerialParameters.StopBits = ONESTOPBIT;
			dcbSerialParameters.Parity = NOPARITY;
			dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(self->handler, &dcbSerialParameters)) {
				printf("ALERT: could not set Serial port parameters\n");
			} else {
				self->connected = true;
				PurgeComm(self->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(self->wait_time);
			}
		}
	}

	return self->connected;
}

void util::serial::disconnect_port() {
	if (self != nullptr) {
		if (self->connected) {
			self->connected = false;
			CloseHandle(self->handler);
		}

		delete self;
	}

	self = nullptr;
}

bool util::serial::is_port_connected() {
	return self->connected;
}

void util::serial::set_wait_time(uint32_t time) {
	self->wait_time = time;
}

int util::serial::read_bytes_from_port(uint8_t* data, uint32_t size) {
	DWORD bytesRead;
	unsigned int toRead = 0;

	ClearCommError(self->handler, &self->errors, &self->status);

	if (self->status.cbInQue > 0) {
		if (self->status.cbInQue > size) {
			toRead = size;
		} else {
			toRead = self->status.cbInQue;
		}
	}

	if (ReadFile(self->handler, data, toRead, &bytesRead, NULL))
		return bytesRead;

	return 0;
}

bool util::serial::write_bytes_to_port(uint8_t* data, uint32_t size) {
	DWORD bytesSend;
 	if (!WriteFile(self->handler, (void*)data, size, &bytesSend, 0)) {
        ClearCommError(self->handler, &self->errors, &self->status);
        return false;
    } else {
		return true;
	}
}
