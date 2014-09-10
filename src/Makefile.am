include $(top_srcdir)/Makefile.common

SUBDIRS = \
          common \
          sound \
          images \
          aurora \
          gui \
          $(EMPTY)

bin_PROGRAMS = phaethon

noinst_HEADERS = \
                 cline.h \
                 $(EMPTY)

phaethon_SOURCES = \
                   cline.cpp \
                   phaethon.cpp \
                   $(EMPTY)

phaethon_LDADD = \
                 gui/libgui.la \
                 aurora/libaurora.la \
                 images/libimages.la \
                 sound/libsound.la \
                 common/libcommon.la \
                 $(LDADD) \
                 $(EMPTY)
