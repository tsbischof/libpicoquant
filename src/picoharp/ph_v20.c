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

#include "ph_v20.h"

#include "../picoharp.h"
#include "../error.h"

int ph_v20_dispatch(FILE *stream_in, FILE *stream_out, pq_header_t *pq_header,
		options_t *options) {
	int result = PQ_SUCCESS;
	ph_v20_header_t *ph_header;

	result = ph_v20_header_read(stream_in, &ph_header);
	if ( ph_header == NULL ) {
		error("Could not read PicoHarp header.\n");
	} else {
		if ( options->print_mode ) {
			if ( ph_header->MeasurementMode == PH_MODE_INTERACTIVE ) {
				fprintf(stream_out, "interactive\n");
			} else if (ph_header->MeasurementMode == PH_MODE_T2 ) {
				fprintf(stream_out, "t2\n");
			} else if (ph_header->MeasurementMode == PH_MODE_T3 ) {
				fprintf(stream_out, "t3\n");
			} else {
				error("Measurement mode not recognized: %"PRId32"\n",
						ph_header->MeasurementMode);
				result = PQ_ERROR_MODE;
			}
		} else {
			if ( ph_header->MeasurementMode == PH_MODE_INTERACTIVE ) {
				result = ph_v20_interactive_stream(stream_in, stream_out, 
						pq_header, ph_header, options);
			} else if ( ph_header->MeasurementMode == PH_MODE_T2  || 
						ph_header->MeasurementMode == PH_MODE_T3 ) {
				result = ph_v20_tttr_stream(stream_in, stream_out, pq_header,
						ph_header, options);
			} else {
				error("Picoharp measurement mode not recognized: %"PRId32".\n", 
						ph_header->MeasurementMode);
				result = PQ_ERROR_MODE;
			}
		} 
	}
	
	debug("Freeing Picoharp header.\n");
	ph_v20_header_free(&ph_header);
	return(result);
}

/* 
 *
 * Routines for the header common to all files.
 *
 */
ph_v20_header_t *ph_v20_header_alloc(int boards, int router_channels) {
	int result = PQ_SUCCESS;
	int i;
	ph_v20_header_t *ph_header = NULL;

	ph_header = (ph_v20_header_t *)malloc(sizeof(ph_v20_header_t));
	if ( ph_header != NULL ) {
		ph_header->Brd = (ph_v20_board_t *)malloc(
				sizeof(ph_v20_board_t)*boards);
		if ( ph_header->Brd != NULL ) {
			for ( i = 0; pq_check(result) && i < boards; i++ ) {
				ph_header->Brd[i].RtCh = (ph_v20_router_channel_t *)malloc(
						sizeof(ph_v20_router_channel_t)*router_channels);

				if ( ph_header->Brd[i].RtCh == NULL ) {
					result = PQ_ERROR_MEM;
				}
			}
		}
	}

	if ( pq_check(result) ) {	
		ph_v20_header_free(&ph_header);
		ph_header = NULL;
	}

	return(ph_header);
}


void ph_v20_header_free(ph_v20_header_t **ph_header) {
	int i;
	
	if (  *ph_header != NULL ) {
		if ( (*ph_header)->Brd != NULL ) {
			for ( i = 0; i < (*ph_header)->NumberOfBoards; i++ ) {
				free((*ph_header)->Brd[i].RtCh);
			}
		}

		free((*ph_header)->Brd);
	}

	free(*ph_header);
}

int ph_v20_header_read(FILE *stream_in, ph_v20_header_t **ph_header) {
	int i;
	size_t n_read;

	*ph_header = (ph_v20_header_t *)malloc(sizeof(ph_v20_header_t));

	if ( *ph_header == NULL ) {
		error("Could not allocate Picoharp header.\n");
		return(PQ_ERROR_MEM);
	}

	/* First, we want to read everything that is static. This is everything
	 * up for the board definitions, which we will pull after we know how
	 * many there are (ph_header->NumberOfBoards)
	 */
	n_read = fread(*ph_header, 
			sizeof(ph_v20_header_t) - sizeof(ph_v20_board_t *), 
			1, 
			stream_in);

	if ( n_read != 1 ) {
		error("Could not read Picoharp header.\n");
		ph_v20_header_free(ph_header);
		return(PQ_ERROR_IO);
	} 
	
	/* Now read the dynamic board data. */
	debug("Allocating board memory for common header.\n");
	(*ph_header)->Brd = (ph_v20_board_t *)malloc(
			(*ph_header)->NumberOfBoards*sizeof(ph_v20_board_t));

	if ( (*ph_header)->Brd == NULL ) {
		error("Could not allocate board memory.\n");
		ph_v20_header_free(ph_header);
		return(PQ_ERROR_MEM);
	}

	/* To do: the manual (and files structure) indicate that 
	 * ph_v20_header->RoutingChannels must always be 4, but the 
	 * actual value in a file is often 1. Add functionality to 
	 * deal with this appropriately.
	 */
	debug("Warning: checking value of RoutingChannels. "
			"See picoharp.c for details.\n");
	(*ph_header)->RoutingChannels = 4;
	
	/* Each board also has many router channels. */
	for ( i = 0; i < (*ph_header)->NumberOfBoards; i++ ) {
		debug("Reading static data for board %d.\n", i);
		n_read = fread(&(*ph_header)->Brd[i], 
			sizeof(ph_v20_board_t) - sizeof(ph_v20_router_channel_t *), 
			1, 
			stream_in);

		if ( n_read != 1 ) {
			ph_v20_header_free(ph_header);
			return(PQ_ERROR_IO);
		}

		debug( "Allocating router channel data for board %d.\n", i);
		(*ph_header)->Brd[i].RtCh = (ph_v20_router_channel_t *)malloc(
				(*ph_header)->RoutingChannels
				*sizeof(ph_v20_router_channel_t));
		if ( (*ph_header)->Brd[i].RtCh == NULL ) {
			ph_v20_header_free(ph_header);
			return(PQ_ERROR_MEM);
		}

		debug( "Reading router channel data for board %d.\n", i);
		n_read = fread((*ph_header)->Brd[i].RtCh, 
				sizeof(ph_v20_router_channel_t), 
				(*ph_header)->RoutingChannels, stream_in);
		if ( n_read != (*ph_header)->RoutingChannels ) {
			ph_v20_header_free(ph_header);
			return(PQ_ERROR_IO);
		}
	}

	return(PQ_SUCCESS);
}

void ph_v20_header_printf(FILE *stream_out, ph_v20_header_t *ph_header) {
	int i;
	int j;

	fprintf(stream_out, "CreatorName = %.*s\n",
			(int)sizeof(ph_header->CreatorName), ph_header->CreatorName);
	fprintf(stream_out, "CreatorVersion = %.*s\n",
			(int)sizeof(ph_header->CreatorVersion), ph_header->CreatorVersion);
	fprintf(stream_out, "FileTime = %.*s\n",
			(int)sizeof(ph_header->FileTime), ph_header->FileTime);
	fprintf(stream_out, "Comment = %.*s\n",
			(int)sizeof(ph_header->Comment), ph_header->Comment);
	fprintf(stream_out, "NumberOfCurves = %"PRId32"\n", 
			ph_header->NumberOfCurves);
	fprintf(stream_out, "BitsPerRecord = %"PRId32"\n", 
			ph_header->BitsPerRecord);
	fprintf(stream_out, "RoutingChannels = %"PRId32"\n", 
			ph_header->RoutingChannels);
	fprintf(stream_out, "NumberOfBoards = %"PRId32"\n", 
			ph_header->NumberOfBoards);
	fprintf(stream_out, "ActiveCurve = %"PRId32"\n", ph_header->ActiveCurve);
	fprintf(stream_out, "MeasurementMode = %"PRId32"\n", 
			ph_header->MeasurementMode);
	fprintf(stream_out, "SubMode = %"PRId32"\n", ph_header->SubMode);
	fprintf(stream_out, "RangeNo = %"PRId32"\n", ph_header->RangeNo);
	fprintf(stream_out, "Offset = %"PRId32"\n", ph_header->Offset);
	fprintf(stream_out, "AcquisitionTime = %"PRId32"\n", 
			ph_header->AcquisitionTime);
	fprintf(stream_out, "StopAt = %"PRId32"\n", ph_header->StopAt);
	fprintf(stream_out, "StopOnOvfl = %"PRId32"\n", ph_header->StopOnOvfl);
	fprintf(stream_out, "Restart = %"PRId32"\n", ph_header->Restart);
	fprintf(stream_out, "DisplayLinLog = %"PRId32"\n", 
			ph_header->DisplayLinLog);
	fprintf(stream_out, "DisplayTimeAxisFrom = %"PRId32"\n", 
			ph_header->DisplayTimeAxisFrom);
	fprintf(stream_out, "DisplayTimeAxisTo = %"PRId32"\n", 
			ph_header->DisplayTimeAxisTo);
	fprintf(stream_out, "DisplayCountAxisTo = %"PRId32"\n",
			ph_header->DisplayCountAxisTo);

	for ( i = 0; i < 8; i++ ) {
		fprintf(stream_out, "DisplayCurve[%d].MapTo = %"PRId32"\n", 
				i, ph_header->DisplayCurve[i].MapTo);
		fprintf(stream_out, "DisplayCurve[%d].Show = %"PRId32"\n",
				i, ph_header->DisplayCurve[i].Show);
	}

	for ( i = 0; i < 3; i++ ) {
		fprintf(stream_out, "Param[%d].Start = %"PRIf32"\n",
				i, ph_header->Param[i].Start);
		fprintf(stream_out, "Param[%d].Step = %"PRIf32"\n",
				i, ph_header->Param[i].Step);
		fprintf(stream_out, "Param[%d].Stop = %"PRIf32"\n",
				i, ph_header->Param[i].Stop);
	}

	fprintf(stream_out, "RepeatMode = %"PRId32"\n", ph_header->RepeatMode);
	fprintf(stream_out, "RepeatsPerCurve = %"PRId32"\n", 
			ph_header->RepeatsPerCurve);
	fprintf(stream_out, "RepeatTime = %"PRId32"\n", ph_header->RepeatTime);
	fprintf(stream_out, "RepeatWaitTime = %"PRId32"\n", 
			ph_header->RepeatWaitTime);
	fprintf(stream_out, "ScriptName = %s\n", ph_header->ScriptName);

	for ( i = 0; i < ph_header->NumberOfBoards; i++ ) {
		fprintf(stream_out, "Brd[%d].HardwareIdent = %s\n",
				i, ph_header->Brd[i].HardwareIdent);
		fprintf(stream_out, "Brd[%d].HardwareVersion = %s\n",
				i, ph_header->Brd[i].HardwareVersion);
		fprintf(stream_out, "Brd[%d].HardwareSerial = %"PRId32"\n",
				i, ph_header->Brd[i].HardwareSerial);
		fprintf(stream_out, "Brd[%d].SyncDivider = %"PRId32"\n",
				i, ph_header->Brd[i].SyncDivider);
		fprintf(stream_out, "Brd[%d].CFDZeroCross0 = %"PRId32"\n",
				i, ph_header->Brd[i].CFDZeroCross0);
		fprintf(stream_out, "Brd[%d].CFDLevel0 = %"PRId32"\n",
				i, ph_header->Brd[i].CFDLevel0);
		fprintf(stream_out, "Brd[%d].CFDZeroCross1 = %"PRId32"\n",
				i, ph_header->Brd[i].CFDZeroCross1);
		fprintf(stream_out, "Brd[%d].CFDLevel1 = %"PRId32"\n",
				i, ph_header->Brd[i].CFDLevel1);
		fprintf(stream_out, "Brd[%d].Resolution = %"PRIf32"\n",
				i, ph_header->Brd[i].Resolution);
		fprintf(stream_out, "Brd[%d].RouterModelCode = %"PRId32"\n",
				i, ph_header->Brd[i].RouterModelCode);
		fprintf(stream_out, "Brd[%d].RouterEnabled = %"PRId32"\n",
				i, ph_header->Brd[i].RouterEnabled);

		for ( j = 0; j < ph_header->RoutingChannels; j++ ) {
			fprintf(stream_out, 
					"Brd[%d].RtCh[%d].InputType = %"PRId32"\n",
					i, j, 
				ph_header->Brd[i].RtCh[j].InputType);
			fprintf(stream_out, 
					"Brd[%d].RtCh[%d].InputLevel = %"PRId32"\n",
					i, j, 
				ph_header->Brd[i].RtCh[j].InputLevel);
			fprintf(stream_out, 
					"Brd[%d].RtCh[%d].InputEdge = %"PRId32"\n",
					i, j, 
				ph_header->Brd[i].RtCh[j].InputEdge);
			fprintf(stream_out, 
					"Brd[%d].RtCh[%d].CFDPresent = %"PRId32"\n",
					i, j, 
				ph_header->Brd[i].RtCh[j].CFDPresent);
			fprintf(stream_out, 
					"Brd[%d].RtCh[%d].CFDLevel = %"PRId32"\n",
					i, j,
				ph_header->Brd[i].RtCh[j].CFDLevel);
			fprintf(stream_out,
					"Brd[%d].RtCh[%d].CFDZCross = %"PRId32"\n",
					i, j,
				ph_header->Brd[i].RtCh[j].CFDZCross);
		}
	}
}

void ph_v20_header_fwrite(FILE *stream_out, ph_v20_header_t *ph_header) {
	int i;

	fwrite(ph_header, 
			sizeof(ph_v20_header_t) - sizeof(ph_v20_board_t *), 
			1, 
			stream_out);
	
	for ( i = 0; i < ph_header->NumberOfBoards; i++ ) {
		fwrite(&(ph_header->Brd[i]), 
				sizeof(ph_v20_board_t) - sizeof(ph_v20_router_channel_t *),
				1,
				stream_out);

		fwrite(&(ph_header->Brd[i].RtCh),
				sizeof(ph_v20_router_channel_t),
				ph_header->RoutingChannels,
				stream_out);
	}
}

