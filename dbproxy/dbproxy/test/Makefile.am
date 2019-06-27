# header files
h_sources = dbmanager.h

# cpp files
cpp_sources = dbmanager.cpp \
dbproxy_test.cpp

bin_PROGRAMS = dbproxy_test
INCLUDES = -I$(top_srcdir)/thirdparty/mysqlclient-5.7.22 -I$(top_srcdir)/common/include -I./
bindir = $(prefix)/dbproxy/bin
dbproxy_test_LDADD = -lpthread -lrt -ldl -lssl $(top_srcdir)/thirdparty/mysqlclient-5.7.22/lib64/libmysqlclient.a $(top_srcdir)/common/lib/libcommon.a 
dbproxy_test_SOURCES = $(h_sources) $(cpp_sources)

