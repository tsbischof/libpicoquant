#include "types.h"

char* ctime32(time32_t *mytime) {
#ifdef __i386__
	return(ctime(mytime));
#endif

#ifdef __x86_64__
	time_t mytime64;
	mytime64 = (int64_t)*mytime;
	return(ctime(&mytime64));
#endif
}

int64_t i64abs(int64_t val) { 
	return( val < 0 ? -val : val ) ;
}
