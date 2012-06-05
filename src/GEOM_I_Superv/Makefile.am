# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#  GEOM GEOM : implementaion of GEOM_Superv.idl
#  File   : Makefile.in
#  Author : Patrick GOLDBRONN (CEA)
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM
#  $Header$

# Libraries targets
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS = \
	GEOM_Superv_i.hh \
	GEOM_List_i.hh \
	GEOM_I_Superv.hxx

# Libraries targets
lib_LTLIBRARIES = libGEOM_SupervEngine.la

dist_libGEOM_SupervEngine_la_SOURCES = \
	GEOM_Superv_i.cc

# additional information to compile and link file

libGEOM_SupervEngine_la_CPPFLAGS =	\
	$(CAS_CPPFLAGS)			\
	$(QT_INCLUDES)			\
	$(KERNEL_CXXFLAGS)		\
	$(GUI_CXXFLAGS)		\
	$(BOOST_CPPFLAGS)		\
	$(CORBA_CXXFLAGS)		\
	$(CORBA_INCLUDES)		\
	-I$(srcdir)/../GEOMGUI		\
	-I$(srcdir)/../GEOMBase		\
	-I$(srcdir)/../GEOMClient	\
	-I$(srcdir)/../GEOMImpl		\
	-I$(srcdir)/../GEOM		\
	-I$(srcdir)/../GEOM_I		\
	-I$(srcdir)/../GEOMAlgo		\
	-I$(top_builddir)/idl

libGEOM_SupervEngine_la_LDFLAGS  =	\
        ../GEOM_I/libGEOMEngine.la      \
	$(CORBA_LIBS)			\
	$(KERNEL_LDFLAGS) -lSalomeGenericObj -lSalomeNS -lSalomeLifeCycleCORBA -lSalomeContainer -lSalomeDSClient \
	$(STDLIB)
