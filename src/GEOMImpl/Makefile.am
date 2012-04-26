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
#  Author : Damien COQUERET (OCC)
#  Modified by : Alexander BORODIN (OCN) - autotools usage
#  Module : GEOM

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libGEOMimpl.la

# header files 
salomeinclude_HEADERS =				\
	GEOMImpl_Gen.hxx			\
	GEOMImpl_IBasicOperations.hxx		\
	GEOMImpl_ITransformOperations.hxx	\
	GEOMImpl_IHealingOperations.hxx		\
	GEOMImpl_I3DPrimOperations.hxx		\
	GEOMImpl_IShapesOperations.hxx		\
	GEOMImpl_IBlocksOperations.hxx		\
	GEOMImpl_IBooleanOperations.hxx		\
	GEOMImpl_ICurvesOperations.hxx		\
	GEOMImpl_ILocalOperations.hxx		\
	GEOMImpl_IInsertOperations.hxx		\
	GEOMImpl_IMeasureOperations.hxx		\
	GEOMImpl_IGroupOperations.hxx		\
	GEOMImpl_IAdvancedOperations.hxx	\
	GEOMImpl_IGlue.hxx			\
	GEOMImpl_PointDriver.hxx		\
	GEOMImpl_IPoint.hxx			\
	GEOMImpl_IPolyline.hxx			\
	GEOMImpl_ICircle.hxx			\
	GEOMImpl_ISpline.hxx			\
	GEOMImpl_IEllipse.hxx			\
	GEOMImpl_IFillet.hxx			\
	GEOMImpl_IFillet1d.hxx			\
	GEOMImpl_IFillet2d.hxx			\
	GEOMImpl_IChamfer.hxx			\
	GEOMImpl_ICopy.hxx			\
	GEOMImpl_IArchimede.hxx			\
	GEOMImpl_IArc.hxx			\
	GEOMImpl_ISketcher.hxx			\
	GEOMImpl_I3DSketcher.hxx		\
	GEOMImpl_IVector.hxx			\
	GEOMImpl_IDisk.hxx			\
	GEOMImpl_IFace.hxx			\
	GEOMImpl_ILine.hxx			\
	GEOMImpl_IPlane.hxx			\
	GEOMImpl_IMarker.hxx			\
	GEOMImpl_ITranslate.hxx			\
	GEOMImpl_IMirror.hxx			\
	GEOMImpl_IOffset.hxx			\
	GEOMImpl_IScale.hxx			\
	GEOMImpl_IRotate.hxx			\
	GEOMImpl_IPosition.hxx			\
	GEOMImpl_IHealing.hxx			\
	GEOMImpl_IImportExport.hxx		\
	GEOMImpl_IBox.hxx			\
	GEOMImpl_IBlocks.hxx			\
	GEOMImpl_IBlockTrsf.hxx			\
	GEOMImpl_IBoolean.hxx			\
	GEOMImpl_ICylinder.hxx			\
	GEOMImpl_ICone.hxx			\
	GEOMImpl_ISphere.hxx			\
	GEOMImpl_ITorus.hxx			\
	GEOMImpl_IPrism.hxx			\
	GEOMImpl_IPipe.hxx			\
	GEOMImpl_IRevolution.hxx		\
	GEOMImpl_IMeasure.hxx			\
	GEOMImpl_IShapes.hxx			\
	GEOMImpl_IFilling.hxx			\
	GEOMImpl_IThruSections.hxx		\
	GEOMImpl_IPartition.hxx			\
	GEOMImpl_IPipeDiffSect.hxx		\
	GEOMImpl_IPipeShellSect.hxx		\
	GEOMImpl_IPipeBiNormal.hxx		\
	GEOMImpl_VectorDriver.hxx		\
	GEOMImpl_LineDriver.hxx			\
	GEOMImpl_DiskDriver.hxx			\
	GEOMImpl_FaceDriver.hxx			\
	GEOMImpl_PlaneDriver.hxx		\
	GEOMImpl_MarkerDriver.hxx		\
	GEOMImpl_TranslateDriver.hxx		\
	GEOMImpl_MirrorDriver.hxx		\
	GEOMImpl_ProjectionDriver.hxx		\
	GEOMImpl_OffsetDriver.hxx		\
	BRepBuilderAPI_GTransform_21423.hxx		\
	BRepBuilderAPI_ModifyShape_21423.hxx		\
	BRepBuilderAPI_NurbsConvert_21423.hxx		\
	BRepTools_Modifier_21423.hxx			\
	BRepTools_NurbsConvertModification_21423.hxx	\
	Handle_BRepTools_NurbsConvertModification_21423.hxx	\
	GEOMImpl_ScaleDriver.hxx		\
	GEOMImpl_PositionDriver.hxx		\
	GEOMImpl_BoxDriver.hxx			\
	GEOMImpl_ConeDriver.hxx			\
	GEOMImpl_CylinderDriver.hxx		\
	GEOMImpl_SphereDriver.hxx		\
	GEOMImpl_TorusDriver.hxx		\
	GEOMImpl_PrismDriver.hxx		\
	GEOMImpl_PipeDriver.hxx			\
	GEOMImpl_ThruSectionsDriver.hxx		\
	GEOMImpl_RevolutionDriver.hxx		\
	GEOMImpl_ShapeDriver.hxx		\
	GEOMImpl_BlockDriver.hxx		\
	GEOMImpl_Block6Explorer.hxx		\
	GEOMImpl_MeasureDriver.hxx		\
	GEOMImpl_PolylineDriver.hxx		\
	GEOMImpl_CircleDriver.hxx		\
	GEOMImpl_EllipseDriver.hxx		\
	GEOMImpl_ArcDriver.hxx			\
	GEOMImpl_SplineDriver.hxx		\
	GEOMImpl_SketcherDriver.hxx		\
	GEOMImpl_3DSketcherDriver.hxx		\
	GEOMImpl_FilletDriver.hxx		\
	GEOMImpl_Fillet1d.hxx	          	\
	GEOMImpl_Fillet1dDriver.hxx		\
	GEOMImpl_Fillet2dDriver.hxx		\
	GEOMImpl_ChamferDriver.hxx		\
	GEOMImpl_BooleanDriver.hxx		\
	GEOMImpl_PartitionDriver.hxx		\
	GEOMImpl_CopyDriver.hxx			\
	GEOMImpl_ExportDriver.hxx		\
	GEOMImpl_ImportDriver.hxx		\
	GEOMImpl_RotateDriver.hxx		\
	GEOMImpl_ArchimedeDriver.hxx		\
	GEOMImpl_HealingDriver.hxx		\
	GEOMImpl_FillingDriver.hxx		\
	GEOMImpl_GlueDriver.hxx			\
	GEOMImpl_Types.hxx			\
	GEOM_GEOMImpl.hxx

ADVANCED_INCLUDES  =
ADVANCED_INCLUDES += GEOMImpl_IPipeTShape.hxx GEOMImpl_PipeTShapeDriver.hxx
##@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@##

salomeinclude_HEADERS += $(ADVANCED_INCLUDES)

dist_libGEOMimpl_la_SOURCES =			\
	GEOMImpl_IBasicOperations.cxx		\
	GEOMImpl_ITransformOperations.cxx	\
	GEOMImpl_IHealingOperations.cxx		\
	GEOMImpl_I3DPrimOperations.cxx		\
	GEOMImpl_IShapesOperations.cxx		\
	GEOMImpl_IBlocksOperations.cxx		\
	GEOMImpl_IBooleanOperations.cxx		\
	GEOMImpl_ICurvesOperations.cxx		\
	GEOMImpl_ILocalOperations.cxx		\
	GEOMImpl_IInsertOperations.cxx		\
	GEOMImpl_IMeasureOperations.cxx		\
	GEOMImpl_IGroupOperations.cxx		\
	GEOMImpl_IAdvancedOperations.cxx	\
	GEOMImpl_Gen.cxx			\
	GEOMImpl_PointDriver.cxx		\
	GEOMImpl_VectorDriver.cxx		\
	GEOMImpl_LineDriver.cxx			\
	GEOMImpl_PlaneDriver.cxx		\
	GEOMImpl_MarkerDriver.cxx		\
	GEOMImpl_TranslateDriver.cxx		\
	GEOMImpl_MirrorDriver.cxx		\
	GEOMImpl_ProjectionDriver.cxx		\
	GEOMImpl_OffsetDriver.cxx		\
	BRepBuilderAPI_GTransform_21423.cxx		\
	BRepBuilderAPI_ModifyShape_21423.cxx		\
	BRepBuilderAPI_NurbsConvert_21423.cxx		\
	BRepTools_Modifier_21423.cxx			\
	BRepTools_NurbsConvertModification_21423.cxx	\
	GEOMImpl_ScaleDriver.cxx		\
	GEOMImpl_PositionDriver.cxx		\
	GEOMImpl_BoxDriver.cxx			\
	GEOMImpl_FaceDriver.cxx			\
	GEOMImpl_DiskDriver.cxx			\
	GEOMImpl_ConeDriver.cxx			\
	GEOMImpl_CylinderDriver.cxx		\
	GEOMImpl_SphereDriver.cxx		\
	GEOMImpl_TorusDriver.cxx		\
	GEOMImpl_PrismDriver.cxx		\
	GEOMImpl_PipeDriver.cxx			\
	GEOMImpl_ThruSectionsDriver.cxx		\
	GEOMImpl_RevolutionDriver.cxx		\
	GEOMImpl_ShapeDriver.cxx		\
	GEOMImpl_BlockDriver.cxx		\
	GEOMImpl_Block6Explorer.cxx		\
	GEOMImpl_MeasureDriver.cxx		\
	GEOMImpl_PolylineDriver.cxx		\
	GEOMImpl_CircleDriver.cxx		\
	GEOMImpl_EllipseDriver.cxx		\
	GEOMImpl_ArcDriver.cxx			\
	GEOMImpl_SplineDriver.cxx		\
	GEOMImpl_SketcherDriver.cxx		\
	GEOMImpl_3DSketcherDriver.cxx		\
	GEOMImpl_FilletDriver.cxx		\
	GEOMImpl_Fillet1d.cxx     		\
	GEOMImpl_Fillet1dDriver.cxx		\
	GEOMImpl_Fillet2dDriver.cxx		\
	GEOMImpl_ChamferDriver.cxx		\
	GEOMImpl_BooleanDriver.cxx		\
	GEOMImpl_PartitionDriver.cxx		\
	GEOMImpl_CopyDriver.cxx			\
	GEOMImpl_ExportDriver.cxx		\
	GEOMImpl_ImportDriver.cxx		\
	GEOMImpl_RotateDriver.cxx		\
	GEOMImpl_ArchimedeDriver.cxx		\
	GEOMImpl_HealingDriver.cxx		\
	GEOMImpl_FillingDriver.cxx		\
	GEOMImpl_GlueDriver.cxx

ADVANCED_SOURCES  =
ADVANCED_SOURCES += GEOMImpl_PipeTShapeDriver.cxx
##@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@##

dist_libGEOMimpl_la_SOURCES += $(ADVANCED_SOURCES)

# additional information to compile and link file

libGEOMimpl_la_CPPFLAGS =		\
	$(CORBA_CXXFLAGS)		\
        $(CORBA_INCLUDES)		\
	$(CAS_CPPFLAGS)			\
	$(KERNEL_CXXFLAGS)		\
	$(BOOST_CPPFLAGS)		\
        $(PYTHON_INCLUDES)              \
	-I$(srcdir)/../ShHealOper	\
	-I$(srcdir)/../NMTDS		\
	-I$(srcdir)/../NMTTools		\
	-I$(srcdir)/../GEOM		\
	-I$(srcdir)/../GEOMAlgo		\
	-I$(srcdir)/../SKETCHER		\
	-I$(srcdir)/../ARCHIMEDE	\
	-I$(top_builddir)/idl

libGEOMimpl_la_LDFLAGS  =			\
	../GEOM/libGEOMbasic.la			\
	../GEOMAlgo/libGEOMAlgo.la		\
	../ShHealOper/libShHealOper.la		\
	../ARCHIMEDE/libGEOMArchimede.la	\
	../SKETCHER/libGEOMSketcher.la		\
	$(KERNEL_LDFLAGS) -lSALOMELocalTrace -lSALOMEBasics	\
	$(STDLIB)				\
	$(CAS_LDPATH) -lTKCAF -lTKFillet -lTKOffset -lTKFeat \
        $(PYTHON_LIBS)

# extra dist files
EXTRA_DIST += GUID.txt
