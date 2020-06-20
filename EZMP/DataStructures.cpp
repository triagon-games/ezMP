#include "DataStructures.h"

uint8_t* Convert64(uint64_t in)
{
	uint8_t* result = (uint8_t*)malloc(sizeof(in));
	if (result == nullptr) throw std::runtime_error("Conversion Failed");
	memset(result, 0, sizeof(in));
	memcpy(result, &in, sizeof(in));
	return result;
}

uint8_t* Convert32(uint32_t in)
{
	uint8_t* result = (uint8_t*)malloc(sizeof(in));
	if (result == nullptr) throw std::runtime_error("Conversion Failed");
	memset(result, 0, sizeof(in));
	memcpy(result, &in, sizeof(in));
	return result;
}

uint8_t* Convert16(uint16_t in)
{
	uint8_t* result = (uint8_t*)malloc(sizeof(in));
	if (result == nullptr) throw std::runtime_error("Conversion Failed");
	memset(result, 0, sizeof(in));
	memcpy(result, &in, sizeof(in));
	return result;
}

uint8_t* ConvertFloat(float in)
{
	uint8_t* result = (uint8_t*)malloc(sizeof(in));
	if (result == nullptr) throw std::runtime_error("Conversion Failed");
	memset(result, 0, sizeof(in));
	memcpy(result, &in, sizeof(in));
	return result;
}

uint8_t* ConvertDouble(double in)
{
	uint8_t* result = (uint8_t*)malloc(sizeof(in));
	if (result == nullptr) throw std::runtime_error("Conversion Failed");
	memset(result, 0, sizeof(in));
	memcpy(result, &in, sizeof(in));
	return result;
}
