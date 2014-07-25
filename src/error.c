/*
 * Copyright (c) 2011-2014, Thomas Bischof
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the Massachusetts Institute of Technology nor the 
 *    names of its contributors may be used to endorse or promote products 
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "error.h"

int verbose = 0;

void debug(char *message, ...) {
/* 
 * If in debug mode, we want to print a message. Otherwise, we want
 * to ignore it.
 */
	va_list args;
	if ( verbose ) {
		va_start(args, message);
		fprintf(stderr, "DEBUG: ");
		vfprintf(stderr, message, args);
		va_end(args);
		fflush(stderr);
	}
}

void error(char *message, ...) {
/* 
 * Handle error messages. This is here in case we ever want to do 
 * something more than just print them.
 */
	va_list args;
	va_start(args, message);
	fprintf(stderr, "ERROR: ");
	vfprintf(stderr, message, args);
	va_end(args);
	fflush(stderr);
}

void warn(char *message, ...) {
/* 
 * Warnings about untested features, etc. 
 */
	va_list args;
	va_start(args, message);
	fprintf(stderr, "WARNING: ");
	vfprintf(stderr, message, args);
	va_end(args);
	fflush(stderr);
}

int pq_check(int status) {
	if ( status >= PQ_SUCCESS ) {
		return(PQ_SUCCESS);
	} else {
		return(status);
	}
}

void pq_record_status_print(char *name, uint64_t count, options_t *options) {
	time_t rawtime;
	struct tm *timeinfo;
	char fmttime[80];

	if ( (options->print_every > 0) && 
			( (count % options->print_every) == 0 ) ) {
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(fmttime, 80, "%Y.%m.%d %H.%M.%S", timeinfo);
		fprintf(stderr, "%s: (%s) Record %20"PRIu64"\n", fmttime, name, count);
	}
}

