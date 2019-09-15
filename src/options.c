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
#include <string.h>
#include <getopt.h>

#include "options.h"
#include "error.h"

void version() {
	fprintf(stderr, "picoquant v%s\n", VERSION);
}

void description() {
	fprintf(stderr, 
"This program decodes data collected using Picoquant hardware. \n"
"The binary data is decoded to detect the hardware version and\n"
"collection mode, and the data are output in a mode-specific\n"
"ascii or binary format.\n"
"\n"
"When called, the data output are either run configuration parameters \n"
"(obtained by passing --resolution-only or --header-only) or the data from\n"
"the measurement. The header is output in an ini-like format, while the\n"
"resolution is output as a float in picoseconds, though for most runs\n"
"this is more accurately described as an integer number of picoseconds.\n"
"\n"
"Hardware and versions supported:\n"
"TimeHarp: v2.0 (thd)\n"
"          v3.0 (thd, t3r)\n"
"          v5.0 (thd)\n"
"          v6.0 (thd, t3r)\n"
"Picoharp: v2.0 (phd, pt2, pt3)\n"
"Hydraharp: v1.0 (hhd, ht2, ht3)\n"
"           v2.0 (hhd, ht2, ht3, ptu)\n"
"\n"
"Data formats (csv):\n"
"(times are integers in picoseconds, bin edges are floats in nanoseconds)\n"
"    Interactive mode:\n"
"        curve number, left bin edge, right bin edge, counts\n"
"\n"
"    T2:\n"
"        channel, time\n"
"\n"
"    T3:\n"
"        channel, pulse, time\n");
}       


void usage() {
	fprintf(stderr,
"              -h, --help: Prints this usage message.\n"
"           -V, --version: Print version information.\n"
"           -v, --verbose: Print debug-level information.\n"
"           -i, --file-in: Specify the input file. By default, this is stdin.\n"
"          -o, --file-out: Specify the output file. By default, \n"
"                          this is stdout.\n"
"        -b, --binary-out: Output mode-specific binary structures instead of \n"
"                          ascii csv.\n"
"       -p, --print-every: Print a status every n entries.\n"
"   -z, --resolution-only: Print the resolution of the measurement, as a\n"
"                          double-precision float in ps.\n"
"       -r, --header-only: Print the file header in text format.\n"
"         -m, --mode-only: Print the mode of the data.\n"
"             -t, --to-t2: For t3 data, use the sync rate todetermine the\n"
"                          time represented by the sync count and output the\n"
"                          data in t2 mode. Note that this will only be\n"
"                          accurate if the sync source is perfectly regular.\n"
"             -n --number: Process n entries. By default, all entries are \n"
"                          processed.\n");
}

int options_parse(int argc, char *argv[], options_t *options) {
	int result = PQ_SUCCESS;
	int c, option_index;

	char *options_string = "hVvi:o:bp:zrmtn:";

	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"verbose", no_argument, 0, 'v'},
		{"version", no_argument, 0, 'V'},
		{"print-every", required_argument, 0, 'p'},

		{"file-in", required_argument, 0, 'i'},
		{"file-out", required_argument, 0, 'o'},

		{"binary-out", no_argument, 0, 'b'},

		{"resolution-only", no_argument, 0, 'z'},
		{"header-only", no_argument, 0, 'r'},
		{"mode-only", no_argument, 0, 'm'},
		{"to-t2", no_argument, 0, 't'},
		{"number", required_argument, 0, 'n'},
		{0, 0, 0, 0}};

	while ( (c = getopt_long(argc, argv, options_string,
						long_options, &option_index)) != -1 ) {
		switch (c) {
			case 'h':
				usage();
				description();
				result = PQ_USAGE;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'V':
				version();
				result = PQ_VERSION;
				break;
			case 'p':
				options->print_every = strtoi32(optarg, NULL, 10);
				break;
			case 'i':
				options->filename_in = strdup(optarg);
				break;
			case 'o':
				options->filename_out = strdup(optarg);
				break;
			case 'b':
				options->binary_out = 1;
				break;
			case 'z':
				options->print_resolution = 1;
				break;
			case 'r':
				options->print_header = 1;
				break;
			case 'm':
				options->print_mode = 1;
				break;
			case 't':
				options->to_t2 = 1;
				break;
			case 'n':
				options->number = strtoi64(optarg, NULL, 10);
				break;
			case '?':
			default:
				usage();
				description();
				result = PQ_ERROR_OPTIONS;
		}
	}

	return(result);
}

void options_init(options_t *options) {
	options->filename_in = NULL;
	options->filename_out = NULL;
	options->print_every = 0;

	options->binary_out = 0;

	options->number = INT64_MAX;
	options->print_header = 0;
	options->print_resolution = 0;
	options->print_mode = 0;
	options->to_t2 = 0;

//	options->hardware_name = NULL;
//	options->hardware_version  = NULL;
}

void options_free(options_t *options) {
	free(options->filename_in);
	free(options->filename_out);
//	free(options->hardware_name);
//	free(options->format_version);
}
