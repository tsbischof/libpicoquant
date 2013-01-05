#ifndef PICOQUANT_H_
#define PICOQUANT_H_

#include <stdio.h>

#include "types.h"
#include "options.h"

#include "header.h"
#include "interactive.h"
#include "t2.h"
#include "t3.h"

// General board and mode dispatch
typedef int (*pq_dispatch_t)(FILE *, FILE *, pq_header_t *, options_t *);

int pq_dispatch(FILE *in_stream, FILE *out_stream, options_t *options);

pq_dispatch_t pq_dispatch_get(options_t *options, pq_header_t *pq_header);

void pq_resolution_print(FILE *out_stream,
		int curve, float64_t resolution, 
		options_t *options);
	
#endif
