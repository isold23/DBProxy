AC_PREREQ(2.59)
AC_INIT(dbproxy, 1.0.0.0, isold.wang@gmail.com)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE
AM_PROG_LIBTOOL

dnl debug release
AC_ARG_ENABLE([debug], AC_HELP_STRING([--enable-debug], [debug info (default is no)]),[debug=yes], [debug=no])
if test "$debug" = "no"; then
echo "compile with release"
CXXFLAGS="-w -O3 -DNDEBUG -Dx86-64 -DVERSION"
else
echo "compile with debug"
CXXFLAGS="-O0 -g -ggdb -Wall -D_DIRECT -D_DEBUG -Dx86-64 -DVERSION"
fi

LDFLAGS = " -lc -lpthread -o"

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl Checks for libraries.
AC_CHECK_LIB([c], [main])
AC_CHECK_LIB([dl], [main])
AC_CHECK_LIB([pthread], [main])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h memory.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h sys/timeb.h termios.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

AC_CHECK_SIZEOF(int,4)

dnl Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([ftime getpass gettimeofday inet_ntoa memmove memset mkdir socket strchr strrchr strstr strtol])

dnl Checks for library of protobuf
AC_ARG_WITH([protobuf-libdir],
    [AS_HELP_STRING([--with-protobuf-libdir=LIB_DIR],
            [location of the protocol buffers libraries, defaults to /usr/lib])],
    [PROTOBUF_LIBDIR="$withval"],
    [PROTOBUF_LIBDIR='/usr/local/protobuf/lib'])
AC_SUBST([PROTOBUF_LIBDIR])

dnl LDFLAGS="$LDFLAGS -L$PROTOBUF_LIBDIR"

dnl AC_CHECK_LIB([protobuf], [main], [], [AC_MSG_ERROR([cannot find protobuf library])])

AC_ARG_WITH([protoc],
    [AS_HELP_STRING([--with-protoc=PATH],
        [location of the protoc protocol buffer compiler binary, defaults to protoc])],
    [PROTOC="$withval"],
    [PROTOC='protoc'])
AC_SUBST([PROTOC])

AC_CONFIG_FILES([Makefile
		common/src/Makefile
        	dbproxy/src/Makefile
        	dbproxy/test/Makefile
		])
AC_OUTPUT
