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

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "th_v20.h"

#include "../timeharp.h"
#include "../error.h"

/*
 *
 * TTTR mode routines.
 *
 */
void th_v20_t3_init(th_v20_header_t *th_header,
		th_v20_tttr_header_t *tttr_header,
		tttr_t *tttr) {
	tttr->origin = 0;
	tttr->overflows = 0;
	tttr->overflow_increment = TH_TTTR_OVERFLOW;
	tttr->sync_rate = tttr_header->SyncRate;
	tttr->resolution_float = th_header->Brd[0].Resolution*1e-9;
	tttr->resolution_int = floor(fabs(tttr->resolution_float*1e12));
}

int th_v20_t3_decode(FILE *stream_in, tttr_t *tttr, t3_t *t3) {
	size_t n_read;
	th_v20_tttr_record_t record;

	n_read = fread(&record, sizeof(record), 1, stream_in);

	if ( n_read != 1 ) {
		if ( ! feof(stream_in) ) {
			error("Could not read t3 record.\n");
			return(PQ_ERROR_IO);
		} else {
			return(PQ_ERROR_EOF);
		}
	} else {
		if ( record.Valid ) {
			/* Normal record. */
			t3->channel = record.Channel;
			t3->pulse = tttr->origin;
			t3->time = record.TimeTag;
			return(PQ_RECORD_T3);
		} else {
			/* Special record. */
			if ( 0x800 & record.Channel ) {
				/* Overflow */
				tttr->overflows++;
				tttr->origin += tttr->overflow_increment;
				return(PQ_RECORD_OVERFLOW);
			} else {
				t3->channel = 0x800;
				t3->pulse = record.Channel;
				return(PQ_RECORD_MARKER);
			}
		}
	}
}
				
int th_v20_t3_stream(FILE *stream_in, FILE *stream_out, 
		th_v20_header_t *th_header, th_v20_tttr_header_t *tttr_header,
		options_t *options) {
	tttr_t tttr;

	th_v20_t3_init(th_header, tttr_header, &tttr);

	if ( options->to_t2 ) {
		error("T3 -> T2 not supported for Timeharp.\n");
		return(PQ_ERROR_OPTIONS);
	} else {
		return(pq_t3_stream(stream_in, stream_out, 
				th_v20_t3_decode, &tttr, options));
	}
}
		
int th_v20_tttr_stream(FILE *stream_in, FILE *stream_out, 
		pq_header_t *pq_header, th_v20_header_t *th_header, 
		options_t *options) { 
	th_v20_tttr_header_t *tttr_header;
	int result;

	result = th_v20_tttr_header_read(stream_in, &tttr_header);
	if ( result != PQ_SUCCESS ) {
		error("Failed while reading the tttr header.\n");
	} else {
		if ( options->print_header ) {
			if ( options->binary_out ) {
				pq_header_fwrite(stream_out, pq_header);
				th_v20_header_fwrite(stream_out, th_header);
				th_v20_tttr_header_fwrite(stream_out, tttr_header);
			} else {
				pq_header_printf(stream_out, pq_header);
				th_v20_header_printf(stream_out, th_header);
				th_v20_tttr_header_printf(stream_out, tttr_header);
			}
			
			result = PQ_SUCCESS;
		} else if ( options->print_resolution ) {
			pq_resolution_print(stream_out, -1,
					(th_header->Brd[0].Resolution*1e3), options);
			result = PQ_SUCCESS;
		} else {
			result = th_v20_t3_stream(stream_in, stream_out,
					th_header, tttr_header, options);
		}
	}

	debug("Freeing tttr header.\n");
	th_v20_tttr_header_free(&tttr_header);
	return(result);
}

int th_v20_tttr_header_read(FILE *stream_in, 
		th_v20_tttr_header_t **tttr_header) {
	size_t n_read;

	*tttr_header = (th_v20_tttr_header_t *)malloc(sizeof(th_v20_tttr_header_t));

	if ( *tttr_header == NULL ) {
		error("Could not allocate tttr header.\n");
		return(PQ_ERROR_MEM);
	}

	n_read = fread(tttr_header, sizeof(th_v20_tttr_header_t), 1, stream_in);
	if ( n_read != 1 ) {
		error("Could not read tttr header.\n");
		th_v20_tttr_header_free(tttr_header);
		return(PQ_ERROR_IO);
	}
	
	return(PQ_SUCCESS);
}
			
void th_v20_tttr_header_free(th_v20_tttr_header_t **tttr_header) { 
	free(*tttr_header);
}

void th_v20_tttr_header_printf(FILE *stream_out, 
		th_v20_tttr_header_t *tttr_header) {
	int i;

	fprintf(stream_out, "TTTRGlobCLock = %"PRId32"\n", 
			tttr_header->TTTRGlobClock);

	for ( i = 0; i < 6; i++ ) {
		fprintf(stream_out, "Reserved[%d] = %"PRId32"\n",
				i, tttr_header->Reserved[i]);
	}

	fprintf(stream_out, "SyncRate = %"PRId32"\n", 
			tttr_header->SyncRate);
	fprintf(stream_out, "AverageCFDRate = %"PRId32"\n", 
			tttr_header->AverageCFDRate);
	fprintf(stream_out, "StopAfter = %"PRId32"\n", 
			tttr_header->StopAfter);
	fprintf(stream_out, "StopReason = %"PRId32"\n", 
			tttr_header->StopReason);
	fprintf(stream_out, "NumberOfRecords = %"PRId32"\n", 
			tttr_header->NumberOfRecords);
	
}

void th_v20_tttr_header_fwrite(FILE *stream_out,
		th_v20_tttr_header_t *tttr_header) {
	fwrite(tttr_header,
			sizeof(th_v20_tttr_header_t),
			1, 
			stream_out);
}
