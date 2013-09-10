# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/NMTTools
  ${PROJECT_SOURCE_DIR}/src/NMTDS
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${CAS_KERNEL} ${CAS_TKBool} ${CAS_BO} ${CAS_TKMesh}
  ${KERNEL_SALOMELocalTrace}
  NMTTools NMTDS
  )

# --- headers ---

SET(GEOMAlgo_HEADERS
  GEOMAlgo_Algo.hxx
  GEOMAlgo_BuilderArea.hxx
  GEOMAlgo_BuilderFace.hxx
  GEOMAlgo_Builder.hxx
  GEOMAlgo_BuilderShape.hxx
  GEOMAlgo_BuilderSolid.hxx
  GEOMAlgo_BuilderTools.hxx
  GEOMAlgo_ClsfBox.hxx
  GEOMAlgo_Clsf.hxx
  GEOMAlgo_ClsfSolid.hxx
  GEOMAlgo_ClsfSurf.hxx
  GEOMAlgo_CoupleOfShapes.hxx
  GEOMAlgo_DataMapIteratorOfDataMapOfOrientedShapeShape.hxx
  GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyInteger.hxx
  GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyShapeShape.hxx
  GEOMAlgo_DataMapIteratorOfDataMapOfRealListOfShape.hxx
  GEOMAlgo_DataMapIteratorOfDataMapOfShapeMapOfShape.hxx
  GEOMAlgo_DataMapIteratorOfDataMapOfShapePnt.hxx
  GEOMAlgo_DataMapIteratorOfDataMapOfShapeReal.hxx
  GEOMAlgo_DataMapIteratorOfDataMapOfShapeShapeSet.hxx
  GEOMAlgo_DataMapOfOrientedShapeShape.hxx
  GEOMAlgo_DataMapOfPassKeyInteger.hxx
  GEOMAlgo_DataMapOfPassKeyShapeShape.hxx
  GEOMAlgo_DataMapOfRealListOfShape.hxx
  GEOMAlgo_DataMapOfShapeMapOfShape.hxx
  GEOMAlgo_DataMapOfShapePnt.hxx
  GEOMAlgo_DataMapOfShapeReal.hxx
  GEOMAlgo_DataMapOfShapeShapeSet.hxx
  GEOMAlgo_FinderShapeOn1.hxx
  GEOMAlgo_FinderShapeOn2.hxx
  GEOMAlgo_FinderShapeOn.hxx
  GEOMAlgo_FinderShapeOnQuad.hxx
  GEOMAlgo_GetInPlace.hxx
  GEOMAlgo_GlueAnalyser.hxx
  GEOMAlgo_GlueDetector.hxx
  GEOMAlgo_Gluer2.hxx
  GEOMAlgo_GluerAlgo.hxx
  GEOMAlgo_Gluer.hxx
  GEOMAlgo_HAlgo.hxx
  GEOMAlgo_IndexedDataMapOfIntegerShape.hxx
  GEOMAlgo_IndexedDataMapOfPassKeyShapeListOfShape.hxx
  GEOMAlgo_IndexedDataMapOfShapeBox.hxx
  GEOMAlgo_IndexedDataMapOfShapeShapeInfo.hxx
  GEOMAlgo_IndexedDataMapOfShapeState.hxx
  GEOMAlgo_KindOfBounds.hxx
  GEOMAlgo_KindOfClosed.hxx
  GEOMAlgo_KindOfName.hxx
  GEOMAlgo_KindOfShape.hxx
  GEOMAlgo_ListIteratorOfListOfCoupleOfShapes.hxx
  GEOMAlgo_ListIteratorOfListOfPnt.hxx
  GEOMAlgo_ListOfCoupleOfShapes.hxx
  GEOMAlgo_ListOfPnt.hxx
  GEOMAlgo_PassKey.hxx
  GEOMAlgo_PassKeyMapHasher.hxx
  GEOMAlgo_PassKeyShape.hxx
  GEOMAlgo_PassKeyShapeMapHasher.hxx
  GEOMAlgo_PWireEdgeSet.hxx
  GEOMAlgo_RemoverWebs.hxx
  GEOMAlgo_ShapeAlgo.hxx
  GEOMAlgo_ShapeInfo.hxx
  GEOMAlgo_ShapeInfoFiller.hxx
  GEOMAlgo_ShapeSet.hxx
  GEOMAlgo_ShapeSolid.hxx
  GEOMAlgo_ShellSolid.hxx
  GEOMAlgo_SolidSolid.hxx
  GEOMAlgo_Splitter.hxx
  GEOMAlgo_StateCollector.hxx
  GEOMAlgo_State.hxx
  GEOMAlgo_SurfaceTools.hxx
  GEOMAlgo_Tools3D.hxx
  GEOMAlgo_Tools.hxx
  GEOMAlgo_VertexSolid.hxx
  GEOMAlgo_WESCorrector.hxx
  GEOMAlgo_WESScaler.hxx
  GEOMAlgo_WireEdgeSet.hxx
  GEOMAlgo_WireSolid.hxx
  GEOMAlgo_WireSplitter.hxx
  )

# --- sources ---

SET(GEOMAlgo_SOURCES
  GEOMAlgo_Algo.cxx
  GEOMAlgo_Builder_0.cxx
  GEOMAlgo_Builder_1.cxx
  GEOMAlgo_Builder_2.cxx
  GEOMAlgo_Builder_3.cxx
  GEOMAlgo_Builder_4.cxx
  GEOMAlgo_BuilderArea.cxx
  GEOMAlgo_Builder.cxx
  GEOMAlgo_BuilderFace.cxx
  GEOMAlgo_BuilderShape.cxx
  GEOMAlgo_BuilderSolid.cxx
  GEOMAlgo_BuilderTools.cxx
  GEOMAlgo_ClsfBox.cxx
  GEOMAlgo_Clsf.cxx
  GEOMAlgo_ClsfSolid.cxx
  GEOMAlgo_ClsfSurf.cxx
  GEOMAlgo_CoupleOfShapes.cxx
  GEOMAlgo_FinderShapeOn1.cxx
  GEOMAlgo_FinderShapeOn2.cxx
  GEOMAlgo_FinderShapeOn.cxx
  GEOMAlgo_FinderShapeOnQuad.cxx
  GEOMAlgo_GetInPlace_1.cxx
  GEOMAlgo_GetInPlace_2.cxx
  GEOMAlgo_GetInPlace_3.cxx
  GEOMAlgo_GetInPlace.cxx
  GEOMAlgo_GlueAnalyser.cxx
  GEOMAlgo_GlueDetector.cxx
  GEOMAlgo_Gluer2_1.cxx
  GEOMAlgo_Gluer2_2.cxx
  GEOMAlgo_Gluer2_3.cxx
  GEOMAlgo_Gluer2.cxx
  GEOMAlgo_GluerAlgo.cxx
  GEOMAlgo_Gluer.cxx
  GEOMAlgo_HAlgo.cxx
  GEOMAlgo_PassKey.cxx
  GEOMAlgo_PassKeyMapHasher.cxx
  GEOMAlgo_PassKeyShape.cxx
  GEOMAlgo_PassKeyShapeMapHasher.cxx
  GEOMAlgo_RemoverWebs.cxx
  GEOMAlgo_ShapeAlgo.cxx
  GEOMAlgo_ShapeInfo.cxx
  GEOMAlgo_ShapeInfoFiller_1.cxx
  GEOMAlgo_ShapeInfoFiller.cxx
  GEOMAlgo_ShapeSet.cxx
  GEOMAlgo_ShapeSolid.cxx
  GEOMAlgo_ShellSolid.cxx
  GEOMAlgo_SolidSolid.cxx
  GEOMAlgo_Splitter.cxx
  GEOMAlgo_StateCollector.cxx
  GEOMAlgo_SurfaceTools.cxx
  GEOMAlgo_Tools_1.cxx
  GEOMAlgo_Tools3D.cxx
  GEOMAlgo_Tools.cxx
  GEOMAlgo_VertexSolid.cxx
  GEOMAlgo_WESCorrector.cxx
  GEOMAlgo_WESScaler.cxx
  GEOMAlgo_WireEdgeSet.cxx
  GEOMAlgo_WireSolid.cxx
  GEOMAlgo_WireSplitter.cxx
  )

# --- rules ---

ADD_LIBRARY(GEOMAlgo ${GEOMAlgo_SOURCES})
TARGET_LINK_LIBRARIES(GEOMAlgo ${_link_LIBRARIES})
INSTALL(TARGETS GEOMAlgo EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${GEOMAlgo_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})


