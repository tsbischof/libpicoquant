#ifndef HEADER_H_
#define HEADER_H_

#include <stdio.h>

// Enforce byte alignment to ensure cross-platform compatibility.
//#pragma pack(4)

// Define the common file header.
typedef struct {
	char Ident[16];
	char FormatVersion[6];
} pq_header_t;

int pq_header_read(FILE *in_stream, pq_header_t *pq_header);
void pq_header_printf(FILE *out_stream, pq_header_t *pq_header);
void pq_header_fwrite(FILE *out_stream, pq_header_t *pq_header);

#endif 
