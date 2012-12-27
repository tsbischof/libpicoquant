#include <stdlib.h>
#include <string.h>

#include "th_v50.h"

#include "../timeharp.h"
#include "../error.h"

/*
 *
 * TTTR mode routines.
 *
 */
void th_v50_t3_init(th_v50_header_t *th_header,
		th_v50_tttr_header_t *tttr_header,
		tttr_t *tttr) {
	tttr->origin = 0;
	tttr->overflows = 0;
	tttr->overflow_increment = TH_TTTR_OVERFLOW;
	tttr->sync_rate = tttr_header->SyncRate;
	tttr->resolution_float = th_header->Brd[0].Resolution*1e-9;
	tttr->resolution_int = (int)(tttr->resolution_float*1e12);
}

int th_v50_t3_decode(FILE *stream_in, tttr_t *tttr, t3_t *t3) {
	int result;
	th_v50_tttr_record_t record;

	result = fread(&record, sizeof(record), 1, stream_in);

	if ( result != 1 ) {
		if ( ! feof(stream_in) ) {
			error("Could not read t3 record.\n");
			return(PQ_ERROR_IO);
		} else {
			return(PQ_ERROR_EOF);
		}
	} else {
		if ( record.Valid ) {
			/* Normal record. */
			t3->channel = 0;
			t3->pulse = tttr->origin + record.TimeTag;
			t3->time = record.Data;
			return(PQ_RECORD_T3);
		} else {
			/* Special record. */
			if ( 0x800 & record.Data ) {
				/* Overflow */
				tttr->overflows++;
				tttr->origin += tttr->overflow_increment;
				return(PQ_RECORD_OVERFLOW);
			} else {
				t3->channel = 0x800;
				t3->pulse = record.Data & 0x07;
				return(PQ_RECORD_MARKER);
			}
		}
	}
}
				
int th_v50_t3_stream(FILE *stream_in, FILE *stream_out, 
		th_v50_header_t *th_header, th_v50_tttr_header_t *tttr_header,
		options_t *options) {
	tttr_t tttr;

	th_v50_t3_init(th_header, tttr_header, &tttr);

	if ( options->to_t2 ) {
		error("T3 -> T2 not supported for Timeharp.\n");
		return(PQ_ERROR_OPTIONS);
	} else {
		return(pq_t3_stream(stream_in, stream_out, 
				th_v50_t3_decode, &tttr, options));
	}
}
		
int th_v50_tttr_stream(FILE *stream_in, FILE *stream_out, 
		pq_header_t *pq_header, th_v50_header_t *th_header, 
		options_t *options) { 
	th_v50_tttr_header_t *tttr_header;
	int result;

	result = th_v50_tttr_header_read(stream_in, &tttr_header);
	if ( result != PQ_SUCCESS ) {
		error("Failed while reading the tttr header.\n");
	} else {
		if ( options->print_header ) {
			if ( options->binary_out ) {
				pq_header_fwrite(stream_out, pq_header);
				th_v50_header_fwrite(stream_out, th_header);
				th_v50_tttr_header_fwrite(stream_out, tttr_header);
			} else {
				pq_header_printf(stream_out, pq_header);
				th_v50_header_printf(stream_out, th_header);
				th_v50_tttr_header_printf(stream_out, tttr_header);
			}
			
			result = PQ_SUCCESS;
		} else if ( options->print_resolution ) {
			pq_resolution_print(stream_out, -1,
					(th_header->Brd[0].Resolution*1e3), options);
			result = PQ_SUCCESS;
		} else {
			result = th_v50_t3_stream(stream_in, stream_out,
					th_header, tttr_header, options);
		}
	}

	debug("Freeing tttr header.\n");
	th_v50_tttr_header_free(&tttr_header);
	return(result);
}

int th_v50_tttr_header_read(FILE *stream_in, 
		th_v50_tttr_header_t **tttr_header) {
	int result;

	*tttr_header = (th_v50_tttr_header_t *)malloc(sizeof(th_v50_tttr_header_t));

	if ( *tttr_header == NULL ) {
		error("Could not allocate tttr header.\n");
		return(PQ_ERROR_MEM);
	}

	result = fread(*tttr_header, 
			sizeof(th_v50_tttr_header_t) - sizeof(int32_t *),
			1, 
			stream_in);
	if ( result != 1 ) {
		error("Could not read tttr header.\n");
		th_v50_tttr_header_free(tttr_header);
		return(PQ_ERROR_IO);
	}

	if ( (*tttr_header)->SpecHeaderLength > 0 ) {
		(*tttr_header)->SpecHeader = (int32_t *)malloc(
				sizeof(int32_t)*(*tttr_header)->SpecHeaderLength);
		if ( (*tttr_header)->SpecHeader == NULL ) {
			error("Could not allocate special header.\n");
			th_v50_tttr_header_free(tttr_header);
			return(PQ_ERROR_MEM);
		}
	}

	return(PQ_SUCCESS);
}
			
void th_v50_tttr_header_free(th_v50_tttr_header_t **tttr_header) { 
	if ( *tttr_header != NULL ) {
		free((*tttr_header)->SpecHeader);
	}

	free(*tttr_header);
}

void th_v50_tttr_header_printf(FILE *stream_out, 
		th_v50_tttr_header_t *tttr_header) {
	int i;

	fprintf(stream_out, "TTTRGlobCLock = %"PRId32"\n", 
			tttr_header->TTTRGlobClock);

	for ( i = 0; i < 6; i++ ) {
		fprintf(stream_out, "Reserved[%d] = %"PRId32"\n",
				i, tttr_header->Reserved[i]);
	}

	fprintf(stream_out, "SyncRate = %"PRId32"\n", 
			tttr_header->SyncRate);
	fprintf(stream_out, "AverageCFDRate = %"PRId32"\n", 
			tttr_header->AverageCFDRate);
	fprintf(stream_out, "StopAfter = %"PRId32"\n", 
			tttr_header->StopAfter);
	fprintf(stream_out, "StopReason = %"PRId32"\n", 
			tttr_header->StopReason);
	fprintf(stream_out, "NumberOfRecords = %"PRId32"\n", 
			tttr_header->NumberOfRecords);
	fprintf(stream_out, "SpecHeaderLength = %"PRId32"\n",
			tttr_header->SpecHeaderLength);

	for ( i = 0; i < tttr_header->SpecHeaderLength; i++ ) {
		fprintf(stream_out, "SpecHeader[%d] = %"PRId32"\n", 
				i, tttr_header->SpecHeader[i]);
	}
}

void th_v50_tttr_header_fwrite(FILE *stream_out,
		th_v50_tttr_header_t *tttr_header) {
	fwrite(tttr_header,
			sizeof(th_v50_tttr_header_t) - sizeof(int32_t *),
			1, 
			stream_out);
	fwrite(tttr_header->SpecHeader,
			sizeof(int32_t),
			tttr_header->SpecHeaderLength,
			stream_out);
}
