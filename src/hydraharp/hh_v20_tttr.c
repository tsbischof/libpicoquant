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

#include <math.h>
#include <stdlib.h>

#include "../hydraharp.h"
#include "hh_v20.h"

#include "../error.h"

void hh_v20_t2_init(hh_v20_header_t *hh_header,
		hh_v20_tttr_header_t *tttr_header,
		tttr_t *tttr) {
	tttr->sync_channel = hh_header->InputChannelsPresent;
	tttr->origin = 0;
	tttr->overflows = 0;
	tttr->overflow_increment = HH_T2_OVERFLOW;
	tttr->sync_rate = tttr_header->SyncRate;
	tttr->resolution_float = HH_BASE_RESOLUTION;
	tttr->resolution_int = floor(fabs(tttr->resolution_float*1e12));
}

void ptu_hh_v20_t2_init(ptu_header_t *header,tttr_t *tttr) {
	tttr->sync_channel = header->HW_InpChannels;
	tttr->origin = 0;
	tttr->overflows = 0;
	tttr->overflow_increment = HH_T2_OVERFLOW;
	tttr->sync_rate = header->TTResult_SyncRate;
	tttr->resolution_float = header->MeasDesc_GlobalResolution;//was *1e-12 but I think that's wrong because it's built in
	tttr->resolution_int = floor(fabs(tttr->resolution_float*1e12));
}

int hh_v20_t2_decode(FILE *stream_in, tttr_t *tttr, t2_t *t2) {
	size_t n_read;
	hh_v20_t2_record_t record;

	n_read = fread(&record, sizeof(record), 1, stream_in);
		
	if ( n_read != 1 ) {
		if ( !feof(stream_in) ) {
			error("Could not read t2 record.\n");
			return(PQ_ERROR_IO);
		} else {
			return(PQ_ERROR_EOF);
		}
	} else {
		if ( record.special ) {
			if ( record.channel == 63 ) {
				/* Overflow */
				tttr->overflows += record.time;
				tttr->origin += (uint64_t)record.time*
						(uint64_t)tttr->overflow_increment;
				return(PQ_RECORD_OVERFLOW);
			} else if ( record.channel == 0 ) {
				/* 
				 * Sync record. 
				 * Label the sync channel as 1 greater than the maximum
				 * signal channel index.
				 */
				t2->channel = tttr->sync_channel;
				t2->time = tttr->origin + record.time;
				return(PQ_RECORD_T2);
			} else {
				/* External marker. */
				t2->time = record.channel;
				return(PQ_RECORD_MARKER);
			}
		} else {
			/* See the ht2 documentation for this, but the gist is that
			 * the counts are registered at double the rate of the reported
			 * resolution, so one count is actually 0.5ps, not 1ps. Cut
			 * the integer values for time in half to get the correct
			 * result.
			 */
			t2->channel = record.channel;
			t2->time = tttr->origin + record.time;
			return(PQ_RECORD_T2);
		}
	}
}

/*
 *
 * Reading and interpreting for t3 mode.
 *
 */
void hh_v20_t3_init(hh_v20_header_t *hh_header,
		hh_v20_tttr_header_t *tttr_header,
		tttr_t *tttr) {
	tttr->sync_channel = hh_header->InputChannelsPresent;
	tttr->origin = 0;
	tttr->overflows = 0;
	tttr->overflow_increment = HH_T3_OVERFLOW;
	tttr->sync_rate = tttr_header->SyncRate;
	tttr->resolution_float = hh_header->Resolution*1e-12;
	tttr->resolution_int = floor(fabs(tttr->resolution_float*1e12));
}

void ptu_hh_v20_t3_init(ptu_header_t *header,tttr_t *tttr) {
	tttr->sync_channel = header->HW_InpChannels;
	tttr->origin = 0;
	tttr->overflows = 0;
	tttr->overflow_increment = HH_T3_OVERFLOW;
	tttr->sync_rate = header->TTResult_SyncRate;
	tttr->resolution_float = header->MeasDesc_BinningFactor*1e-12;//was *1e-12 but I think that's wrong because it's built in
	tttr->resolution_int = floor(fabs(tttr->resolution_float*1e12));
}

int hh_v20_t3_decode(FILE *stream_in, tttr_t *tttr, t3_t *t3) {
	size_t n_read;
	hh_v20_t3_record_t record;
		
	n_read = fread(&record, sizeof(record), 1, stream_in);

	if ( n_read != 1 ) {
		if ( !feof(stream_in) ) {
			error("Could not read t3 record.\n");
			return(PQ_ERROR_IO);
		} else {
			return(PQ_ERROR_EOF);
		}
	} else {
		if ( record.special ) {
			if ( record.channel == 63 ) {
				/* Overflow */
				tttr->overflows += record.dtime;
				tttr->origin += record.nsync*HH_T3_OVERFLOW;
				return(PQ_RECORD_OVERFLOW);
			} else {
				/* External marker.  */
				t3->time = record.channel;
				return(PQ_RECORD_MARKER);
			}
		} else {
			t3->channel = record.channel;
			t3->pulse = tttr->origin + record.nsync;
			t3->time = record.dtime * tttr->resolution_int;
			return(PQ_RECORD_T3);
		}
	}
}

/*
 *
 * Streaming for t2 or t3 mode.
 *
 */
int hh_v20_tttr_stream(FILE *stream_in, FILE *stream_out,
		pq_header_t *pq_header, hh_v20_header_t *hh_header, 
		options_t *options) {
	hh_v20_tttr_header_t *tttr_header;
	int result;

	result = hh_v20_tttr_header_read(stream_in, &tttr_header);

	if ( result != PQ_SUCCESS ) {
		error("Failed while reading tttr header.\n");
		return(result);
	} else {
		if ( options->print_header ) {
			if ( options->binary_out ) {
				pq_header_fwrite(stream_out, pq_header);
				hh_v20_header_fwrite(stream_out, hh_header);
				hh_v20_tttr_header_fwrite(stream_out, tttr_header);
			} else {
				pq_header_printf(stream_out, pq_header);
				hh_v20_header_printf(stream_out, hh_header);
				hh_v20_tttr_header_printf(stream_out, tttr_header);
			}

			result = PQ_SUCCESS;
		} else {
			if ( hh_header->MeasurementMode == HH_MODE_T2 ) {
				debug("Found mode ht2.\n");
				result = hh_v20_t2_stream(stream_in, stream_out, 
						hh_header, tttr_header, options);
			} else if ( hh_header->MeasurementMode == HH_MODE_T3 ) {
				debug("Found mode ht3.\n");
				result = hh_v20_t3_stream(stream_in, stream_out,
						hh_header, tttr_header, options);
			} else {
				debug("Unrecognized mode.\n");
				result = PQ_ERROR_MODE;
			}
		}
	}

	debug("Freeing tttr header.\n");
	hh_v20_tttr_header_free(&tttr_header);
	return(result);
}

int hh_v20_t2_stream(FILE *stream_in, FILE *stream_out,
		hh_v20_header_t *hh_header, 
		hh_v20_tttr_header_t *tttr_header, options_t *options) {
	tttr_t tttr;
	
	hh_v20_t2_init(hh_header, tttr_header, &tttr);

	if ( options->print_resolution ){
		pq_resolution_print(stream_out, -1,
				tttr.resolution_float*1e12, options);
		return(PQ_SUCCESS);
	} else {
		return(pq_t2_stream(stream_in, stream_out,
				hh_v20_t2_decode, &tttr, options));
	} 
}

int hh_v20_t3_stream(FILE *stream_in, FILE *stream_out,
		hh_v20_header_t *hh_header, 
		hh_v20_tttr_header_t *tttr_header, options_t *options) {
	tttr_t tttr;

	hh_v20_t3_init(hh_header, tttr_header, &tttr);

	if ( options->print_resolution ){
		pq_resolution_print(stream_out, -1,
				tttr.resolution_float*1e12, options);
		return(PQ_SUCCESS);
	} else {
		return(pq_t3_stream(stream_in, stream_out,
				hh_v20_t3_decode, &tttr, options));
	}
}

int ptu_hh_v20_t2_stream(FILE *stream_in, FILE *stream_out,
		ptu_header_t *ptu_header, options_t *options){
	tttr_t tttr;
	
	ptu_hh_v20_t2_init(ptu_header, &tttr);//write

	if ( options->print_resolution ) {
		pq_resolution_print(stream_out, -1,
				tttr.resolution_float*1e12, options);
		return(PQ_SUCCESS);
	} else {
		return(pq_t2_stream(stream_in, stream_out,
				hh_v20_t2_decode, &tttr, options));//is this the right decode?
	}
}

int ptu_hh_v20_t3_stream(FILE *stream_in, FILE *stream_out,
		ptu_header_t *ptu_header, options_t *options){
	tttr_t tttr;
	
	ptu_hh_v20_t3_init(ptu_header, &tttr);//write
	
	if ( options->print_resolution ) {
		pq_resolution_print(stream_out, -1,
				tttr.resolution_float*1e12, options);
		return(PQ_SUCCESS);
	} else {
		return(pq_t3_stream(stream_in, stream_out,
				hh_v20_t3_decode, &tttr, options));//is this the right decode?
	}
}

/*
 * 
 * Header for tttr mode (t2, t3)
 *
 */
int hh_v20_tttr_header_read(FILE *stream_in, 
		hh_v20_tttr_header_t **tttr_header) {
	size_t n_read;

	*tttr_header = (hh_v20_tttr_header_t *)malloc(sizeof(hh_v20_tttr_header_t));

	if ( *tttr_header == NULL ) {
		error("Could not allocate tttr header.\n");
		return(PQ_ERROR_MEM);
	}

	n_read = fread(*tttr_header, 
			sizeof(hh_v20_tttr_header_t)-sizeof(uint32_t *), 
			1, 
			stream_in);
	if ( n_read != 1 ) {
		error("Could not read tttr header.\n");
		hh_v20_tttr_header_free(tttr_header);
		return(PQ_ERROR_IO);
	}

	(*tttr_header)->ImgHdr = (uint32_t *)malloc(
			(*tttr_header)->ImgHdrSize*sizeof(uint32_t));
	if ( (*tttr_header)->ImgHdr == NULL ) {
		error("Could not allocate memory for tttr image header.\n");
		hh_v20_tttr_header_free(tttr_header);
		return(PQ_ERROR_MEM);
	}

	n_read = fread((*tttr_header)->ImgHdr, 
			sizeof(uint32_t),
			(*tttr_header)->ImgHdrSize, 
			stream_in);
	if ( n_read != (*tttr_header)->ImgHdrSize ) {
		error("Could not read Hydraharp tttr image header.\n");
		hh_v20_tttr_header_free(tttr_header);
		return(PQ_ERROR_IO);
	}

	return(PQ_SUCCESS);
}

void hh_v20_tttr_header_free(hh_v20_tttr_header_t **tttr_header) {
	if ( *tttr_header != NULL ) {
		free((*tttr_header)->ImgHdr);
		free(*tttr_header);
	}
}

void hh_v20_tttr_header_printf(FILE *stream_out,
		hh_v20_tttr_header_t *tttr_header) {
	int i;

	fprintf(stream_out, "SyncRate = %"PRId32"\n", tttr_header->SyncRate);
	fprintf(stream_out, "StopAfter = %"PRId32"\n", tttr_header->StopAfter);
	fprintf(stream_out, "StopReason = %"PRId32"\n", tttr_header->StopReason);
	fprintf(stream_out, "ImgHdrSize = %"PRId32"\n", tttr_header->ImgHdrSize);
	fprintf(stream_out, "NumRecords = %"PRId64"\n", tttr_header->NumRecords);

	for ( i = 0; i < tttr_header->ImgHdrSize; i++ ) {
		fprintf(stream_out, "ImgHdr[%d] = %"PRIu32"\n", i,
				tttr_header->ImgHdr[i]);
	}
}

void hh_v20_tttr_header_fwrite(FILE *stream_out,
		hh_v20_tttr_header_t *tttr_header) {
	fwrite(tttr_header,
			sizeof(hh_v20_tttr_header_t) - sizeof(uint32_t *),
			1,
			stream_out);
	fwrite(tttr_header->ImgHdr,
			sizeof(uint32_t),
			tttr_header->ImgHdrSize,
			stream_out);
}	
