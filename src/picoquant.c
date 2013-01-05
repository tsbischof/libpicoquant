#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>

#include "picoquant.h"
#include "error.h"
#include "options.h"

#include "picoharp.h"
#include "hydraharp.h"
#include "timeharp.h"

#include "interactive.h"
#include "continuous.h"
#include "t2.h"
#include "t3.h"

int pq_dispatch(FILE *stream_in, FILE *stream_out, options_t *options) {
	int result;
	pq_header_t pq_header;
	pq_dispatch_t dispatch;

	/* Do the actual work, if we have no errors. */
	result = pq_header_read(stream_in, &pq_header);

	if ( result ) {
		error("Could not read string header.\n");
	} else {
		dispatch = pq_dispatch_get(options, &pq_header);
		if ( dispatch == NULL ) {
			error("Could not identify board %s.\n", pq_header.Ident);
		} else {
			result = dispatch(stream_in, stream_out, 
					&pq_header, options);
		}
	}

	return(result);
}       


pq_dispatch_t pq_dispatch_get(options_t *options, pq_header_t *pq_header) {
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

void pq_resolution_print(FILE *stream_out, 
		int curve, float64_t resolution, 
		options_t *options) {
	if ( options->binary_out ) {
		fwrite(&resolution, 1, sizeof(resolution), stream_out);
	} else {
		if ( curve >= 0 ) {
			fprintf(stream_out, "%d,", curve);
		}
		fprintf(stream_out, "%.2"PRIf64"\n", resolution);
	}
}
	
