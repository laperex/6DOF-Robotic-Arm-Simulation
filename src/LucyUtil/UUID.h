#pragma once

#include <stdint.h>
#ifndef UUID_SYSTEM_GENERATOR
	#define UUID_SYSTEM_GENERATOR
#endif
#include <stduuid/uuid.h>

#define UTIL_UUID uint64_t
#define UTIL_NULL_UUID (UTIL_UUID)0
#define UTIL_GENERATE_UUID *(UTIL_UUID*)uuids::uuid_system_generator{}().as_bytes().data()