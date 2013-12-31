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
