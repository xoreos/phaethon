Changes in Phaethon version 0.0.6
=================================

This release of Phaethon is mostly a "maintenance release", keeping the
foundation in sync with xoreos. There are no major new features.

There are, however, a number of smaller bug fixes. Also, Phaethon can now open
ERF V2.1 files and display swizzled Xbox SBM images. Additionally, BIF files
can be inspected directly, even when no corresponding KEY file can be found
(this does mean, though, that the filenames are missing).


Changes in Phaethon version 0.0.5
=================================

Phaethon has been rebuild from the ground up, converting it from a wxWidgets
application to a Qt5 application. We celebrate this with the first "official"
release of Phaethon, complete with pre-built binary packages, just like
xoreos-tools and xoreos proper.

Phaethon is a graphical resource explorer for the BioWare games. It can load
the directory structure of the game, look into archives and show various
resource types, like images and texts, play sounds and music, etc.

Complex formats like GFF are not yet show, and no 3D models either. That is,
however, a planned feature for the future.

Also in the future, Phaethon might include live editors, able to change
many of the resource types, allowing for easy, instant modding.
