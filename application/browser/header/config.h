/* config.h.  Generated by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* Define if debugging was enabled. */
/* #undef DEBUG */

/* Define to 1 if you have the <dlfcn.h> header file. */
//#define HAVE_DLFCN_H 1

/* Define to 1 if you have the `dlopen' function. */
/* #undef HAVE_DLOPEN */

/* Define to 1 if you have the <dmalloc.h> header file. */
/* #undef HAVE_DMALLOC_H */

/* Define to 1 if you have the `gethostbyname' function. */
//#define HAVE_GETHOSTBYNAME 1

/* Define to 1 if you have the `getopt' function. */
//#define HAVE_GETOPT 1

/* Define to 1 if you have the <getopt.h> header file. */
/*#define HAVE_GETOPT_H 1*/

/* Define to 1 if you have the `getopt_long' function. */
/*#define HAVE_GETOPT_LONG 1*/

/* Define if pthread library is GNU pth. */
/* #undef HAVE_GNU_PTH */

/* Define if you have the inttypes.h header file. */
/*#define HAVE_INTTYPES_H 1*/

/* Define to 1 if you have the `ccmalloc' library (-lccmalloc). */
/* #undef HAVE_LIBCCMALLOC */

/* Define to 1 if you have the `curses' library (-lcurses). */
/* #undef HAVE_LIBCURSES */

/* Define to 1 if you have the `dl' library (-ldl). */
//#define HAVE_LIBDL 1

/* Define to 1 if you have the `dmallocth' library (-ldmallocth). */
/* #undef HAVE_LIBDMALLOCTH */

/* Define to 1 if you have the `efence' library (-lefence). */
/* #undef HAVE_LIBEFENCE */

/* Define if you have the gif library '-lgif'. */
/* #undef HAVE_LIBGIF */

/* Define if you have the jpeg library '-ljpeg'. */
/* #undef HAVE_LIBJPEG */
#define HAVE_LIBJPEG 1

/* Define if you have the Magick library '-lMagick'. */
/* #undef HAVE_LIBMAGICK */

/* Define to 1 if you have the `ncurses' library (-lncurses). */
/* #undef HAVE_LIBNCURSES */

/* Define to 1 if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define if you have the png library '-lpng'. */
/* #undef HAVE_LIBPNG */
#define HAVE_LIBPNG 1
/* Define to 1 if you have the `pthread' library (-lpthread). */
// #define HAVE_LIBPTHREAD 1

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define if you have the ungif library '-lungif'. */
/* #undef HAVE_LIBUNGIF */
#define HAVE_LIBUNGIF 1
/* Define to 1 if you have the <magick/api.h> header file. */
/* #undef HAVE_MAGICK_API_H */

/* Define to 1 if you have the <magick.h> header file. */
/* #undef HAVE_MAGICK_H */

/* Define to 1 if you have the <magick/magick.h> header file. */
/* #undef HAVE_MAGICK_MAGICK_H */

/* Define to 1 if you have the <memory.h> header file. */
//#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <pthread.h> header file. */
/*#define HAVE_PTHREAD_H 1*/

/* Define to 1 if you have the `socket' function. */
//#define HAVE_SOCKET 1

/* Define to 1 if you have the <stdint.h> header file. */
//#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
//#define HAVE_STRINGS_H 1*/

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
//#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
//#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
//#define HAVE_UNISTD_H 1

/* Name of package */
#define PACKAGE "zen"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "zen"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "zen 0.2.3"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "zen"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.2.3"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.2.3"

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define to `unsigned short' if <sys/types.h> does not define. */
typedef unsigned short uint16_t;
/*#undef uint16_t*/

/* Define to `unsigned int' if <sys/types.h> does not define. */
typedef unsigned int uint32_t;
/*#undef uint32_t*/

/* Define to `unsigned char' if <sys/types.h> does not define. */
typedef unsigned char uint8_t;
/*#undef uint8_t*/

// added by wichang (060410)
#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2

#define AVAILABLE_UI "aaa"

//typedef int ssize_t;

#define __FUNCTION__	""


#define RTLD_LAZY	0x00001
#define RTLD_NOW	0x00001
#define RTLD_BINDING_MASK	0x00001
#define RTLD_NOLOAD	0x00001

#define FB unsigned char
//typedef int size_t;

#define IS_SPACE_ALPHA(x) (isspace(x) && !(x&0x80))

#define IS_SPACE_UNI(x) (isspace(x) || (x&0x80))

