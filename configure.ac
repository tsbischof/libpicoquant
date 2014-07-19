AC_PREREQ([2.69])
AC_INIT(libpicoquant, 0.3, tbischof@mit.edu)
AC_CONFIG_SRCDIR([src])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([m], [sin])

# Checks for header files.
AC_CHECK_HEADERS([float.h inttypes.h limits.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor memmove memset strdup strstr strtol])

AC_CONFIG_FILES([GNUmakefile man/GNUmakefile src/GNUmakefile])
AC_OUTPUT
