# Copyright (C) 2012-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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
  ${PTHREAD_INCLUDE_DIR}
  ${KERNEL_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/ShHealOper
  ${PROJECT_SOURCE_DIR}/src/GEOM 
  ${PROJECT_SOURCE_DIR}/src/BlockFix
  ${PROJECT_SOURCE_DIR}/src/GEOMAlgo 
  ${PROJECT_SOURCE_DIR}/src/GEOMUtils
  ${PROJECT_SOURCE_DIR}/src/SKETCHER
  ${PROJECT_SOURCE_DIR}/src/ARCHIMEDE
  ${PROJECT_SOURCE_DIR}/src/XAO
  ${PROJECT_SOURCE_DIR}/src/ShHealOper
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${PYTHON_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${CAS_TKFeat}
  ${CAS_TKFillet}
  ${PYTHON_LIBRARIES}
  ShHealOper GEOMbasic BlockFix GEOMAlgo GEOMUtils GEOMSketcher GEOMArchimede XAO
  ${KERNEL_SALOMELocalTrace}
  )

# --- headers ---

SET(GEOMImpl_HEADERS
  GEOMImpl_Gen.hxx
  GEOMImpl_IBasicOperations.hxx
  GEOMImpl_ITransformOperations.hxx
  GEOMImpl_IHealingOperations.hxx
  GEOMImpl_I3DPrimOperations.hxx
  GEOMImpl_IShapesOperations.hxx
  GEOMImpl_IBlocksOperations.hxx
  GEOMImpl_IBooleanOperations.hxx
  GEOMImpl_ICurvesOperations.hxx
  GEOMImpl_ILocalOperations.hxx
  GEOMImpl_IInsertOperations.hxx
  GEOMImpl_IECallBack.hxx
  GEOMImpl_IMeasureOperations.hxx
  GEOMImpl_IGroupOperations.hxx
  GEOMImpl_IFieldOperations.hxx
  GEOMImpl_IBaseIEOperations.hxx
  GEOMImpl_IGlue.hxx
  GEOMImpl_PointDriver.hxx
  GEOMImpl_IPoint.hxx
  GEOMImpl_IPolyline.hxx
  GEOMImpl_IPolyline2D.hxx
  GEOMImpl_ICircle.hxx
  GEOMImpl_ISpline.hxx
  GEOMImpl_IEllipse.hxx
  GEOMImpl_IExtract.hxx
  GEOMImpl_IFillet.hxx
  GEOMImpl_IFillet1d.hxx
  GEOMImpl_IFillet2d.hxx
  GEOMImpl_IChamfer.hxx
  GEOMImpl_ICopy.hxx
  GEOMImpl_IArchimede.hxx
  GEOMImpl_IArc.hxx
  GEOMImpl_ISketcher.hxx
  GEOMImpl_I3DSketcher.hxx
  GEOMImpl_IVector.hxx
  GEOMImpl_IDisk.hxx
  GEOMImpl_IFace.hxx
  GEOMImpl_ILine.hxx
  GEOMImpl_IPlane.hxx
  GEOMImpl_IMarker.hxx
  GEOMImpl_ITranslate.hxx
  GEOMImpl_IMirror.hxx
  GEOMImpl_IProjection.hxx
  GEOMImpl_IProjOnCyl.hxx
  GEOMImpl_IOffset.hxx
  GEOMImpl_IScale.hxx
  GEOMImpl_IRotate.hxx
  GEOMImpl_IPosition.hxx
  GEOMImpl_IHealing.hxx
  GEOMImpl_IImportExport.hxx
  GEOMImpl_IBox.hxx
  GEOMImpl_IBlocks.hxx
  GEOMImpl_IBlockTrsf.hxx
  GEOMImpl_IBoolean.hxx
  GEOMImpl_ICylinder.hxx
  GEOMImpl_ICone.hxx
  GEOMImpl_ISphere.hxx
  GEOMImpl_ITorus.hxx
  GEOMImpl_ITransferData.hxx
  GEOMImpl_IPrism.hxx
  GEOMImpl_IPipe.hxx
  GEOMImpl_IPipePath.hxx
  GEOMImpl_IRevolution.hxx
  GEOMImpl_IMeasure.hxx
  GEOMImpl_IShapes.hxx
  GEOMImpl_IShapeExtend.hxx
  GEOMImpl_IFilling.hxx
  GEOMImpl_IThruSections.hxx
  GEOMImpl_IPartition.hxx
  GEOMImpl_IPipeDiffSect.hxx
  GEOMImpl_IPipeShellSect.hxx
  GEOMImpl_IPipeBiNormal.hxx
  GEOMImpl_ICurveParametric.hxx
  GEOMImpl_IIsoline.hxx
  GEOMImpl_VectorDriver.hxx
  GEOMImpl_LineDriver.hxx
  GEOMImpl_DiskDriver.hxx
  GEOMImpl_FaceDriver.hxx
  GEOMImpl_PlaneDriver.hxx
  GEOMImpl_MarkerDriver.hxx
  GEOMImpl_TranslateDriver.hxx
  GEOMImpl_MirrorDriver.hxx
  GEOMImpl_ProjectionDriver.hxx
  GEOMImpl_OffsetDriver.hxx
  GEOMImpl_ScaleDriver.hxx
  GEOMImpl_PositionDriver.hxx
  GEOMImpl_BoxDriver.hxx
  GEOMImpl_ConeDriver.hxx
  GEOMImpl_CylinderDriver.hxx
  GEOMImpl_SphereDriver.hxx
  GEOMImpl_TorusDriver.hxx
  GEOMImpl_PrismDriver.hxx
  GEOMImpl_PipeDriver.hxx
  GEOMImpl_PipePathDriver.hxx
  GEOMImpl_ThruSectionsDriver.hxx
  GEOMImpl_RevolutionDriver.hxx
  GEOMImpl_ShapeDriver.hxx
  GEOMImpl_BlockDriver.hxx
  GEOMImpl_Block6Explorer.hxx
  GEOMImpl_MeasureDriver.hxx
  GEOMImpl_PolylineDriver.hxx
  GEOMImpl_PolylineDumper.hxx
  GEOMImpl_CircleDriver.hxx
  GEOMImpl_EllipseDriver.hxx
  GEOMImpl_ArcDriver.hxx
  GEOMImpl_SplineDriver.hxx
  GEOMImpl_SketcherDriver.hxx
  GEOMImpl_3DSketcherDriver.hxx
  GEOMImpl_FilletDriver.hxx
  GEOMImpl_Fillet1d.hxx
  GEOMImpl_Fillet1dDriver.hxx
  GEOMImpl_Fillet2dDriver.hxx
  GEOMImpl_ChamferDriver.hxx
  GEOMImpl_BooleanDriver.hxx
  GEOMImpl_PartitionDriver.hxx
  GEOMImpl_CopyDriver.hxx
  GEOMImpl_ExportDriver.hxx
  GEOMImpl_ImportDriver.hxx
  GEOMImpl_RotateDriver.hxx
  GEOMImpl_ArchimedeDriver.hxx
  GEOMImpl_HealingDriver.hxx
  GEOMImpl_FillingDriver.hxx
  GEOMImpl_GlueDriver.hxx
  GEOMImpl_Types.hxx
  GEOM_GEOMImpl.hxx
  )
# --- sources ---

SET(GEOMImpl_SOURCES
  GEOMImpl_IBasicOperations.cxx
  GEOMImpl_ITransformOperations.cxx
  GEOMImpl_IHealingOperations.cxx
  GEOMImpl_I3DPrimOperations.cxx
  GEOMImpl_IShapesOperations.cxx
  GEOMImpl_IBlocksOperations.cxx
  GEOMImpl_IBooleanOperations.cxx
  GEOMImpl_ICurvesOperations.cxx
  GEOMImpl_ILocalOperations.cxx
  GEOMImpl_IInsertOperations.cxx
  GEOMImpl_IECallBack.cxx
  GEOMImpl_IMeasureOperations.cxx
  GEOMImpl_IGroupOperations.cxx
  GEOMImpl_IFieldOperations.cxx
  GEOMImpl_IBaseIEOperations.cxx
  GEOMImpl_IPolyline2D.cxx
  GEOMImpl_ITransferData.cxx
  GEOMImpl_Gen.cxx
  GEOMImpl_PointDriver.cxx
  GEOMImpl_VectorDriver.cxx
  GEOMImpl_LineDriver.cxx
  GEOMImpl_PlaneDriver.cxx
  GEOMImpl_MarkerDriver.cxx
  GEOMImpl_TranslateDriver.cxx
  GEOMImpl_MirrorDriver.cxx
  GEOMImpl_ProjectionDriver.cxx
  GEOMImpl_OffsetDriver.cxx
  GEOMImpl_ScaleDriver.cxx
  GEOMImpl_PositionDriver.cxx
  GEOMImpl_BoxDriver.cxx
  GEOMImpl_FaceDriver.cxx
  GEOMImpl_DiskDriver.cxx
  GEOMImpl_ConeDriver.cxx
  GEOMImpl_CylinderDriver.cxx
  GEOMImpl_SphereDriver.cxx
  GEOMImpl_TorusDriver.cxx
  GEOMImpl_PrismDriver.cxx
  GEOMImpl_PipeDriver.cxx
  GEOMImpl_PipePathDriver.cxx
  GEOMImpl_ThruSectionsDriver.cxx
  GEOMImpl_RevolutionDriver.cxx
  GEOMImpl_ShapeDriver.cxx
  GEOMImpl_BlockDriver.cxx
  GEOMImpl_Block6Explorer.cxx
  GEOMImpl_MeasureDriver.cxx
  GEOMImpl_PolylineDriver.cxx
  GEOMImpl_PolylineDumper.cxx
  GEOMImpl_CircleDriver.cxx
  GEOMImpl_EllipseDriver.cxx
  GEOMImpl_ArcDriver.cxx
  GEOMImpl_SplineDriver.cxx
  GEOMImpl_SketcherDriver.cxx
  GEOMImpl_3DSketcherDriver.cxx
  GEOMImpl_FilletDriver.cxx
  GEOMImpl_Fillet1d.cxx
  GEOMImpl_Fillet1dDriver.cxx
  GEOMImpl_Fillet2dDriver.cxx
  GEOMImpl_ChamferDriver.cxx
  GEOMImpl_BooleanDriver.cxx
  GEOMImpl_PartitionDriver.cxx
  GEOMImpl_CopyDriver.cxx
  GEOMImpl_ExportDriver.cxx
  GEOMImpl_ImportDriver.cxx
  GEOMImpl_RotateDriver.cxx
  GEOMImpl_ArchimedeDriver.cxx
  GEOMImpl_HealingDriver.cxx
  GEOMImpl_FillingDriver.cxx
  GEOMImpl_GlueDriver.cxx
  GEOMImpl_FieldDriver.cxx
  )

# --- rules ---

ADD_LIBRARY(GEOMImpl ${GEOMImpl_SOURCES})
TARGET_LINK_LIBRARIES(GEOMImpl ${_link_LIBRARIES})
INSTALL(TARGETS GEOMImpl EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${GEOMImpl_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
