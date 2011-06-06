# Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

TEMPLATE = lib
TARGET = GEOMAlgo
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

STDLIB = -lstdc++

CAS_LDPATH = -L$$(CASROOT)/Linux/lib 

INCLUDEPATH += $${CAS_CPPFLAGS} $${KERNEL_CXXFLAGS} ../NMTDS ../NMTTools

LIBS += -L$$(GEOM_ROOT_DIR)/lib -lNMTTools $${STDLIB} $${CAS_LDPATH} -lTKBO -lTKShHealing -lTKBool -lTKMesh 

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = BlockFix_BlockFixAPI.hxx
HEADERS += BlockFix_BlockFixAPI.lxx
HEADERS += BlockFix_CheckTool.hxx
HEADERS += Handle_BlockFix_BlockFixAPI.hxx
HEADERS += GEOMAlgo_Algo.hxx
HEADERS += GEOMAlgo_BuilderArea.hxx
HEADERS += GEOMAlgo_BuilderFace.hxx
HEADERS += GEOMAlgo_Builder.hxx
HEADERS += GEOMAlgo_BuilderShape.hxx
HEADERS += GEOMAlgo_BuilderSolid.hxx
HEADERS += GEOMAlgo_BuilderTools.hxx
HEADERS += GEOMAlgo_ClsfBox.hxx
HEADERS += GEOMAlgo_Clsf.hxx
HEADERS += GEOMAlgo_ClsfSurf.hxx
HEADERS += GEOMAlgo_CoupleOfShapes.hxx
HEADERS += GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyInteger.hxx
HEADERS += GEOMAlgo_DataMapIteratorOfDataMapOfRealListOfShape.hxx
HEADERS += GEOMAlgo_DataMapIteratorOfDataMapOfShapeReal.hxx
HEADERS += GEOMAlgo_DataMapIteratorOfDataMapOfShapeShapeSet.hxx
HEADERS += GEOMAlgo_DataMapNodeOfDataMapOfPassKeyInteger.hxx
HEADERS += GEOMAlgo_DataMapNodeOfDataMapOfRealListOfShape.hxx
HEADERS += GEOMAlgo_DataMapNodeOfDataMapOfShapeReal.hxx
HEADERS += GEOMAlgo_DataMapNodeOfDataMapOfShapeShapeSet.hxx
HEADERS += GEOMAlgo_DataMapOfPassKeyInteger.hxx
HEADERS += GEOMAlgo_DataMapOfRealListOfShape.hxx
HEADERS += GEOMAlgo_DataMapOfShapeReal.hxx
HEADERS += GEOMAlgo_DataMapOfShapeShapeSet.hxx
HEADERS += GEOMAlgo_FinderShapeOn1.hxx
HEADERS += GEOMAlgo_FinderShapeOn2.hxx
HEADERS += GEOMAlgo_FinderShapeOn.hxx
HEADERS += GEOMAlgo_FinderShapeOnQuad.hxx
HEADERS += GEOMAlgo_GlueAnalyser.hxx
HEADERS += GEOMAlgo_Gluer.hxx
HEADERS += GEOMAlgo_HAlgo.hxx
HEADERS += GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape.hxx
HEADERS += GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfPassKeyListOfShape.hxx
HEADERS += GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfPassKeyShapeListOfShape.hxx
HEADERS += GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeBox.hxx
HEADERS += GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeState.hxx
HEADERS += GEOMAlgo_IndexedDataMapOfIntegerShape.hxx
HEADERS += GEOMAlgo_IndexedDataMapOfPassKeyListOfShape.hxx
HEADERS += GEOMAlgo_IndexedDataMapOfPassKeyShapeListOfShape.hxx
HEADERS += GEOMAlgo_IndexedDataMapOfShapeBox.hxx
HEADERS += GEOMAlgo_IndexedDataMapOfShapeState.hxx
HEADERS += GEOMAlgo_ListIteratorOfListOfCoupleOfShapes.hxx
HEADERS += GEOMAlgo_ListIteratorOfListOfPnt.hxx
HEADERS += GEOMAlgo_ListNodeOfListOfCoupleOfShapes.hxx
HEADERS += GEOMAlgo_ListNodeOfListOfPnt.hxx
HEADERS += GEOMAlgo_ListOfCoupleOfShapes.hxx
HEADERS += GEOMAlgo_ListOfPnt.hxx
HEADERS += GEOMAlgo_PassKey.hxx
HEADERS += GEOMAlgo_PassKeyMapHasher.hxx
HEADERS += GEOMAlgo_PassKeyShape.hxx
HEADERS += GEOMAlgo_PassKeyShapeMapHasher.hxx
HEADERS += GEOMAlgo_PWireEdgeSet.hxx
HEADERS += GEOMAlgo_ShapeAlgo.hxx
HEADERS += GEOMAlgo_ShapeSet.hxx
HEADERS += GEOMAlgo_ShapeSolid.hxx
HEADERS += GEOMAlgo_ShellSolid.hxx
HEADERS += GEOMAlgo_SolidSolid.hxx
HEADERS += GEOMAlgo_Splitter.hxx
HEADERS += GEOMAlgo_StateCollector.hxx
HEADERS += GEOMAlgo_State.hxx
HEADERS += GEOMAlgo_SurfaceTools.hxx
HEADERS += GEOMAlgo_Tools3D.hxx
HEADERS += GEOMAlgo_Tools.hxx
HEADERS += GEOMAlgo_VertexSolid.hxx
HEADERS += GEOMAlgo_WESCorrector.hxx
HEADERS += GEOMAlgo_WireEdgeSet.hxx
HEADERS += GEOMAlgo_WireSolid.hxx
HEADERS += GEOMAlgo_WireSplitter.hxx
HEADERS += Handle_GEOMAlgo_ClsfBox.hxx
HEADERS += Handle_GEOMAlgo_Clsf.hxx
HEADERS += Handle_GEOMAlgo_ClsfSurf.hxx
HEADERS += Handle_GEOMAlgo_DataMapNodeOfDataMapOfPassKeyInteger.hxx
HEADERS += Handle_GEOMAlgo_DataMapNodeOfDataMapOfRealListOfShape.hxx
HEADERS += Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapeReal.hxx
HEADERS += Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapeShapeSet.hxx
HEADERS += Handle_GEOMAlgo_HAlgo.hxx
HEADERS += Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape.hxx
HEADERS += Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfPassKeyListOfShape.hxx
HEADERS += Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfPassKeyShapeListOfShape.hxx
HEADERS += Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeBox.hxx
HEADERS += Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeState.hxx
HEADERS += Handle_GEOMAlgo_ListNodeOfListOfCoupleOfShapes.hxx
HEADERS += Handle_GEOMAlgo_ListNodeOfListOfPnt.hxx
HEADERS += GEOMAlgo_ClsfSolid.hxx
HEADERS += GEOMAlgo_ClsfSolid.ixx
HEADERS += GEOMAlgo_ClsfSolid.jxx
HEADERS += GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyShapeShape.hxx
HEADERS += GEOMAlgo_DataMapNodeOfDataMapOfPassKeyShapeShape.hxx
HEADERS += GEOMAlgo_DataMapOfPassKeyShapeShape.hxx
HEADERS += GEOMAlgo_Gluer1.cxx
HEADERS += GEOMAlgo_Gluer1.hxx
HEADERS += GEOMAlgo_Gluer1.ixx
HEADERS += GEOMAlgo_Gluer1.jxx
HEADERS += GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo.hxx
HEADERS += GEOMAlgo_IndexedDataMapOfShapeShapeInfo.hxx
HEADERS += GEOMAlgo_KindOfBounds.hxx
HEADERS += GEOMAlgo_KindOfClosed.hxx
HEADERS += GEOMAlgo_KindOfName.hxx
HEADERS += GEOMAlgo_KindOfShape.hxx
HEADERS += GEOMAlgo_ShapeInfo.hxx
HEADERS += GEOMAlgo_ShapeInfo.ixx
HEADERS += GEOMAlgo_ShapeInfo.jxx
HEADERS += GEOMAlgo_ShapeInfoFiller.hxx
HEADERS += GEOMAlgo_ShapeInfoFiller.ixx
HEADERS += GEOMAlgo_ShapeInfoFiller.jxx
HEADERS += Handle_GEOMAlgo_ClsfSolid.hxx
HEADERS += Handle_GEOMAlgo_DataMapNodeOfDataMapOfPassKeyShapeShape.hxx
HEADERS += Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo.hxx

SOURCES  = BlockFix.cxx
SOURCES += BlockFix_BlockFixAPI.cxx
SOURCES += BlockFix_CheckTool.cxx
SOURCES += BlockFix_PeriodicSurfaceModifier.cxx
SOURCES += BlockFix_SphereSpaceModifier.cxx
SOURCES += BlockFix_UnionEdges.cxx
SOURCES += BlockFix_UnionFaces.cxx
SOURCES += GEOMAlgo_Algo.cxx
SOURCES += GEOMAlgo_Builder_0.cxx
SOURCES += GEOMAlgo_Builder_1.cxx
SOURCES += GEOMAlgo_Builder_2.cxx
SOURCES += GEOMAlgo_Builder_3.cxx
SOURCES += GEOMAlgo_Builder_4.cxx
SOURCES += GEOMAlgo_BuilderArea.cxx
SOURCES += GEOMAlgo_Builder.cxx
SOURCES += GEOMAlgo_BuilderFace.cxx
SOURCES += GEOMAlgo_BuilderShape.cxx
SOURCES += GEOMAlgo_BuilderSolid.cxx
SOURCES += GEOMAlgo_BuilderTools.cxx
SOURCES += GEOMAlgo_ClsfBox.cxx
SOURCES += GEOMAlgo_Clsf.cxx
SOURCES += GEOMAlgo_ClsfSurf.cxx
SOURCES += GEOMAlgo_CoupleOfShapes.cxx
SOURCES += GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyInteger_0.cxx
SOURCES += GEOMAlgo_DataMapIteratorOfDataMapOfRealListOfShape_0.cxx
SOURCES += GEOMAlgo_DataMapIteratorOfDataMapOfShapeReal_0.cxx
SOURCES += GEOMAlgo_DataMapIteratorOfDataMapOfShapeShapeSet_0.cxx
SOURCES += GEOMAlgo_DataMapNodeOfDataMapOfPassKeyInteger_0.cxx
SOURCES += GEOMAlgo_DataMapNodeOfDataMapOfRealListOfShape_0.cxx
SOURCES += GEOMAlgo_DataMapNodeOfDataMapOfShapeReal_0.cxx
SOURCES += GEOMAlgo_DataMapNodeOfDataMapOfShapeShapeSet_0.cxx
SOURCES += GEOMAlgo_DataMapOfPassKeyInteger_0.cxx
SOURCES += GEOMAlgo_DataMapOfRealListOfShape_0.cxx
SOURCES += GEOMAlgo_DataMapOfShapeReal_0.cxx
SOURCES += GEOMAlgo_DataMapOfShapeShapeSet_0.cxx
SOURCES += GEOMAlgo_FinderShapeOn1.cxx
SOURCES += GEOMAlgo_FinderShapeOn2.cxx
SOURCES += GEOMAlgo_FinderShapeOn.cxx
SOURCES += GEOMAlgo_FinderShapeOnQuad.cxx
SOURCES += GEOMAlgo_GlueAnalyser.cxx
SOURCES += GEOMAlgo_Gluer.cxx
SOURCES += GEOMAlgo_HAlgo.cxx
SOURCES += GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape_0.cxx
SOURCES += GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfPassKeyListOfShape_0.cxx
SOURCES += GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfPassKeyShapeListOfShape_0.cxx
SOURCES += GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeBox_0.cxx
SOURCES += GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeState_0.cxx
SOURCES += GEOMAlgo_IndexedDataMapOfIntegerShape_0.cxx
SOURCES += GEOMAlgo_IndexedDataMapOfPassKeyListOfShape_0.cxx
SOURCES += GEOMAlgo_IndexedDataMapOfPassKeyShapeListOfShape_0.cxx
SOURCES += GEOMAlgo_IndexedDataMapOfShapeBox_0.cxx
SOURCES += GEOMAlgo_IndexedDataMapOfShapeState_0.cxx
SOURCES += GEOMAlgo_ListIteratorOfListOfCoupleOfShapes_0.cxx
SOURCES += GEOMAlgo_ListIteratorOfListOfPnt_0.cxx
SOURCES += GEOMAlgo_ListNodeOfListOfCoupleOfShapes_0.cxx
SOURCES += GEOMAlgo_ListNodeOfListOfPnt_0.cxx
SOURCES += GEOMAlgo_ListOfCoupleOfShapes_0.cxx
SOURCES += GEOMAlgo_ListOfPnt_0.cxx
SOURCES += GEOMAlgo_PassKey.cxx
SOURCES += GEOMAlgo_PassKeyMapHasher.cxx
SOURCES += GEOMAlgo_PassKeyShape.cxx
SOURCES += GEOMAlgo_PassKeyShapeMapHasher.cxx
SOURCES += GEOMAlgo_ShapeAlgo.cxx
SOURCES += GEOMAlgo_ShapeSet.cxx
SOURCES += GEOMAlgo_ShapeSolid.cxx
SOURCES += GEOMAlgo_ShellSolid.cxx
SOURCES += GEOMAlgo_SolidSolid.cxx
SOURCES += GEOMAlgo_Splitter.cxx
SOURCES += GEOMAlgo_StateCollector.cxx
SOURCES += GEOMAlgo_SurfaceTools.cxx
SOURCES += GEOMAlgo_Tools3D.cxx
SOURCES += GEOMAlgo_Tools.cxx
SOURCES += GEOMAlgo_VertexSolid.cxx
SOURCES += GEOMAlgo_WESCorrector.cxx
SOURCES += GEOMAlgo_WireEdgeSet.cxx
SOURCES += GEOMAlgo_WireSolid.cxx
SOURCES += GEOMAlgo_WireSplitter.cxx
SOURCES += GEOMAlgo_ClsfSolid.cxx
SOURCES += GEOMAlgo_DataMapNodeOfDataMapOfPassKeyShapeShape_0.cxx
SOURCES += GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyShapeShape_0.cxx
SOURCES += GEOMAlgo_DataMapOfPassKeyShapeShape_0.cxx
SOURCES += GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo_0.cxx
SOURCES += GEOMAlgo_ShapeInfoFiller_1.cxx
SOURCES += GEOMAlgo_IndexedDataMapOfShapeShapeInfo_0.cxx
SOURCES += GEOMAlgo_ShapeInfo.cxx
SOURCES += GEOMAlgo_ShapeInfoFiller.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
