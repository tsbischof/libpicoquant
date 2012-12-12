#ifndef FILES_H_
#define FILES_H_

#include <stdio.h>

int stream_open(FILE **stream, FILE *default_stream, 
		char *filename, char *mode);
void stream_close(FILE *stream, FILE *default_stream);

int open_streams(FILE **in_stream, char *in_filename,
		FILE **out_stream, char *out_filename);
void free_streams(FILE *in_stream, FILE *out_stream);

#endif
