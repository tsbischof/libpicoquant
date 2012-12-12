#include <stdlib.h>

#include "../timeharp.h"
#include "th_v60.h"

#include "../error.h"

int th_v60_dispatch(FILE *in_stream, FILE *out_stream, pq_header_t *pq_header,
		options_t *options) {
	int result;
	th_v60_header_t th_header;

	result = th_v60_header_read(in_stream, &th_header, options);
	if ( result ) {
		error("Could not read Timeharp header.\n");
	} else {
		if ( th_header.MeasurementMode == TH_MODE_INTERACTIVE ) {
			result = th_v60_interactive_stream(in_stream, out_stream, 
					pq_header, &th_header, options);
		} else if ( th_header.MeasurementMode == TH_MODE_CONTINUOUS ) {
			error("Continuous mode for version 6.0 not yet supported.\n");
			result = PQ_MODE_ERROR;
		} else if ( th_header.MeasurementMode == TH_MODE_TTTR ) {
			error("TTTR mode for version 6.0 not yet supported.\n");
		result = PQ_MODE_ERROR;
		} else {
			error("Mode not recognized: %d.\n", th_header.MeasurementMode);
			result = PQ_MODE_ERROR;
		}
	}
	
	th_v60_header_free(&th_header);
	
	return(result);
}

/* 
 *
 * Routines for the header common to all files.
 *
 */
int th_v60_header_read(FILE *in_stream, th_v60_header_t *th_header,
		options_t *options ) {
	int result;
	
	/* First, we want to read everything that is static. This is everything
	 * up for the board definitions, which we will pull after we know how
	 * many there are (th_header->NumberOfBoards)
	 */
	debug("Reading static part of common header.\n");
	result = fread(th_header, 
			sizeof(th_v60_header_t) - sizeof(th_v60_board_t *), 
			1, in_stream);
	if ( result != 1 ) {
		return(PQ_READ_ERROR);
	} 
	
	/* Now read the dynamic board data. */
	th_header->Brd = (th_v60_board_t *)malloc(sizeof(th_v60_board_t)
			*th_header->NumberOfBoards);

	if ( th_header->Brd == NULL ) {
		return(PQ_MEM_ERROR);
	}

	result = fread(th_header->Brd,
			sizeof(th_v60_board_t), th_header->NumberOfBoards, in_stream);
	if ( result != th_header->NumberOfBoards ) {
		return(PQ_READ_ERROR);
	}

	debug("Finished reading common header.\n");

	return(PQ_SUCCESS);
}

void th_v60_header_free(th_v60_header_t *th_header) {
	free(th_header->Brd);
}

/*
 *
 * Routines for interactive files.
 *
 */

int th_v60_interactive_read(FILE *in_stream,
		th_v60_header_t *th_header,
		th_v60_interactive_t **interactive, options_t *options) {
	int i;
	int result;

	*interactive = (th_v60_interactive_t *)malloc(sizeof(th_v60_interactive_t)*
				th_header->NumberOfCurves);
	if ( *interactive == NULL ) {
		return(PQ_MEM_ERROR);
	}
	
	for ( i = 0; i < th_header->NumberOfCurves; i++ ) {
		result = fread(&(*interactive)[i],
				sizeof(th_v60_interactive_t) - sizeof(uint32_t *), 1, 
				in_stream);
		if ( result != 1 ) {
			error("Could not allocate memory for curve %"PRId32".\n", i);
			return(PQ_READ_ERROR);
		}

		(*interactive)[i].Counts = (uint32_t *)malloc(sizeof(uint32_t)*
				th_header->NumberOfChannels);
		if ( (*interactive)[i].Counts == NULL ) {
			error("Could not allocate memory for counts of curve %"PRId32".\n", i);
			return(PQ_MEM_ERROR);
		}

		result = fread((*interactive)[i].Counts,
				sizeof(uint32_t), th_header->NumberOfChannels, 
				in_stream);
		if ( result != th_header->NumberOfChannels ) {
			error("Could not read counts for curve %"PRId32".\n", i);
			return(PQ_READ_ERROR);
		}
	}

	return(PQ_SUCCESS);
}

void th_v60_interactive_free(th_v60_interactive_t **interactive, 
		th_v60_header_t *th_header) {
	int i;
	for ( i = 0; i < th_header->NumberOfCurves; i++ ) {
		free((*interactive)[i].Counts);
	}
	free(*interactive);
}


/* 
 *
 * Streaming for interactive files.
 *
 */
int th_v60_interactive_stream(FILE *in_stream, FILE *out_stream,
		pq_header_t *pq_header, th_v60_header_t *th_header, 
		options_t *options) {
	int result;
	th_v60_interactive_t *interactive;
	int i;

	/* Read interactive header. */
	result = th_v60_interactive_read(in_stream, th_header, 
			&interactive, options);
	if ( result != PQ_SUCCESS ) {
		error("Failed while reading interactive header.\n");
	} 

	if ( options->print_header ) {
		pq_header_print(out_stream, pq_header);
		th_v60_header_print(out_stream, th_header);
		th_v60_interactive_header_print(out_stream, th_header,
			&interactive);
	} else if ( options->print_resolution) { 
		for ( i = 0; i < th_header->NumberOfCurves; i++ ) {
			fprintf(out_stream, "%d,", i);
			print_resolution(out_stream, 
					interactive[i].Resolution*1e3, options);
		}
	} else { 
	/* Read and print interactive data. */
		th_v60_interactive_data_print(out_stream, th_header,
			&interactive, options);
	}

	/* Clean and return. */
	debug("Freeing interactive header.\n");
	th_v60_interactive_free(&interactive, th_header);
	return(PQ_SUCCESS);
}

/* 
 *
 * Routines to print the various parameters.
 *
 */
void th_v60_header_print(FILE *out_stream, 
		th_v60_header_t *th_header) {
	int i;

	fprintf(out_stream, "CreatorName = %s\n",
			th_header->CreatorName);
	fprintf(out_stream, "CreatorVersion = %s\n", 
			th_header->CreatorVersion);
	fprintf(out_stream, "FileTime = %s\n",
			th_header->FileTime);
	fprintf(out_stream, "Comment = %s\n",
			th_header->Comment);
	fprintf(out_stream, "NumberOfChannels = %"PRId32"\n", 
			th_header->NumberOfChannels);
	fprintf(out_stream, "NumberOfCurves = %"PRId32"\n",
			th_header->NumberOfCurves);
	fprintf(out_stream, "BitsPerChannel = %"PRId32"\n", 
			th_header->BitsPerChannel);
	fprintf(out_stream, "RoutingChannels = %"PRId32"\n", 
			th_header->RoutingChannels);
	fprintf(out_stream, "NumberOfBoards = %"PRId32"\n", 
			th_header->NumberOfBoards);
	fprintf(out_stream, "ActiveCurve = %"PRId32"\n", th_header->ActiveCurve);
	fprintf(out_stream, "MeasurementMode = %"PRId32"\n", 
			th_header->MeasurementMode);
	fprintf(out_stream, "SubMode = %"PRId32"\n", th_header->SubMode);
	fprintf(out_stream, "RangeNo = %"PRId32"\n", th_header->RangeNo);
	fprintf(out_stream, "Offset = %"PRId32"\n", th_header->Offset);
	fprintf(out_stream, "AcquisitionTime = %"PRId32"\n", 
			th_header->AcquisitionTime);
	fprintf(out_stream, "StopAt = %"PRId32"\n", th_header->StopAt);
	fprintf(out_stream, "StopOnOvfl = %"PRId32"\n", th_header->StopOnOvfl);
	fprintf(out_stream, "Restart = %"PRId32"\n", th_header->Restart);
	fprintf(out_stream, "DisplayLinLog = %"PRId32"\n", 
			th_header->DisplayLinLog);
	fprintf(out_stream, "DisplayTimeAxisFrom = %"PRId32"\n", 
			th_header->DisplayTimeAxisFrom);
	fprintf(out_stream, "DisplayTimeAxisTo = %"PRId32"\n", 
			th_header->DisplayTimeAxisTo);
	fprintf(out_stream, "DisplayCountAxisTo = %"PRId32"\n",
			th_header->DisplayCountAxisTo);

	for ( i = 0; i < 8; i++ ) {
		fprintf(out_stream, "DisplayCurve[%d].MapTo = %"PRId32"\n", 
				i, th_header->DisplayCurve[i].MapTo);
		fprintf(out_stream, "DisplayCurve[%d].Show = %"PRId32"\n",
				i, th_header->DisplayCurve[i].Show);
	}

	for ( i = 0; i < 3; i++ ) {
		fprintf(out_stream, "Param[%d].Start = %"PRIf32"\n",
				i, th_header->Param[i].Start);
		fprintf(out_stream, "Param[%d].Step = %"PRIf32"\n",
				i, th_header->Param[i].Step);
		fprintf(out_stream, "Param[%d].Stop = %"PRIf32"\n",
				i, th_header->Param[i].Stop);
	}

	fprintf(out_stream, "RepeatMode = %"PRId32"\n", th_header->RepeatMode);
	fprintf(out_stream, "RepeatsPerCurve = %"PRId32"\n", 
			th_header->RepeatsPerCurve);
	fprintf(out_stream, "RepeatTime = %"PRId32"\n", th_header->RepeatTime);
	fprintf(out_stream, "RepeatWaitTime = %"PRId32"\n", 
			th_header->RepeatWaitTime);
	fprintf(out_stream, "ScriptName = %s\n", th_header->ScriptName);

	for ( i = 0; i < th_header->NumberOfBoards; i++ ) {
		fprintf(out_stream, "Brd[%d].HardwareIdent = %s\n",
				i, th_header->Brd[i].HardwareIdent);
		fprintf(out_stream, "Brd[%d].HardwareVersion = %s\n",
				i, th_header->Brd[i].HardwareVersion);
		fprintf(out_stream, "Brd[%d].BoardSerial = %"PRId32"\n",
				i, th_header->Brd[i].BoardSerial);
		fprintf(out_stream, "Brd[%d].CFDZeroCross = %"PRId32"\n",
				i, th_header->Brd[i].CFDZeroCross);
		fprintf(out_stream, "Brd[%d].CFDDiscriminatorMin = %"PRId32"\n",
				i, th_header->Brd[i].CFDDiscriminatorMin);
		fprintf(out_stream, "Brd[%d].SYNCLevel = %"PRId32"\n",
				i, th_header->Brd[i].SYNCLevel);
		fprintf(out_stream, "Brd[%d].Resolution = %"PRIf32"\n",
				i, th_header->Brd[i].Resolution);
	}
}

void th_v60_interactive_header_print(FILE *out_stream, 
		th_v60_header_t *th_header, 
		th_v60_interactive_t **interactive) {
	int i;

	for ( i = 0; i < th_header->NumberOfCurves; i++ ) {
		fprintf(out_stream, "Crv[%d].CurveIndex = %"PRId32"\n",
			i, (*interactive)[i].CurveIndex); 
		fprintf(out_stream, "Crv[%d].TimeOfRecording = %s",
			i, ctime32(&(*interactive)[i].TimeOfRecording));
		fprintf(out_stream, "Crv[%d].BoardSerial = %"PRId32"\n",
			i, (*interactive)[i].BoardSerial);
		fprintf(out_stream, "Crv[%d].CFDZeroCross = %"PRId32"\n",
			i, (*interactive)[i].CFDZeroCross);
		fprintf(out_stream, "Crv[%d].CFDDiscrMin = %"PRId32"\n",
			i, (*interactive)[i].CFDDiscrMin);
		fprintf(out_stream, "Crv[%d].SyncLevel = %"PRId32"\n",
			i, (*interactive)[i].SyncLevel);
		fprintf(out_stream, "Crv[%d].CurveOffset = %"PRId32"\n",
			i, (*interactive)[i].CurveOffset);
		fprintf(out_stream, "Crv[%d].RoutingChannel = %"PRId32"\n",
			i, (*interactive)[i].RoutingChannel);
		fprintf(out_stream, "Crv[%d].SubMode = %"PRId32"\n",
			i, (*interactive)[i].SubMode);
		fprintf(out_stream, "Crv[%d].MeasMode = %"PRId32"\n",
			i, (*interactive)[i].MeasMode);
		fprintf(out_stream, "Crv[%d].P1 = %"PRIf32"\n",
			i, (*interactive)[i].P1);
		fprintf(out_stream, "Crv[%d].P2 = %"PRIf32"\n",
			i, (*interactive)[i].P2);
		fprintf(out_stream, "Crv[%d].P3 = %"PRIf32"\n",
			i, (*interactive)[i].P3);
		fprintf(out_stream, "Crv[%d].RangeNo = %"PRId32"\n",
			i, (*interactive)[i].RangeNo);
		fprintf(out_stream, "Crv[%d].Offset = %"PRId32"\n",
			i, (*interactive)[i].Offset);
		fprintf(out_stream, "Crv[%d].AcquisitionTime = %"PRId32"\n",
			i, (*interactive)[i].AcquisitionTime);
		fprintf(out_stream, "Crv[%d].StopAfter = %"PRId32"\n",
			i, (*interactive)[i].StopAfter);
		fprintf(out_stream, "Crv[%d].StopReason = %"PRId32"\n",
			i, (*interactive)[i].StopReason);
		fprintf(out_stream, "Crv[%d].SyncRate = %"PRId32"\n",
			i, (*interactive)[i].SyncRate);
		fprintf(out_stream, "Crv[%d].CFDCountRate = %"PRId32"\n",
			i, (*interactive)[i].CFDCountRate);
		fprintf(out_stream, "Crv[%d].TDCCountRate = %"PRId32"\n",
			i, (*interactive)[i].TDCCountRate);
		fprintf(out_stream, "Crv[%d].IntegralCount = %"PRId32"\n",
			i, (*interactive)[i].IntegralCount);
		fprintf(out_stream, "Crv[%d].Resolution = %"PRIf32"\n",
			i, (*interactive)[i].Resolution);
		fprintf(out_stream, "Crv[%d].ExtDevices = %"PRId32"\n",
			i, (*interactive)[i].ExtDevices);
		fprintf(out_stream, "Crv[%d].Reserved = %"PRId32"\n",
			i, (*interactive)[i].Reserved);
	}
}

void th_v60_interactive_data_print(FILE *out_stream, 
		th_v60_header_t *th_header, 
		th_v60_interactive_t **interactive,
		options_t *options) {
	int i;
	int j;
	float64_t left_time;
	float64_t time_step;

	for ( i = 0; i < th_header->NumberOfCurves; i++ ) {
		left_time = (float64_t)(*interactive)[i].Offset;
		time_step = (*interactive)[i].Resolution;
		for ( j = 0; j < th_header->NumberOfChannels; j++ ) { 
			pq_print_interactive(out_stream, i, left_time, left_time+time_step, 
					(*interactive)[i].Counts[j], options);
			left_time += time_step;
		}
	}
}
