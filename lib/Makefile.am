SUBDIRS = event filehighlight mcconfig search skin tty vfs strutil widget .

if ENABLE_MCLIB
    LIB_VERSION=`echo $(LIBMC_VERSION) | \
	tr '.' ' '| \
	while read v1 v2 v3; do echo $$v2':'$$v3':'$$v1; done`

    AM_LDFLAGS=-no-undefined -version-info $(LIB_VERSION) -release $(LIBMC_RELEASE)

    lib_LTLIBRARIES = libmc.la
else
    noinst_LTLIBRARIES = libmc.la
endif


SUBLIB_includes = \
	event.h event-types.h \
	filehighlight.h \
	idx.h \
	intprops-internal.h \
	intprops.h \
	mcconfig.h \
	search.h \
	skin.h \
	strutil.h \
	widget.h

SRC_mc_utils = \
	terminal.c terminal.h \
	utilunix.c \
	unixcompat.h \
	util.c util.h


libmc_la_SOURCES = \
	$(SUBLIB_includes) \
	$(SRC_mc_utils) \
	file-entry.h \
	fileloc.h \
	fs.h \
	hook.c hook.h \
	glibcompat.c glibcompat.h \
	global.c global.h \
	keybind.c keybind.h \
	lock.c lock.h \
	serialize.c serialize.h \
	shell.c shell.h \
	stat-size.h \
	timefmt.c timefmt.h

if USE_MAINTAINER_MODE
libmc_la_SOURCES += logging.c logging.h
endif

libmc_la_SOURCES += charsets.c charsets.h

EXTRA_DIST = \
	stdckdint.in.h

AM_CPPFLAGS = $(GLIB_CFLAGS) -I$(top_srcdir)

libmc_la_LIBADD = \
	event/libmcevent.la \
	filehighlight/libmcfilehighlight.la \
	mcconfig/libmcconfig.la \
	search/libsearch.la \
	strutil/libmcstrutil.la \
	skin/libmcskin.la \
	tty/libmctty.la \
	vfs/libmcvfs.la \
	widget/libmcwidget.la

libmc_la_LIBADD += $(MCLIBS) $(SLANGLIB)

if HAVE_GMODULE
    libmc_la_LIBADD += $(GMODULE_LIBS)
else
    libmc_la_LIBADD += $(GLIB_LIBS)
endif
