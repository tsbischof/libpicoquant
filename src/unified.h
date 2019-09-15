#ifndef UNIFIED_H_
#define UNIFIED_H_

#include <stdio.h>
#include "header.h"
#include "options.h"

#define PU_TAG_Empty8      0xFFFF0008
#define PU_TAG_Bool8       0x00000008
#define PU_TAG_Int8        0x10000008
#define PU_TAG_BitSet64    0x11000008
#define PU_TAG_Color8      0x12000008
#define PU_TAG_Float8      0x20000008
#define PU_TAG_TDateTime   0x21000008
#define PU_TAG_Float8Array 0x2001FFFF
#define PU_TAG_AnsiString  0x4001FFFF
#define PU_TAG_WideString  0x4002FFFF
#define PU_TAG_BinaryBlob  0xFFFFFFFF

#define PU_RECORD_PH_T3 0x00010303
#define PU_RECORD_PH_T2 0x00010203
#define PU_RECORD_HH_V1_T3 0x00010304
#define PU_RECORD_HH_V1_T2 0x00010204
#define PU_RECORD_HH_V2_T3 0x01010304
#define PU_RECORD_HH_V2_T2 0x01010204
#define PU_RECORD_TH_260_NT3 0x00010305
#define PU_RECORD_TH_260_NT2 0x00010205
#define PU_RECORD_TH_260_PT3 0x00010306
#define PU_RECORD_TH_260_PT2 0x00010206

typedef struct pu_tag_t {
	char ident[32];
	int32_t index;
	uint32_t type;
	int64_t value;
} pu_tag_t;

typedef struct pu_options_t {
	int64_t record_type;
	float64_t resolution_seconds;
} pu_options_t;

int pu_dispatch(FILE *stream_in, FILE *stream_out, pu_header_t *pu_header, options_t *options);
int pu_tags_read(FILE *stream_in, FILE *stream_out, pu_header_t *pu_header, options_t *options, pu_options_t *pu_options);

#endif
