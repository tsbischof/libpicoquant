#include "tttr.h"
#include "error.h"

void tttr_marker_print(FILE *stream_out, uint64_t marker) {
	warn("External marker: %"PRIu64"\n", marker);
}

