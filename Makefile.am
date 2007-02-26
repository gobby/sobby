bin_PROGRAMS    = sobby
sobby_LDADD     = $(sobby_LIBS)
sobby_CPPFLAGS  = $(sobby_CFLAGS) -I../inc/
noinst_HEADERS  = inc/gselector.hpp
noinst_HEADERS += inc/config.hpp
noinst_HEADERS += inc/buffer_def.hpp
noinst_HEADERS += inc/autosaver.hpp
noinst_HEADERS += inc/autosave_folder.hpp
noinst_HEADERS += inc/command_executer.hpp
noinst_HEADERS += inc/server.hpp
noinst_HEADERS += inc/common.hpp

sobby_SOURCES   = src/gselector.cpp
sobby_SOURCES  += src/config.cpp
sobby_SOURCES  += src/buffer_def.cpp
sobby_SOURCES  += src/common.cpp
sobby_SOURCES  += src/autosaver.cpp
sobby_SOURCES  += src/autosave_folder.cpp
sobby_SOURCES  += src/command_executer.cpp
sobby_SOURCES  += src/server.cpp
sobby_SOURCES  += src/main.cpp

dist_man_MANS = sobby.1

# Fix some permissions if they are not set properly when building the tarball
dist-hook:
	chmod u+x $(distdir)/missing $(distdir)/install-sh \
		  $(distdir)/config.guess $(distdir)/config.sub \
		  $(distdir)/depcomp

