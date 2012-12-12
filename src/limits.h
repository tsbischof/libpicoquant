#ifndef LIMITS_H_
#define LIMITS_H_

#define SCALE_UNKNOWN -1
#define SCALE_LINEAR 1
#define SCALE_LOG 2
#define SCALE_LOG_ZERO 3

#include "types.h"

typedef struct {
	int64_t lower;
	int64_t upper;
} window_t;

typedef struct {
	float64_t lower;
	int32_t bins;
	float64_t upper;
} limits_t;

int str_to_limits(char *str, limits_t *limits);
int scale_parse(char *str, int *scale);

#endif
