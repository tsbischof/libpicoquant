#ifndef T3_H_
#define T3_H_

#include <stdio.h>

#include "types.h"
#include "tttr.h"
#include "t2.h"
#include "options.h"

typedef struct {
	uint32_t channel;
	uint64_t pulse;
	uint64_t time;
} t3_t;

typedef int (*pq_t3_decode_t)(FILE *, tttr_t *, t3_t *);
typedef void (*pq_t3_print_t)(FILE *, t3_t *);

int pq_t3_stream(FILE *stream_in, FILE *stream_out, pq_t3_decode_t decode,
		tttr_t *tttr, options_t *options);
int pq_t3_next(FILE *stream_in, pq_t3_decode_t decode, tttr_t *tttr, t3_t *t3);
void pq_t3_printf(FILE *stream_out, t3_t *record);
void pq_t3_fwrite(FILE *stream_out, t3_t *record);

void pq_t3_to_t2(t3_t *record_in, t2_t *record_out, tttr_t *tttr);

#endif
