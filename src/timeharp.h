#ifndef TIMEHARP_H_
#define TIMEHARP_H_

#define TH_TTTR_OVERFLOW 65536

#define TH_MODE_INTERACTIVE 0
#define TH_MODE_CONTINUOUS 1
#define TH_MODE_TTTR 2

#include <stdio.h>
#include "picoquant.h"

int th_dispatch(FILE *in_stream, FILE *out_stream, pq_header_t *pq_header,
				options_t *options);

#endif
