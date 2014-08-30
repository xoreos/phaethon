include $(top_srcdir)/Makefile.common

SUBDIRS = \
          common \
          aurora \
          $(EMPTY)

bin_PROGRAMS = phaethon

noinst_HEADERS = \
                 cline.h \
                 eventid.h \
                 about.h \
                 mainwindow.h \
                 $(EMPTY)

phaethon_SOURCES = \
                   cline.cpp \
                   about.cpp \
                   mainwindow.cpp \
                   phaethon.cpp \
                   $(EMPTY)

phaethon_LDADD = \
                 aurora/libaurora.la \
                 common/libcommon.la \
                 $(LDADD) \
                 $(EMPTY)
