#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "options.h"

#define USAGE 1

extern int verbose;

void debug(char *message, ...);
void error(char *message, ...);
void warn(char *message, ...);

void print_status(char *name, long long record_number, options_t *options);
int parse_result(int result);

#endif
