#include <string.h>

#include "hydraharp.h"
#include "hydraharp/hh_v10.h"
#include "hydraharp/hh_v20.h"

#include "error.h"

int hh_dispatch(FILE *in_stream, FILE *out_stream, pq_header_t *pq_header, 
		options_t *options) {
	int result;

	if ( ! strcmp("1.0", pq_header->FormatVersion) ) {
		result = hh_v10_dispatch(in_stream, out_stream, pq_header, options);
	} else if ( ! strcmp("2.0", pq_header->FormatVersion) ) {
		result = hh_v20_dispatch(in_stream, out_stream, pq_header, options);
	} else {
		error("HydraHarp version not supported: %s\n",
				pq_header->FormatVersion);
		result = PQ_ERROR_VERSION;
	}

	return(result);
}

