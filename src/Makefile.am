include $(top_srcdir)/Makefile.common

SUBDIRS = \
          common \
          $(EMPTY)

bin_PROGRAMS = phaethon

noinst_HEADERS = \
                 eventid.h \
                 mainwindow.h \
                 $(EMPTY)

phaethon_SOURCES = \
                   mainwindow.cpp \
                   phaethon.cpp \
                   $(EMPTY)

phaethon_LDADD = \
                 common/libcommon.la \
                 $(LDADD) \
                 $(EMPTY)
