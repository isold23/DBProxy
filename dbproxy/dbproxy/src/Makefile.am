# header files
h_sources = dp_common.h \
mysql_function.h \
mysql_command.h \
mysql_net_packet.h \
mysql_net.h \
dp_config.h \
dp_subworker.h \
dp_worker.h

# cpp files
cpp_sources = dp_common.cpp \
mysql_command.cpp \
mysql_net_packet.cpp \
mysql_net.cpp \
dp_config.cpp \
dp_worker.cpp \
dp_subworker.cpp \
dbproxy.cpp

bin_PROGRAMS = dbproxy
INCLUDES = -I$(top_srcdir)/thirdparty/mysqlclient-5.7.22 -I$(top_srcdir)/common/include -I./
bindir = $(prefix)/dbproxy/bin
dbproxy_LDADD = -lpthread -lrt -ldl -lssl $(top_srcdir)/common/lib/libcommon.a $(top_srcdir)/thirdparty/mysqlclient-5.7.22/lib64/libmysqlclient.a
dbproxy_SOURCES = $(h_sources) $(cpp_sources)

