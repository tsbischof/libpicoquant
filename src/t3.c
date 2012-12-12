#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "t3.h"
#include "error.h"

int next_t3(FILE *in_stream, t3_t *record, options_t *options) {
	int result;

	if ( options->binary_in ) {
		result = ( fread(record, sizeof(t3_t), 1, in_stream) != 1);
	} else {
		result = ( fscanf(in_stream, "%"PRId32",%"PRId64",%"PRId32"",
				&(record->channel),
				&(record->pulse),
				&(record->time)) != 3 );
	}
	
	return(result);
}

void print_t3(FILE *out_stream, t3_t *record, 
		int print_newline, options_t *options) {
	if ( options->binary_out ) {
		fwrite(record, sizeof(t3_t), 1, out_stream);
	} else {
		fprintf(out_stream, "%"PRId32",%"PRId64",%"PRId32, 
				record->channel,
				record->pulse,
				record->time);
		if ( print_newline == NEWLINE ) {
			fprintf(out_stream, "\n");
		}
	}
}

int t3_comparator(const void *a, const void *b) {
	/* Comparator to be used with standard sorting algorithms (qsort) to sort
	 * t3 photons. 
     */
	/* The comparison must be done explicitly to avoid issues associated with
	 * casting int64_t to int. If we just return the difference, any value
	 * greater than max_int would cause problems.
	 */
	/* The comparator needed for sorting a list of t3 photons. Follows the 
     * standard of qsort (-1 sorted, 0 equal, 1 unsorted)
	 */
	int64_t difference;

	if ( ((t3_t *)a)->pulse == ((t3_t *)b)->pulse ) {
		difference = ((t3_t *)a)->time - ((t3_t *)b)->time;
	} else {
		difference = ((t3_t *)a)->pulse - ((t3_t *)b)->pulse;
	}

	return( difference > 0 );
}

t3_queue_t *allocate_t3_queue(int queue_length) {
	int result = 0;
	t3_queue_t *queue;

	queue = (t3_queue_t *)malloc(sizeof(t3_queue_t));
	if ( queue == NULL ) {
		result = -1;
	} else {
		queue->length = queue_length;
		queue->left_index = -1;
		queue->right_index = -1;
	
		queue->queue = (t3_t *)malloc(sizeof(t3_t)*queue->length);
		if ( queue->queue == NULL ) {
			result = -1;
		}
	}

	if ( result ) {
		free_t3_queue(&queue);
	}

	return(queue);
}

void free_t3_queue(t3_queue_t **queue) {
	if ( *queue != NULL ) {
		if ( (*queue)->queue != NULL ) {
			free((*queue)->queue);
		}
		free(*queue);
	}
}

int t3_queue_full(t3_queue_t *queue) {
	/* If the queue has no free spaces, returns true. */
	return( queue->right_index - queue->left_index >= (queue->length-1) );
}

int t3_queue_pop(t3_queue_t *queue, t3_t *record) {
	int result = t3_queue_front(queue, record);

	if ( ! result ) {
		queue->left_index++;
		if ( queue->left_index > queue->right_index ) {
			queue->left_index = -1;
			queue->right_index = -1;
		}
	}

	return(result);
}
		

int t3_queue_push(t3_queue_t *queue, t3_t *record) {
	int64_t next_index = (queue->right_index + 1) % queue->length;
	
	debug("Pushing at index %"PRId64"\n", next_index);
	if ( t3_queue_full(queue) ) {
		error("Queue overflow, with limits (%"PRId64", %"PRId64"). "
				"Extend its size to continue with the calculation.\n",
				queue->left_index, queue->right_index);
		return(-1);
	} else {
		memcpy(&(queue->queue[next_index]), record, sizeof(t3_t));
		queue->right_index++;
		if ( queue->left_index < 0 ) {
			queue->left_index = 0;
		}
		return(0);
	}
}

int t3_queue_front(t3_queue_t *queue, t3_t *record) {
	int64_t index = queue->left_index % queue->length;
	debug("Front of queue is at index %"PRId64"\n", index);
	if ( queue->left_index < 0 ) {
		return(-1);
	} else {
		memcpy(record, 
				&(queue->queue[index]),
				sizeof(t3_t));
		return(0);
	}
}

int t3_queue_back(t3_queue_t *queue, t3_t *record) {
	int64_t index = queue->right_index % queue->length;
	debug("Back of queue is at index %"PRId64"\n", index);
	if ( queue->right_index < 0 ) {
		return(-1);
	} else {
		memcpy(record,
				&(queue->queue[index]),
				sizeof(t3_t));
		return(0);
	}
}

int t3_queue_index(t3_queue_t *queue, t3_t *record, int index) {
	int64_t true_index = (queue->left_index + index) % queue->length;

	if ( index > t3_queue_size(queue) ) {
		debug("Requested return of non-existent index: %"PRId64"\n", 
				true_index);
		return(-1);
	} else { 
		memcpy(record, &(queue->queue[true_index]), sizeof(t3_t));
		return(0);
	}
}

int64_t t3_queue_size(t3_queue_t *queue) {
	if ( queue->left_index < 0 || queue->right_index < 0 ) {
		return(0);
	} else {
		return(queue->right_index - queue->left_index + 1);
	}
}

void t3_queue_sort(t3_queue_t *queue) {
	/* First, check if the queue wraps around. If it does, we need to move 
	 * everything into one continous block. If it does not, it is already in 
	 * a continuous block and is ready for sorting.
	 */
	int64_t right = queue->right_index % queue->length;
	int64_t left = queue->left_index % queue->length;
	if ( t3_queue_full(queue) ) {
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
					sizeof(t3_t)*(queue->length - left));
		} else if ( left != 0 && ! t3_queue_full(queue) ) {
			/* There is one continuous block, so move it to the front. 
			 * ---xxxx -> xxxx---
			 */
			debug("Queue is offset from beginning, moving it forward.\n");
			memmove(queue->queue,
					&(queue->queue[left]),
					sizeof(t3_t)*t3_queue_size(queue));
		} else {
			debug("Queue starts at the beginning of the array, "
					"no action needed to make it contiguous.\n");
		}
	}

	queue->right_index = t3_queue_size(queue) - 1;
	queue->left_index = 0;

	debug("Sorting %"PRId64" photons.\n", t3_queue_size(queue));

	qsort(queue->queue, 
			t3_queue_size(queue), sizeof(t3_t), t3_comparator);
}

void yield_t3_queue(FILE *out_stream, t3_queue_t *queue, options_t *options) {
	t3_t record;
	while ( ! t3_queue_pop(queue, &record) ) {
		print_t3(out_stream, &record, NEWLINE, options);
	}
}

/* These functions break up the photons into subsets by dividing time into
 * windows of fixed length. Thus we need to be able to receive a photon from a 
 * window, and to iterate between the windows.
 */
void init_t3_window(t3_window_t *window, 
		int64_t start_pulse, options_t *options) {
	if ( options->set_start_time ) {
		window->limits.lower = options->start_time;
	} else {
		window->limits.lower = start_pulse;
	}

	if ( options->bin_width ) {
		window->limits.upper = options->bin_width*
				((int)floor(window->limits.lower/options->bin_width)+1);
	} else {
		window->limits.upper = 0;
	}
	window->width = options->bin_width;

	window->set_pulse_limit = options->set_stop_time;
	window->pulse_limit = options->stop_time;

	if ( window->set_pulse_limit 
			&& window->limits.upper > window->pulse_limit ) {
		window->limits.upper = window->pulse_limit;
	}
}

void next_t3_window(t3_window_t *window) {
	window->limits.lower = window->limits.upper;
	window->limits.upper += window->width;

	if ( window->set_pulse_limit 
			&& window->limits.upper > window->pulse_limit ) {
		window->limits.upper = window->pulse_limit;
	}
}

int init_t3_windowed_stream(t3_windowed_stream_t *stream, 
		FILE *in_stream, options_t *options) {
	stream->yielded_photon = 0;
	stream->in_stream = in_stream;

	if ( next_t3(in_stream, &(stream->current_photon), options) ) {
		return(-1);
	} else {
		init_t3_window(&(stream->window), 
				stream->current_photon.pulse, options);
	}

	return(0);
}

int next_t3_windowed(t3_windowed_stream_t *stream, t3_t *record,
		options_t *options) {
	/* Deal with the actual photon now. */
	if ( stream->yielded_photon ) {
		stream->current_photon.channel = -1;
	}
		
	if ( stream->current_photon.channel == -1 ) {
		if ( next_t3(stream->in_stream, 
				&(stream->current_photon), options) ) {
			return(-1);
		}
	}

	/* Test that the photon is within the absolute limits */ 
	if ( options->set_start_time &&
			stream->current_photon.pulse < options->start_time ) {
		stream->yielded_photon = 1;
		return(2);
	} 

	if ( options->set_stop_time && 
			stream->current_photon.pulse >= options->stop_time ) {
		return(-1);
	}

	/* Process the photon normally. */
	if ( (! options->count_all) 
			&& stream->current_photon.pulse > stream->window.limits.upper ) {
		stream->yielded_photon = 0;
		return(1);
	} else if ( stream->current_photon.pulse < stream->window.limits.lower ) {
		/* this helps prevent inifinite loops: imagine that we define the 
		 * window to start after the photon, at which point we keep moving 
		 * forward without ever finding the correct window.
 		 */
		return(-2);
	} else {
		stream->yielded_photon = 1;
		
		record->channel = stream->current_photon.channel;
		record->pulse = stream->current_photon.pulse;
		record->time = stream->current_photon.time;
		return(0);
	}
}
