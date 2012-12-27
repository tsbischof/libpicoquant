#include <string.h>

#include "picoharp.h"

#include "picoharp/ph_v20.h"
#include "error.h"

int ph_dispatch(FILE *stream_in, FILE *stream_out, pq_header_t *pq_header,
		options_t *options) {
	int result;

	if ( ! strcmp("2.0", pq_header->FormatVersion) ) {
		result = ph_v20_dispatch(stream_in, stream_out, pq_header, options);
	} else {
		error("Picoharp version not supported: %s\n", 
				pq_header->FormatVersion);
		result = PQ_ERROR_VERSION;
	}

	return(result);
}
