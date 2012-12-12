#ifndef TYPES_H_
#define TYPES_H_

#include <time.h>
#include <stdint.h>
#include <inttypes.h>

#ifdef __i386__
typedef time_t time32_t;
#endif
#ifdef __x86_64__
typedef int32_t time32_t;
#endif

typedef float float32_t;
typedef double float64_t;

#define PRIf32 "f"
#define SCNf32 "f"
#define PRIf64 "lf"
#define SCNf64 "lf"

char* ctime32(time32_t *mytime);

#define strtoi32 strtol
#define strtoi64 strtoll

int64_t i64abs(int64_t val);

#endif
