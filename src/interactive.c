#include "interactive.h"

void pq_interactive_bin_printf(FILE *out_stream, pq_interactive_bin_t *bin) {
/*
 * Print the interactive bin, in ascii format.
 */
	fprintf(out_stream, "%"PRIu32",%.2"PRIf64",%.2"PRIf64",%"PRIu32"\n",
			bin->curve,
			bin->bin_left*1e3,
			bin->bin_right*1e3,
			bin->counts);
}

void pq_interactive_bin_fwrite(FILE *out_stream, pq_interactive_bin_t *bin) {
/* 
 * Write the interactive bin to file, in binary format.
 */
	fwrite(bin, sizeof(pq_interactive_bin_t), 1, out_stream);
}

