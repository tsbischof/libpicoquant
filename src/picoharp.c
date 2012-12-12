#include <string.h>

#include "picoharp.h"

#include "picoharp/ph_v20.h"
#include "error.h"

int ph_dispatch(FILE *in_stream, FILE *out_stream, pq_header_t *pq_header,
		options_t *options) {
	int result;

	if ( ! strcmp("2.0", pq_header->FormatVersion) ) {
		result = ph_v20_dispatch(in_stream, out_stream, pq_header, options);
	} else {
		error("Picoharp version not supported: %s\n", 
				pq_header->FormatVersion);
		result = PQ_VERSION_ERROR;
	}

	return(result);
}
