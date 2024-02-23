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

#include "header.h"

#include "error.h"

#include <string.h>

int pq_unified_header_read(FILE *stream_in, pq_header_t *pq_header, 
						   pu_header_t *pu_header) {
	int result;
	char magic[16];

    /* Prior to the introduction of ptu/phu files, the magic bytes were a
     * a 16-char ident and 6-char version. This turned into an 8-char ident
     * and 8-char version. So we will read the first 16 char and branch based
     * on whether those remaining 6 char need to be read.
     */
	result = fread(&magic[0], sizeof(char), sizeof(magic), stream_in);

	if ( result != sizeof(magic) ) {
		error("Could not read magic bytes\n");
	} else {
		if ( ! strncmp(magic, "PQTTTR", 6) && ! strncmp(magic, "PQHIST", 6) ) {
			memcpy(&(pu_header->Ident[0]), magic, 8*sizeof(char));
			memcpy(&(pu_header->Version[0]), &(magic[8]), 8*sizeof(char));
			debug("Ident: %.*s\n", 8, pu_header->Ident);
			debug("Version: %.*s\n", 8, pu_header->Version);

			result = PQ_FORMAT_UNIFIED;
		} else {
			strncpy(&(pq_header->Ident[0]), magic, 16);

			result = fread(&(pq_header->FormatVersion[0]), sizeof(char), sizeof(pq_header->FormatVersion), stream_in);
			if ( result != sizeof(pq_header->FormatVersion) ) {
				error("Could not read version string.\n");
				result = PQ_ERROR_IO;
			} else {
				result = PQ_FORMAT_CLASSIC;
			}

			debug("Ident: %.*s\n", sizeof(pq_header->Ident), pq_header->Ident);
			debug("FormatVersion: %.*s\n", sizeof(pq_header->FormatVersion), pq_header->FormatVersion);
		}
	}

	return(result);
}

int pq_header_read(FILE *stream_in, pq_header_t *pq_header) {
	size_t n_read;

	n_read = fread(pq_header, sizeof(pq_header_t), 1, stream_in);
	if ( n_read == 1 ) {
		return(PQ_SUCCESS);
	} else {
		return(PQ_ERROR_IO);
	}
}

void pq_header_printf(FILE *stream_out, pq_header_t *pq_header) {
	fprintf(stream_out, "Ident = %.*s\n", 
			(int)sizeof(pq_header->Ident), pq_header->Ident);
	fprintf(stream_out, "FormatVersion = %.*s\n", 
			(int)sizeof(pq_header->Ident), pq_header->FormatVersion);
}

void pq_header_fwrite(FILE *stream_out, pq_header_t *pq_header) {
	fwrite(pq_header, sizeof(pq_header_t), 1, stream_out);
}
