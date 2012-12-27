#ifndef HYDRAHARP_H_
#define HYDRAHARP_H_

#define HH_SYNC_CHANNEL 4

#define HH_BASE_RESOLUTION 1e-12
#define HH_T2_OVERFLOW 33552000
#define HH_T3_OVERFLOW 1024

#define HH_MODE_INTERACTIVE 0
#define HH_MODE_RESERVED 1
#define HH_MODE_T2 2 
#define HH_MODE_T3 3

#include <stdio.h>
#include "picoquant.h"

int hh_dispatch(FILE *in_stream, FILE *out_stream, pq_header_t *pq_header,
		options_t *options);

#endif
