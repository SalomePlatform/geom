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

#  GEOMUtils : implementaion of some common methods for GEOMImpl package
#  File      : Makefile.am
#  Author    : Julia DOROVSKIKH

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libGEOMUtils.la

# header files
salomeinclude_HEADERS =			\
	GEOMUtils.hxx

dist_libGEOMUtils_la_SOURCES =		\
	GEOMUtils.cxx

# additional information to compile and link file

libGEOMUtils_la_CPPFLAGS =		\
	$(CAS_CPPFLAGS)			\
	$(KERNEL_CXXFLAGS)		\
	$(BOOST_CPPFLAGS)		\
	-I$(srcdir)/../ShHealOper	\
	-I$(srcdir)/../NMTDS		\
	-I$(srcdir)/../NMTTools		\
	-I$(srcdir)/../BlockFix		\
	-I$(srcdir)/../GEOMAlgo		\
	-I$(srcdir)/../SKETCHER		\
	-I$(srcdir)/../ARCHIMEDE

libGEOMUtils_la_LDFLAGS  =			\
	../BlockFix/libBlockFix.la		\
	../GEOMAlgo/libGEOMAlgo.la		\
	../ShHealOper/libShHealOper.la		\
	../ARCHIMEDE/libGEOMArchimede.la	\
	../SKETCHER/libGEOMSketcher.la		\
	$(KERNEL_LDFLAGS) -lSALOMELocalTrace -lSALOMEBasics	\
	$(CAS_LDPATH) -lTKCAF -lTKFillet -lTKOffset -lTKFeat	\
	$(STDLIB)
