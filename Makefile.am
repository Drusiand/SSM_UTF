## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = po

dist_doc_DATA = \
	README \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

AM_CPPFLAGS = \
        -DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
        -DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
        -DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-ISSM_2018 \
	-IHighFive/include -I/usr/include/hdf5/serial -L/usr/lib/x86_64-linux-gnu/hdf5/serial \
        $(SSM_CFLAGS)

AM_CFLAGS =\
         -Wall\
         -g

bin_PROGRAMS = ssm

# headers that must be processed with moc to generate C++ source files
ssm_qtheaders = SSM_2018/model.h

# the source files that will be generated via moc.  The string "_moc" is inserted
# to, hopefully, avoid name collisions
ssm_moc_sources = $(ssm_qtheaders:.h=_moc.cpp)

ssm_SOURCES = \
        SSM_2018/main.cpp \
        SSM_2018/functions-grammar.cc \
        $(ssm_moc_sources)

ssm_HEADERS = \
        SSM_2018/data.h \
        SSM_2018/parametrs.h \
        SSM_2018/functions-grammar.h \
        SSM_2018/nlreg.h \
        $(ssm_qtheaders)

ssmdir = \
        $(includedir)/ssm

ssm_CXXFLAGS = -fPIC \
                -fopenmp

ssm_CFLAGS = -fPIC

ssm_LDFLAGS = -fPIC

ssm_LDADD = $(SSM_LIBS) \
        -lhdf5 \
        -lhdf5_cpp -lhdf5 \
	-L/usr/lib/x86_64-linux-gnu/hdf5/serial

if NATIVE_WIN32
ssm_LDFLAGS += -mwindows
endif

# A traditional-style Make suffix rule for generating *_moc.cpp files from *.h
# files via moc.  For the former (especially) to be recognized as a suffix,
# you need to tell Automake about it via the `SUFFIXES` variable.
# The mkdir is present to support out-of-source building.
# Assumes that the MOC variable will be set by configure.
.h_moc.cpp:
	$(MKDIR_P) `dirname $@`
	moc -o $@ $(AM_CPPFLAGS) $<

SUFFIXES = .h _moc.cpp

EXTRA_DIST = \
	$(INTLTOOL_FILES)

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)
