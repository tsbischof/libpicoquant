#include <stdlib.h>

#include "../hydraharp.h"
#include "hh_v20.h"

#include "../error.h"

int hh_v20_dispatch(FILE *in_stream, FILE *out_stream, pq_header_t *pq_header, 
		options_t *options) {
	int result;
	hh_v20_header_t hh_header;

	result = hh_v20_header_read(in_stream, &hh_header, options);
	if ( result ) {
		error("Could not read HydraHarp header.\n");
		return(result);
	}

	if ( hh_header.MeasurementMode == HH_MODE_INTERACTIVE ) {
		result = hh_v20_interactive_stream(in_stream, out_stream, pq_header, 
				&hh_header, options);
	} else if ( hh_header.MeasurementMode == HH_MODE_T2  || 
				hh_header.MeasurementMode == HH_MODE_T3 ) {
		result = hh_v20_tttr_stream(in_stream, out_stream, pq_header,
				&hh_header, options);
	} else {
		error("Picoharp measurement mode not recognized: %"PRId32".\n", 
				hh_header.MeasurementMode);
		result = PQ_MODE_ERROR;
	} 

	hh_v20_header_free(&hh_header);
	return(result);
}

/* 
 *
 * Routines for the header common to all files.
 *
 */
int hh_v20_header_read(FILE *in_stream, hh_v20_header_t *hh_header,
		options_t *options ) {
	int i;
	int result;
	
	/* First, we want to read everything that is static. This is everything
	 * up for the board definitions, which we will pull after we know how
	 * many there are (hh_header->NumberOfBoards)
	 */
	debug("Reading static part of common header.\n");
	result = fread(hh_header, 
			sizeof(hh_v20_header_t)
			- sizeof(hh_v20_input_channel_t *) - sizeof(int32_t *),
			 1, in_stream);
	if ( result != 1 ) {
		return(PQ_READ_ERROR);
	} 

	debug("Allocating board memory for common header.\n");
	hh_header->InpChan = (hh_v20_input_channel_t *)malloc(
			hh_header->InputChannelsPresent
			*sizeof(hh_v20_input_channel_t));

	if ( hh_header->InpChan == NULL ) {
		return(PQ_MEM_ERROR);
	}
	
	debug("Reading input channel data for common header.\n");
	result = fread(hh_header->InpChan, sizeof(hh_v20_input_channel_t),
			hh_header->InputChannelsPresent, in_stream);
	if ( result != hh_header->InputChannelsPresent ) {
		return(PQ_READ_ERROR);
	} 

	/* And the input rates. */
	debug("Allocating input rate memory.\n");
	hh_header->InputRate = (int32_t *)malloc(
			hh_header->InputChannelsPresent
			*sizeof(int32_t));
	if ( hh_header->InputRate == NULL ) {
		return(PQ_MEM_ERROR);
	}

	if ( hh_header->MeasurementMode == HH_MODE_INTERACTIVE ) {
		/* Interactive mode does not have input rates, for whatever 
		 * reason. Fill the space with empty data, for the sake of 
		 * simplicity.
		 */
		for ( i = 0; i < hh_header->InputChannelsPresent; i++) {
			hh_header->InputRate[i] = 0;
		}
	} else {
 		debug("Reading input rates.\n");
		result = fread(hh_header->InputRate, sizeof(int32_t),
				hh_header->InputChannelsPresent, in_stream);
		if ( result != hh_header->InputChannelsPresent ) {
			return(PQ_READ_ERROR);
		}
	}

	return(PQ_SUCCESS);
}


void hh_v20_header_free(hh_v20_header_t *hh_header) {
	free(hh_header->InpChan);
}

/*
 *
 * Interactive header routines.
 *
 */
int hh_v20_interactive_header_read(FILE *in_stream, 
		hh_v20_header_t *hh_header,
		hh_v20_interactive_t *interactive, options_t *options) {
	/* Read the static header data. This is a precusor to reading the
	 * curve data itself, but separating the two gives us a chance to 
	 * do some simpler debugging. These can be merged in the future for
	 * speed, but for now speed is not an issue. 
	 */
	int result;
	
	interactive->Curve = (hh_v20_curve_t *)malloc(
			hh_header->NumberOfCurves*sizeof(hh_v20_curve_t));

	if ( interactive->Curve == NULL ) {
		error("Could not allocate memory for curve headers.\n");
		return(PQ_MEM_ERROR);
	}

	debug("Reading curve header data.\n");
	result = fread(interactive->Curve, sizeof(hh_v20_curve_t),
			hh_header->NumberOfCurves, in_stream);
	if ( result != hh_header->NumberOfCurves ) {
		error( "Could not read curve headers.\n");
		return(PQ_READ_ERROR);
	}

	return(PQ_SUCCESS);
}

int hh_v20_interactive_data_read(FILE *in_stream,
		hh_v20_header_t *hh_header,
		hh_v20_interactive_t *interactive, options_t *options) {
	/* Now the curve data. Allocate this as a 2d array and populate it 
	 * as such.
	 */
	int result;
	int i;

	interactive->Counts = (uint32_t **)malloc(
			hh_header->NumberOfCurves*sizeof(uint32_t *));

	if ( interactive->Counts == NULL ) {
		error("Could not allocate memory for curve data.\n");
		return(PQ_MEM_ERROR);
	}

	for ( i = 0; i < hh_header->NumberOfCurves; i++ ) {
		debug("Reading data for curve %d.\n", i);
		interactive->Counts[i] = (uint32_t *)malloc(
				interactive->Curve[i].HistogramBins*sizeof(uint32_t)); 
		if ( interactive->Counts[i] == NULL ) {
			error("Could not allocate memory for curve %d data.\n", i);
			return(PQ_MEM_ERROR);
		}

		result = fread(interactive->Counts[i], sizeof(uint32_t),
				interactive->Curve[i].HistogramBins, in_stream);
		if ( result != interactive->Curve[i].HistogramBins ) {
			error("Could not read data for curve %d.\n", i);
			return(PQ_READ_ERROR);
		}
	}
	return(PQ_SUCCESS);
}

void hh_v20_interactive_header_free(hh_v20_interactive_t *interactive) {
	free(interactive->Curve);
}

void hh_v20_interactive_data_free(hh_v20_header_t *hh_header,
		hh_v20_interactive_t *interactive) {
	int i;

	for ( i = 0; i < hh_header->NumberOfCurves; i++ ) {
		free(interactive->Counts[i]);	
	}
	free(interactive->Counts);
}

/* 
 *
 * Streaming for interactive files.
 *
 */
int hh_v20_interactive_stream(FILE *in_stream, FILE *out_stream,
		pq_header_t *pq_header, hh_v20_header_t *hh_header, 
		options_t *options) {
	int result;
	hh_v20_interactive_t interactive;
	int i;

	/* Read interactive header. */
	result = hh_v20_interactive_header_read(in_stream, hh_header,
			&interactive, options);
	if ( result != PQ_SUCCESS ) {
		error("Failed while reading interactive header.\n");
	}

	if ( options->print_header ) {
		pq_header_print(out_stream, pq_header);
		hh_v20_header_print(out_stream, hh_header);
		hh_v20_interactive_header_print(out_stream, hh_header,
				&interactive);
	} else if ( options->print_resolution ) {
		for ( i = 0; i < hh_header->NumberOfCurves; i++ ) {
			fprintf(out_stream, "%d,", i);
			print_resolution(out_stream, 
					interactive.Curve[i].Resolution, options);
		}
	} else {
	/* Read and print interactive data. */
		result = hh_v20_interactive_data_read(in_stream, hh_header, 
				&interactive, options);
		if ( result != PQ_SUCCESS ) {
			error("Failed while reading interactive data.\n");
			return(PQ_READ_ERROR);
		}
		hh_v20_interactive_data_print(out_stream, hh_header,
				&interactive, options);

		debug("Freeing interactive data.\n");
		hh_v20_interactive_data_free(hh_header, &interactive);
	}

	/* Clean and return. */

	debug("Freeing interactive header.\n");
	hh_v20_interactive_header_free(&interactive);
	return(PQ_SUCCESS);
}

/*
 * 
 * Header for tttr mode (t2, t3)
 *
 */
int hh_v20_tttr_header_read(FILE *in_stream, hh_v20_tttr_header_t *tttr_header,
		options_t *options) {
	int result;

	result = fread(tttr_header, 
			sizeof(hh_v20_tttr_header_t)-sizeof(uint32_t *), 1, in_stream);
	if ( result != 1 ) {
		error("Could not read tttr header.\n");
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

void hh_v20_tttr_header_free(hh_v20_tttr_header_t *tttr_header) {
	free(tttr_header->ImgHdr);
}

/*
 *
 * Reading and interpreting for t2 mode.
 *
 */
int hh_v20_t2_record_stream(FILE *in_stream, FILE *out_stream, 
		hh_v20_header_t *hh_header,
		hh_v20_tttr_header_t *tttr_header, options_t *options) {
	int result;
	int overflows = 0;
	int64_t record_count = 0;
	hh_v20_t2_record_t record;
	int64_t base_time = 0;

	while ( !feof(in_stream) && record_count < options->number ) {
		result = fread(&record, sizeof(record), 1, in_stream);
		
		if ( result != 1 ) {
			if ( !feof(in_stream) ) {
				error("Could not read t2 record.\n");
				return(PQ_READ_ERROR);
			}
		} else {
			if ( record.special ) {
				if ( record.channel == 63 ) {
					overflows += record.time;
					base_time += record.time*HH_T2_OVERFLOW;
				} else if ( record.channel == 0 ) {
					/* Sync record. */
					record_count++;
					/* Label the sync channel as 1 greater than the maximum
					 * signal channel index.
					 */
					pq_print_t2(out_stream, record_count,
							hh_header->InputChannelsPresent,
							base_time/2, record.time/2,
							options);
				} else {
					external_marker(out_stream, record.channel, options);
				}
			} else {
				record_count++;
				/* See the ht2 documentation for this, but the gist is that
				 * the counts are registered at float64_t the rate of the reported
				 * resolution, so one count is actually 0.5ps, not 1ps. Cut
				 * the integer values for time in half to get the correct
				 * result.
				 */
				pq_print_t2(out_stream, record_count,
						record.channel,
						base_time/2, record.time/2,
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
int hh_v20_t3_record_stream(FILE *in_stream, FILE *out_stream, 
		hh_v20_header_t *hh_header,
		hh_v20_tttr_header_t *tttr_header, options_t *options) {
	/* Read the stream until we either reach the target number of entries
	 * or the end of the stream.
	 */

	int result;
	int overflows = 0;
	int64_t record_count = 0;
	hh_v20_t3_record_t record;
	int64_t base_nsync = 0;

	int64_t sync_period = (int64_t)(1e12/
			(float64_t)tttr_header->SyncRate);
	unsigned int resolution_int = (unsigned int)(hh_header->Resolution);
	debug("Sync period: %lld\n", sync_period);

	while ( !feof(in_stream) && record_count < options->number ) {
		/* First, read a value from the stream. */
		result = fread(&record, sizeof(record), 1, in_stream);

		if ( result != 1 ) {
			if ( !feof(in_stream) ) {
				error("Could not read t3 record.\n");
				return(PQ_READ_ERROR);
			}
		} else {
			if ( record.special ) {
				if ( record.channel == 63 ) {
					/* Overflow */
					overflows += record.dtime;
					base_nsync += record.nsync*HH_T3_OVERFLOW;
				} else {
					/* External marker.  */
					external_marker(out_stream, record.channel, options);
				}
			} else {
				record_count++;
				/* 1e-3 turns picoseconds to nanoseconds */
				if ( options->to_t2 ) {
					pq_print_t2(out_stream, record_count,
						record.channel,
						(base_nsync+record.nsync)*sync_period, 
						record.dtime*resolution_int,
						options);
				} else {
					pq_print_t3(out_stream, record_count,
						record.channel,
						base_nsync, record.nsync,
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
int hh_v20_tttr_stream(FILE *in_stream, FILE *out_stream,
		pq_header_t *pq_header, hh_v20_header_t *hh_header, 
		options_t *options) {
	hh_v20_tttr_header_t tttr_header;
	int result;

	debug("Reading tttr header.\n");

	if ( (result = hh_v20_tttr_header_read(in_stream, &tttr_header, options)) 
			!= PQ_SUCCESS ) {
		error("Failed while reading tttr header.\n");
		return(result);
	}

	/* Now that we have read the header, check what we want to do with it. */
	if ( options->print_header ) {
		pq_header_print(out_stream, pq_header);
		hh_v20_header_print(out_stream, hh_header);
		hh_v20_tttr_header_print(out_stream, &tttr_header);
		result = PQ_SUCCESS;
	} else if ( options->print_resolution ) {
		/* HydraHarp has half the resolution it reports, according to the 
 		 * documentation.
 		 */
//		fprintf(out_stream, "%e\n", hh_header->Resolution*2e-3);
		print_resolution(out_stream, (hh_header->Resolution*2), options);
//		fprintf(out_stream, "%d\n", (int)(HH_BASE_RESOLUTION*2e3));
	} else {
		if ( hh_header->MeasurementMode == HH_MODE_T2 ) {
			debug("Found mode ht2.\n");
			result = hh_v20_t2_record_stream(in_stream, out_stream, 
					hh_header, &tttr_header, options);
		} else if ( hh_header->MeasurementMode == HH_MODE_T3 ) {
			debug("Found mode ht3.\n");
			result = hh_v20_t3_record_stream(in_stream, out_stream,
					hh_header, &tttr_header, options);
		} else {
			debug("Unrecognized mode.\n");
			result = PQ_MODE_ERROR;
		}
	}

	hh_v20_tttr_header_free(&tttr_header);

	return(result);
}

/* 
 *
 * Routines to print the various parameters.
 *
 */
void hh_v20_header_print(FILE *out_stream, 
		hh_v20_header_t *hh_header) {
	int i;

	fprintf(out_stream, "BitsPerRecord = %"PRId32"\n", 
			hh_header->BitsPerRecord);
	fprintf(out_stream, "ActiveCurve = %"PRId32"\n",
			hh_header->ActiveCurve);
	fprintf(out_stream, "MeasurementMode = %"PRId32"\n", 
			hh_header->MeasurementMode);
	fprintf(out_stream, "SubMode = %"PRId32"\n", hh_header->SubMode);
	fprintf(out_stream, "Binning = %"PRId32"\n", hh_header->Binning);
	fprintf(out_stream, "Resolution = %"PRIf64"\n", hh_header->Resolution);
	fprintf(out_stream, "Offset = %"PRId32"\n", hh_header->Offset);
	fprintf(out_stream, "AcquisitionTime = %"PRId32"\n", 
			hh_header->AcquisitionTime);
	fprintf(out_stream, "StopAt = %"PRId32"\n", hh_header->StopAt);
	fprintf(out_stream, "StopOnOvfl = %"PRId32"\n", hh_header->StopOnOvfl);
	fprintf(out_stream, "Restart = %"PRId32"\n", hh_header->Restart);
	fprintf(out_stream, "DisplayLinLog = %"PRId32"\n", 
			hh_header->DisplayLinLog);
	fprintf(out_stream, "DisplayTimeAxisFrom = %"PRIu32"\n", 
			hh_header->DisplayTimeAxisFrom);
	fprintf(out_stream, "DisplayTimeAxisTo = %"PRIu32"\n", 
			hh_header->DisplayTimeAxisTo);
	fprintf(out_stream, "DisplayCountAxisFrom = %"PRIu32"\n",
			hh_header->DisplayCountAxisFrom);
	fprintf(out_stream, "DisplayCountAxisTo = %"PRIu32"\n",
			hh_header->DisplayCountAxisTo);

	for ( i = 0; i < 8; i++ ) {
		fprintf(out_stream, "DisplayCurve[%d].MapTo = %"PRId32"\n", 
				i, hh_header->DisplayCurve[i].MapTo);
		fprintf(out_stream, "DisplayCurve[%d].Show = %"PRId32"\n",
				i, hh_header->DisplayCurve[i].Show);
	}

	for ( i = 0; i < 3; i++ ) {
		fprintf(out_stream, "Param[%d].Start = %"PRIf32"\n",
				i, hh_header->Param[i].Start);
		fprintf(out_stream, "Param[%d].Step = %"PRIf32"\n",
				i, hh_header->Param[i].Step);
		fprintf(out_stream, "Param[%d].Stop = %"PRIf32"\n",
				i, hh_header->Param[i].Stop);
	}

	fprintf(out_stream, "RepeatMode = %"PRId32"\n", hh_header->RepeatMode);
	fprintf(out_stream, "RepeatsPerCurve = %"PRId32"\n", 
			hh_header->RepeatsPerCurve);
	fprintf(out_stream, "RepeatTime = %"PRId32"\n", hh_header->RepeatTime);
	fprintf(out_stream, "RepeatWaitTime = %"PRId32"\n", 
			hh_header->RepeatWaitTime);
	fprintf(out_stream, "ScriptName = %s\n", hh_header->ScriptName);
	fprintf(out_stream, "HardwareIdent = %s\n", 
			hh_header->HardwareIdent);
	fprintf(out_stream, "HardwarePartNo = %s\n", 
			hh_header->HardwarePartNo);
	fprintf(out_stream, "HardwareSerial = %"PRId32"\n", 
			hh_header->HardwareSerial);
	fprintf(out_stream, "NumerOfModules = %"PRId32"\n", 
			hh_header->NumberOfModules);

	for ( i = 0; i < hh_header->NumberOfModules; i++ ) {
		fprintf(out_stream, "ModuleInfo[%d].Model = %"PRId32"\n",
				i, hh_header->ModuleInfo[i].Model);
		fprintf(out_stream, "ModuleInfo[%d].Version = %"PRId32"\n",
				i, hh_header->ModuleInfo[i].Version);
	}

	fprintf(out_stream, "BaseResolution = %"PRIf64"\n", 
			hh_header->BaseResolution);
	fprintf(out_stream, "InputsEnabled = %"PRId64"\n", 
			hh_header->InputsEnabled);
	fprintf(out_stream, "InputChannelsPresent = %"PRId32"\n", 
			hh_header->InputChannelsPresent);
	fprintf(out_stream, "RefClockSource = %"PRId32"\n", 
			hh_header->RefClockSource);
	fprintf(out_stream, "ExtDevices = %"PRId32"\n", hh_header->ExtDevices);
	fprintf(out_stream, "MarkerSettings = %"PRId32"\n", 
			hh_header->MarkerSettings);
	fprintf(out_stream, "SyncDivider = %"PRId32"\n", hh_header->SyncDivider);
	fprintf(out_stream, "SyncCFDLevel = %"PRId32"\n", hh_header->SyncCFDLevel);
	fprintf(out_stream, "SyncCFDZeroCross = %"PRId32"\n", 
			hh_header->SyncCFDZeroCross);
	fprintf(out_stream, "SyncOffset = %"PRId32"\n", hh_header->SyncOffset);
	
	for ( i = 0; i < hh_header->InputChannelsPresent; i++ ) {
		fprintf(out_stream, "InpChan[%d].ModuleIdx = %"PRId32"\n",
			i, hh_header->InpChan[i].ModuleIdx);
		fprintf(out_stream, "InpChan[%d].CFDLevel = %"PRId32"\n",
			i, hh_header->InpChan[i].CFDLevel);
		fprintf(out_stream, "InpChan[%d].CFDZeroCross = %"PRId32"\n",
			i, hh_header->InpChan[i].CFDZeroCross);
		fprintf(out_stream, "InpChan[%d].Offset = %"PRId32"\n",
			i, hh_header->InpChan[i].Offset);
	}

	for ( i = 0; i < hh_header->InputChannelsPresent; i++ ) {
		fprintf(out_stream, "InputRate[%d] = %"PRId32"\n", 
				i, hh_header->InputRate[i]);
	}
}

void hh_v20_tttr_header_print(FILE *out_stream,
		hh_v20_tttr_header_t *tttr_header) {
	int i;

	fprintf(out_stream, "SyncRate = %"PRId32"\n", tttr_header->SyncRate);
	fprintf(out_stream, "StopAfter = %"PRId32"\n", tttr_header->StopAfter);
	fprintf(out_stream, "StopReason = %"PRId32"\n", tttr_header->StopReason);
	fprintf(out_stream, "ImgHdrSize = %"PRId32"\n", tttr_header->ImgHdrSize);
	fprintf(out_stream, "NumRecords = %"PRId64"\n", tttr_header->NumRecords);

	for ( i = 0; i < tttr_header->ImgHdrSize; i++ ) {
		fprintf(out_stream, "ImgHdr[%d] = %"PRIu32"\n", i,
				tttr_header->ImgHdr[i]);
	}
}

void hh_v20_interactive_header_print(FILE *out_stream, 
		hh_v20_header_t *hh_header, 
		hh_v20_interactive_t *interactive) {
	int i;
	int j;

	for ( i = 0; i < hh_header->NumberOfCurves; i++ ) {
		fprintf(out_stream, "Curve[%d].CurveIndex = %"PRId32"\n",
			i, interactive->Curve[i].CurveIndex);
		fprintf(out_stream, "Curve[%d].TimeOfRecording = %s",
			i, ctime32(&interactive->Curve[i].TimeOfRecording));
		fprintf(out_stream, "Curve[%d].HardwareIdent = %s\n",
			i, interactive->Curve[i].HardwareIdent);
		fprintf(out_stream, "Curve[%d].HardwareVersion = %s\n",
			i, interactive->Curve[i].HardwareVersion);
		fprintf(out_stream, "Curve[%d].HardwareSerial = %"PRId32"\n",
			i, interactive->Curve[i].HardwareSerial);
		fprintf(out_stream, "Curve[%d].NoOfModules = %"PRId32"\n",
			i, interactive->Curve[i].NoOfModules);

		for ( j = 0; j < hh_header->NumberOfModules; j++ ) {
			fprintf(out_stream, "Curve[%d].Module[%d].Model = %"PRId32"\n",
				i, j, interactive->Curve[i].Module[j].Model);
			fprintf(out_stream, "Curve[%d].Module[%d].Version = %"PRId32"\n",
				i, j, interactive->Curve[i].Module[j].Version);
		}

		fprintf(out_stream, "Curve[%d].BaseResolution = %"PRIf64"\n",
			i, interactive->Curve[i].BaseResolution);
		fprintf(out_stream, "Curve[%d].InputsEnabled = %"PRId64"\n",
			i, interactive->Curve[i].InputsEnabled);
		fprintf(out_stream, "Curve[%d].InpChanPresent = %"PRId32"\n",
			i, interactive->Curve[i].InpChanPresent);
		fprintf(out_stream, "Curve[%d].RefClockSource = %"PRId32"\n",
			i, interactive->Curve[i].RefClockSource);
		fprintf(out_stream, "Curve[%d].ExtDevices = %"PRId32"\n",
			i, interactive->Curve[i].ExtDevices);
		fprintf(out_stream, "Curve[%d].MarkerSettings = %"PRId32"\n",
			i, interactive->Curve[i].MarkerSettings);
		fprintf(out_stream, "Curve[%d].SyncDivider = %"PRId32"\n",
			i, interactive->Curve[i].SyncDivider);
		fprintf(out_stream, "Curve[%d].SyncCFDLevel = %"PRId32"\n",
			i, interactive->Curve[i].SyncCFDLevel);
		fprintf(out_stream, "Curve[%d].SyncCFDZero = %"PRId32"\n",
			i, interactive->Curve[i].SyncCFDZero);
		fprintf(out_stream, "Curve[%d].SyncOffset = %"PRId32"\n",
			i, interactive->Curve[i].SyncOffset);
		fprintf(out_stream, "Curve[%d].InpModuleIdx = %"PRId32"\n",
			i, interactive->Curve[i].InpModuleIdx);
		fprintf(out_stream, "Curve[%d].InpCFDLevel = %"PRId32"\n",
			i, interactive->Curve[i].InpCFDLevel);
		fprintf(out_stream, "Curve[%d].InpCFDZeroCross = %"PRId32"\n",
			i, interactive->Curve[i].InpCFDZeroCross);
		fprintf(out_stream, "Curve[%d].InpOffset = %"PRId32"\n",
			i, interactive->Curve[i].InpOffset);
		fprintf(out_stream, "Curve[%d].InpChannel = %"PRId32"\n",
			i, interactive->Curve[i].InpChannel);
		fprintf(out_stream, "Curve[%d].MeasMode = %"PRId32"\n",
			i, interactive->Curve[i].MeasMode);
		fprintf(out_stream, "Curve[%d].SubMode = %"PRId32"\n",
			i, interactive->Curve[i].SubMode);
		fprintf(out_stream, "Curve[%d].Binning = %"PRId32"\n",
			i, interactive->Curve[i].Binning);
		fprintf(out_stream, "Curve[%d].Resolution = %"PRIf64"\n",
			i, interactive->Curve[i].Resolution);
		fprintf(out_stream, "Curve[%d].Offset = %"PRId32"\n",
			i, interactive->Curve[i].Offset);
		fprintf(out_stream, "Curve[%d].AcquisitionTime = %"PRId32"\n",
			i, interactive->Curve[i].AcquisitionTime);
		fprintf(out_stream, "Curve[%d].StopAfter = %"PRId32"\n",
			i, interactive->Curve[i].StopAfter);
		fprintf(out_stream, "Curve[%d].StopReason = %"PRId32"\n",
			i, interactive->Curve[i].StopReason);
		fprintf(out_stream, "Curve[%d].P1 = %"PRIf32"\n",
			i, interactive->Curve[i].P1);
		fprintf(out_stream, "Curve[%d].P2 = %"PRIf32"\n",
			i, interactive->Curve[i].P2);
		fprintf(out_stream, "Curve[%d].P3 = %"PRIf32"\n",
			i, interactive->Curve[i].P3);
		fprintf(out_stream, "Curve[%d].SyncRate = %"PRId32"\n",
			i, interactive->Curve[i].SyncRate);
		fprintf(out_stream, "Curve[%d].InputRate = %"PRId32"\n",
			i, interactive->Curve[i].InputRate);
		fprintf(out_stream, "Curve[%d].HistCountRate = %"PRId32"\n",
			i, interactive->Curve[i].HistCountRate);
		fprintf(out_stream, "Curve[%d].IntegralCount = %"PRId64"\n",
			i, interactive->Curve[i].IntegralCount);
		fprintf(out_stream, "Curve[%d].HistogramBins = %"PRId32"\n",
			i, interactive->Curve[i].HistogramBins);
		fprintf(out_stream, "Curve[%d].DataOffset = %"PRId32"\n",
			i, interactive->Curve[i].DataOffset);
	}
}

void hh_v20_interactive_data_print(FILE *out_stream, 
		hh_v20_header_t *hh_header, 
		hh_v20_interactive_t *interactive,
		options_t *options) {
	int i;
	int j;
	float64_t left_time;
	float64_t time_step;

	for ( i = 0; i < hh_header->NumberOfCurves; i++ ) {
		left_time = (float64_t)interactive->Curve[i].Offset;
		time_step = interactive->Curve[i].Resolution*1e-3;
		for ( j = 0; j < interactive->Curve[i].HistogramBins; j++ ) { 
			pq_print_interactive(out_stream, i, left_time, left_time+time_step, 
					interactive->Counts[i][j], options);
			/* Check the picosecond conversion and time offset */
			left_time += time_step;
		}
	}
}
