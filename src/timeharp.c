#include <string.h>

#include "timeharp.h"
#include "timeharp/th_v20.h"
#include "timeharp/th_v30.h"
#include "timeharp/th_v50.h"
#include "timeharp/th_v60.h"

#include "error.h"

int th_dispatch(FILE *in_stream, FILE *out_stream, pq_header_t *pq_header,
		options_t *options) {
	int result;
	
	if ( ! strcmp("2.0", pq_header->FormatVersion) ) {
		result = th_v20_dispatch(in_stream, out_stream, pq_header, options);
	} else if ( ! strcmp("3.0", pq_header->FormatVersion) ) {
		result = th_v30_dispatch(in_stream, out_stream, pq_header, options);
	} else if ( ! strcmp("5.0", pq_header->FormatVersion) ) {
		result = th_v50_dispatch(in_stream, out_stream, pq_header, options);
	} else if ( ! strcmp("6.0", pq_header->FormatVersion) ) {
		result = th_v60_dispatch(in_stream, out_stream, pq_header, options);
	} else {
		error("Timeharp version not supported: %s.\n",
				pq_header->FormatVersion);
		result = PQ_ERROR_VERSION;
	}

	return(result);
}
