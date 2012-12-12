#ifndef MODES_H_
#define MODES_H_

#define MODE_UNKNOWN -1
#define MODE_INTERACTIVE 0
#define MODE_T2 2
#define MODE_T3 3
#define MODE_VECTOR 0xBEEF

int mode_parse(int *mode, char *mode_string);

#endif
