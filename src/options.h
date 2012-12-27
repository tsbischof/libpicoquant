#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "types.h"

typedef struct {
	char *filename_in;
	char *filename_out;
	int print_every; 
	int binary_out; 
	int64_t number; 
	int print_header; 
	int print_resolution; 
	int to_t2; 
} options_t;


void usage();
void description();
void version();

void options_init(options_t *options);
int options_parse(int argc, char *argv[], options_t *options);
void options_free(options_t *options);

#endif
