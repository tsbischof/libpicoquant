#ifndef PICOHARP_H_
#define PICOHARP_H_

#define PH_BASE_RESOLUTION 4e-12 

#define PH_T2_OVERFLOW 210698240
#define PH_T3_OVERFLOW 65536

#define PH_MODE_INTERACTIVE 0
#define PH_MODE_RESERVED 1
#define PH_MODE_T2 2
#define PH_MODE_T3 3

#include <stdio.h>
#include "picoquant.h"

int ph_dispatch(FILE *in_stream, FILE *out_stream, pq_header_t *pq_header,
		options_t *options);

#endif
