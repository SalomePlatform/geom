#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#  File   : Makefile.in
#  Author : Vasily Rusyaev (Open Cascade NN)
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : doc
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

EXTRA_DIST += images input static/footer.html static/doxygen.css

guidocdir = $(docdir)/gui/GEOM
guidoc_DATA = images/head.png

usr_docs: doxyfile_tui doxyfile_py doxyfile
	echo "===========================================" ;			\
	echo "Replacing geompyDC by geompy" ;					\
	echo "===========================================" ;			\
	sed -e "/class geompyDC/d" -e "s/^ *#/#/g" -e "s/^ *def /def /g" 	\
	  -e "s/geompyDC/geompy/g" $(top_srcdir)/src/GEOM_SWIG/geompyDC.py > 	\
	  $(top_builddir)/src/GEOM_SWIG/geompy.py ; 				\
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
	rm -f $(top_builddir)/src/GEOM_SWIG/geompy.py

docs: usr_docs

clean-local:
	@for filen in `find . -maxdepth 1` ; do					\
	  case $${filen} in							\
	    ./Makefile | ./doxyfile | ./doxyfile_py | ./doxyfile_tui ) ;;	\
	    . | .. ) ;;								\
	    *) echo "Removing $${filen}" ; rm -rf $${filen} ;;			\
	  esac ;								\
	done ;

install-data-local: usr_docs
	$(INSTALL) -d $(DESTDIR)$(docdir)/gui/GEOM
	@for filen in `find . -maxdepth 1` ; do							\
	  case $${filen} in									\
	    ./Makefile | ./doxyfile | ./doxyfile_py | ./doxyfile_tui ) ;;			\
	    ./doxyfile.bak | ./doxyfile_py.bak | ./doxyfile_tui.bak ) ;;			\
	    . | .. ) ;;										\
	    *) echo "Installing $${filen}" ; cp -rp $${filen} $(DESTDIR)$(docdir)/gui/GEOM ;;	\
	  esac ;										\
	done ;

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)/gui/GEOM
