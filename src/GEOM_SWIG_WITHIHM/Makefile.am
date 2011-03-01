#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#  File   : Makefile.in
#  Author : Nicolas REJNERI, Paul RASCLE
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM
#  $Header$
# Libraries targets
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# ===============================================================
# Swig targets
# ===============================================================
# (cf. http://www.geocities.com/foetsch/python/swig_linux.htm)
#
# Step 1: build the wrapping source files with swig
#
# libSALOME_LifeCycleCORBA.i -- swig --> swig_wrap.cpp
#                                        libSALOME_Swig.py
#
# Step 2: build the dynamic library from cpp built source files and
#         dependant libraries.
#
# swig_wrap.cpp -- gcc --> swig_wrap.o    |-- link --> _libSALOME_Swig.la
#                          +              |
#                          dependant libs |
#
# The file libGEOM_Swig.py will be installed to the
# <prefix>/bin/salome directory.
# The library _libGEOM_Swig.so will be installed to the 
# <prefix>/lib/python<version>/site-package/salome directory.
#

SWIG_FLAGS    = @SWIG_FLAGS@ -I$(srcdir) -I$(srcdir)/../GEOMGUI
SWIG_SOURCES  = libGEOM_Swig.i

salomepython_PYTHON = libGEOM_Swig.py
salomepyexec_LTLIBRARIES = _libGEOM_Swig.la

libGEOM_Swig.py: swig_wrap.cpp
BUILT_SOURCES = swig_wrap.cpp
_libGEOM_Swig_la_SOURCES        = $(SWIG_SOURCES)
nodist__libGEOM_Swig_la_SOURCES = $(BUILT_SOURCES)

salomeinclude_HEADERS           = $(SWIG_SOURCES)

_libGEOM_Swig_la_CPPFLAGS =		\
	$(QT_INCLUDES)			\
	$(PYTHON_INCLUDES)		\
	$(CAS_CPPFLAGS)			\
	$(VTK_INCLUDES)			\
	$(OGL_INCLUDES)			\
	$(KERNEL_CXXFLAGS)		\
	$(GUI_CXXFLAGS)			\
	$(CORBA_CXXFLAGS)		\
	$(CORBA_INCLUDES)		\
	-I$(srcdir)/../GEOMGUI		\
	-I$(top_builddir)/idl

_libGEOM_Swig_la_LDFLAGS = -module
_libGEOM_Swig_la_LIBADD  =		\
	../GEOMGUI/libGEOM.la		\
	$(PYTHON_LIBS)

swig_wrap.cpp : $(SWIG_SOURCES)
	$(SWIG) $(SWIG_FLAGS) -o $@ $<

CLEANFILES = swig_wrap.cpp libGEOM_Swig.py

