#include "header.h"

#include "error.h"

int pq_header_read(FILE *stream_in, pq_header_t *pq_header) {
	int result;
	result = fread(pq_header, sizeof(pq_header_t), 1, stream_in);
	if ( result == 1 ) {
		return(PQ_SUCCESS);
	} else {
		return(PQ_ERROR_IO);
	}
}

void pq_header_printf(FILE *stream_out, pq_header_t *pq_header) {
	fprintf(stream_out, "Ident = %s\n", pq_header->Ident);
	fprintf(stream_out, "FormatVersion = %s\n", pq_header->FormatVersion);
}

void pq_header_fwrite(FILE *stream_out, pq_header_t *pq_header) {
	fwrite(pq_header, sizeof(pq_header_t), 1, stream_out);
}
