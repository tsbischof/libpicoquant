#ifndef TTTR_H_
#define TTTR_H_

#include <stdio.h>

#include "types.h"

typedef struct {
	unsigned int sync_channel;
	int64_t origin;
	unsigned int overflows;
	unsigned int overflow_increment;
	unsigned int sync_rate;
	float64_t resolution_float;
	unsigned int resolution_int;
} tttr_t;

void tttr_marker_print(FILE *stream_out, uint64_t marker);

#endif
