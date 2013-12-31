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

#include "../hydraharp.h"
#include "hh_v10.h"

#include "../error.h"

int hh_v10_dispatch(FILE *stream_in, FILE *stream_out, pq_header_t *pq_header, 
		options_t *options) {
	int result = PQ_SUCCESS;
	hh_v10_header_t *hh_header;

	result = hh_v10_header_read(stream_in, &hh_header);
	if ( hh_header == NULL ) {
		error("Could not read HydraHarp header.\n");
	} else {
		if ( options->print_mode ) {
			if ( hh_header->MeasurementMode == HH_MODE_INTERACTIVE ) {
				fprintf(stream_out, "interactive\n");
			} else if ( hh_header->MeasurementMode == HH_MODE_T2 ) {
				fprintf(stream_out, "t2\n");
			} else if ( hh_header->MeasurementMode == HH_MODE_T3 ) {
				fprintf(stream_out, "t3\n");
			} else {
				error("Hydraharp mode not recognized: %"PRId32".\n",
						hh_header->MeasurementMode);
				result = PQ_ERROR_MODE;
			}
		} else {
			if ( hh_header->MeasurementMode == HH_MODE_INTERACTIVE ) {
				result = hh_v10_interactive_stream(stream_in, stream_out, 
						pq_header, hh_header, options);
			} else if ( hh_header->MeasurementMode == HH_MODE_T2  || 
							hh_header->MeasurementMode == HH_MODE_T3 ) {
				result = hh_v10_tttr_stream(stream_in, stream_out, pq_header,
						hh_header, options);
			} else {
				error("Hydraharp measurement mode not recognized: %"PRId32".\n",
						hh_header->MeasurementMode);
				result = PQ_ERROR_MODE;
			} 
		}
	}

	debug("Freeing Hydraharp header.\n");
	hh_v10_header_free(&hh_header);
	return(result);
}

/* 
 *
 * Routines for the header common to all files.
 *
 */
hh_v10_header_t *hh_v10_header_alloc(int input_channels) {
	hh_v10_header_t *hh_header = NULL;

	hh_header = (hh_v10_header_t *)malloc(sizeof(hh_v10_header_t));
	if ( hh_header != NULL ) {
		hh_header->InpChan = (hh_v10_input_channel_t *)malloc(
				sizeof(hh_v10_input_channel_t)*input_channels);
	} 

	if ( hh_header == NULL || hh_header->InpChan == NULL ) {
		hh_v10_header_free(&hh_header);
		hh_header = NULL;
	}

	return(hh_header);
}

void hh_v10_header_free(hh_v10_header_t **hh_header) {
	if ( *hh_header != NULL ) {
		free((*hh_header)->InpChan);
		free(*hh_header);
	}
}

int hh_v10_header_read(FILE *stream_in, hh_v10_header_t **hh_header) {
	int i;
	size_t n_read;

	*hh_header = (hh_v10_header_t *)malloc(sizeof(hh_v10_header_t));

	if ( *hh_header == NULL ) {
		error("Could not allocate Hydraharp header.\n");
		return(PQ_ERROR_MEM);
	}

	/* First, we want to read everything that is static. This is everything
	 * up for the board definitions, which we will pull after we know how
	 * many there are (hh_header->NumberOfBoards)
	 */
	debug("Reading static part of common header.\n");
	n_read = fread(*hh_header, 
			sizeof(hh_v10_header_t)
			- sizeof(hh_v10_input_channel_t *) - sizeof(int32_t *),
			 1, 
			stream_in);

	if ( n_read != 1 ) {
		error("Could not read Hydraharp header.\n");
		hh_v10_header_free(hh_header);
		return(PQ_ERROR_IO);
	} 

	debug("Allocating board memory for common header.\n");
	(*hh_header)->InpChan = (hh_v10_input_channel_t *)malloc(
			(*hh_header)->InputChannelsPresent
			*sizeof(hh_v10_input_channel_t));

	if ( (*hh_header)->InpChan == NULL ) {
		error("Could not allocate input channel memory.\n");	
		hh_v10_header_free(hh_header);
		return(PQ_ERROR_MEM);
	}
	
	debug("Reading input channel data for common header.\n");
	n_read = fread((*hh_header)->InpChan, 
			sizeof(hh_v10_input_channel_t),
			(*hh_header)->InputChannelsPresent, 
			stream_in);
	if ( n_read != (*hh_header)->InputChannelsPresent ) {
		error("Could not read input channel headers.\n");
		hh_v10_header_free(hh_header);
		return(PQ_ERROR_IO);
	} 

	/* And the input rates. */
	debug("Allocating input rate memory.\n");
	(*hh_header)->InputRate = (int32_t *)malloc(
			(*hh_header)->InputChannelsPresent
			*sizeof(int32_t));
	if ( (*hh_header)->InputRate == NULL ) {
		error("Could not allocate input rate memory.\n");
		hh_v10_header_free(hh_header);
		return(PQ_ERROR_MEM);
	}

	if ( (*hh_header)->MeasurementMode == HH_MODE_INTERACTIVE ) {
		/* Interactive mode does not have input rates, for whatever 
		 * reason. Fill the space with empty data, for the sake of 
		 * simplicity.
		 */
		for ( i = 0; i < (*hh_header)->InputChannelsPresent; i++) {
			(*hh_header)->InputRate[i] = 0;
		}
	} else {
 		debug("Reading input rates.\n");
		n_read = fread((*hh_header)->InputRate, sizeof(int32_t),
				(*hh_header)->InputChannelsPresent, stream_in);
		if ( n_read != (*hh_header)->InputChannelsPresent ) {
			error("Could not read channel input rates.\n");
			hh_v10_header_free(hh_header);
			return(PQ_ERROR_IO);
		}
	}

	return(PQ_SUCCESS);
}

/* 
 *
 * Routines to print the various parameters.
 *
 */
void hh_v10_header_printf(FILE *stream_out, 
		hh_v10_header_t *hh_header) {
	int i;

	fprintf(stream_out, "NumberOfCurves = %"PRId32"\n",
			hh_header->NumberOfCurves);
	fprintf(stream_out, "BitsPerRecord = %"PRId32"\n", 
			hh_header->BitsPerRecord);
	fprintf(stream_out, "ActiveCurve = %"PRId32"\n",
			hh_header->ActiveCurve);
	fprintf(stream_out, "MeasurementMode = %"PRId32"\n", 
			hh_header->MeasurementMode);
	fprintf(stream_out, "SubMode = %"PRId32"\n", hh_header->SubMode);
	fprintf(stream_out, "Binning = %"PRId32"\n", hh_header->Binning);
	fprintf(stream_out, "Resolution = %"PRIf64"\n", hh_header->Resolution);
	fprintf(stream_out, "Offset = %"PRId32"\n", hh_header->Offset);
	fprintf(stream_out, "AcquisitionTime = %"PRId32"\n", 
			hh_header->AcquisitionTime);
	fprintf(stream_out, "StopAt = %"PRId32"\n", hh_header->StopAt);
	fprintf(stream_out, "StopOnOvfl = %"PRId32"\n", hh_header->StopOnOvfl);
	fprintf(stream_out, "Restart = %"PRId32"\n", hh_header->Restart);
	fprintf(stream_out, "DisplayLinLog = %"PRId32"\n", 
			hh_header->DisplayLinLog);
	fprintf(stream_out, "DisplayTimeAxisFrom = %"PRIu32"\n", 
			hh_header->DisplayTimeAxisFrom);
	fprintf(stream_out, "DisplayTimeAxisTo = %"PRIu32"\n", 
			hh_header->DisplayTimeAxisTo);
	fprintf(stream_out, "DisplayCountAxisFrom = %"PRIu32"\n",
			hh_header->DisplayCountAxisFrom);
	fprintf(stream_out, "DisplayCountAxisTo = %"PRIu32"\n",
			hh_header->DisplayCountAxisTo);

	for ( i = 0; i < 8; i++ ) {
		fprintf(stream_out, "DisplayCurve[%d].MapTo = %"PRId32"\n", 
				i, hh_header->DisplayCurve[i].MapTo);
		fprintf(stream_out, "DisplayCurve[%d].Show = %"PRId32"\n",
				i, hh_header->DisplayCurve[i].Show);
	}

	for ( i = 0; i < 3; i++ ) {
		fprintf(stream_out, "Param[%d].Start = %"PRIf32"\n",
				i, hh_header->Param[i].Start);
		fprintf(stream_out, "Param[%d].Step = %"PRIf32"\n",
				i, hh_header->Param[i].Step);
		fprintf(stream_out, "Param[%d].Stop = %"PRIf32"\n",
				i, hh_header->Param[i].Stop);
	}

	fprintf(stream_out, "RepeatMode = %"PRId32"\n", hh_header->RepeatMode);
	fprintf(stream_out, "RepeatsPerCurve = %"PRId32"\n", 
			hh_header->RepeatsPerCurve);
	fprintf(stream_out, "RepeatTime = %"PRId32"\n", hh_header->RepeatTime);
	fprintf(stream_out, "RepeatWaitTime = %"PRId32"\n", 
			hh_header->RepeatWaitTime);
	fprintf(stream_out, "ScriptName = %.*s\n", 
			(int)sizeof(hh_header->ScriptName), hh_header->ScriptName);
	fprintf(stream_out, "HardwareIdent = %.*s\n", 
			(int)sizeof(hh_header->HardwareIdent), hh_header->HardwareIdent);
	fprintf(stream_out, "HardwarePartNo = %.*s\n", 
			(int)sizeof(hh_header->HardwarePartNo), hh_header->HardwarePartNo);
	fprintf(stream_out, "HardwareSerial = %"PRId32"\n", 
			hh_header->HardwareSerial);
	fprintf(stream_out, "NumberOfModules = %"PRId32"\n", 
			hh_header->NumberOfModules);

	for ( i = 0; i < hh_header->NumberOfModules; i++ ) {
		fprintf(stream_out, "ModuleInfo[%d].Model = %"PRId32"\n",
				i, hh_header->ModuleInfo[i].Model);
		fprintf(stream_out, "ModuleInfo[%d].Version = %"PRId32"\n",
				i, hh_header->ModuleInfo[i].Version);
	}

	fprintf(stream_out, "BaseResolution = %"PRIf64"\n", 
			hh_header->BaseResolution);
	fprintf(stream_out, "InputsEnabled = %"PRId64"\n", 
			hh_header->InputsEnabled);
	fprintf(stream_out, "InputChannelsPresent = %"PRId32"\n", 
			hh_header->InputChannelsPresent);
	fprintf(stream_out, "RefClockSource = %"PRId32"\n", 
			hh_header->RefClockSource);
	fprintf(stream_out, "ExtDevices = %"PRId32"\n", hh_header->ExtDevices);
	fprintf(stream_out, "MarkerSettings = %"PRId32"\n", 
			hh_header->MarkerSettings);
	fprintf(stream_out, "SyncDivider = %"PRId32"\n", hh_header->SyncDivider);
	fprintf(stream_out, "SyncCFDLevel = %"PRId32"\n", hh_header->SyncCFDLevel);
	fprintf(stream_out, "SyncCFDZeroCross = %"PRId32"\n", 
			hh_header->SyncCFDZeroCross);
	fprintf(stream_out, "SyncOffset = %"PRId32"\n", hh_header->SyncOffset);
	
	for ( i = 0; i < hh_header->InputChannelsPresent; i++ ) {
		fprintf(stream_out, "InpChan[%d].ModuleIdx = %"PRId32"\n",
			i, hh_header->InpChan[i].ModuleIdx);
		fprintf(stream_out, "InpChan[%d].CFDLevel = %"PRId32"\n",
			i, hh_header->InpChan[i].CFDLevel);
		fprintf(stream_out, "InpChan[%d].CFDZeroCross = %"PRId32"\n",
			i, hh_header->InpChan[i].CFDZeroCross);
		fprintf(stream_out, "InpChan[%d].Offset = %"PRId32"\n",
			i, hh_header->InpChan[i].Offset);
	}

	for ( i = 0; i < hh_header->InputChannelsPresent; i++ ) {
		fprintf(stream_out, "InputRate[%d] = %"PRId32"\n", 
				i, hh_header->InputRate[i]);
	}
}

void hh_v10_header_fwrite(FILE *stream_out, hh_v10_header_t *hh_header) {
	fwrite(hh_header,
			sizeof(hh_v10_header_t) 
			- sizeof(hh_v10_input_channel_t)
			- sizeof(int32_t *),
			1,
			stream_out);
	fwrite(&(hh_header->InpChan),
			sizeof(hh_v10_input_channel_t),
			hh_header->InputChannelsPresent,
			stream_out);
	fwrite(&(hh_header->InputRate),
			sizeof(int32_t),
			hh_header->InputChannelsPresent,
			stream_out);
}
