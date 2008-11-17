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

EXTRA_DIST+=GEOM

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
	cp -fr $(srcdir)/GEOM ./INPUT; \
	cp -f ./GEOM/doxyfile ./INPUT; \
	cd INPUT; \
	sed 's|../../../share/salome|$(top_srcdir)|' ./doxyfile > ./doxyfile1; \
	sed 's|../../build/salome|$(top_builddir)|' ./doxyfile1 > ./doxyfile2; \
	mv -f doxyfile2 doxyfile1; \
	echo "DOXYGEN SUPPORT PYTHON - @DOXYGEN_WITH_PYTHON@"; \
	if( test "x@DOXYGEN_WITH_PYTHON@" = "xyes"); then \
	  sed 's|python_extension_must_be_here|*.py|' ./doxyfile1 > ./doxyfile2; \
	  mv -f doxyfile2 doxyfile1; \
	  $(DOXYGEN) -u ./doxyfile1; \
	else \
	  sed 's|python_extension_must_be_here||' ./doxyfile1 > ./doxyfile2; \
	  mv -f doxyfile2 doxyfile1; \
	fi; \
	if( test "x@DOXYGEN_WITH_STL@" = "xyes"); then \
	  sed -e 's|BUILTIN_STL_SUPPORT    = NO|BUILTIN_STL_SUPPORT    = YES|' ./doxyfile1 > ./doxyfile2; \
	  mv -f doxyfile2 doxyfile1; \
	fi; \
	mv -f doxyfile1 doxyfile; \
	echo "Running doxygen in directory:"`pwd`; \
	$(DOXYGEN) ./doxyfile; \
	cd ../; \
	cp -fr $(srcdir)/GEOM/sources/ GEOM/ ; \
	rm -fr INPUT ; \
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

doctuidir=$(docdir)/tui/GEOM

nodist_doctui_DATA= GEOM/doxyfile
EXTRA_DIST+= GEOM/doxyfile.in
