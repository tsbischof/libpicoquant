/*
Copyright (c) 2011-2019, Thomas Bischof
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

#include <stdlib.h>
#include <string.h>

int pu_dispatch(FILE *stream_in, FILE *stream_out, pu_header_t *pu_header, options_t *options) {
	pu_tags_read(stream_in, stream_out, pu_header, options);
	return(PQ_SUCCESS);
}

int pu_tags_read(FILE *stream_in, FILE *stream_out, pu_header_t *pu_header, options_t *options) {
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
			fprintf(stream_out, "%s[%d] = ", tag.ident, tag.index);
		} else {
			fprintf(stream_out, "%s = ", tag.ident);
		}
		
		switch ( tag.type ) {
			case PU_TAG_Empty8:
				fprintf(stream_out, "null");
				break;
			case PU_TAG_Bool8:
				fprintf(stream_out, "%s", tag.value ? "true" : "false");
				break;
			case PU_TAG_Int8:
				fprintf(stream_out, "%ld", (uint64_t)tag.value);
				break;
			case PU_TAG_BitSet64:
			case PU_TAG_Color8:  // just print both BitSet64 and Color8 for now
				fprintf(stream_out, "0x%lx", (uint64_t)tag.value);
				break;
			case PU_TAG_Float8:
				memcpy((char *)&value_float, (char *)(&tag.value), sizeof(float64_t));
				fprintf(stream_out, "%E", value_float);
				break;
			case PU_TAG_TDateTime:
				fprintf(stream_out, "0x%016lx", tag.value);
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
						fprintf(stream_out, "%lf", value_float);
						if ( index + 1 != tag.value ) {
							fprintf(stream_out, ", ");
						}
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
					fprintf(stream_out, "%.*s", (int32_t)tag.value, buffer_char);
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
					fprintf(stream_out, "%.*ls", (int32_t)tag.value, buffer_wchar);
				}

				free(buffer_wchar);
				break;
			case PU_TAG_BinaryBlob:
				break;
			default:
				error("Unknown tag type: 0x%016x.\n", tag.type);
				result = PQ_ERROR_UNKNOWN_DATA;
				break;
		}

		fprintf(stream_out, "\n");
	} while ( ! result && strcmp(tag.ident, "Header_End") );

	return(result);
}
