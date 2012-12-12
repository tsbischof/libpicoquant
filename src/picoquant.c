#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>

#include "picoquant.h"
#include "error.h"
#include "t2.h"
#include "t3.h"
#include "options.h"

#include "picoharp.h"
#include "hydraharp.h"
#include "timeharp.h"

int pq_dispatch(FILE *in_stream, FILE *out_stream, options_t *options) {
	int result;
	pq_header_t pq_header;
	pq_dispatch_t dispatch;

	/* Do the actual work, if we have no errors. */
	result = pq_header_read(in_stream, &pq_header);

	if ( result ) {
		error("Could not read string header.\n");
	} else {
		dispatch = pq_get_dispatch(options, &pq_header);
		if ( dispatch == NULL ) {
			error("Could not identify board %s.\n", pq_header.Ident);
		} else {
			result = dispatch(in_stream, out_stream, 
					&pq_header, options);
		}
	}

	return(result);
}       


pq_dispatch_t pq_get_dispatch(options_t *options, pq_header_t *pq_header) {
	if ( ! strcmp(pq_header->Ident, "PicoHarp 300") ) {
		return(ph_dispatch);
	} else if ( ! strcmp(pq_header->Ident, "TimeHarp 200") ) {
		return(th_dispatch);
	} else if ( ! strcmp(pq_header->Ident, "HydraHarp") ) {
		return(hh_dispatch);
	} else {
		return(NULL);
	}
}

int pq_header_read(FILE *in_stream, pq_header_t *pq_header) {
	int result;
	result = fread(pq_header, sizeof(pq_header_t), 1, in_stream);
	if ( result == 1 ) {
		return(PQ_SUCCESS);
	} else {
		return(PQ_READ_ERROR);
	}
}

void pq_header_print(FILE *out_stream, pq_header_t *pq_header) {
	fprintf(out_stream, "Ident = %s\n", pq_header->Ident);
	fprintf(out_stream, "FormatVersion = %s\n", pq_header->FormatVersion);
}

void pq_print_t2(FILE *out_stream, int64_t count,
		int32_t channel, 
		int64_t base_time, uint32_t record_time,
		options_t *options) {
	t2_t record;

	record.channel = channel;
	record.time = base_time + (int64_t)record_time;

	print_t2(out_stream, &record, NEWLINE, options);
	print_status("picoquant", count, options);
}

void pq_print_t3(FILE *out_stream, int64_t count,
		int32_t channel, 
		int64_t base_nsync, uint32_t record_nsync,
		uint32_t record_dtime,
		options_t *options) {
	t3_t record;

	record.channel = channel;
	record.pulse = base_nsync + (int64_t)record_nsync;
	record.time = record_dtime;

	print_t3(out_stream, &record, NEWLINE, options);
	print_status("picoquant", count, options);
}

void pq_print_tttr(FILE *out_stream, int64_t count,
		unsigned int histogram_channel, int n_histogram_channels,
		int64_t base_time, unsigned int record_time,
		options_t *options) {
	/* This attempts to make the tttr record look t3-like. It is actually a
 	 * record of a 0->1 stop-start event, found in the histogram channel
 	 * at the time specified by the internal clock, but ultimately we can treat
 	 * the clock tick as a sync pulse and go from there.
 	 */
	t3_t record;

	/* Channel is really 1, but we only have one data channel */
	record.channel = 0;
	record.pulse = base_time + (int64_t)record_time;

	/* The histogram channels seem to count backwards, with an upper limit
	 * of n_histogram_channels.
	 */
	record.time = /*n_histogram_channels - */ histogram_channel;

	print_t3(out_stream, &record, NEWLINE, options);
	print_status("picoquant", count, options);
}

void pq_print_interactive(FILE *out_stream, 
		int32_t curve, float64_t left_time,
		float64_t right_time, int32_t counts, options_t *options) {
	if ( options->binary_out ) {
		fwrite(&curve, 1, sizeof(curve), out_stream);
		fwrite(&left_time, 1, sizeof(left_time), out_stream);
		fwrite(&right_time, 1, sizeof(right_time), out_stream);
		fwrite(&counts, 1, sizeof(counts), out_stream);
	} else {
		fprintf(out_stream, 
				"%"PRId32",%.3"PRIf64",%.3"PRIf64",%"PRId32"\n", 
				curve, left_time,
				right_time, counts);
	}
	fflush(out_stream);
}

void external_marker(FILE *out_stream, uint32_t marker, 
		options_t *options) {
	fprintf(stderr, "External marker: %"PRIu32"\n", marker);
}

void print_resolution(FILE *out_stream, float64_t resolution, 
		options_t *options) {
	/* Given float representing the resolution in picoseconds, print the 
	 * appropriate value.
	 */
	fprintf(out_stream, "%le\n", resolution);
}
