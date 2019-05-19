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

#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "options.h"
#include "picoquant.h"
#include "files.h"
//#include "ptu_dispatch.h"

int main(int argc, char *argv[]) {
	/* This software is designed to read in Picoquant data files and
	 * translate them to well-defined ascii and binary formats. It acts
	 * as a filter in the Unix sense, in that it is designed to accept a
	 * stream of raw data and output a stream of processed data.
	 */
	options_t options;

	int result = 0;

	FILE *stream_in = NULL;
	FILE *stream_out = NULL;

	options_init(&options);
	result = options_parse(argc, argv, &options);

	if ( result == PQ_SUCCESS ) {
		result = streams_open(&stream_in, options.filename_in, 
				&stream_out, options.filename_out);
	}

	if ( result == PQ_SUCCESS ) {
		/* Do the actual work, if there are no errors. */
		result = pq_dispatch(stream_in, stream_out, &options);
	}
		
	debug("Freeing options.\n");
	options_free(&options);
	debug("Closing streams.\n");
	streams_close(stream_in, stream_out);

	return(pq_check(result));
}       

