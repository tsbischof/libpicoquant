#ifndef STREAM_H_
#define STREAM_H_

typedef struct _pq_stream_t {
	record_t record;

	int (*next)(struct _pq_stream_t *stream);
} pq_stream_t;

pq_stream_t *pq_stream_alloc()
pq_stream_free()
pq_stream_next()

#endif
