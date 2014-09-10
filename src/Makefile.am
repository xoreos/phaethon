include $(top_srcdir)/Makefile.common

SUBDIRS = \
          common \
          sound \
          aurora \
          $(EMPTY)

bin_PROGRAMS = phaethon

noinst_HEADERS = \
                 cline.h \
                 eventid.h \
                 about.h \
                 mainwindow.h \
                 resourcetree.h \
                 soundcontrols.h \
                 $(EMPTY)

phaethon_SOURCES = \
                   cline.cpp \
                   about.cpp \
                   mainwindow.cpp \
                   resourcetree.cpp \
                   soundcontrols.cpp \
                   phaethon.cpp \
                   $(EMPTY)

phaethon_LDADD = \
                 aurora/libaurora.la \
                 sound/libsound.la \
                 common/libcommon.la \
                 $(LDADD) \
                 $(EMPTY)
