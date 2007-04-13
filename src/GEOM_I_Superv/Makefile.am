#  GEOM GEOM : implementaion of GEOM_Superv.idl
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
#  Author : Patrick GOLDBRONN (CEA)
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM
#  $Header$

# Libraries targets

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =

# Libraries targets
lib_LTLIBRARIES = libGEOM_SupervEngine.la

dist_libGEOM_SupervEngine_la_SOURCES = \
	GEOM_Superv_i.cc

#LIB_SERVER_IDL = SALOMEDS.idl SALOMEDS_Attributes.idl SALOME_Component.idl SALOME_Exception.idl \
#                 SALOME_GenericObj.idl SALOME_ModuleCatalog.idl SALOME_Session.idl GEOM_Gen.idl GEOM_Superv.idl

# additionnal information to compil and link file
libGEOM_SupervEngine_la_CPPFLAGS = \
	$(CAS_CPPFLAGS) \
	$(QT_INCLUDES) \
	$(KERNEL_CXXFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(CORBA_CXXFLAGS) \
	$(CORBA_INCLUDES) \
	-I$(srcdir)/../GEOMGUI \
	-I$(srcdir)/../GEOMBase \
	-I$(srcdir)/../GEOMClient \
	-I$(srcdir)/../GEOMImpl \
	-I$(srcdir)/../GEOM \
	-I$(srcdir)/../GEOM_I \
	-I$(srcdir)/../GEOMAlgo \
	-I$(top_builddir)/idl \
	-I$(top_builddir)/salome_adm/unix

libGEOM_SupervEngine_la_LDFLAGS  = \
	../GEOM_I/libGEOMEngine.la \
	$(KERNEL_LDFLAGS) -lSalomeGenericObj -lSalomeNS -lSalomeLifeCycleCORBA -lSalomeContainer -lSalomeDSClient \
	$(CORBA_LIBS) \
	$(STDLIB)
