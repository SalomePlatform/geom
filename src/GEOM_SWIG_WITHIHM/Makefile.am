#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#
#
#  File   : Makefile.in
#  Author : Nicolas REJNERI, Paul RASCLE
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM
#  $Header$


# Libraries targets

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
# The file libSALOME_Swigcmodule.py will be installed in
# <prefix>/lib/python<version>/site-package/salome.
# The library will be installed in the same place.
#

# this option puts it to dist
#BUILT_SOURCES = swig_wrap.cpp

SWIG_FLAGS    = \
	@SWIG_FLAGS@ \
	-I$(srcdir) \
	-I$(srcdir)/../GEOMGUI

SWIG_SOURCES  = libGEOM_Swig.i

# Libraries targets
lib_LTLIBRARIES = libGEOM_Swigcmodule.la

nodist_pkgpython_DATA = libGEOM_Swig.py
libGEOM_Swig.py: swig_wrap.cpp

libGEOM_Swigcmodule_la_SOURCES = \
	$(BUILT_SOURCES) \
	$(SWIG_SOURCES)

nodist_libGEOM_Swigcmodule_la_SOURCES = swig_wrap.cpp

#LIB_CLIENT_IDL = SALOMEDS.idl \
#                 SALOMEDS_Attributes.idl \
#		 SALOME_Exception.idl \
#		 GEOM_Gen.idl \
#		 SALOME_Component.idl \
#		 SALOME_GenericObj.idl

libGEOM_Swigcmodule_la_CPPFLAGS = \
	$(QT_INCLUDES) \
	$(PYTHON_INCLUDES) \
	$(CAS_CPPFLAGS) \
	$(VTK_INCLUDES) \
	$(OGL_INCLUDES) \
	$(KERNEL_CXXFLAGS) \
	$(GUI_CXXFLAGS) \
	$(CORBA_CXXFLAGS) \
	$(CORBA_INCLUDES) \
	-I$(srcdir)/../GEOMGUI \
	-I$(top_builddir)/idl \
	-I$(top_builddir)/salome_adm/unix

libGEOM_Swigcmodule_la_LDFLAGS = \
	../GEOMGUI/libGEOM.la


libGEOM_Swigcmodule_la_LIBADD  = \
	$(PYTHON_LIBS)

swig_wrap.cpp : $(SWIG_SOURCES)
	$(SWIG) $(SWIG_FLAGS) -o $@ $<

CLEANFILES = \
	swig_wrap.cpp

#
# ===============================================================
# Files to be installed
# ===============================================================
#

# Scripts to be installed.
dist_salomescript_DATA= \
	libGEOM_Swig.py

install-exec-hook:\
	$(libdir)/_libGEOM_Swig.so

$(libdir)/_libGEOM_Swig.so:
	( cd $(libdir); ln -sf libGEOM_Swigcmodule.so _libGEOM_Swig.so; )
