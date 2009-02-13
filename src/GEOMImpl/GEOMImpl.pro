#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
TEMPLATE = lib
TARGET = GEOMimpl
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

STDLIB = -lstdc++

CAS_LDPATH = -L$$(CASROOT)/Linux/lib

INCLUDEPATH += $${CAS_CPPFLAGS} $${KERNEL_CXXFLAGS} $${BOOST_CPPFLAGS} ../ShHealOper ../NMTAlgo ../NMTTools ../GEOM ../GEOMAlgo ../SKETCHER ../ARCHIMEDE ../../salome_adm/unix

LIBS += -L$$(GEOM_ROOT_DIR)/lib -lGEOMbasic -lNMTAlgo -lGEOMAlgo -lShHealOper -lGEOMArchimede -lGEOMSketcher $${KERNEL_LDFLAGS} -lSALOMELocalTrace $${STDLIB} $${CAS_LDPATH} -lTKCAF -lTKFillet -lTKOffset

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += GEOMIMPL_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = GEOMImpl_Gen.hxx
HEADERS += GEOMImpl_IBasicOperations.hxx
HEADERS += GEOMImpl_ITransformOperations.hxx
HEADERS += GEOMImpl_IHealingOperations.hxx
HEADERS += GEOMImpl_I3DPrimOperations.hxx
HEADERS += GEOMImpl_IShapesOperations.hxx
HEADERS += GEOMImpl_IBlocksOperations.hxx
HEADERS += GEOMImpl_IBooleanOperations.hxx
HEADERS += GEOMImpl_ICurvesOperations.hxx
HEADERS += GEOMImpl_ILocalOperations.hxx
HEADERS += GEOMImpl_IInsertOperations.hxx
HEADERS += GEOMImpl_IMeasureOperations.hxx
HEADERS += GEOMImpl_IGroupOperations.hxx
HEADERS += GEOMImpl_CopyDriver.hxx
HEADERS += GEOMImpl_Types.hxx
HEADERS += GEOM_GEOMImpl.hxx

SOURCES  = GEOMImpl_IBasicOperations.cxx
SOURCES += GEOMImpl_ITransformOperations.cxx
SOURCES += GEOMImpl_IHealingOperations.cxx
SOURCES += GEOMImpl_I3DPrimOperations.cxx
SOURCES += GEOMImpl_IShapesOperations.cxx
SOURCES += GEOMImpl_IBlocksOperations.cxx
SOURCES += GEOMImpl_IBooleanOperations.cxx
SOURCES += GEOMImpl_ICurvesOperations.cxx
SOURCES += GEOMImpl_ILocalOperations.cxx
SOURCES += GEOMImpl_IInsertOperations.cxx
SOURCES += GEOMImpl_IMeasureOperations.cxx
SOURCES += GEOMImpl_IGroupOperations.cxx
SOURCES += GEOMImpl_Gen.cxx
SOURCES += GEOMImpl_PointDriver.cxx
SOURCES += GEOMImpl_VectorDriver.cxx
SOURCES += GEOMImpl_LineDriver.cxx
SOURCES += GEOMImpl_PlaneDriver.cxx
SOURCES += GEOMImpl_MarkerDriver.cxx
SOURCES += GEOMImpl_TranslateDriver.cxx
SOURCES += GEOMImpl_MirrorDriver.cxx
SOURCES += GEOMImpl_OffsetDriver.cxx
SOURCES += GEOMImpl_ScaleDriver.cxx
SOURCES += GEOMImpl_PositionDriver.cxx
SOURCES += GEOMImpl_BoxDriver.cxx
SOURCES += GEOMImpl_ConeDriver.cxx
SOURCES += GEOMImpl_CylinderDriver.cxx
SOURCES += GEOMImpl_SphereDriver.cxx
SOURCES += GEOMImpl_TorusDriver.cxx
SOURCES += GEOMImpl_PrismDriver.cxx
SOURCES += GEOMImpl_PipeDriver.cxx
SOURCES += GEOMImpl_ThruSectionsDriver.cxx
SOURCES += GEOMImpl_RevolutionDriver.cxx
SOURCES += GEOMImpl_ShapeDriver.cxx
SOURCES += GEOMImpl_BlockDriver.cxx
SOURCES += GEOMImpl_Block6Explorer.cxx
SOURCES += GEOMImpl_MeasureDriver.cxx
SOURCES += GEOMImpl_PolylineDriver.cxx
SOURCES += GEOMImpl_CircleDriver.cxx
SOURCES += GEOMImpl_EllipseDriver.cxx
SOURCES += GEOMImpl_ArcDriver.cxx
SOURCES += GEOMImpl_SplineDriver.cxx
SOURCES += GEOMImpl_SketcherDriver.cxx
SOURCES += GEOMImpl_FilletDriver.cxx
SOURCES += GEOMImpl_Fillet2dDriver.cxx
SOURCES += GEOMImpl_ChamferDriver.cxx
SOURCES += GEOMImpl_BooleanDriver.cxx
SOURCES += GEOMImpl_PartitionDriver.cxx
SOURCES += GEOMImpl_CopyDriver.cxx
SOURCES += GEOMImpl_ExportDriver.cxx
SOURCES += GEOMImpl_ImportDriver.cxx
SOURCES += GEOMImpl_RotateDriver.cxx
SOURCES += GEOMImpl_ArchimedeDriver.cxx
SOURCES += GEOMImpl_HealingDriver.cxx
SOURCES += GEOMImpl_FillingDriver.cxx
SOURCES += GEOMImpl_GlueDriver.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
