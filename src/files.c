#include <stdio.h>

#include "error.h"
#include "files.h"

int stream_open(FILE **stream, FILE *default_stream, 
		char *filename, char *mode) {
	if ( filename == NULL ) {
		*stream = default_stream;
	} else {
		debug("Opening %s for with mode %s.\n", filename, mode);
		*stream = fopen(filename, mode);

		if ( *stream == NULL ) {
			error("Could not open %s with mode %s.\n", filename, mode);
			return(-1);
		}
	}

	return(0);
}

void stream_close(FILE *stream, FILE *default_stream) { 
	if ( stream != NULL && stream != default_stream ) {
		fclose(stream);
	}
}

int open_streams(FILE **in_stream, char *in_filename,
		FILE **out_stream, char *out_filename) {
	return( stream_open(in_stream, stdin, in_filename, "r") +
			stream_open(out_stream, stdout, out_filename, "w") );
}

void free_streams(FILE *in_stream, FILE *out_stream) {
	stream_close(in_stream, stdin);
	stream_close(out_stream, stdout);
}
