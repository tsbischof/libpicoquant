#include <math.h> 

#include "t3.h"
#include "error.h"

int pq_t3_stream(FILE *stream_in, FILE *stream_out, pq_t3_decode_t decode, 
		tttr_t *tttr, options_t *options) {
	/* 
	 * Use the specified decoder to process the incoming stream of t3 records.
	 */
	int64_t record_count = 0;
	int result = PQ_SUCCESS;
	t3_t t3;
	t2_t t2;
	pq_t3_print_t print_t3;
	pq_t2_print_t print_t2;

	if ( options->binary_out ) {
		print_t3 = pq_t3_fwrite;
		print_t2 = pq_t2_fwrite;
	} else {
		print_t3 = pq_t3_printf;
		print_t2 = pq_t2_printf;
	}

	/* To do: add some logic to read a large amount of records (e.g. 1024) and
	 * then process them. This should reduce the number of disk i/o calls.
	 */
	while ( ! pq_check(result) && 
			! feof(stream_in) &&
			record_count < options->number ) {
		result = pq_t3_next(stream_in, decode, tttr, &t3);

		if ( ! pq_check(result) ) {
			/* Found a record, process it. */
			if ( result == PQ_RECORD_T3 ) {
				record_count++;
				pq_record_status_print("picoquant", record_count, options);
				if ( options->to_t2 ) {
					pq_t3_to_t2(&t3, &t2, tttr);
					print_t2(stream_out, &t2);
				} else {
					print_t3(stream_out, &t3);
				}
			} else if ( result == PQ_RECORD_MARKER ) {
				tttr_marker_print(stream_out, t3.pulse);
			} else if ( result == PQ_RECORD_OVERFLOW ) {
				/* overflows must be performed in the decoder. */
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

int pq_t3_next(FILE *stream_in, pq_t3_decode_t decode, 
		tttr_t *tttr, t3_t *t3) {
	return(decode(stream_in, tttr, t3));
}

void pq_t3_printf(FILE *stream_out, t3_t *record) {
	fprintf(stream_out, "%"PRIu32",%"PRIu64",%"PRIu64"\n",
			record->channel,
			record->pulse,
			record->time);
}

void pq_t3_fwrite(FILE *stream_out, t3_t *record) {
	fwrite(record, sizeof(t3_t), 1, stream_out);
}

void pq_t3_to_t2(t3_t *record_in, t2_t *record_out, tttr_t *tttr) {
/*
 * Use the t3 sync rate to convert the pulse number into a t3-like time, in ps.
 */
	record_out->channel = record_in->channel;
	record_out->time = (int64_t)floor(
		record_in->pulse * (1e12 / tttr->sync_rate) );
}
