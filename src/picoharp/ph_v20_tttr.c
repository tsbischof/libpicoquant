#include <stdlib.h>

#include "ph_v20.h"

#include "../picoharp.h"
#include "../error.h"

/*
 *
 * Reading and interpreting for t2 mode.
 *
 */
void ph_v20_t2_init(ph_v20_header_t *ph_header,
		ph_v20_tttr_header_t *tttr_header, 
		tttr_t *tttr) {
	tttr->origin = 0;
	tttr->overflows = 0;
	tttr->overflow_increment = PH_T2_OVERFLOW;
	tttr->sync_rate = tttr_header->InpRate0;
	tttr->resolution_float = PH_V20_BASE_RESOLUTION;
	tttr->resolution_int = (int)(tttr->resolution_float * 1e12);
}

int ph_v20_t2_decode(FILE *stream_in, tttr_t *tttr, t2_t *t2) {
	int result;
	ph_v20_t2_record_t record;

	result = fread(&record, sizeof(record), 1, stream_in);

	if ( result != 1 ) {
		if ( ! feof(stream_in) ) {
			error("Could not read Picoharp t2 record.\n");	
			return(PQ_ERROR_IO);
		} else { 
			return(PQ_ERROR_EOF);
		}
	} else {
		/* Now, interpret the record as an overflow or data. */
		if ( record.channel == 15 ) {
			/* Special record */
			if ( (record.time & 01111) == 01111 ) {
				/* External marker. */
				t2->channel = 15;
				t2->time = record.time;
				return(PQ_RECORD_MARKER);
			} else {
				/* Overflow */
				tttr->overflows++;
				tttr->origin += tttr->overflow_increment;
				return(PQ_RECORD_OVERFLOW);
			}
		} else {
			t2->channel = record.channel;
			t2->time = tttr->origin * tttr->resolution_int + 
					record.time * tttr->resolution_int;
			return(PQ_RECORD_T2);
		}
	}
}

/*
 *
 * Reading and interpreting for t3 mode.
 *
 */

void ph_v20_t3_init(ph_v20_header_t *ph_header,
		ph_v20_tttr_header_t *tttr_header, 
		tttr_t *tttr) {
	tttr->origin = 0;
	tttr->overflows = 0;
	tttr->overflow_increment = PH_T3_OVERFLOW;
	tttr->sync_rate = tttr_header->InpRate0;
	tttr->resolution_float = ph_header->Brd[0].Resolution*1e-9;
	tttr->resolution_int = (int)(tttr->resolution_float*1e12);
}

int ph_v20_t3_decode(FILE *stream_in, tttr_t *tttr, t3_t *t3) {
	int result;
	ph_v20_t3_record_t record;

	result = fread(&record, sizeof(record), 1, stream_in);
	if ( result != 1 ) {
		if ( !feof(stream_in) ) {
			error("Could not read t3 record.\n");
			return(PQ_ERROR_IO);
		} else {
			return(PQ_ERROR_EOF);
		}
	} else {
		if ( record.channel == 15 ) {
			/* Special record. */
			if ( record.dtime == 0 ) {
				tttr->overflows++;
				tttr->origin += tttr->overflow_increment;
				return(PQ_RECORD_OVERFLOW);
			} else {
				t3->channel = 15;
				t3->pulse = record.dtime;
				return(PQ_RECORD_MARKER);
			}
		} else {
			t3->channel = record.channel;
			t3->pulse = tttr->origin + record.nsync;
			t3->time = record.dtime * tttr->resolution_int;
			return(PQ_RECORD_T3);
		}
	}
}

/*
 *
 * Streaming for t2 or t3 mode.
 *
 */
int ph_v20_tttr_stream(FILE *stream_in, FILE *stream_out,
		pq_header_t *pq_header, ph_v20_header_t *ph_header, 
		options_t *options) {
	ph_v20_tttr_header_t *tttr_header;
	int result;

	result = ph_v20_tttr_header_read(stream_in, &tttr_header);
	if ( result != PQ_SUCCESS ) {
		error("Failed while reading tttr header.\n");
	} else {
		if ( options->print_header ) {
			debug("Printing file header.\n");
				if ( options->binary_out ) {
				pq_header_fwrite(stream_out, pq_header);
				ph_v20_header_fwrite(stream_out, ph_header);
				ph_v20_tttr_header_fwrite(stream_out, tttr_header);
			} else {
				pq_header_printf(stream_out, pq_header);
				ph_v20_header_printf(stream_out, ph_header);
				ph_v20_tttr_header_printf(stream_out, tttr_header);
			}
			
			result = PQ_SUCCESS;
		} else if ( options->print_resolution ) {
			pq_resolution_print(stream_out, -1, 
					(ph_header->Brd[0].Resolution*1e3), options);
			result = PQ_SUCCESS;
		} else {
			if ( ph_header->MeasurementMode == PH_MODE_T2 ) {
				result = ph_v20_t2_stream(stream_in, stream_out, 
						ph_header, tttr_header, options);
			} else if ( ph_header->MeasurementMode == PH_MODE_T3 ) {
				result = ph_v20_t3_stream(stream_in, stream_out,
						ph_header, tttr_header, options);
			} else {
				debug("Unrecognized mode.\n");
				result = PQ_ERROR_MODE;
			}
		}
	}

	debug("Cleaning tttr header.\n");
	ph_v20_tttr_header_free(&tttr_header);
	return(result);
}

int ph_v20_t2_stream(FILE *stream_in, FILE *stream_out, 
		ph_v20_header_t *ph_header, ph_v20_tttr_header_t *tttr_header,
		options_t *options) {
	tttr_t tttr;

	ph_v20_t2_init(ph_header, tttr_header, &tttr);
	return(pq_t2_stream(stream_in, stream_out, 
			ph_v20_t2_decode, &tttr, options));
}

int ph_v20_t3_stream(FILE *stream_in, FILE *stream_out, 
		ph_v20_header_t *ph_header, ph_v20_tttr_header_t *tttr_header,
		options_t *options) {
	tttr_t tttr;

	ph_v20_t3_init(ph_header, tttr_header, &tttr);

	return(pq_t3_stream(stream_in, stream_out,
			ph_v20_t3_decode, &tttr, options));
}

/*
 * 
 * Header for tttr mode (t2, t3)
 *
 */
int ph_v20_tttr_header_read(FILE *stream_in, 
		ph_v20_tttr_header_t **tttr_header) {
	int result;

	*tttr_header = (ph_v20_tttr_header_t *)malloc(sizeof(ph_v20_tttr_header_t));

	if ( *tttr_header == NULL ) {
		error("Could not allocate tttr header.\n");
		return(PQ_ERROR_MEM);
	}

	result = fread(*tttr_header, 
			sizeof(ph_v20_tttr_header_t)-sizeof(uint32_t *), 
			1, 
			stream_in);
	if ( result != 1 ) {
		error("Could not read Picoharp tttr header.\n");
		ph_v20_tttr_header_free(tttr_header);
		return(PQ_ERROR_IO);
	}

	(*tttr_header)->ImgHdr = (uint32_t *)malloc(
			(*tttr_header)->ImgHdrSize*sizeof(uint32_t));
	if ( (*tttr_header)->ImgHdr == NULL ) {
		error("Could not allocate memory for Picoharp tttr image header.\n");
		ph_v20_tttr_header_free(tttr_header);
		return(PQ_ERROR_MEM);
	}

	result = fread((*tttr_header)->ImgHdr, 
			sizeof(uint32_t),
			(*tttr_header)->ImgHdrSize, 
			stream_in);
	if ( result != (*tttr_header)->ImgHdrSize ) {
		error("Could not read Picoharp tttr image header.\n");
		ph_v20_tttr_header_free(tttr_header);
		return(PQ_ERROR_IO);
	}

	return(PQ_SUCCESS);
}

void ph_v20_tttr_header_free(ph_v20_tttr_header_t **tttr_header) {
	if ( *tttr_header != NULL ) {
		free((*tttr_header)->ImgHdr);
		free(*tttr_header);
	} 
}

void ph_v20_tttr_header_printf(FILE *stream_out,
		ph_v20_tttr_header_t *tttr_header) {
	int i;
	fprintf(stream_out, "ExtDevices = %"PRId32"\n", tttr_header->ExtDevices);

	for ( i = 0; i < 2; i++ ) {
		fprintf(stream_out, "Reserved[%d] = %"PRId32"\n", i,
				tttr_header->Reserved[i]);
	}

	fprintf(stream_out, "InpRate0 = %"PRId32"\n", tttr_header->InpRate0);
	fprintf(stream_out, "InpRate1 = %"PRId32"\n", tttr_header->InpRate1);
	fprintf(stream_out, "StopAfter = %"PRId32"\n", tttr_header->StopAfter);
	fprintf(stream_out, "StopReason = %"PRId32"\n", tttr_header->StopReason);
	fprintf(stream_out, "NumRecords = %"PRId32"\n", tttr_header->NumRecords);
	fprintf(stream_out, "ImgHdrSize = %"PRId32"\n", tttr_header->ImgHdrSize);

	for ( i = 0; i < tttr_header->ImgHdrSize; i++ ) {
		fprintf(stream_out, "ImgHdr[%d] = %"PRIu32"\n", i,
				tttr_header->ImgHdr[i]);
	}
}

void ph_v20_tttr_header_fwrite(FILE *stream_out,
		ph_v20_tttr_header_t *tttr_header) {
	fwrite(tttr_header, 
			sizeof(ph_v20_tttr_header_t) - sizeof(uint32_t *),
			1,
			stream_out);
	fwrite(tttr_header->ImgHdr,
			sizeof(uint32_t),
			tttr_header->ImgHdrSize,
			stream_out);
}

