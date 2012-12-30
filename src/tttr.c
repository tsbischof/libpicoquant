#include "tttr.h"
#include "error.h"

void tttr_marker_print(FILE *stream_out, int64_t marker) {
	warn("External marker: %"PRId64"\n", marker);
}

