#ifndef T2_H_
#define T2_H_

#include <stdio.h>

#include "types.h"
#include "tttr.h"
#include "options.h"

typedef struct {
	uint32_t channel;
	uint64_t time;
} t2_t;

typedef int (*pq_t2_decode_t)(FILE *, tttr_t *, t2_t *);
typedef void (*pq_t2_print_t)(FILE *, t2_t *);

int pq_t2_stream(FILE *stream_in, FILE *stream_out, pq_t2_decode_t decode,
		tttr_t *tttr, options_t *options);
int pq_t2_next(FILE *stream_in, pq_t2_decode_t decode, tttr_t *tttr, t2_t *t2);
void pq_t2_printf(FILE *stream_out, t2_t *record);
void pq_t2_fwrite(FILE *stream_out, t2_t *record);

#endif
