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
#include <string.h>
#include <math.h>

#include "../timeharp.h"
#include "th_v30.h"

#include "../error.h"
#include "../interactive.h"

/* 
 *
 * Streaming for interactive files.
 *
 */
int th_v30_interactive_stream(FILE *stream_in, FILE *stream_out,
		pq_header_t *pq_header, th_v30_header_t *th_header, 
		options_t *options) {
	int result;
	th_v30_interactive_t *interactive;
	int i;

	/* Read interactive header. */
	result = th_v30_interactive_read(stream_in, th_header, &interactive);
	if ( result != PQ_SUCCESS ) {
		error("Failed while reading interactive header.\n");
	} else {
		if ( options->print_header ) {
			pq_header_printf(stream_out, pq_header);
			th_v30_header_printf(stream_out, th_header);
			th_v30_interactive_header_printf(stream_out, th_header,
				&interactive);
		} else if ( options->print_resolution ) {
			for ( i = 0; i < th_header->NumberOfCurves; i++ ) {
				pq_resolution_print(stream_out, i,
						(interactive[i].Resolution*1e3), options);
			}
		} else { 
		/* Read and print interactive data. */
			th_v30_interactive_data_print(stream_out, th_header,
				&interactive, options);
		}
	}

	/* Clean and return. */
	debug("Freeing interactive header.\n");
	th_v30_interactive_free(th_header, &interactive);
	return(PQ_SUCCESS);
}

int th_v30_interactive_read(FILE *stream_in,
		th_v30_header_t *th_header,
		th_v30_interactive_t **interactive) {
	int i;
	size_t n_read;

	*interactive = (th_v30_interactive_t *)malloc(sizeof(th_v30_interactive_t)*
				th_header->NumberOfCurves);
	if ( *interactive == NULL ) {
		error("Could not allocate interactive data.\n");
		return(PQ_ERROR_MEM);
	}
	
	for ( i = 0; i < th_header->NumberOfCurves; i++ ) {
		n_read = fread(&(*interactive)[i],
				sizeof(th_v30_interactive_t) - sizeof(uint32_t *), 1, 
				stream_in);
		if ( n_read != 1 ) {
			error("Could not allocate memory for curve %"PRId32".\n", i);
			return(PQ_ERROR_IO);
		}

		(*interactive)[i].Counts = (uint32_t *)malloc(sizeof(uint32_t)*
				th_header->NumberOfChannels);
		if ( (*interactive)[i].Counts == NULL ) {
			error("Could not allocate memory for counts of curve "
					"%"PRId32".\n", i);
			return(PQ_ERROR_MEM);
		}

		n_read = fread((*interactive)[i].Counts,
				sizeof(uint32_t), th_header->NumberOfChannels, 
				stream_in);
		if ( n_read != th_header->NumberOfChannels ) {
			error("Could not read counts for curve %"PRId32".\n", i);
			return(PQ_ERROR_IO);
		}
	}

	return(PQ_SUCCESS);
}

void th_v30_interactive_free(th_v30_header_t *th_header,
		th_v30_interactive_t **interactive) {
	int i;
	for ( i = 0; i < th_header->NumberOfCurves; i++ ) {
		free((*interactive)[i].Counts);
	}
	free(*interactive);
}


void th_v30_interactive_header_printf(FILE *stream_out, 
		th_v30_header_t *th_header, 
		th_v30_interactive_t **interactive) {
	int i;

	for ( i = 0; i < th_header->NumberOfCurves; i++ ) {
		fprintf(stream_out, "Crv[%d].CurveIndex = %"PRId32"\n",
			i, (*interactive)[i].CurveIndex); 
		fprintf(stream_out, "Crv[%d].TimeOfRecording = %s",
			i, ctime32(&(*interactive)[i].TimeOfRecording));
		fprintf(stream_out, "Crv[%d].BoardSerial = %"PRId32"\n",
			i, (*interactive)[i].BoardSerial);
		fprintf(stream_out, "Crv[%d].CFDZeroCross = %"PRId32"\n",
			i, (*interactive)[i].CFDZeroCross);
		fprintf(stream_out, "Crv[%d].CFDDiscrMin = %"PRId32"\n",
			i, (*interactive)[i].CFDDiscrMin);
		fprintf(stream_out, "Crv[%d].SyncLevel = %"PRId32"\n",
			i, (*interactive)[i].SyncLevel);
		fprintf(stream_out, "Crv[%d].CurveOffset = %"PRId32"\n",
			i, (*interactive)[i].CurveOffset);
		fprintf(stream_out, "Crv[%d].RoutingChannel = %"PRId32"\n",
			i, (*interactive)[i].RoutingChannel);
		fprintf(stream_out, "Crv[%d].SubMode = %"PRId32"\n",
			i, (*interactive)[i].SubMode);
		fprintf(stream_out, "Crv[%d].MeasMode = %"PRId32"\n",
			i, (*interactive)[i].MeasMode);
		fprintf(stream_out, "Crv[%d].P1 = %"PRIf32"\n",
			i, (*interactive)[i].P1);
		fprintf(stream_out, "Crv[%d].P2 = %"PRIf32"\n",
			i, (*interactive)[i].P2);
		fprintf(stream_out, "Crv[%d].P3 = %"PRIf32"\n",
			i, (*interactive)[i].P3);
		fprintf(stream_out, "Crv[%d].RangeNo = %"PRId32"\n",
			i, (*interactive)[i].RangeNo);
		fprintf(stream_out, "Crv[%d].Offset = %"PRId32"\n",
			i, (*interactive)[i].Offset);
		fprintf(stream_out, "Crv[%d].AcquisitionTime = %"PRId32"\n",
			i, (*interactive)[i].AcquisitionTime);
		fprintf(stream_out, "Crv[%d].StopAfter = %"PRId32"\n",
			i, (*interactive)[i].StopAfter);
		fprintf(stream_out, "Crv[%d].StopReason = %"PRId32"\n",
			i, (*interactive)[i].StopReason);
		fprintf(stream_out, "Crv[%d].SyncRate = %"PRId32"\n",
			i, (*interactive)[i].SyncRate);
		fprintf(stream_out, "Crv[%d].CFDCountRate = %"PRId32"\n",
			i, (*interactive)[i].CFDCountRate);
		fprintf(stream_out, "Crv[%d].TDCCountRate = %"PRId32"\n",
			i, (*interactive)[i].TDCCountRate);
		fprintf(stream_out, "Crv[%d].IntegralCount = %"PRId32"\n",
			i, (*interactive)[i].IntegralCount);
		fprintf(stream_out, "Crv[%d].Resolution = %"PRIf32"\n",
			i, (*interactive)[i].Resolution);
		fprintf(stream_out, "Crv[%d].reserve1 = %"PRId32"\n",
			i, (*interactive)[i].reserve1);
		fprintf(stream_out, "Crv[%d].reserve2 = %"PRId32"\n",
			i, (*interactive)[i].reserve2);
	}
}

void th_v30_interactive_data_print(FILE *stream_out, 
		th_v30_header_t *th_header, 
		th_v30_interactive_t **interactive,
		options_t *options) {
	unsigned int i;
	int j;
	pq_interactive_bin_t bin;
	float64_t origin;
	float64_t time_step;

	pq_interactive_bin_print_t print;

	if ( options->binary_out ) {
		print = pq_interactive_bin_fwrite;
	} else {
		print = pq_interactive_bin_printf;
	}

	for ( i = 0; i < th_header->NumberOfCurves; i++ ) {
		bin.curve = i;
		
		origin = (*interactive)[i].Offset*1e3;
		time_step = (*interactive)[i].Resolution*1e3;
		for ( j = 0; j < th_header->NumberOfChannels; j++ ) { 
			bin.bin_left = origin + j*time_step;
			bin.bin_right = bin.bin_left + time_step;
			bin.counts = (*interactive)[i].Counts[j];
	
			print(stream_out, &bin);
		}
	}
}
