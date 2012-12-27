#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>
#include <stdarg.h>

#include "options.h"

// Mode codes
#define PQ_USAGE                        1
#define PQ_HEADER                       2
#define PQ_VERSION                      3
#define PQ_RESOLUTION                   4
#define PQ_DATA                         5
#define PQ_RECORD_INTERACTIVE           6
#define PQ_RECORD_CONTINUOUS            7
#define PQ_RECORD_T2                    8
#define PQ_RECORD_T3                    9
#define PQ_RECORD_MARKER               10
#define PQ_RECORD_OVERFLOW             11


// Error codes
#define	PQ_SUCCESS	                    0
#define PQ_ERROR_OPTIONS               -1
#define PQ_ERROR_IO                    -2
#define PQ_ERROR_VERSION               -3
#define PQ_ERROR_EOF                   -4
#define PQ_ERROR_UNKNOWN_DATA          -5
#define PQ_ERROR_MODE                  -6
#define PQ_ERROR_MEM                   -7

extern int verbose;

void debug(char *message, ...);
void error(char *message, ...);
void warn(char *message, ...);

int pq_check(int status);
void pq_record_status_print(char *name, uint64_t count, options_t *options);

#endif
