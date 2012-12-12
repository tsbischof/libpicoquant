#include <stdio.h>
#include <string.h>

#include "error.h"
#include "modes.h"

int mode_parse(int *mode, char *mode_string) { 
	if ( mode_string == NULL ) {
		error("Must specify a mode.\n");
		*mode = MODE_UNKNOWN;
		return(-1);
	} 

	if ( !strcmp("t2", mode_string) ) {
		debug("Found mode t2.\n");
		*mode = MODE_T2;
		return(0);
	} else if ( !strcmp("t3", mode_string) ) {
		debug("Found mode t3.\n");
		*mode = MODE_T3;
		return(0);
	} else if ( ! strcmp("vector", mode_string) ) {
		debug("Found mode vector.\n");
		*mode = MODE_VECTOR;
		return(0);
	} else {
		error("Mode not recognized: %s.\n", mode_string);
		*mode = MODE_UNKNOWN;
		return(-1);
	}
}
