#ifndef KITRA_PACKET_GENERATOR_H
#define KITRA_PACKET_GENERATOR_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "kitra_small_utility.h"

bool kitra_generate(char* packet,char* format,uint32_t* filter_mask, ...);
bool kitra_generate_packet(char* buffer, uint32_t buffer_size,char* format, uint32_t filter_mask, ...);

#endif