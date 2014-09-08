include $(top_srcdir)/Makefile.common

SUBDIRS = \
          decoders \
          $(EMPTY)

noinst_LTLIBRARIES = libsound.la

noinst_HEADERS = \
                 types.h \
                 audiostream.h \
                 sound.h \
                 $(EMPTY)

libsound_la_SOURCES = \
                      audiostream.cpp \
                      sound.cpp \
                      $(EMPTY)

libsound_la_LIBADD = \
                     decoders/libdecoders.la \
                     $(EMPTY)
