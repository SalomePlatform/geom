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

#  GEOM ARCHIMEDE : algorithm implementation
#  File   : Makefile.am
#  Author : Nicolas REJNERI
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

salomeinclude_HEADERS =	\
	Archimede_VolumeSection.hxx

# Libraries targets

lib_LTLIBRARIES = libGEOMArchimede.la

# sources files
dist_libGEOMArchimede_la_SOURCES =	\
	Archimede_VolumeSection.cxx


# additional information to compile and link file

libGEOMArchimede_la_CPPFLAGS =	\
	$(CAS_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)

libGEOMArchimede_la_LDFLAGS  =			\
	$(KERNEL_LDFLAGS) -lSALOMELocalTrace	\
	$(CAS_KERNEL)				\
	$(CAS_MODELER) -lTKMesh			\
	$(STDLIB)
