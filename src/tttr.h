#ifndef TTTR_H_
#define TTTR_H_

#include <stdio.h>

#include "types.h"

typedef struct {
	int sync_channel;
	int64_t origin;
	int overflows;
	int overflow_increment;
	int sync_rate;
	float64_t resolution_float;
	int resolution_int;
} tttr_t;

void tttr_marker_print(FILE *stream_out, int marker);

#endif
