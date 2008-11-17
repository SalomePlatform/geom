#  Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
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
#  Module : doc

include $(top_srcdir)/adm_local/unix/make_common_starter.am

EXTRA_DIST += images static

dev_docs: doxyfile
	echo "===========================================" ;			\
	echo "Replacing geompyDC by geompy" ;					\
	echo "===========================================" ;			\
	sed -e "/class geompyDC/d" -e "s/^ *#/#/g" -e "s/^ *def /def /g" 	\
	  -e "s/geompyDC/geompy/g" $(top_srcdir)/src/GEOM_SWIG/geompyDC.py > 	\
	  $(top_builddir)/src/GEOM_SWIG/geompy.py ; 				\
	echo "===========================================" ;			\
	echo "Generating TUI documentation" ;					\
	echo "===========================================" ;			\
	$(DOXYGEN) doxyfile ; 							\
	rm -f $(top_builddir)/src/GEOM_SWIG/geompy.py

clean-local:
	-rm -fr GEOM doxygen.bak

install-data-local:
	if test -d GEOM; then \
	  $(INSTALL) -d $(DESTDIR)$(docdir)/tui ; \
	  cp -rp GEOM $(DESTDIR)$(docdir)/tui ; \
	fi;

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)/tui/GEOM
