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

	FILE *in_stream = NULL;
	FILE *out_stream = NULL;

	program_options_t program_options = {
		12, 
"This program decodes data collected using Picoquant hardware. \n"
"The binary data is decoded to detect the hardware version and\n"
"collection mode, and the data are output in a mode-specific\n"
"ascii format.\n"
"\n"
"For the most detailed information, see the pdf documentation.\n"
"A brief overview of features follows here.\n"
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
"        channel, pulse, time\n",
		{OPT_HELP, OPT_VERSION, OPT_VERBOSE, OPT_FILE_IN, OPT_FILE_OUT,
				OPT_BINARY_IN, OPT_BINARY_OUT,
				OPT_NUMBER, OPT_PRINT_EVERY, 
				OPT_PRINT_HEADER, OPT_PRINT_RESOLUTION,
				OPT_TO_T2}};
		
	result = parse_options(argc, argv, &options, &program_options);

	result += open_streams(&in_stream, options.in_filename,
			&out_stream, options.out_filename);

	/* Do the actual work, if we have no errors. */
	if ( ! result ) {
		result = pq_dispatch(in_stream, out_stream, &options);
	}
		
	/* Cleanup! */
	free_options(&options);
	free_streams(in_stream, out_stream);

	return(parse_result(result));
}       

