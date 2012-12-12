#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "error.h"
#include "limits.h"

int str_to_limits(char *str, limits_t *limits) {
	int result;

	debug("Parsing limits: %s.\n", str);

	if ( str == NULL ) {
		error("Fatal error, no limits specified.\n");
		return(-1);
	}

	result = sscanf(str, "%"PRIf64",%"PRId32",%"PRIf64"", 
				&(limits->lower), 
				&(limits->bins),
				&(limits->upper));

	if ( result != 3 ) {
		error("Limits could not be parsed: %s.\n"
				"The correct format is lower,bins,upper (no spaces).\n",
				str);
		return(-1);
	}

	if ( limits->lower >= limits->upper ) {
		error("Lower limit must be less than upper limit "
				"(%"PRIf64", %"PRIf64" specified)\n", 
				limits->lower, limits->upper);
		return(-1);
	}

	if ( limits->bins <= 0 ) {
		error("Must have at least one bin.\n");
		return(-1);
	}

	return(0);
}

int scale_parse(char *str, int *scale) {
	if ( str == NULL ) {
		*scale = SCALE_LINEAR;
	} else {
		if ( !strcmp(str, "log") ) {
			*scale = SCALE_LOG;
		} else if ( !strcmp(str, "linear") ) {
			*scale = SCALE_LINEAR;
		} else if ( !strcmp(str, "log-zero") ) {
			*scale = SCALE_LOG_ZERO;
		} else {
			*scale = SCALE_UNKNOWN;
			error("Scale specified but not recognized: %s\n", str);
		}
	}
	
	return(*scale == SCALE_UNKNOWN);
}
