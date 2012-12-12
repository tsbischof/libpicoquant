#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "error.h"
#include "t2.h"

int next_t2(FILE *in_stream, t2_t *record, options_t *options) {
	int result;

	if ( options->binary_in ) {
		result = fread(record, sizeof(t2_t), 1, in_stream);
		result = (result != 1);
	} else {
		result = fscanf(in_stream, 
				"%"SCNd32",%"SCNd64,
			 	&(record->channel),
				&(record->time));
		result = (result != 2);
	}
	
	return(result);
}

void print_t2(FILE *out_stream, t2_t *record, int print_newline,
		options_t *options) {
	if ( options->binary_out ) {
		fwrite(record, sizeof(t2_t), 1, out_stream);
	} else {
		fprintf(out_stream, "%"PRId32",%"PRId64, 
				record->channel,
				record->time);

		if ( print_newline == NEWLINE ) {
			fprintf(out_stream, "\n");
		}
	}
}

int t2_comparator(const void *a, const void *b) {
	/* Comparator to be used with standard sorting algorithms (qsort) to sort
	 * t2 photons. 
     */
	/* The comparison must be done explicitly to avoid issues associated with
	 * casting int64_t to int. If we just return the difference, any value
	 * greater than max_int would cause problems.
	 */
	int64_t difference = ((t2_t *)a)->time - ((t2_t *)b)->time;
	return( difference > 0 );
}

t2_queue_t *allocate_t2_queue(int queue_length) {
	int result = 0;
	t2_queue_t *queue;

	queue = (t2_queue_t *)malloc(sizeof(t2_queue_t));
	if ( queue == NULL ) {
		result = -1;
	} else {
		queue->length = queue_length;
		queue->left_index = -1;
		queue->right_index = -1;
	
		queue->queue = (t2_t *)malloc(sizeof(t2_t)*queue->length);
		if ( queue->queue == NULL ) {
			result = -1;
		}
	}

	if ( result ) {
		free_t2_queue(&queue);
	}

	return(queue);
}

void free_t2_queue(t2_queue_t **queue) {
	if ( *queue != NULL ) {
		if ( (*queue)->queue != NULL ) {
			free((*queue)->queue);
		}
		free(*queue);
	}
}

int t2_queue_full(t2_queue_t *queue) {
	/* If the queue has no free spaces, returns true. */
	return( queue->right_index - queue->left_index >= (queue->length-1) );
}

int t2_queue_pop(t2_queue_t *queue, t2_t *record) {
	int result = t2_queue_front(queue, record);

	if ( ! result ) {
		queue->left_index++;
		if ( queue->left_index > queue->right_index ) {
			queue->left_index = -1;
			queue->right_index = -1;
		}
	}

	return(result);
}
		

int t2_queue_push(t2_queue_t *queue, t2_t *record) {
	int64_t next_index = (queue->right_index + 1) % queue->length;
	
	debug("Pushing at index %"PRId64"\n", next_index);
	if ( t2_queue_full(queue) ) {
		error("Queue overflow, with limits (%"PRId64", %"PRId64"). "
				"Extend its size to continue with the calculation.\n",
				queue->left_index, queue->right_index);
		return(-1);
	} else {
		memcpy(&(queue->queue[next_index]), record, sizeof(t2_t));
		queue->right_index++;
		if ( queue->left_index < 0 ) {
			queue->left_index = 0;
		}
		return(0);
	}
}

int t2_queue_front(t2_queue_t *queue, t2_t *record) {
	int64_t index = queue->left_index % queue->length;
	debug("Front of queue is at index %"PRId64"\n", index);
	if ( queue->left_index < 0 ) {
		return(-1);
	} else {
		memcpy(record, 
				&(queue->queue[index]),
				sizeof(t2_t));
		return(0);
	}
}

int t2_queue_back(t2_queue_t *queue, t2_t *record) {
	int64_t index = queue->right_index % queue->length;
	debug("Back of queue is at index %"PRId64"\n", index);
	if ( queue->right_index < 0 ) {
		return(-1);
	} else {
		memcpy(record,
				&(queue->queue[index]),
				sizeof(t2_t));
		return(0);
	}
}

int t2_queue_index(t2_queue_t *queue, t2_t *record, int index) {
	int64_t true_index = (queue->left_index + index) % queue->length;

	if ( index > t2_queue_size(queue) ) {
		debug("Requested return of non-existent index: %"PRId64"\n", 
				true_index);
		return(-1);
	} else { 
		memcpy(record, &(queue->queue[true_index]), sizeof(t2_t));
		return(0);
	}
}

int64_t t2_queue_size(t2_queue_t *queue) {
	if ( queue->left_index < 0 || queue->right_index < 0 ) {
		return(0);
	} else {
		return(queue->right_index - queue->left_index + 1);
	}
}

void t2_queue_sort(t2_queue_t *queue) {
	/* First, check if the queue wraps around. If it does, we need to move 
	 * everything into one continous block. If it does not, it is already in 
	 * a continuous block and is ready for sorting.
	 */
	int64_t right = queue->right_index % queue->length;
	int64_t left = queue->left_index % queue->length;
	if ( t2_queue_full(queue) ) {
		debug("Queue is full, no action is needed to make it contiguous for "
				"sorting.\n");
	} else {
		warn("Sorting with a non-full queue is not thoroughly tested. "
				"Use these results at your own risk.\n");
		if ( right < left ) {
			/* The queue wraps around, so join the two together by moving the 
			 * right-hand bit over. 
			 * xxxxx---yyyy -> xxxxxyyyy---
			 */
			debug("Queue loops around, moving records to "
					"make them contiguous.\n");
			memmove(&(queue->queue[right+1]), 
					&(queue->queue[left]),
					sizeof(t2_t)*(queue->length - left));
		} else if ( left != 0 && ! t2_queue_full(queue) ) {
			/* There is one continuous block, so move it to the front. 
			 * ---xxxx -> xxxx---
			 */
			debug("Queue is offset from beginning, moving it forward.\n");
			memmove(queue->queue,
					&(queue->queue[left]),
					sizeof(t2_t)*t2_queue_size(queue));
		} else {
			debug("Queue starts at the beginning of the array, "
					"no action needed to make it contiguous.\n");
		}
	}

	queue->right_index = t2_queue_size(queue) - 1;
	queue->left_index = 0;

	debug("Sorting %"PRId64" photons.\n", t2_queue_size(queue));

	qsort(queue->queue, 
			t2_queue_size(queue), sizeof(t2_t), t2_comparator);
	/*t2_queue_front(queue, &record);
	fprintf(stderr, "%"PRId32",%"PRId64"\n", record.channel, record.time);
	t2_queue_back(queue, &record);
	fprintf(stderr, "%"PRId32",%"PRId64"\n", record.channel, record.time);*/
}

void yield_t2_queue(FILE *out_stream, t2_queue_t *queue, options_t *options) {
	t2_t record;
	while ( ! t2_queue_pop(queue, &record) ) {
		print_t2(out_stream, &record, NEWLINE, options);
	}
}

/* These functions break up the photons into subsets by dividing time into
 * windows of fixed length. Thus we need to be able to receive a photon from 
 * a window, and to iterate between the windows.
 */
void init_t2_window(t2_window_t *window, 
		int64_t start_time, options_t *options) {
	if ( options->set_start_time ) {
		window->limits.lower = options->start_time;
	} else {
		window->limits.lower = start_time;
	}

	if ( options->bin_width ) {
		window->limits.upper = options->bin_width*
				((int)floor(window->limits.lower/options->bin_width)+1);
	} else {
		window->limits.upper = 0;
	}

	window->width = options->bin_width;

	window->set_time_limit = options->set_stop_time;
	window->time_limit = options->stop_time;

	if ( window->set_time_limit && window->limits.upper > window->time_limit ) {
		window->limits.upper = window->time_limit;
	}
}

void next_t2_window(t2_window_t *window) {
	window->limits.lower = window->limits.upper;
	window->limits.upper += window->width;

	if ( window->set_time_limit && window->limits.upper > window->time_limit ) {
		window->limits.upper = window->time_limit;
	}
}

int init_t2_windowed_stream(t2_windowed_stream_t *stream, 
		FILE *in_stream, options_t *options) {
	stream->yielded_photon = 0;
	stream->in_stream = in_stream;

	if ( next_t2(in_stream, &(stream->current_photon), options) ) {
		return(-1);
	} else {
		init_t2_window(&(stream->window), 
				stream->current_photon.time, options);
	}

	return(0);
}

int next_t2_windowed(t2_windowed_stream_t *stream, t2_t *record,
		options_t *options) {
	/* Deal with the actual photon now. */
	if ( stream->yielded_photon ) {
		stream->current_photon.channel = -1;
	}
		
	if ( stream->current_photon.channel == -1 ) {
		if ( next_t2(stream->in_stream, 
				&(stream->current_photon), options) ) {
			return(-1);
		}
	}

	/* Test that the photon is within the absolute limits */ 
	if ( options->set_start_time &&
			stream->current_photon.time < options->start_time ) {
		stream->yielded_photon = 1;
		return(2);
	} 

	if ( options->set_stop_time && 
			stream->current_photon.time >= options->stop_time ) {
		return(-1);
	}

	/* Process the photon normally. */
	if ( (! options->count_all) 
			&& stream->current_photon.time > stream->window.limits.upper ) {
		stream->yielded_photon = 0;
		return(1);
	} else if ( stream->current_photon.time < stream->window.limits.lower ) {
		/* this helps prevent inifinite loops: imagine that we define the 
		 * window to start after the photon, at which point we keep moving 
		 * forward without ever finding the correct window.
 		 */
		return(-2);
	} else {
		stream->yielded_photon = 1;
		
		record->channel = stream->current_photon.channel;
		record->time = stream->current_photon.time;
		return(0);
	}
}
