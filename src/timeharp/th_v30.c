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

#include "../timeharp.h"
#include "th_v30.h"

#include "../error.h"

int th_v30_dispatch(FILE *stream_in, FILE *stream_out, pq_header_t *pq_header,
		options_t *options) {
	int result = PQ_SUCCESS;
	th_v30_header_t *th_header;

	result = th_v30_header_read(stream_in, &th_header);
	if ( result != PQ_SUCCESS ) {
		error("Could not read Timeharp header.\n");
	} else {
		if ( options->print_mode ) { 
			if ( th_header->MeasurementMode == TH_MODE_INTERACTIVE ) {
				fprintf(stream_out, "interactive\n");
			} else if ( th_header->MeasurementMode == TH_MODE_TTTR ) {
				fprintf(stream_out, "t3\n");
			} else if ( th_header->MeasurementMode == TH_MODE_CONTINUOUS ) {
				fprintf(stream_out, "continuous\n");
			} else {
				error("Mode not recognized: %d\n", th_header->MeasurementMode);
				result = PQ_ERROR_MODE;
			}
		} else {	
			if ( th_header->MeasurementMode == TH_MODE_INTERACTIVE ) {
				result = th_v30_interactive_stream(stream_in, stream_out,
						pq_header, th_header, options);
			} else if ( th_header->MeasurementMode == TH_MODE_CONTINUOUS ) {
				error("Continuous mode for version 2.0 not yet supported.\n");
				result = PQ_ERROR_MODE;
			} else if ( th_header->MeasurementMode == TH_MODE_TTTR ) {
				result = th_v30_tttr_stream(stream_in, stream_out,
						pq_header, th_header, options);
			result = PQ_ERROR_MODE;
			} else {
				error("Mode not recognized: %d.\n", th_header->MeasurementMode);
				result = PQ_ERROR_MODE;
			}
		}
	}
	
	debug("Freeing Timeharp header.\n");
	th_v30_header_free(&th_header);
	return(result);
}

/* 
 *
 * Routines for the header common to all files.
 *
 */
int th_v30_header_read(FILE *stream_in, th_v30_header_t **th_header ) {
	size_t n_read;
	
	*th_header = (th_v30_header_t *)malloc(sizeof(th_v30_header_t));

	if ( *th_header == NULL ) {
		error("Could not allocate Timeharp header.\n");
		return(PQ_ERROR_MEM);
	}

	/* First, we want to read everything that is static. This is everything
	 * up for the board definitions, which we will pull after we know how
	 * many there are (th_header->NumberOfBoards)
	 */
	debug("Reading static part of common header.\n");
	n_read = fread(*th_header, 
			sizeof(th_v30_header_t) - sizeof(th_v30_board_t *), 
			1, stream_in);
	if ( n_read != 1 ) {
		error("Could not read Timeharp header.\n");
		th_v30_header_free(th_header);
		return(PQ_ERROR_IO);
	} 
	
	/* Now read the dynamic board data. */
	(*th_header)->Brd = (th_v30_board_t *)malloc(sizeof(th_v30_board_t)
			*(*th_header)->NumberOfBoards);

	if ( (*th_header)->Brd == NULL ) {
		error("Could not allocate board data.\n");
		th_v30_header_free(th_header);
		return(PQ_ERROR_MEM);
	}

	n_read = fread((*th_header)->Brd,
			sizeof(th_v30_board_t), 
			(*th_header)->NumberOfBoards, 
			stream_in);
	if ( n_read != (*th_header)->NumberOfBoards ) {
		error("Could not read board data.\n");
		th_v30_header_free(th_header);
		return(PQ_ERROR_IO);
	}

	debug("Finished reading common header.\n");
	return(PQ_SUCCESS);
}

void th_v30_header_free(th_v30_header_t **th_header) {
	if ( *th_header != NULL ) {
		free((*th_header)->Brd);
		free(*th_header);
	}
}

/* 
 *
 * Routines to print the various parameters.
 *
 */
void th_v30_header_printf(FILE *stream_out, 
		th_v30_header_t *th_header) {
	int i;

	fprintf(stream_out, "HardwareVersion = %.*s\n", 
			(int)sizeof(th_header->HardwareVersion), 
			th_header->HardwareVersion);
	fprintf(stream_out, "FileTime = %.*s\n",
			(int)sizeof(th_header->FileTime), th_header->FileTime);
	fprintf(stream_out, "Comment = %.*s\n",
			(int)sizeof(th_header->Comment), th_header->Comment);
	fprintf(stream_out, "NumberOfChannels = %"PRId32"\n", 
			th_header->NumberOfChannels);
	fprintf(stream_out, "NumberOfCurves = %"PRId32"\n",
			th_header->NumberOfCurves);
	fprintf(stream_out, "BitsPerChannel = %"PRId32"\n", 
			th_header->BitsPerChannel);
	fprintf(stream_out, "RoutingChannels = %"PRId32"\n", 
			th_header->RoutingChannels);
	fprintf(stream_out, "NumberOfBoards = %"PRId32"\n", 
			th_header->NumberOfBoards);
	fprintf(stream_out, "ActiveCurve = %"PRId32"\n", th_header->ActiveCurve);
	fprintf(stream_out, "MeasurementMode = %"PRId32"\n", 
			th_header->MeasurementMode);
	fprintf(stream_out, "HistogrammingMode = %"PRId32"\n", 
			th_header->HistogrammingMode);
	fprintf(stream_out, "RangeNo = %"PRId32"\n", th_header->RangeNo);
	fprintf(stream_out, "Offset = %"PRId32"\n", th_header->Offset);
	fprintf(stream_out, "AcquisitionTime = %"PRId32"\n", 
			th_header->AcquisitionTime);
	fprintf(stream_out, "StopAt = %"PRId32"\n", th_header->StopAt);
	fprintf(stream_out, "StopOnOvfl = %"PRId32"\n", th_header->StopOnOvfl);
	fprintf(stream_out, "Restart = %"PRId32"\n", th_header->Restart);
	fprintf(stream_out, "DisplayLinLog = %"PRId32"\n", 
			th_header->DisplayLinLog);
	fprintf(stream_out, "DisplayTimeAxisFrom = %"PRId32"\n", 
			th_header->DisplayTimeAxisFrom);
	fprintf(stream_out, "DisplayTimeAxisTo = %"PRId32"\n", 
			th_header->DisplayTimeAxisTo);
	fprintf(stream_out, "DisplayCountAxisTo = %"PRId32"\n",
			th_header->DisplayCountAxisTo);

	for ( i = 0; i < 8; i++ ) {
		fprintf(stream_out, "DisplayCurve[%d].MapTo = %"PRId32"\n", 
				i, th_header->DisplayCurve[i].MapTo);
		fprintf(stream_out, "DisplayCurve[%d].Show = %"PRId32"\n",
				i, th_header->DisplayCurve[i].Show);
	}

	for ( i = 0; i < 3; i++ ) {
		fprintf(stream_out, "Param[%d].Start = %"PRIf32"\n",
				i, th_header->Param[i].Start);
		fprintf(stream_out, "Param[%d].Step = %"PRIf32"\n",
				i, th_header->Param[i].Step);
		fprintf(stream_out, "Param[%d].Stop = %"PRIf32"\n",
				i, th_header->Param[i].Stop);
	}

	fprintf(stream_out, "RepeatMode = %"PRId32"\n", th_header->RepeatMode);
	fprintf(stream_out, "RepeatsPerCurve = %"PRId32"\n", 
			th_header->RepeatsPerCurve);
	fprintf(stream_out, "RepeatTime = %"PRId32"\n", th_header->RepeatTime);
	fprintf(stream_out, "RepeatWaitTime = %"PRId32"\n", 
			th_header->RepeatWaitTime);
	fprintf(stream_out, "ScriptName = %s\n", th_header->ScriptName);

	for ( i = 0; i < th_header->NumberOfBoards; i++ ) {
		fprintf(stream_out, "Brd[%d].BoardSerial = %"PRId32"\n",
				i, th_header->Brd[i].BoardSerial);
		fprintf(stream_out, "Brd[%d].CFDZeroCross = %"PRId32"\n",
				i, th_header->Brd[i].CFDZeroCross);
		fprintf(stream_out, "Brd[%d].CFDDiscriminatorMin = %"PRId32"\n",
				i, th_header->Brd[i].CFDDiscriminatorMin);
		fprintf(stream_out, "Brd[%d].SYNCLevel = %"PRId32"\n",
				i, th_header->Brd[i].SYNCLevel);
		fprintf(stream_out, "Brd[%d].Resolution = %"PRIf32"\n",
				i, th_header->Brd[i].Resolution);
	}
}

void th_v30_header_fwrite(FILE *stream_out, th_v30_header_t *th_header) {
	fwrite(th_header,
			sizeof(th_v30_header_t) - sizeof(uint32_t *),
			1,
			stream_out);
	fwrite(th_header->Brd,
			sizeof(th_v30_board_t),
			th_header->NumberOfBoards,
			stream_out);
}

