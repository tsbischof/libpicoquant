#ifndef INTERACTIVE_H_
#define INTERACTIVE_H_

#include <stdio.h>

#include "types.h"

typedef struct {
	uint32_t curve;
	float64_t bin_left;
	float64_t bin_right;
	uint32_t counts;
} pq_interactive_bin_t;

typedef void (*pq_interactive_bin_print_t)(FILE *, pq_interactive_bin_t *);

void pq_interactive_bin_printf(FILE *out_stream, pq_interactive_bin_t *bin);
void pq_interactive_bin_fwrite(FILE *out_stream, pq_interactive_bin_t *bin);

#endif 
