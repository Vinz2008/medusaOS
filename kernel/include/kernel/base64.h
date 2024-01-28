#include <stddef.h>
#include <stdint.h>

char* base64_encode(const unsigned char* data, size_t input_length,
                    size_t* output_length);
uint8_t* base64_decode(const char* data, size_t input_length,
                       size_t* output_length);