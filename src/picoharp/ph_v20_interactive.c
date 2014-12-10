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

#include "ph_v20.h"

#include "../picoharp.h"
#include "../interactive.h"
#include "../error.h"

/* 
 *
 * Streaming for interactive files.
 *
 */
int ph_v20_interactive_stream(FILE *stream_in, FILE *stream_out,
		pq_header_t *pq_header, ph_v20_header_t *ph_header, 
		options_t *options) {
	int result;
	ph_v20_interactive_t *interactive;
	int i;

	/* Read interactive header. */
	result = ph_v20_interactive_header_read(stream_in, ph_header, &interactive);

	if ( result != PQ_SUCCESS ) {
		error("Failed while reading interactive header.\n");
	} else {
		if ( options->print_header ) {
			if ( options->binary_out ) {
				pq_header_fwrite(stream_out, pq_header);
				ph_v20_header_fwrite(stream_out, ph_header);
				ph_v20_interactive_header_fwrite(stream_out, ph_header, 
					interactive);
			} else { 
				pq_header_printf(stream_out, pq_header);
				ph_v20_header_printf(stream_out, ph_header);
				ph_v20_interactive_header_printf(stream_out, ph_header,
					interactive);
			}
		} else if ( options->print_resolution ) {
			for ( i = 0; i < ph_header->NumberOfCurves; i++ ) {
				pq_resolution_print(stream_out, i, 
						(interactive->Curve[i].Resolution*1e3), options);
			}
		} else {
		/* Read and print interactive data. */
			result = ph_v20_interactive_data_read(stream_in, ph_header, 
					interactive);
	
			if ( result == PQ_SUCCESS ) {
				ph_v20_interactive_data_print(stream_out, ph_header,
						interactive, options);
			} else {
				error("Failed while reading interactive data.\n");
			}
			
			debug("Freeing interactive data.\n");
			ph_v20_interactive_data_free(ph_header, interactive);
		}
	}

	/* Clean and return. */

	debug("Freeing interactive header.\n");
	ph_v20_interactive_header_free(&interactive);
	return(result);
}

/*
 *
 * Interactive header routines.
 *
 */
ph_v20_interactive_t *ph_v20_interactive_header_alloc(int curves) {
	ph_v20_interactive_t *interactive;
	
	interactive = (ph_v20_interactive_t *)malloc(sizeof(ph_v20_interactive_t));

	if ( interactive != NULL ) {
		interactive->Curve = (ph_v20_curve_t *)malloc(
				sizeof(ph_v20_curve_t)*curves);
	}

	if ( interactive == NULL || interactive->Curve == NULL ) {
		ph_v20_interactive_header_free(&interactive);
		interactive = NULL;
	}

	return(interactive);
}

void ph_v20_interactive_header_free(ph_v20_interactive_t **interactive) {
	if ( *interactive != NULL ) {
		free((*interactive)->Curve);
	}

	free(*interactive);
}

int ph_v20_interactive_header_read(FILE *stream_in, 
		ph_v20_header_t *ph_header,
		ph_v20_interactive_t **interactive) {
	/* Read the static header data. This is a precusor to reading the
	 * curve data itself, but separating the two gives us a chance to 
	 * do some simpler debugging. These can be merged in the future for
	 * speed, but for now speed is not an issue. 
	 */
	int result;

	*interactive = (ph_v20_interactive_t *)malloc(sizeof(ph_v20_interactive_t));

	if ( *interactive == NULL ) {
		error("Could not allocate memory for interactive header.\n");
		ph_v20_interactive_header_free(interactive);
		return(PQ_ERROR_MEM);
	}

	(*interactive)->Curve = (ph_v20_curve_t *)malloc(
			ph_header->NumberOfCurves*sizeof(ph_v20_curve_t));

	if ( (*interactive)->Curve == NULL ) {
		error("Could not allocate memory for Picoharp curve headers.\n");
		ph_v20_interactive_header_free(interactive);
		return(PQ_ERROR_MEM);
	}

	debug("Reading curve header data.\n");
	result = fread((*interactive)->Curve, 
			sizeof(ph_v20_curve_t),
			ph_header->NumberOfCurves, 
			stream_in);

	if ( result != ph_header->NumberOfCurves ) {
		error( "Could not read Picoharp curve headers.\n");
		ph_v20_interactive_header_free(interactive);
		return(PQ_ERROR_IO);
	}

	return(PQ_SUCCESS);
}

void ph_v20_interactive_header_printf(FILE *stream_out, 
		ph_v20_header_t *ph_header, 
		ph_v20_interactive_t *interactive) {
	int i;

	for ( i = 0; i < ph_header->NumberOfCurves; i++ ) {
		fprintf(stream_out, "Crv[%d].CurveIndex = %"PRId32"\n",
			i, interactive->Curve[i].CurveIndex);
		fprintf(stream_out, "Crv[%d].TimeOfRecording = %s",
			i, ctime32(&interactive->Curve[i].TimeOfRecording));
		fprintf(stream_out, "Crv[%d].HardwareIdent = %s\n",
			i, interactive->Curve[i].HardwareIdent);
		fprintf(stream_out, "Crv[%d].HardwareVersion = %s\n",
			i, interactive->Curve[i].HardwareVersion);
		fprintf(stream_out, "Crv[%d].HardwareSerial = %"PRId32"\n",
			i, interactive->Curve[i].HardwareSerial);
		fprintf(stream_out, "Crv[%d].SyncDivider = %"PRId32"\n",
			i, interactive->Curve[i].SyncDivider);
		fprintf(stream_out, "Crv[%d].CFDZeroCross0 = %"PRId32"\n",
			i, interactive->Curve[i].CFDZeroCross0);
		fprintf(stream_out, "Crv[%d].CFDLevel0 = %"PRId32"\n",
			i, interactive->Curve[i].CFDLevel0);
		fprintf(stream_out, "Crv[%d].CFDZeroCross1 = %"PRId32"\n",
			i, interactive->Curve[i].CFDZeroCross1);
		fprintf(stream_out, "Crv[%d].CFDLevel1 = %"PRId32"\n",
			i, interactive->Curve[i].CFDLevel1);
		fprintf(stream_out, "Crv[%d].Offset = %"PRId32"\n",
			i, interactive->Curve[i].Offset);
		fprintf(stream_out, "Crv[%d].RoutingChannel = %"PRId32"\n",
			i, interactive->Curve[i].RoutingChannel);
		fprintf(stream_out, "Crv[%d].ExtDevices = %"PRId32"\n",
			i, interactive->Curve[i].ExtDevices);
		fprintf(stream_out, "Crv[%d].MeasMode = %"PRId32"\n",
			i, interactive->Curve[i].MeasMode);
		fprintf(stream_out, "Crv[%d].SubMode = %"PRId32"\n",
			i, interactive->Curve[i].SubMode);
		fprintf(stream_out, "Crv[%d].P1 = %"PRIf32"\n",
			i, interactive->Curve[i].P1);
		fprintf(stream_out, "Crv[%d].P2 = %"PRIf32"\n",
			i, interactive->Curve[i].P2);
		fprintf(stream_out, "Crv[%d].P3 = %"PRIf32"\n",
			i, interactive->Curve[i].P3);
		fprintf(stream_out, "Crv[%d].RangeNo = %"PRId32"\n",
			i, interactive->Curve[i].RangeNo);
		fprintf(stream_out, "Crv[%d].Resolution = %"PRIf32"\n",
			i, interactive->Curve[i].Resolution);
		fprintf(stream_out, "Crv[%d].Channels = %"PRId32"\n",
			i, interactive->Curve[i].Channels);
		fprintf(stream_out, "Crv[%d].AcquisitionTime = %"PRId32"\n",
			i, interactive->Curve[i].AcquisitionTime);
		fprintf(stream_out, "Crv[%d].StopAfter = %"PRId32"\n",
			i, interactive->Curve[i].StopAfter);
		fprintf(stream_out, "Crv[%d].StopReason = %"PRId32"\n",
			i, interactive->Curve[i].StopReason);
		fprintf(stream_out, "Crv[%d].InpRate0 = %"PRId32"\n",
			i, interactive->Curve[i].InpRate0);
		fprintf(stream_out, "Crv[%d].InpRate1 = %"PRId32"\n",
			i, interactive->Curve[i].InpRate1);
		fprintf(stream_out, "Crv[%d].HistCountRate = %"PRId32"\n",
			i, interactive->Curve[i].HistCountRate);
		fprintf(stream_out, "Crv[%d].IntegralCount = %"PRId64"\n",
			i, interactive->Curve[i].IntegralCount);
		fprintf(stream_out, "Crv[%d].Reserved = %"PRId32"\n",
			i, interactive->Curve[i].Reserved);
		fprintf(stream_out, "Crv[%d].DataOffset = %"PRId32"\n",
			i, interactive->Curve[i].DataOffset);
		fprintf(stream_out, "Crv[%d].RouterModelCode = %"PRId32"\n",
			i, interactive->Curve[i].RouterModelCode);
		fprintf(stream_out, "Crv[%d].RouterEnabled = %"PRId32"\n",
			i, interactive->Curve[i].RouterEnabled);
		fprintf(stream_out, "Crv[%d].RtCh_InputType = %"PRId32"\n",
			i, interactive->Curve[i].RtCh_InputType);
		fprintf(stream_out, "Crv[%d].RtCh_InputLevel = %"PRId32"\n",
			i, interactive->Curve[i].RtCh_InputLevel);
		fprintf(stream_out, "Crv[%d].RtCh_InputEdge = %"PRId32"\n",
			i, interactive->Curve[i].RtCh_InputEdge);
		fprintf(stream_out, "Crv[%d].RtCh_CFDPresent = %"PRId32"\n",
			i, interactive->Curve[i].RtCh_CFDPresent);
		fprintf(stream_out, "Crv[%d].RtCh_CFDLevel = %"PRId32"\n",
			i, interactive->Curve[i].RtCh_CFDLevel);
		fprintf(stream_out, "Crv[%d].RtCh_CFDZeroCross = %"PRId32"\n",
			i, interactive->Curve[i].RtCh_CFDZeroCross);
	}
}

void ph_v20_interactive_header_fwrite(FILE *stream_out, 
		ph_v20_header_t *ph_header, 
		ph_v20_interactive_t *interactive) {
	fwrite(interactive,
			sizeof(ph_v20_interactive_t),
			ph_header->NumberOfCurves,
			stream_out);
}

/*
 * Interactive data
 */
int ph_v20_interactive_data_read(FILE *stream_in,
		ph_v20_header_t *ph_header,
		ph_v20_interactive_t *interactive) {
	int result;
	int i;

	interactive->Counts = (uint32_t **)malloc(
			ph_header->NumberOfCurves*sizeof(uint32_t *));

	if ( interactive->Counts == NULL ) {
		error("Could not allocate memory for Picoharp curve data.\n");
		return(PQ_ERROR_MEM);
	}

	for ( i = 0; i < ph_header->NumberOfCurves; i++ ) {
		debug("Reading data for curve %d.\n", i);
		interactive->Counts[i] = (uint32_t *)malloc(
				interactive->Curve[i].Channels*sizeof(uint32_t));

		if ( interactive->Counts[i] == NULL ) {
			error("Could not allocate memory for Picoharp curve %d data.\n", i);
			return(PQ_ERROR_MEM);
		}

		result = fread(interactive->Counts[i], sizeof(uint32_t),
				interactive->Curve[i].Channels, stream_in);
		if ( result != interactive->Curve[i].Channels ) {
			error("Could not read data for Picoharp curve %d.\n", i);
			return(PQ_ERROR_IO);
		}
	}

	return(PQ_SUCCESS);
}

void ph_v20_interactive_data_free(ph_v20_header_t *ph_header,
		ph_v20_interactive_t *interactive) {
	int i;

	if ( interactive->Counts != NULL ) {
		for ( i = 0; i < ph_header->NumberOfCurves; i++ ) {
			free(interactive->Counts[i]);	
		}
		
		free(interactive->Counts);
	}
}

void ph_v20_interactive_data_print(FILE *stream_out, 
		ph_v20_header_t *ph_header, 
		ph_v20_interactive_t *interactive,
		options_t *options) {
	unsigned int i;
	int j;
	int64_t origin;	
	int64_t time_step;
	pq_interactive_bin_t bin;
	pq_interactive_bin_print_t print;

	if ( options->binary_out ) {
		print = pq_interactive_bin_fwrite;
	} else {
		print = pq_interactive_bin_printf;
	}

	for ( i = 0; i < ph_header->NumberOfCurves; i++ ) {
		bin.curve = i;
		origin = (int64_t)interactive->Curve[i].Offset;
		time_step = (int64_t)round(interactive->Curve[i].Resolution*1e3);
		for ( j = 0; j < interactive->Curve[i].Channels; j++ ) { 
			bin.bin_left = origin + time_step*j;
			bin.bin_right = bin.bin_left + time_step;
			bin.counts = interactive->Counts[i][j];
			print(stream_out, &bin);
		}
	}
}
