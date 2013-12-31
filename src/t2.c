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

#include "t2.h"

#include "error.h"

int pq_t2_stream(FILE *stream_in, FILE *stream_out,
		pq_t2_decode_t decode, tttr_t *tttr, options_t *options) {
	/* 
	 * Use the specified decoder to process the incoming stream of t2 records.
	 */
	int64_t record_count = 0;
	int result = PQ_SUCCESS;
	t2_t t2;
	pq_t2_print_t print;

	if ( options->binary_out ) {
		print = pq_t2_fwrite;
	} else {
		print = pq_t2_fprintf;
	}

	/* To do: add some logic to read a large amount of records (e.g. 1024) and
	 * then process them. This should reduce the number of disk i/o calls.
	 */
	while ( ! pq_check(result) && 
			! feof(stream_in) &&
			record_count < options->number ) {
		result = pq_t2_next(stream_in, decode, tttr, &t2);

		if ( ! pq_check(result) ) {
			/* Found a record, process it. */
			if ( result == PQ_RECORD_T2 ) {
				record_count++;
				pq_record_status_print("picoquant", record_count, options);
				print(stream_out, &t2);
			} else if ( result == PQ_RECORD_MARKER ) {
				tttr_marker_print(stream_out, t2.time);
			} else if ( result == PQ_RECORD_OVERFLOW ) {
				/* overflow must be performed in the decoder. */
			} else { 
				error("Record type not recognized: %d\n", result);
				result = PQ_ERROR_UNKNOWN_DATA;
			}
		} else if ( result == PQ_ERROR_EOF ) {
			/* loop will take care of the EOF */
			result = PQ_SUCCESS;
		}
	}

	return(result);
}	

int pq_t2_next(FILE *stream_in, pq_t2_decode_t decode, 
		tttr_t *tttr, t2_t *t2) {
/*
 * Return the next t2 record from the incoming stream.
 */
	return(decode(stream_in, tttr, t2));
}

int pq_t2_fprintf(FILE *stream_out, t2_t *record) {
/* 
 * Print the t2 record in csv format.
 */
	fprintf(stream_out, "%"PRIu32",%"PRIu64"\n", 
			record->channel,
			record->time);

	return( ! ferror(stream_out) ? PQ_SUCCESS : PQ_ERROR_IO );
}

int pq_t2_fwrite(FILE *stream_out, t2_t *record) {
/*
 * Write the t2 record as a binary structure.
 */
	fwrite(record, sizeof(t2_t), 1, stream_out);

	return ( ! ferror(stream_out) ? PQ_SUCCESS : PQ_ERROR_IO );
}
