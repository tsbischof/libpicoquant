/* For dealing with mutable strings, we need to wrap the normal immutable 
 * string with a freshly-created one. Bounding the result helps prevent
 * buffer overflows.
 */
%include "cstring.i";
%cstring_bounded_mutable(char *, 16384); /* Most are shorter, but the warnings
                                          * are very long.
                                          */

/* Create the pointers needed for storing various values. */
%include "cpointer.i";
%pointer_class(int, intp);
%pointer_class(double, doublep);

/* Data arrays are important. */
%include "carrays.i";
%array_class(uint, uint_array);
