bin_PROGRAMS    = sobby
sobby_LDADD     = $(sobby_LIBS)
sobby_CPPFLAGS  = $(sobby_CFLAGS) -I../inc/
noinst_HEADERS  = inc/io/main_connection.hpp
noinst_HEADERS += inc/io/buffer_wrapper.hpp
noinst_HEADERS += inc/server.hpp
noinst_HEADERS += inc/common.hpp
sobby_SOURCES   = src/io/main_connection_generic.cpp
sobby_SOURCES  += src/io/buffer_wrapper.cpp
sobby_SOURCES  += src/common.cpp
sobby_SOURCES  += src/server.cpp
sobby_SOURCES  += src/main.cpp

