#include "t2.h"

#include "error.h"

int pq_t2_stream(FILE *stream_in, FILE *stream_out,
		pq_t2_decode_t decode, tttr_t *tttr, options_t *options) {
	/* 
	 * Use the specified decoder to process the incoming stream of t2 records.
	 */
	int64_t record_count = 0;
	int result = PQ_SUCCESS;
	t2_t t2;
	pq_t2_print_t print;

	if ( options->binary_out ) {
		print = pq_t2_fwrite;
	} else {
		print = pq_t2_printf;
	}

	/* To do: add some logic to read a large amount of records (e.g. 1024) and
	 * then process them. This should reduce the number of disk i/o calls.
	 */

	while ( ! pq_check(result) && 
			! feof(stream_in) &&
			record_count < options->number ) {
		result = pq_t2_next(stream_in, decode, tttr, &t2);

		if ( ! pq_check(result) ) {
			/* Found a record, process it. */
			if ( result == PQ_RECORD_T2 ) {
				record_count++;
				pq_record_status_print("picoquant", record_count, options);
				print(stream_out, &t2);
			} else if ( result == PQ_RECORD_MARKER ) {
				tttr_marker_print(stream_out, t2.time);
			} else if ( result == PQ_RECORD_OVERFLOW ) {
				/* overflow must be performed in the decoder. */
			} else { 
				error("Record type not recognized: %d\n", result);
				result = PQ_ERROR_UNKNOWN_DATA;
			}
		} else if ( result == PQ_ERROR_EOF ) {
			/* loop will take care of the EOF */
			result = PQ_SUCCESS;
		}
	}

	return(result);
}	

int pq_t2_next(FILE *in_stream, pq_t2_decode_t decode, 
		tttr_t *tttr, t2_t *t2) {
/*
 * Return the next t2 record from the incoming stream.
 */
	return(decode(in_stream, tttr, t2));
}

void pq_t2_printf(FILE *out_stream, t2_t *record) {
/* 
 * Print the t2 record in csv format.
 */
	fprintf(out_stream, "%"PRIu32",%"PRIu64"\n", 
			record->channel,
			record->time);
}

void pq_t2_fwrite(FILE *out_stream, t2_t *record) {
/*
 * Write the t2 record as a binary structure.
 */
	fwrite(record, sizeof(t2_t), 1, out_stream);
}
