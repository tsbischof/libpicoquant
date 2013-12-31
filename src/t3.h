/*
 * Copyright (c) 2011-2014, Thomas Bischof
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the Massachusetts Institute of Technology nor the 
 *    names of its contributors may be used to endorse or promote products 
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

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
typedef int (*pq_t3_print_t)(FILE *, t3_t *);

int pq_t3_stream(FILE *stream_in, FILE *stream_out, pq_t3_decode_t decode,
		tttr_t *tttr, options_t *options);
int pq_t3_next(FILE *stream_in, pq_t3_decode_t decode, tttr_t *tttr, t3_t *t3);
int pq_t3_fprintf(FILE *stream_out, t3_t *record);
int pq_t3_fwrite(FILE *stream_out, t3_t *record);

void pq_t3_to_t2(t3_t *record_in, t2_t *record_out, tttr_t *tttr);

#endif
