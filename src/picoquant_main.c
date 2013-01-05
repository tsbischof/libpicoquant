#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "options.h"
#include "picoquant.h"
#include "files.h"

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

