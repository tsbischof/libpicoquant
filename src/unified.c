/*
Copyright (c) 2011-2019, 2023 Thomas Bischof
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the Massachusetts Institute of Technology nor the 
   names of its contributors may be used to endorse or promote products 
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
 */

#include "unified.h"
#include "error.h"

#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "hydraharp.h"
#include "hydraharp/hh_v10.h"
#include "hydraharp/hh_v20.h"
#include "picoharp.h"
#include "picoharp/ph_v20.h"
#include "timeharp.h"
#include "timeharp/th_v20.h"
#include "timeharp/th_v30.h"
#include "timeharp/th_v50.h"
#include "timeharp/th_v60.h"

#define TAG_PRINT(x) if ( options->print_header ) { x; }
#define NOT_IMPLEMENTED error("Mode 0x%08lx not implemented\n", pu_options.record_type); break;

int pu_dispatch(FILE *stream_in, FILE *stream_out, pu_header_t *pu_header, options_t *options) {
	int result;
	pu_options_t pu_options;

	hh_v10_header_t hh_v10_header;
	hh_v10_tttr_header_t hh_v10_tttr;
	hh_v20_header_t hh_v20_header;
	hh_v20_tttr_header_t hh_v20_tttr;

	ph_v20_header_t ph_v20_header;
	ph_v20_tttr_header_t ph_v20_tttr;

	result = pu_tags_read(stream_in, stream_out, pu_header, options, &pu_options);

	if ( result == PQ_SUCCESS ) {
		if ( options->print_header ) {
			; // this was handled implicitly in pu_tags_read
			// TODO: implement a dictionary type to read in the tags, then print separately.
		} else if ( options->print_resolution ) { 
			error("Unified resolution print not yet implemented.\n");
			result = PQ_ERROR_OPTIONS;
		} else {
			// actually stream data
			debug("Record type: 0x%08lx\n", pu_options.record_type);
			debug("Resolution (ps): %lld\n", (uint64_t)(1e12*pu_options.resolution_seconds));

			switch ( pu_options.record_type ) {
				case PU_RECORD_PH_T3:
				case PU_RECORD_PH_T2:
					ph_v20_tttr.InpRate0 = pu_options.sync_rate;
					ph_v20_tttr.StopAfter = pu_options.stop_after;
					ph_v20_tttr.StopReason = 0;
					ph_v20_tttr.ImgHdrSize = 0;
					ph_v20_tttr.NumRecords = pu_options.number_of_records;

					if ( pu_options.record_type == PU_RECORD_PH_T3 ) {
						ph_v20_t3_stream(stream_in, stream_out, &ph_v20_header, &ph_v20_tttr, options);
					} else {
						ph_v20_t2_stream(stream_in, stream_out, &ph_v20_header, &ph_v20_tttr, options);
					}
						
					break;
				case PU_RECORD_HH_V1_T3:
				case PU_RECORD_HH_V1_T2:
					hh_v10_header.Resolution = round(pu_options.resolution_seconds*1e12);
					hh_v10_header.InputChannelsPresent = pu_options.input_channels_present;

					hh_v10_tttr.SyncRate = pu_options.sync_rate;
					hh_v10_tttr.StopAfter = pu_options.stop_after;
					hh_v10_tttr.StopReason = 0;
					hh_v10_tttr.ImgHdrSize = 0;
					hh_v10_tttr.NumRecords = pu_options.number_of_records;

					if ( pu_options.record_type == PU_RECORD_HH_V1_T3 ) {
						hh_v10_t3_stream(stream_in, stream_out, &hh_v10_header, &hh_v10_tttr, options);
					} else {
						hh_v10_t2_stream(stream_in, stream_out, &hh_v10_header, &hh_v10_tttr, options);
					}
					break;
				case PU_RECORD_HH_V2_T3:
				case PU_RECORD_HH_V2_T2:
					hh_v20_header.Resolution = round(pu_options.resolution_seconds*1e12);
					hh_v20_header.InputChannelsPresent = pu_options.input_channels_present;

					hh_v20_tttr.SyncRate = pu_options.sync_rate;
					hh_v20_tttr.StopAfter = pu_options.stop_after;
					hh_v20_tttr.StopReason = 0;
					hh_v20_tttr.ImgHdrSize = 0;
					hh_v20_tttr.NumRecords = pu_options.number_of_records;

					if ( pu_options.record_type == PU_RECORD_HH_V2_T3 ) {
						hh_v20_t3_stream(stream_in, stream_out, &hh_v20_header, &hh_v20_tttr, options);
					} else {
						hh_v20_t2_stream(stream_in, stream_out, &hh_v20_header, &hh_v20_tttr, options);
					}
						
					break;
				case PU_RECORD_TH_260_NT3:
				case PU_RECORD_TH_260_NT2:
				case PU_RECORD_TH_260_PT3:
				case PU_RECORD_TH_260_PT2:
					error("This mode is not implemented due to a lack of test data. Please open a pull request at https://github.com/tsbischof/picoquant-sample-data and we will get to work on the decoder.\n");
					NOT_IMPLEMENTED;
				default:
					error("Unknown record type:  0x%08lx\n", pu_options.record_type);
					result = PQ_ERROR_MODE;
			}
		}
	} 

	return(result);
}

int pu_tags_read(FILE *stream_in, FILE *stream_out, pu_header_t *pu_header, options_t *options, pu_options_t *pu_options) {
	int result;
	size_t index;
	pu_tag_t tag;
	float64_t value_float;
	char *buffer_char;
	wchar_t *buffer_wchar;
	float64_t *buffer_float64;

	do {
		result = fread(&tag, sizeof(tag), 1, stream_in);
		if ( result != 1 ) {
			result = PQ_ERROR_IO;
			break;
		} else {
			result = PQ_SUCCESS;
		}

		if ( tag.index > 0 ) {
			TAG_PRINT(fprintf(stream_out, "%s[%d] = ", tag.ident, tag.index))
		} else {
			TAG_PRINT(fprintf(stream_out, "%s = ", tag.ident))
		}
		
		switch ( tag.type ) {
			case PU_TAG_Empty8:
				TAG_PRINT(fprintf(stream_out, "null"))
				break;
			case PU_TAG_Bool8:
				TAG_PRINT(fprintf(stream_out, "%s", tag.value ? "true" : "false"))
				break;
			case PU_TAG_Int8:
				if ( ! strcmp(tag.ident, "TTResultFormat_TTTRRecType") ) {
					pu_options->record_type = tag.value;
				} else if ( ! strcmp(tag.ident, "HW_InpChannels") ) {
					pu_options->input_channels_present = tag.value;
				} else if ( ! strcmp(tag.ident, "TTResult_SyncRate") ) {
					pu_options->sync_rate = tag.value;
				} else if ( ! strcmp(tag.ident, "TTResult_StopAfter") ) {
					pu_options->stop_after = tag.value;
				} else if ( ! strcmp(tag.ident, "TTResult_NumberOfRecords") ) {
					pu_options->number_of_records = tag.value;
				}

				TAG_PRINT(fprintf(stream_out, "%" PRId64, (int64_t)tag.value))
				break;
			case PU_TAG_BitSet64:
			case PU_TAG_Color8:  // just print both BitSet64 and Color8 for now
				TAG_PRINT(fprintf(stream_out, "0x%" PRIu64 "x", (uint64_t)tag.value))
				break;
			case PU_TAG_Float8:
				memcpy((char *)&value_float, (char *)(&tag.value), sizeof(float64_t));

				if ( ! strcmp(tag.ident, "MeasDesc_Resolution") ) { 
					pu_options->resolution_seconds = value_float;
				}

				TAG_PRINT(fprintf(stream_out, "%E", value_float))
				break;
			case PU_TAG_TDateTime:
				TAG_PRINT(fprintf(stream_out, "0x%016" PRIx64, tag.value))
				break;
			case PU_TAG_Float8Array:
				buffer_float64 = (float64_t *)malloc(tag.value*sizeof(float64_t));
				result = fread(buffer_float64, sizeof(float64_t), tag.value, stream_in);
				if ( result != tag.value ) {
					error("Could not read float array\n.");
					result = PQ_ERROR_IO;
				} else { 
					result = PQ_SUCCESS;
					for ( index = 0; index < tag.value; index++ ) {
						memcpy((char *)&value_float, (char *)(&tag.value), sizeof(float64_t));
						TAG_PRINT(
							fprintf(stream_out, "%lf", value_float);
							if ( index + 1 != tag.value ) {
								fprintf(stream_out, ", ");
							} 
						)
					}
				}

				free(buffer_float64);
				break;
			case PU_TAG_AnsiString:
				buffer_char = (char *)malloc(tag.value*sizeof(char));
				result = fread(buffer_char, sizeof(char), tag.value, stream_in);
				if ( result != tag.value ) {
					error("Could not read ANSI string.\n");
					result = PQ_ERROR_IO;
				} else { 
					result = PQ_SUCCESS;
					TAG_PRINT(fprintf(stream_out, "%.*s", (int32_t)tag.value, buffer_char))
				}

				free(buffer_char);
				break;
			case PU_TAG_WideString:
				buffer_wchar = (wchar_t *)malloc(tag.value*sizeof(wchar_t));
				result = fread(buffer_wchar, sizeof(wchar_t), tag.value, stream_in);
				if ( result != tag.value ) {
					error("Could not read ANSI string.\n");
					result = PQ_ERROR_IO;
				} else { 
					result = PQ_SUCCESS;
					TAG_PRINT(fprintf(stream_out, "%.*ls", (int32_t)tag.value, buffer_wchar))
				}

				free(buffer_wchar);
				break;
			case PU_TAG_BinaryBlob:
				buffer_char = malloc(tag.value*sizeof(char));
				result = fread(buffer_char, sizeof(char), tag.value, stream_in);
				
				if ( result != tag.value ) { 
					error("Could not read binary blob.\n");
					result = PQ_ERROR_IO;
				} else {
					result = PQ_SUCCESS;
					TAG_PRINT(
						for ( index = 0; index < tag.value; index++ ) {
							fprintf(stream_out, "%02x", buffer_char[index] & 0xff);
						}
					)
				}

				free(buffer_char);
				break;
			default:
				error("Unknown tag type: 0x%016x.\n", tag.type);
				result = PQ_ERROR_UNKNOWN_DATA;
				break;
		}

		TAG_PRINT(fprintf(stream_out, "\n"))
	} while ( ! result && strcmp(tag.ident, "Header_End") );

	return(result);
}
