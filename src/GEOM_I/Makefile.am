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

#  GEOM GEOM : implementaion of GEOM_Gen.idl
#  File   : Makefile.in
#  Author : Patrick GOLDBRONN (CEA)
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libGEOMEngine.la

# header files 
salomeinclude_HEADERS =			\
	GEOM_Object_i.hh		\
        GEOM_IOperations_i.hh		\
        GEOM_IBasicOperations_i.hh	\
	GEOM_IHealingOperations_i.hh	\
        GEOM_I3DPrimOperations_i.hh	\
	GEOM_IShapesOperations_i.hh	\
	GEOM_IBlocksOperations_i.hh	\
	GEOM_IBooleanOperations_i.hh	\
	GEOM_ICurvesOperations_i.hh	\
	GEOM_ILocalOperations_i.hh	\
	GEOM_IInsertOperations_i.hh	\
	GEOM_ITransformOperations_i.hh	\
	GEOM_IMeasureOperations_i.hh	\
	GEOM_IGroupOperations_i.hh	\
	GEOM_IAdvancedOperations_i.hh	\
	GEOM_Gen_i.hh			\
	GEOM_GEOM_I.hxx

dist_libGEOMEngine_la_SOURCES =		\
	GEOM_Object_i.cc		\
	GEOM_IOperations_i.cc		\
	GEOM_IBasicOperations_i.cc	\
        GEOM_IHealingOperations_i.cc	\
	GEOM_I3DPrimOperations_i.cc	\
	GEOM_IShapesOperations_i.cc	\
	GEOM_IBlocksOperations_i.cc	\
	GEOM_IBooleanOperations_i.cc	\
	GEOM_ICurvesOperations_i.cc	\
	GEOM_ILocalOperations_i.cc	\
	GEOM_IInsertOperations_i.cc	\
	GEOM_ITransformOperations_i.cc	\
	GEOM_IMeasureOperations_i.cc	\
	GEOM_IGroupOperations_i.cc	\
	GEOM_IAdvancedOperations_i.cc	\
	GEOM_Gen_i.cc			\
	GEOM_DumpPython.cc

# additional information to compile and link file

libGEOMEngine_la_CPPFLAGS =		\
	$(CORBA_CXXFLAGS)		\
	$(CORBA_INCLUDES)		\
	$(CAS_CPPFLAGS)			\
	$(BOOST_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)		\
	-I$(srcdir)/../SKETCHER		\
	-I$(srcdir)/../ARCHIMEDE	\
	-I$(srcdir)/../GEOMImpl		\
	-I$(srcdir)/../GEOMAlgo		\
	-I$(srcdir)/../GEOM		\
	-I$(top_builddir)/idl


libGEOMEngine_la_LDFLAGS  =			\
	../../idl/libSalomeIDLGEOM.la		\
	../ARCHIMEDE/libGEOMArchimede.la	\
	../GEOMImpl/libGEOMimpl.la		\
	../SKETCHER/libGEOMSketcher.la		\
	../GEOM/libGEOMbasic.la			\
	$(KERNEL_LDFLAGS) -lOpUtil -lSalomeNS -lSalomeContainer -lSalomeGenericObj -lTOOLSDS	\
	$(CAS_DATAEXCHANGE)			\
	$(CAS_LDPATH) -lTKFillet -lTKOffset

# \
#	$(STDLIB) \
#	$(CORBA_LIBS)
