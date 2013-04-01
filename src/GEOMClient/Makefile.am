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

#  GEOM GEOMClient : tool to transfer BREP files from GEOM server to GEOM client
#  File   : Makefile.in
#  Author : Patrick GOLDBRONN (CEA)
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =	\
	GEOM_Client.hxx

# Libraries targets
lib_LTLIBRARIES = libGEOMClient.la

# Sources files
dist_libGEOMClient_la_SOURCES =	\
		GEOM_Client.cxx

# additional information to compile and link file

libGEOMClient_la_CPPFLAGS =	\
	$(CORBA_CXXFLAGS)	\
        $(CORBA_INCLUDES)	\
	$(CAS_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)	\
	-I$(top_builddir)/idl

libGEOMClient_la_LDFLAGS  =				\
	../../idl/libSalomeIDLGEOM.la			\
	../GEOM/libGEOMbasic.la				\
	$(KERNEL_LDFLAGS) -lSalomeIDLKernel -lSALOMEBasics -lOpUtil	\
	$(CAS_TKTopAlgo)

# \
#	$(CORBA_LIBS) \
#	#$(STDLIB)

