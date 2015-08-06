ACLOCAL_AMFLAGS = -I m4 --install

EXTRA_DIST = \
             autogen.sh \
             BUILDDEP.md \
             Doxyfile \
             README.dox \
             $(EMPTY)

dist_doc_DATA = \
                COPYING \
                AUTHORS \
                ChangeLog \
                TODO \
                README.md \
                CODE_OF_CONDUCT.md \
                $(EMPTY)

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
	rm -rf doc/doxygen/*.tmp
	rm -rf doc/doxygen/*.db
