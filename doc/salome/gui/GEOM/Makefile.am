# Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

#  File   : Makefile.in
#  Author : Vasily Rusyaev (Open Cascade NN)
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : doc
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

EXTRA_DIST += images input static/footer.html static/doxygen.css static/SALOME_BOA_PA.pdf

guidocdir = $(docdir)/gui/GEOM
guidoc_DATA = images/head.png static/SALOME_BOA_PA.pdf

usr_docs: doxyfile_tui doxyfile_py doxyfile
	echo "===========================================" ;			\
	echo "Replacing geompyDC by geompy" ;					\
	echo "===========================================" ;			\
	$(KERNEL_ROOT_DIR)/bin/salome/prepare_generating_doc.py geompy.py $(top_srcdir)/src/GEOM_SWIG/geompyDC.py geompy ;	\
	echo "===========================================" ;			\
	echo "Generating examples documentation" ;				\
	echo "===========================================" ;			\
	$(DOXYGEN) doxyfile_tui ;						\
	echo "===========================================" ;			\
	echo "Generating Python interface documentation";			\
	echo "===========================================" ;			\
	$(DOXYGEN) doxyfile_py ;						\
	echo "===========================================" ;			\
	echo "Generating GUI documentation" ;					\
	echo "===========================================" ;			\
	$(DOXYGEN) doxyfile ; 							\
	rm -f ./geompy.py

docs: usr_docs

clean-local:
	@for filen in `find . -maxdepth 1` ; do					\
	  case $${filen} in							\
	    ./Makefile* | ./doxyfile* | ./images | ./input | ./static ) ;;	\
	    . | .. | ./CVS ) ;;							\
	    *) echo "Removing $${filen}" ; rm -rf $${filen} ;;			\
	  esac ;								\
	done ;

install-data-local: usr_docs
	@if [ -e index.html ]; then \
	$(INSTALL) -d $(DESTDIR)$(docdir)/gui/GEOM ; \
	for filen in `find . -maxdepth 1` ; do							\
	  case $${filen} in									\
	    ./Makefile* | ./doxyfile* ) ;;			\
	    . | .. | ./static ) ;;										\
	    *) echo "Installing $${filen}" ; cp -rp $${filen} $(DESTDIR)$(docdir)/gui/GEOM ;;	\
	  esac ;										\
	done ; \
	cp -rp $(srcdir)/images/head.png $(DESTDIR)$(docdir)/gui/GEOM/geompy_doc ;              \
	fi


uninstall-local:
	rm -rf $(DESTDIR)$(docdir)/gui/GEOM
