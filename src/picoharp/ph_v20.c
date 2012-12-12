#include <stdlib.h>

#include "../picoharp.h"
#include "ph_v20.h"

#include "../error.h"

int ph_v20_dispatch(FILE *in_stream, FILE *out_stream, pq_header_t *pq_header,
		options_t *options) {
	int result;
	ph_v20_header_t ph_header;

	result = ph_v20_header_read(in_stream, &ph_header, options);
	if ( result ) {
		error("Could not read PicoHarp header.\n");
		return(result);
	}

	if ( ph_header.MeasurementMode == PH_MODE_INTERACTIVE ) {
		result = ph_v20_interactive_stream(in_stream, out_stream, pq_header, 
				&ph_header, options);
	} else if ( ph_header.MeasurementMode == PH_MODE_T2  || 
				ph_header.MeasurementMode == PH_MODE_T3 ) {
		result = ph_v20_tttr_stream(in_stream, out_stream, pq_header,
				&ph_header, options);
	} else {
		error("Picoharp measurement mode not recognized: %"PRId32".\n", 
				ph_header.MeasurementMode);
		result = PQ_MODE_ERROR;
	} 

	ph_v20_header_free(&ph_header);
	return(result);
}

/* 
 *
 * Routines for the header common to all files.
 *
 */
int ph_v20_header_read(FILE *in_stream, ph_v20_header_t *ph_header,
		options_t *options ) {
	int i;
	int result;
	
	/* First, we want to read everything that is static. This is everything
	 * up for the board definitions, which we will pull after we know how
	 * many there are (ph_header->NumberOfBoards)
	 */
	debug("Reading static part of common header.\n");
	result = fread(ph_header, 
			sizeof(ph_v20_header_t) - sizeof(ph_v20_board_t *), 
			1, in_stream);
	if ( result != 1 ) {
		return(PQ_READ_ERROR);
	} 
	
	/* Now read the dynamic board data. */
	debug("Allocating board memory for common header.\n");
	ph_header->Brd = (ph_v20_board_t *)malloc(
			ph_header->NumberOfBoards*sizeof(ph_v20_board_t));

	if ( ph_header->Brd == NULL ) {
		error("Could not allocate board memory.\n");
		return(PQ_MEM_ERROR);
	}
	/* To do: the manual (and files structure) indicate that 
	 * ph_v20_header->RoutingChannels must always be 4, but the 
	 * actual value in a file is often 1. Add functionality to 
	 * deal with this appropriately.
	 */
	debug("Warning: checking value of RoutingChannels. "
			"See picoharp.c for details.\n");
	ph_header->RoutingChannels = 4;
	
	/* Each board also has many router channels. */
	for ( i = 0; i < ph_header->NumberOfBoards; i++ ) {
		debug("Reading static data for board %d.\n", i);
		result = fread(&ph_header->Brd[i], 
			sizeof(ph_v20_board_t) - sizeof(ph_v20_router_channel_t *), 
			1, in_stream);
		if ( result != 1 ) {
			return(PQ_READ_ERROR);
		}

		debug(
			"Allocating router channel data for board %d.\n", i);
		ph_header->Brd[i].RtCh = (ph_v20_router_channel_t *)malloc(
				ph_header->RoutingChannels
				*sizeof(ph_v20_router_channel_t));
		if ( ph_header->Brd[i].RtCh == NULL ) {
			return(PQ_MEM_ERROR);
		}

		debug(
			"Reading router channel data for board %d.\n", i);
		result = fread(ph_header->Brd[i].RtCh, 
				sizeof(ph_v20_router_channel_t), 
				ph_header->RoutingChannels, in_stream);
		if ( result != ph_header->RoutingChannels ) {
			return(PQ_READ_ERROR);
		}
	}

	return(PQ_SUCCESS);
}


void ph_v20_header_free(ph_v20_header_t *ph_header) {
	int i;
	
	for ( i = 0; i < ph_header->NumberOfBoards; i++ ) {
		free(ph_header->Brd[i].RtCh);
	}

	free(ph_header->Brd);

}

/*
 *
 * Interactive header routines.
 *
 */
int ph_v20_interactive_header_read(FILE *in_stream, 
		ph_v20_header_t *ph_header,
		ph_v20_interactive_t *interactive, options_t *options) {
	/* Read the static header data. This is a precusor to reading the
	 * curve data itself, but separating the two gives us a chance to 
	 * do some simpler debugging. These can be merged in the future for
	 * speed, but for now speed is not an issue. 
	 */
	int result;
	
	interactive->Curve = (ph_v20_curve_t *)malloc(
			ph_header->NumberOfCurves*sizeof(ph_v20_curve_t));

	if ( interactive->Curve == NULL ) {
		error("Could not allocate memory for Picoharp curve headers.\n");
		return(PQ_MEM_ERROR);
	}

	debug("Reading curve header data.\n");
	result = fread(interactive->Curve, sizeof(ph_v20_curve_t),
			ph_header->NumberOfCurves, in_stream);
	if ( result != ph_header->NumberOfCurves ) {
		error( "Could not read Picoharp curve headers.\n");
		return(PQ_READ_ERROR);
	}

	return(PQ_SUCCESS);
}

int ph_v20_interactive_data_read(FILE *in_stream,
		ph_v20_header_t *ph_header,
		ph_v20_interactive_t *interactive, options_t *options) {
	/* Now the curve data. Allocate this as a 2d array and populate it 
	 * as such.
	 */
	int result;
	int i;

	interactive->Counts = (uint32_t **)malloc(
			ph_header->NumberOfCurves*sizeof(uint32_t *));

	if ( interactive->Counts == NULL ) {
		error("Could not allocate memory for Picoharp curve data.\n");
		return(PQ_MEM_ERROR);
	}

	for ( i = 0; i < ph_header->NumberOfCurves; i++ ) {
		debug("Reading data for curve %d.\n", i);
		interactive->Counts[i] = (uint32_t *)malloc(
				interactive->Curve[i].Channels*sizeof(uint32_t));

		if ( interactive->Counts[i] == NULL ) {
			error("Could not allocate memory for Picoharp curve %d data.\n", i);
			return(PQ_MEM_ERROR);
		}

		result = fread(interactive->Counts[i], sizeof(uint32_t),
				interactive->Curve[i].Channels, in_stream);
		if ( result != interactive->Curve[i].Channels ) {
			error("Could not read data for Picoharp curve %d.\n", i);
			return(PQ_READ_ERROR);
		}
	}
	return(PQ_SUCCESS);
}

void ph_v20_interactive_header_free(ph_v20_interactive_t *interactive) {
	free(interactive->Curve);
}

void ph_v20_interactive_data_free(ph_v20_header_t *ph_header,
		ph_v20_interactive_t *interactive) {
	int i;

	for ( i = 0; i < ph_header->NumberOfCurves; i++ ) {
		free(interactive->Counts[i]);	
	}
	free(interactive->Counts);
}

/* 
 *
 * Streaming for interactive files.
 *
 */
int ph_v20_interactive_stream(FILE *in_stream, FILE *out_stream,
		pq_header_t *pq_header, ph_v20_header_t *ph_header, 
		options_t *options) {
	int result;
	ph_v20_interactive_t interactive;
	int i;

	/* Read interactive header. */
	result = ph_v20_interactive_header_read(in_stream, ph_header,
			&interactive, options);
	if ( result != PQ_SUCCESS ) {
		error("Failed while reading interactive header.\n");
	}

	if ( options->print_header ) {
		pq_header_print(out_stream, pq_header);
		ph_v20_header_print(out_stream, ph_header);
		ph_v20_interactive_header_print(out_stream, ph_header, &interactive);
	} else if ( options->print_resolution ) {
		for ( i = 0; i < ph_header->NumberOfCurves; i++ ) {
			fprintf(out_stream, "%d,", i);
			print_resolution(out_stream, 
					(interactive.Curve[i].Resolution*1e3), options);
		}
	} else {
	/* Read and print interactive data. */
		result = ph_v20_interactive_data_read(in_stream, ph_header, 
				&interactive, options);
		if ( result != PQ_SUCCESS ) {
			error("Failed while reading interactive data.\n");
			return(PQ_READ_ERROR);
		}
		ph_v20_interactive_data_print(out_stream, ph_header,
				&interactive, options);

		debug("Freeing interactive data.\n");
		ph_v20_interactive_data_free(ph_header, &interactive);
	}

	/* Clean and return. */

	debug("Freeing interactive header.\n");
	ph_v20_interactive_header_free(&interactive);
	return(PQ_SUCCESS);
}

/*
 * 
 * Header for tttr mode (t2, t3)
 *
 */
int ph_v20_tttr_header_read(FILE *in_stream, ph_v20_tttr_header_t *tttr_header,
		options_t *options) {
	int result;

	result = fread(tttr_header, 
		sizeof(ph_v20_tttr_header_t)-sizeof(uint32_t *), 1, in_stream);
	if ( result != 1 ) {
		error("Could not read Picoharp tttr header.\n");
		return(PQ_READ_ERROR);
	}

	tttr_header->ImgHdr = (uint32_t *)malloc(
			tttr_header->ImgHdrSize*sizeof(uint32_t));
	if ( tttr_header->ImgHdr == NULL ) {
		error("Could not allocate memory for Picoharp tttr image header.\n");
		return(PQ_MEM_ERROR);
	}

	result = fread(tttr_header->ImgHdr, sizeof(uint32_t),
			tttr_header->ImgHdrSize, in_stream);
	if ( result != tttr_header->ImgHdrSize ) {
		error("Could not read Picoharp tttr image header.\n");
		return(PQ_READ_ERROR);
	}

	return(PQ_SUCCESS);
}

void ph_v20_tttr_header_free(ph_v20_tttr_header_t *tttr_header) {
	free(tttr_header->ImgHdr);
}

/*
 *
 * Reading and interpreting for t2 mode.
 *
 */
int ph_v20_t2_record_stream(FILE *in_stream, FILE *out_stream, 
		ph_v20_header_t *ph_header,
		ph_v20_tttr_header_t *tttr_header, options_t *options) {
	/* Read the stream until we either reach the target number of entries
	 * or the end of the stream.
	 */

	int result;
	int64_t record_count = 0;
	int overflows = 0;
	ph_v20_t2_record_t record;

	/* Time based on the number of overflows, such that we do not need to
	 * multiply each time we want to print the absolute time.
	 */
	int64_t base_time = 0; 

	uint32_t resolution_int = (uint32_t)(PH_V20_BASE_RESOLUTION*1e12);

	debug("Record limit:             %10d.\n", options->number);
	debug("Listed number of records: %10d.\n", tttr_header->NumRecords);

	while ( ! feof(in_stream) && record_count < options->number ) {
		/* First, read a value from the stream. */
		result = fread(&record, sizeof(record), 1, in_stream);

		if ( result != 1 ) {
			if ( ! feof(in_stream) ) {
				error("Could not read Picoharp t2 record.\n");
				return(PQ_READ_ERROR);
			}
		} else {
			/* Now, interpret the record as an overflow or data. */
			if ( record.channel == 15 ) {
				/* Special record */
				if ( (record.time & 01111) == 01111 ) {
					/* External marker. */
					external_marker(out_stream, record.time, options);
				} else {
					/* Overflow */
					overflows++;
					base_time += PH_T2_OVERFLOW;
				}
			} else {
				record_count++;
				pq_print_t2(out_stream, record_count,
					record.channel,
					base_time*resolution_int, 
					record.time*resolution_int,
					options);
			}
		}
	}

	return(PQ_SUCCESS);
}


/*
 *
 * Reading and interpreting for t3 mode.
 *
 */
int ph_v20_t3_record_stream(FILE *in_stream, FILE *out_stream, 
		ph_v20_header_t *ph_header,
		ph_v20_tttr_header_t *tttr_header, options_t *options) {
	int result;
	int overflows = 0;
	int64_t record_count = 0;
	ph_v20_t3_record_t record;
	int64_t base_nsync = 0;
	int64_t sync_period = (int64_t)(1e12/(float64_t)tttr_header->InpRate0);
	debug("Sync period: %lld\n", sync_period);
	uint32_t resolution_int = (uint32_t)(
			ph_header->Brd[0].Resolution*1e3);

	while ( !feof(in_stream) && record_count < options->number ) {
		result = fread(&record, sizeof(record), 1, in_stream);
		
		if ( result != 1 ) {
			if ( !feof(in_stream) ) {
				error("Could not read t3 record.\n");
				return(PQ_READ_ERROR);
			}
		} else {
			if ( record.channel == 15 ) {
				/* Special record. */
				if ( record.dtime == 0 ) {
					overflows++;
					base_nsync += PH_T3_OVERFLOW;
				} else {
					external_marker(out_stream, record.dtime, options);
				}
			} else {
				record_count++;
				if ( options->to_t2 ) {
					pq_print_t2(out_stream, record_count,
							record.channel,
							(base_nsync+record.nsync)*sync_period,
							record.dtime*resolution_int,
							options);
				} else {
					pq_print_t3(out_stream, record_count,
							record.channel,
							base_nsync, 
							record.nsync,
							record.dtime*resolution_int,
							options);
				}
			}
		}
	}
				
	return(PQ_SUCCESS);
}

/*
 *
 * Streaming for t2 or t3 mode.
 *
 */
int ph_v20_tttr_stream(FILE *in_stream, FILE *out_stream,
		pq_header_t *pq_header, ph_v20_header_t *ph_header, options_t *options) {
	ph_v20_tttr_header_t tttr_header;
	int result;

	result = ph_v20_tttr_header_read(in_stream, &tttr_header, options);
	if ( result ) {
		error("Failed while reading tttr header.\n");
		return(result);
	}

	/* Now that we have read the header, check what we want to do with it. */
	if ( options->print_header ) {
		debug("Printing file header.\n");
		pq_header_print(out_stream, pq_header);
		ph_v20_header_print(out_stream, ph_header);
		ph_v20_tttr_header_print(out_stream, &tttr_header);
		result = PQ_SUCCESS;
	} else if ( options->print_resolution ) {
		print_resolution(out_stream, 
				(ph_header->Brd[0].Resolution*1e3), options);
	} else {
		if ( ph_header->MeasurementMode == PH_MODE_T2 ) {
			result = ph_v20_t2_record_stream(in_stream, out_stream, 
					ph_header, &tttr_header, options);
		} else if ( ph_header->MeasurementMode == PH_MODE_T3 ) {
			result = ph_v20_t3_record_stream(in_stream, out_stream,
					ph_header, &tttr_header, options);
		} else {
			debug("Unrecognized mode.\n");
			result = PQ_MODE_ERROR;
		}
	}

	ph_v20_tttr_header_free(&tttr_header);

	return(result);
}

/* 
 *
 * Routines to print the various parameters.
 *
 */
void ph_v20_header_print(FILE *out_stream, 
		ph_v20_header_t *ph_header) {
	int i;
	int j;

	fprintf(out_stream, "CreatorName = %s\n",
			ph_header->CreatorName);
	fprintf(out_stream, "CreatorVersion = %s\n",
			ph_header->CreatorVersion);
	fprintf(out_stream, "FileTime = %s\n",
			ph_header->FileTime);
	fprintf(out_stream, "Comment = %s\n",
			ph_header->Comment);
	fprintf(out_stream, "NumberOfCurves = %"PRId32"\n", 
			ph_header->NumberOfCurves);
	fprintf(out_stream, "BitsPerRecord = %"PRId32"\n", 
			ph_header->BitsPerRecord);
	fprintf(out_stream, "RoutingChannels = %"PRId32"\n", 
			ph_header->RoutingChannels);
	fprintf(out_stream, "NumberOfBoards = %"PRId32"\n", 
			ph_header->NumberOfBoards);
	fprintf(out_stream, "ActiveCurve = %"PRId32"\n", ph_header->ActiveCurve);
	fprintf(out_stream, "MeasurementMode = %"PRId32"\n", 
			ph_header->MeasurementMode);
	fprintf(out_stream, "SubMode = %"PRId32"\n", ph_header->SubMode);
	fprintf(out_stream, "RangeNo = %"PRId32"\n", ph_header->RangeNo);
	fprintf(out_stream, "Offset = %"PRId32"\n", ph_header->Offset);
	fprintf(out_stream, "AcquisitionTime = %"PRId32"\n", 
			ph_header->AcquisitionTime);
	fprintf(out_stream, "StopAt = %"PRId32"\n", ph_header->StopAt);
	fprintf(out_stream, "StopOnOvfl = %"PRId32"\n", ph_header->StopOnOvfl);
	fprintf(out_stream, "Restart = %"PRId32"\n", ph_header->Restart);
	fprintf(out_stream, "DisplayLinLog = %"PRId32"\n", 
			ph_header->DisplayLinLog);
	fprintf(out_stream, "DisplayTimeAxisFrom = %"PRId32"\n", 
			ph_header->DisplayTimeAxisFrom);
	fprintf(out_stream, "DisplayTimeAxisTo = %"PRId32"\n", 
			ph_header->DisplayTimeAxisTo);
	fprintf(out_stream, "DisplayCountAxisTo = %"PRId32"\n",
			ph_header->DisplayCountAxisTo);

	for ( i = 0; i < 8; i++ ) {
		fprintf(out_stream, "DisplayCurve[%d].MapTo = %"PRId32"\n", 
				i, ph_header->DisplayCurve[i].MapTo);
		fprintf(out_stream, "DisplayCurve[%d].Show = %"PRId32"\n",
				i, ph_header->DisplayCurve[i].Show);
	}

	for ( i = 0; i < 3; i++ ) {
		fprintf(out_stream, "Param[%d].Start = %"PRIf32"\n",
				i, ph_header->Param[i].Start);
		fprintf(out_stream, "Param[%d].Step = %"PRIf32"\n",
				i, ph_header->Param[i].Step);
		fprintf(out_stream, "Param[%d].Stop = %"PRIf32"\n",
				i, ph_header->Param[i].Stop);
	}

	fprintf(out_stream, "RepeatMode = %"PRId32"\n", ph_header->RepeatMode);
	fprintf(out_stream, "RepeatsPerCurve = %"PRId32"\n", 
			ph_header->RepeatsPerCurve);
	fprintf(out_stream, "RepeatTime = %"PRId32"\n", ph_header->RepeatTime);
	fprintf(out_stream, "RepeatWaitTime = %"PRId32"\n", 
			ph_header->RepeatWaitTime);
	fprintf(out_stream, "ScriptName = %s\n", ph_header->ScriptName);

	for ( i = 0; i < ph_header->NumberOfBoards; i++ ) {
		fprintf(out_stream, "Brd[%d].HardwareIdent = %s\n",
				i, ph_header->Brd[i].HardwareIdent);
		fprintf(out_stream, "Brd[%d].HardwareVersion = %s\n",
				i, ph_header->Brd[i].HardwareVersion);
		fprintf(out_stream, "Brd[%d].HardwareSerial = %"PRId32"\n",
				i, ph_header->Brd[i].HardwareSerial);
		fprintf(out_stream, "Brd[%d].SyncDivider = %"PRId32"\n",
				i, ph_header->Brd[i].SyncDivider);
		fprintf(out_stream, "Brd[%d].CFDZeroCross0 = %"PRId32"\n",
				i, ph_header->Brd[i].CFDZeroCross0);
		fprintf(out_stream, "Brd[%d].CFDLevel0 = %"PRId32"\n",
				i, ph_header->Brd[i].CFDLevel0);
		fprintf(out_stream, "Brd[%d].CFDZeroCross1 = %"PRId32"\n",
				i, ph_header->Brd[i].CFDZeroCross1);
		fprintf(out_stream, "Brd[%d].CFDLevel1 = %"PRId32"\n",
				i, ph_header->Brd[i].CFDLevel1);
		fprintf(out_stream, "Brd[%d].Resolution = %"PRIf32"\n",
				i, ph_header->Brd[i].Resolution);
		fprintf(out_stream, "Brd[%d].RouterModelCode = %"PRId32"\n",
				i, ph_header->Brd[i].RouterModelCode);
		fprintf(out_stream, "Brd[%d].RouterEnabled = %"PRId32"\n",
				i, ph_header->Brd[i].RouterEnabled);

		for ( j = 0; j < ph_header->RoutingChannels; j++ ) {
			fprintf(out_stream, 
					"Brd[%d].RtCh[%d].InputType = %"PRId32"\n",
					i, j, 
				ph_header->Brd[i].RtCh[j].InputType);
			fprintf(out_stream, 
					"Brd[%d].RtCh[%d].InputLevel = %"PRId32"\n",
					i, j, 
				ph_header->Brd[i].RtCh[j].InputLevel);
			fprintf(out_stream, 
					"Brd[%d].RtCh[%d].InputEdge = %"PRId32"\n",
					i, j, 
				ph_header->Brd[i].RtCh[j].InputEdge);
			fprintf(out_stream, 
					"Brd[%d].RtCh[%d].CFDPresent = %"PRId32"\n",
					i, j, 
				ph_header->Brd[i].RtCh[j].CFDPresent);
			fprintf(out_stream, 
					"Brd[%d].RtCh[%d].CFDLevel = %"PRId32"\n",
					i, j,
				ph_header->Brd[i].RtCh[j].CFDLevel);
			fprintf(out_stream,
					"Brd[%d].RtCh[%d].CFDZCross = %"PRId32"\n",
					i, j,
				ph_header->Brd[i].RtCh[j].CFDZCross);
		}
	}
}

void ph_v20_tttr_header_print(FILE *out_stream,
		ph_v20_tttr_header_t *tttr_header) {
	int i;
	fprintf(out_stream, "ExtDevices = %"PRId32"\n", tttr_header->ExtDevices);

	for ( i = 0; i < 2; i++ ) {
		fprintf(out_stream, "Reserved[%d] = %"PRId32"\n", i,
				tttr_header->Reserved[i]);
	}

	fprintf(out_stream, "InpRate0 = %"PRId32"\n", tttr_header->InpRate0);
	fprintf(out_stream, "InpRate1 = %"PRId32"\n", tttr_header->InpRate1);
	fprintf(out_stream, "StopAfter = %"PRId32"\n", tttr_header->StopAfter);
	fprintf(out_stream, "StopReason = %"PRId32"\n", tttr_header->StopReason);
	fprintf(out_stream, "NumRecords = %"PRId32"\n", tttr_header->NumRecords);
	fprintf(out_stream, "ImgHdrSize = %"PRId32"\n", tttr_header->ImgHdrSize);

	for ( i = 0; i < tttr_header->ImgHdrSize; i++ ) {
		fprintf(out_stream, "ImgHdr[%d] = %"PRIu32"\n", i,
				tttr_header->ImgHdr[i]);
	}
}

void ph_v20_interactive_header_print(FILE *out_stream, 
		ph_v20_header_t *ph_header, 
		ph_v20_interactive_t *interactive) {
	int i;

	for ( i = 0; i < ph_header->NumberOfCurves; i++ ) {
		fprintf(out_stream, "Crv[%d].CurveIndex = %"PRId32"\n",
			i, interactive->Curve[i].CurveIndex);
		fprintf(out_stream, "Crv[%d].TimeOfRecording = %s",
			i, ctime32(&interactive->Curve[i].TimeOfRecording));
		fprintf(out_stream, "Crv[%d].HardwareIdent = %s\n",
			i, interactive->Curve[i].HardwareIdent);
		fprintf(out_stream, "Crv[%d].HardwareVersion = %s\n",
			i, interactive->Curve[i].HardwareVersion);
		fprintf(out_stream, "Crv[%d].HardwareSerial = %"PRId32"\n",
			i, interactive->Curve[i].HardwareSerial);
		fprintf(out_stream, "Crv[%d].SyncDivider = %"PRId32"\n",
			i, interactive->Curve[i].SyncDivider);
		fprintf(out_stream, "Crv[%d].CFDZeroCross0 = %"PRId32"\n",
			i, interactive->Curve[i].CFDZeroCross0);
		fprintf(out_stream, "Crv[%d].CFDLevel0 = %"PRId32"\n",
			i, interactive->Curve[i].CFDLevel0);
		fprintf(out_stream, "Crv[%d].CFDZeroCross1 = %"PRId32"\n",
			i, interactive->Curve[i].CFDZeroCross1);
		fprintf(out_stream, "Crv[%d].CFDLevel1 = %"PRId32"\n",
			i, interactive->Curve[i].CFDLevel1);
		fprintf(out_stream, "Crv[%d].Offset = %"PRId32"\n",
			i, interactive->Curve[i].Offset);
		fprintf(out_stream, "Crv[%d].RoutingChannel = %"PRId32"\n",
			i, interactive->Curve[i].RoutingChannel);
		fprintf(out_stream, "Crv[%d].ExtDevices = %"PRId32"\n",
			i, interactive->Curve[i].ExtDevices);
		fprintf(out_stream, "Crv[%d].MeasMode = %"PRId32"\n",
			i, interactive->Curve[i].MeasMode);
		fprintf(out_stream, "Crv[%d].SubMode = %"PRId32"\n",
			i, interactive->Curve[i].SubMode);
		fprintf(out_stream, "Crv[%d].P1 = %"PRIf32"\n",
			i, interactive->Curve[i].P1);
		fprintf(out_stream, "Crv[%d].P2 = %"PRIf32"\n",
			i, interactive->Curve[i].P2);
		fprintf(out_stream, "Crv[%d].P3 = %"PRIf32"\n",
			i, interactive->Curve[i].P3);
		fprintf(out_stream, "Crv[%d].RangeNo = %"PRId32"\n",
			i, interactive->Curve[i].RangeNo);
		fprintf(out_stream, "Crv[%d].Resolution = %"PRIf32"\n",
			i, interactive->Curve[i].Resolution);
		fprintf(out_stream, "Crv[%d].Channels = %"PRId32"\n",
			i, interactive->Curve[i].Channels);
		fprintf(out_stream, "Crv[%d].AcquisitionTime = %"PRId32"\n",
			i, interactive->Curve[i].AcquisitionTime);
		fprintf(out_stream, "Crv[%d].StopAfter = %"PRId32"\n",
			i, interactive->Curve[i].StopAfter);
		fprintf(out_stream, "Crv[%d].StopReason = %"PRId32"\n",
			i, interactive->Curve[i].StopReason);
		fprintf(out_stream, "Crv[%d].InpRate0 = %"PRId32"\n",
			i, interactive->Curve[i].InpRate0);
		fprintf(out_stream, "Crv[%d].InpRate1 = %"PRId32"\n",
			i, interactive->Curve[i].InpRate1);
		fprintf(out_stream, "Crv[%d].HistCountRate = %"PRId32"\n",
			i, interactive->Curve[i].HistCountRate);
		fprintf(out_stream, "Crv[%d].IntegralCount = %"PRId64"\n",
			i, interactive->Curve[i].IntegralCount);
		fprintf(out_stream, "Crv[%d].Reserved = %"PRId32"\n",
			i, interactive->Curve[i].Reserved);
		fprintf(out_stream, "Crv[%d].DataOffset = %"PRId32"\n",
			i, interactive->Curve[i].DataOffset);
		fprintf(out_stream, "Crv[%d].RouterModelCode = %"PRId32"\n",
			i, interactive->Curve[i].RouterModelCode);
		fprintf(out_stream, "Crv[%d].RouterEnabled = %"PRId32"\n",
			i, interactive->Curve[i].RouterEnabled);
		fprintf(out_stream, "Crv[%d].RtCh_InputType = %"PRId32"\n",
			i, interactive->Curve[i].RtCh_InputType);
		fprintf(out_stream, "Crv[%d].RtCh_InputLevel = %"PRId32"\n",
			i, interactive->Curve[i].RtCh_InputLevel);
		fprintf(out_stream, "Crv[%d].RtCh_InputEdge = %"PRId32"\n",
			i, interactive->Curve[i].RtCh_InputEdge);
		fprintf(out_stream, "Crv[%d].RtCh_CFDPresent = %"PRId32"\n",
			i, interactive->Curve[i].RtCh_CFDPresent);
		fprintf(out_stream, "Crv[%d].RtCh_CFDLevel = %"PRId32"\n",
			i, interactive->Curve[i].RtCh_CFDLevel);
		fprintf(out_stream, "Crv[%d].RtCh_CFDZeroCross = %"PRId32"\n",
			i, interactive->Curve[i].RtCh_CFDZeroCross);
	}
}

void ph_v20_interactive_data_print(FILE *out_stream, 
		ph_v20_header_t *ph_header, 
		ph_v20_interactive_t *interactive,
		options_t *options) {
	int i;
	int j;
	float64_t left_time;
	float64_t time_step;

	for ( i = 0; i < ph_header->NumberOfCurves; i++ ) {
		left_time = (float64_t)interactive->Curve[i].Offset;
		time_step = interactive->Curve[i].Resolution;
		for ( j = 0; j < interactive->Curve[i].Channels; j++ ) { 
			pq_print_interactive(out_stream, i, left_time, left_time+time_step, 
					interactive->Counts[i][j], options);
			left_time += interactive->Curve[i].Resolution;
		}
	}
}
