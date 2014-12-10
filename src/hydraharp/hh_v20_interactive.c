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

#include "hh_v20.h"

#include "../hydraharp.h"
#include "../header.h"
#include "../error.h"

int hh_v20_interactive_stream(FILE *stream_in, FILE *stream_out,
		pq_header_t *pq_header, hh_v20_header_t *hh_header, 
		options_t *options) {
	int result;
	hh_v20_interactive_t *interactive;
	int i;

	/* Read interactive header. */
	result = hh_v20_interactive_header_read(stream_in, 
			hh_header, &interactive);
	if ( result != PQ_SUCCESS ) {
		error("Failed while reading interactive header.\n");
	} else {
		if ( options->print_header ) {
			if ( options->binary_out ) {
				pq_header_fwrite(stream_out, pq_header);
				hh_v20_header_fwrite(stream_out, hh_header);
				hh_v20_interactive_header_fwrite(stream_out, hh_header,
						interactive);
			} else {
				pq_header_printf(stream_out, pq_header);
				hh_v20_header_printf(stream_out, hh_header);
				hh_v20_interactive_header_printf(stream_out, hh_header,
						interactive);
			}
		} else if ( options->print_resolution ) {
			for ( i = 0; i < hh_header->NumberOfCurves; i++ ) {
				pq_resolution_print(stream_out, i, 
						interactive->Curve[i].Resolution, options);
			}
		} else {
		/* Read and print interactive data. */
			result = hh_v20_interactive_data_read(stream_in, hh_header, 
					interactive);

			if ( result == PQ_SUCCESS ) {
				hh_v20_interactive_data_print(stream_out, hh_header,
						interactive, options);
			} else {
				error("Failed while reading interactive data.\n");
			}

			debug("Freeing interactive data.\n");
			hh_v20_interactive_data_free(hh_header, interactive);
		}
	}

	/* Clean and return. */

	debug("Freeing interactive header.\n");
	hh_v20_interactive_header_free(&interactive);
	return(result);
}

int hh_v20_interactive_header_read(FILE *stream_in, 
		hh_v20_header_t *hh_header,
		hh_v20_interactive_t **interactive) {
	/* Read the static header data. This is a precusor to reading the
	 * curve data itself, but separating the two gives us a chance to 
	 * do some simpler debugging. These can be merged in the future for
	 * speed, but for now speed is not an issue. 
	 */
	size_t n_read;
	
	*interactive = (hh_v20_interactive_t *)malloc(sizeof(hh_v20_interactive_t));
	
	if ( *interactive == NULL ) {
		error("Could not allocate memory for interactive header.\n");
		hh_v20_interactive_header_free(interactive);
		return(PQ_ERROR_MEM);
	}

	(*interactive)->Curve = (hh_v20_curve_t *)malloc(
			hh_header->NumberOfCurves*sizeof(hh_v20_curve_t));

	if ( (*interactive)->Curve == NULL ) {
		error("Could not allocate memory for curve headers.\n");
		hh_v20_interactive_header_free(interactive);
		return(PQ_ERROR_MEM);
	}

	debug("Reading curve header data.\n");
	n_read = fread((*interactive)->Curve, 
			sizeof(hh_v20_curve_t),
			hh_header->NumberOfCurves, 
			stream_in);

	if ( n_read != hh_header->NumberOfCurves ) {
		error("Could not read curve headers.\n");
		hh_v20_interactive_header_free(interactive);
		return(PQ_ERROR_IO);
	}

	return(PQ_SUCCESS);
}

void hh_v20_interactive_header_free(hh_v20_interactive_t **interactive) {
	if ( *interactive != NULL ) {
		free((*interactive)->Curve);
		free(*interactive);
	}
}

void hh_v20_interactive_header_printf(FILE *stream_out, 
		hh_v20_header_t *hh_header, 
		hh_v20_interactive_t *interactive) {
	int i;
	int j;

	for ( i = 0; i < hh_header->NumberOfCurves; i++ ) {
		fprintf(stream_out, "Curve[%d].CurveIndex = %"PRId32"\n",
			i, interactive->Curve[i].CurveIndex);
		fprintf(stream_out, "Curve[%d].TimeOfRecording = %s",
			i, ctime32(&interactive->Curve[i].TimeOfRecording));
		fprintf(stream_out, "Curve[%d].HardwareIdent = %.*s\n",
			i, 
			(int)sizeof(interactive->Curve[i].HardwareIdent),
			interactive->Curve[i].HardwareIdent);
		fprintf(stream_out, "Curve[%d].HardwareVersion = %.*s\n",
			i, 
			(int)sizeof(interactive->Curve[i].HardwareVersion),
			interactive->Curve[i].HardwareVersion);
		fprintf(stream_out, "Curve[%d].HardwareSerial = %"PRId32"\n",
			i, interactive->Curve[i].HardwareSerial);
		fprintf(stream_out, "Curve[%d].NoOfModules = %"PRId32"\n",
			i, interactive->Curve[i].NoOfModules);

		for ( j = 0; j < hh_header->NumberOfModules; j++ ) {
			fprintf(stream_out, "Curve[%d].Module[%d].Model = %"PRId32"\n",
				i, j, interactive->Curve[i].Module[j].Model);
			fprintf(stream_out, "Curve[%d].Module[%d].Version = %"PRId32"\n",
				i, j, interactive->Curve[i].Module[j].Version);
		}

		fprintf(stream_out, "Curve[%d].BaseResolution = %"PRIf64"\n",
			i, interactive->Curve[i].BaseResolution);
		fprintf(stream_out, "Curve[%d].InputsEnabled = %"PRId64"\n",
			i, interactive->Curve[i].InputsEnabled);
		fprintf(stream_out, "Curve[%d].InpChanPresent = %"PRId32"\n",
			i, interactive->Curve[i].InpChanPresent);
		fprintf(stream_out, "Curve[%d].RefClockSource = %"PRId32"\n",
			i, interactive->Curve[i].RefClockSource);
		fprintf(stream_out, "Curve[%d].ExtDevices = %"PRId32"\n",
			i, interactive->Curve[i].ExtDevices);
		fprintf(stream_out, "Curve[%d].MarkerSettings = %"PRId32"\n",
			i, interactive->Curve[i].MarkerSettings);
		fprintf(stream_out, "Curve[%d].SyncDivider = %"PRId32"\n",
			i, interactive->Curve[i].SyncDivider);
		fprintf(stream_out, "Curve[%d].SyncCFDLevel = %"PRId32"\n",
			i, interactive->Curve[i].SyncCFDLevel);
		fprintf(stream_out, "Curve[%d].SyncCFDZero = %"PRId32"\n",
			i, interactive->Curve[i].SyncCFDZero);
		fprintf(stream_out, "Curve[%d].SyncOffset = %"PRId32"\n",
			i, interactive->Curve[i].SyncOffset);
		fprintf(stream_out, "Curve[%d].InpModuleIdx = %"PRId32"\n",
			i, interactive->Curve[i].InpModuleIdx);
		fprintf(stream_out, "Curve[%d].InpCFDLevel = %"PRId32"\n",
			i, interactive->Curve[i].InpCFDLevel);
		fprintf(stream_out, "Curve[%d].InpCFDZeroCross = %"PRId32"\n",
			i, interactive->Curve[i].InpCFDZeroCross);
		fprintf(stream_out, "Curve[%d].InpOffset = %"PRId32"\n",
			i, interactive->Curve[i].InpOffset);
		fprintf(stream_out, "Curve[%d].InpChannel = %"PRId32"\n",
			i, interactive->Curve[i].InpChannel);
		fprintf(stream_out, "Curve[%d].MeasMode = %"PRId32"\n",
			i, interactive->Curve[i].MeasMode);
		fprintf(stream_out, "Curve[%d].SubMode = %"PRId32"\n",
			i, interactive->Curve[i].SubMode);
		fprintf(stream_out, "Curve[%d].Binning = %"PRId32"\n",
			i, interactive->Curve[i].Binning);
		fprintf(stream_out, "Curve[%d].Resolution = %"PRIf64"\n",
			i, interactive->Curve[i].Resolution);
		fprintf(stream_out, "Curve[%d].Offset = %"PRId32"\n",
			i, interactive->Curve[i].Offset);
		fprintf(stream_out, "Curve[%d].AcquisitionTime = %"PRId32"\n",
			i, interactive->Curve[i].AcquisitionTime);
		fprintf(stream_out, "Curve[%d].StopAfter = %"PRId32"\n",
			i, interactive->Curve[i].StopAfter);
		fprintf(stream_out, "Curve[%d].StopReason = %"PRId32"\n",
			i, interactive->Curve[i].StopReason);
		fprintf(stream_out, "Curve[%d].P1 = %"PRIf32"\n",
			i, interactive->Curve[i].P1);
		fprintf(stream_out, "Curve[%d].P2 = %"PRIf32"\n",
			i, interactive->Curve[i].P2);
		fprintf(stream_out, "Curve[%d].P3 = %"PRIf32"\n",
			i, interactive->Curve[i].P3);
		fprintf(stream_out, "Curve[%d].SyncRate = %"PRId32"\n",
			i, interactive->Curve[i].SyncRate);
		fprintf(stream_out, "Curve[%d].InputRate = %"PRId32"\n",
			i, interactive->Curve[i].InputRate);
		fprintf(stream_out, "Curve[%d].HistCountRate = %"PRId32"\n",
			i, interactive->Curve[i].HistCountRate);
		fprintf(stream_out, "Curve[%d].IntegralCount = %"PRId64"\n",
			i, interactive->Curve[i].IntegralCount);
		fprintf(stream_out, "Curve[%d].HistogramBins = %"PRId32"\n",
			i, interactive->Curve[i].HistogramBins);
		fprintf(stream_out, "Curve[%d].DataOffset = %"PRId32"\n",
			i, interactive->Curve[i].DataOffset);
	}
}

void hh_v20_interactive_header_fwrite(FILE *stream_out,
		hh_v20_header_t *hh_header,
		hh_v20_interactive_t *interactive) {
	fwrite(interactive->Curve,
			sizeof(hh_v20_curve_t),
			hh_header->NumberOfCurves,
			stream_out);
}


/*
 * Interactive data.
 */
int hh_v20_interactive_data_read(FILE *stream_in,
		hh_v20_header_t *hh_header,
		hh_v20_interactive_t *interactive) {
	/* Now the curve data. Allocate this as a 2d array and populate it 
	 * as such.
	 */
	size_t n_read;
	int i;

	interactive->Counts = (uint32_t **)malloc(
			hh_header->NumberOfCurves*sizeof(uint32_t *));

	if ( interactive->Counts == NULL ) {
		error("Could not allocate memory for curve data.\n");
		return(PQ_ERROR_MEM);
	}

	for ( i = 0; i < hh_header->NumberOfCurves; i++ ) {
		debug("Reading data for curve %d.\n", i);
		interactive->Counts[i] = (uint32_t *)malloc(
				interactive->Curve[i].HistogramBins*sizeof(uint32_t)); 

		if ( interactive->Counts[i] == NULL ) {
			error("Could not allocate memory for curve %d data.\n", i);
			return(PQ_ERROR_MEM);
		}

		n_read = fread(interactive->Counts[i], sizeof(uint32_t),
				interactive->Curve[i].HistogramBins, stream_in);
		if ( n_read != interactive->Curve[i].HistogramBins ) {
			error("Could not read data for curve %d.\n", i);
			return(PQ_ERROR_IO);
		}
	}
	return(PQ_SUCCESS);
}

void hh_v20_interactive_data_free(hh_v20_header_t *hh_header,
		hh_v20_interactive_t *interactive) {
	int i;

	if ( interactive->Counts != NULL ) {
		for ( i = 0; i < hh_header->NumberOfCurves; i++ ) {
			free(interactive->Counts[i]);	
		}

		free(interactive->Counts);
	}
}

void hh_v20_interactive_data_print(FILE *stream_out, 
		hh_v20_header_t *hh_header, 
		hh_v20_interactive_t *interactive,
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

	for ( i = 0; i < hh_header->NumberOfCurves; i++ ) {
		bin.curve = i;
		origin = (int64_t)(interactive->Curve[i].Offset*1e3);
		time_step = (int64_t)round(interactive->Curve[i].Resolution);
		for ( j = 0; j < interactive->Curve[i].HistogramBins; j++ ) { 
			bin.bin_left = origin + time_step*j;
			bin.bin_right = bin.bin_left + time_step;
			bin.counts = interactive->Counts[i][j];
			print(stream_out, &bin);
		}
	}
}
