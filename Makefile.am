ACLOCAL_AMFLAGS = -I m4 --install

EXTRA_DIST = README.md

SUBDIRS = \
          gitstamp \
          utf8cpp \
          src \
          $(EMPTY)

doxygen:
	doxygen

doxygen-clean:
	rm -rf doc/doxygen/html/
	rm -rf doc/doxygen/latex/
	rm -rf doc/doxygen/man/
