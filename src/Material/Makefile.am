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

# File    : Makefile.am
# Author  : Margarita KARPUNINA, Open CASCADE S.A.S. (margarita.karpunina@opencascade.com)
# Package : Material
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libMaterial.la

# header files 
salomeinclude_HEADERS =			\
	Material.h                      \
	Material_Model.h		\
	Material_ResourceMgr.h

dist_libMaterial_la_SOURCES =		\
	Material_Model.cxx		\
	Material_ResourceMgr.cxx

MOC_FILES =				\
	Material_ResourceMgr_moc.cxx

nodist_libMaterial_la_SOURCES =		\
	$(MOC_FILES)

# additional information to compile and link file
libMaterial_la_CPPFLAGS =		\
	$(QT_INCLUDES)			\
	$(VTK_INCLUDES)			\
	$(CAS_CPPFLAGS)			\
	$(KERNEL_CXXFLAGS)		\
	$(GUI_CXXFLAGS)			\
	-I$(srcdir)/../OBJECT

libMaterial_la_LDFLAGS  = -lVTKViewer -lOCCViewer -lsuit -lSalomeApp \
	../OBJECT/libGEOMObject.la		\
	$(GUI_LDFLAGS) -lqtx \
	$(QT_MT_LIBS) \
	$(CAS_LDFLAGS) -lTKV3d -lTKGeomBase \
	$(VTK_LIBS)

dist_salomeres_DATA = resources/SalomeMaterial.xml
