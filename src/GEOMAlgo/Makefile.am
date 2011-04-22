#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

#  GEOM GEOMAlgo : tools for Glue Faces algorithm
#  File   : Makefile.in
#  Author : Julia DOROVSKIKH
#  Module : GEOM

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libGEOMAlgo.la

dist_libGEOMAlgo_la_SOURCES =			\
	BlockFix.cxx				\
	BlockFix_BlockFixAPI.cxx		\
	BlockFix_CheckTool.cxx			\
	BlockFix_PeriodicSurfaceModifier.cxx	\
	BlockFix_SphereSpaceModifier.cxx	\
	BlockFix_UnionEdges.cxx			\
	BlockFix_UnionFaces.cxx			\
	GEOMAlgo_Algo.cxx			\
	GEOMAlgo_Builder_0.cxx			\
	GEOMAlgo_Builder_1.cxx			\
	GEOMAlgo_Builder_2.cxx			\
	GEOMAlgo_Builder_3.cxx			\
	GEOMAlgo_Builder_4.cxx			\
	GEOMAlgo_BuilderArea.cxx		\
	GEOMAlgo_Builder.cxx			\
	GEOMAlgo_BuilderFace.cxx		\
	GEOMAlgo_BuilderShape.cxx		\
	GEOMAlgo_BuilderSolid.cxx		\
	GEOMAlgo_BuilderTools.cxx		\
	GEOMAlgo_ClsfBox.cxx			\
	GEOMAlgo_Clsf.cxx			\
	GEOMAlgo_ClsfSolid.cxx			\
	GEOMAlgo_ClsfSurf.cxx			\
	GEOMAlgo_CoupleOfShapes.cxx		\
	GEOMAlgo_DataMapIteratorOfDataMapOfOrientedShapeShape_0.cxx	\
	GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyInteger_0.cxx		\
	GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyShapeShape_0.cxx	\
	GEOMAlgo_DataMapIteratorOfDataMapOfRealListOfShape_0.cxx	\
	GEOMAlgo_DataMapIteratorOfDataMapOfShapeReal_0.cxx		\
	GEOMAlgo_DataMapIteratorOfDataMapOfShapeShapeSet_0.cxx		\
	GEOMAlgo_DataMapNodeOfDataMapOfOrientedShapeShape_0.cxx		\
	GEOMAlgo_DataMapNodeOfDataMapOfPassKeyInteger_0.cxx		\
	GEOMAlgo_DataMapNodeOfDataMapOfPassKeyShapeShape_0.cxx		\
	GEOMAlgo_DataMapNodeOfDataMapOfRealListOfShape_0.cxx		\
	GEOMAlgo_DataMapNodeOfDataMapOfShapeReal_0.cxx			\
	GEOMAlgo_DataMapNodeOfDataMapOfShapeShapeSet_0.cxx		\
	GEOMAlgo_DataMapOfOrientedShapeShape_0.cxx	\
	GEOMAlgo_DataMapOfPassKeyInteger_0.cxx		\
	GEOMAlgo_DataMapOfPassKeyShapeShape_0.cxx	\
	GEOMAlgo_DataMapOfRealListOfShape_0.cxx		\
	GEOMAlgo_DataMapOfShapeReal_0.cxx		\
	GEOMAlgo_DataMapOfShapeShapeSet_0.cxx		\
	GEOMAlgo_FinderShapeOn1.cxx		\
	GEOMAlgo_FinderShapeOn2.cxx		\
	GEOMAlgo_FinderShapeOn.cxx		\
	GEOMAlgo_FinderShapeOnQuad.cxx		\
	GEOMAlgo_GlueAnalyser.cxx		\
	GEOMAlgo_Gluer.cxx			\
	GEOMAlgo_GlueDetector.cxx		\
	GEOMAlgo_Gluer2.cxx			\
	GEOMAlgo_Gluer2_1.cxx			\
	GEOMAlgo_Gluer2_2.cxx			\
	GEOMAlgo_Gluer2_3.cxx			\
	GEOMAlgo_GluerAlgo.cxx			\
	GEOMAlgo_Gluer1.cxx			\
	GEOMAlgo_HAlgo.cxx			\
	GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape_0.cxx			\
	GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfPassKeyListOfShape_0.cxx		\
	GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfPassKeyShapeListOfShape_0.cxx	\
	GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeBox_0.cxx		\
	GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo_0.cxx	\
	GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeState_0.cxx		\
	GEOMAlgo_IndexedDataMapOfIntegerShape_0.cxx				\
	GEOMAlgo_IndexedDataMapOfPassKeyListOfShape_0.cxx			\
	GEOMAlgo_IndexedDataMapOfPassKeyShapeListOfShape_0.cxx			\
	GEOMAlgo_IndexedDataMapOfShapeBox_0.cxx					\
	GEOMAlgo_IndexedDataMapOfShapeShapeInfo_0.cxx				\
	GEOMAlgo_IndexedDataMapOfShapeState_0.cxx				\
	GEOMAlgo_ListIteratorOfListOfCoupleOfShapes_0.cxx		\
	GEOMAlgo_ListIteratorOfListOfPnt_0.cxx				\
	GEOMAlgo_ListNodeOfListOfCoupleOfShapes_0.cxx			\
	GEOMAlgo_ListNodeOfListOfPnt_0.cxx	\
	GEOMAlgo_ListOfCoupleOfShapes_0.cxx	\
	GEOMAlgo_ListOfPnt_0.cxx		\
	GEOMAlgo_PassKey.cxx			\
	GEOMAlgo_PassKeyMapHasher.cxx		\
	GEOMAlgo_PassKeyShape.cxx		\
	GEOMAlgo_PassKeyShapeMapHasher.cxx	\
	GEOMAlgo_ShapeAlgo.cxx			\
	GEOMAlgo_ShapeInfo.cxx			\
	GEOMAlgo_ShapeInfoFiller.cxx		\
	GEOMAlgo_ShapeInfoFiller_1.cxx		\
	GEOMAlgo_ShapeSet.cxx			\
	GEOMAlgo_ShapeSolid.cxx			\
	GEOMAlgo_ShellSolid.cxx			\
	GEOMAlgo_SolidSolid.cxx			\
	GEOMAlgo_Splitter.cxx			\
	GEOMAlgo_StateCollector.cxx		\
	GEOMAlgo_SurfaceTools.cxx		\
	GEOMAlgo_Tools3D.cxx			\
	GEOMAlgo_Tools.cxx			\
	GEOMAlgo_Tools_1.cxx			\
	GEOMAlgo_VertexSolid.cxx		\
	GEOMAlgo_WESCorrector.cxx		\
	GEOMAlgo_WESScaler.cxx			\
	GEOMAlgo_WireEdgeSet.cxx		\
	GEOMAlgo_WireSolid.cxx			\
	GEOMAlgo_WireSplitter.cxx

# header files
salomeinclude_HEADERS =					\
	BlockFix.hxx					\
	BlockFix.ixx					\
	BlockFix.jxx					\
	BlockFix_BlockFixAPI.hxx			\
	BlockFix_BlockFixAPI.ixx			\
	BlockFix_BlockFixAPI.jxx			\
	BlockFix_BlockFixAPI.lxx			\
	BlockFix_CheckTool.hxx				\
	BlockFix_CheckTool.ixx				\
	BlockFix_CheckTool.jxx				\
	BlockFix_PeriodicSurfaceModifier.hxx		\
	BlockFix_PeriodicSurfaceModifier.ixx		\
	BlockFix_PeriodicSurfaceModifier.jxx		\
	BlockFix_SphereSpaceModifier.hxx		\
	BlockFix_SphereSpaceModifier.ixx		\
	BlockFix_SphereSpaceModifier.jxx		\
	BlockFix_UnionEdges.hxx				\
	BlockFix_UnionEdges.ixx				\
	BlockFix_UnionEdges.jxx				\
	BlockFix_UnionFaces.hxx				\
	BlockFix_UnionFaces.ixx				\
	BlockFix_UnionFaces.jxx				\
	Handle_BlockFix_BlockFixAPI.hxx 		\
	Handle_BlockFix_SphereSpaceModifier.hxx		\
	Handle_BlockFix_PeriodicSurfaceModifier.hxx	\
	GEOMAlgo_Algo.hxx				\
	GEOMAlgo_Algo.ixx				\
	GEOMAlgo_Algo.jxx				\
	GEOMAlgo_BuilderArea.hxx			\
	GEOMAlgo_BuilderArea.ixx			\
	GEOMAlgo_BuilderArea.jxx			\
	GEOMAlgo_BuilderFace.hxx			\
	GEOMAlgo_BuilderFace.ixx			\
	GEOMAlgo_BuilderFace.jxx			\
	GEOMAlgo_Builder.hxx				\
	GEOMAlgo_Builder.ixx				\
	GEOMAlgo_Builder.jxx				\
	GEOMAlgo_BuilderShape.hxx			\
	GEOMAlgo_BuilderShape.ixx			\
	GEOMAlgo_BuilderShape.jxx			\
	GEOMAlgo_BuilderSolid.hxx			\
	GEOMAlgo_BuilderSolid.ixx			\
	GEOMAlgo_BuilderSolid.jxx			\
	GEOMAlgo_BuilderTools.hxx			\
	GEOMAlgo_BuilderTools.ixx			\
	GEOMAlgo_BuilderTools.jxx			\
	GEOMAlgo_ClsfBox.hxx				\
	GEOMAlgo_ClsfBox.ixx				\
	GEOMAlgo_ClsfBox.jxx				\
	GEOMAlgo_Clsf.hxx				\
	GEOMAlgo_Clsf.ixx				\
	GEOMAlgo_Clsf.jxx				\
	GEOMAlgo_ClsfSolid.hxx				\
	GEOMAlgo_ClsfSolid.ixx				\
	GEOMAlgo_ClsfSolid.jxx				\
	GEOMAlgo_ClsfSurf.hxx				\
	GEOMAlgo_ClsfSurf.ixx				\
	GEOMAlgo_ClsfSurf.jxx				\
	GEOMAlgo_CoupleOfShapes.hxx			\
	GEOMAlgo_CoupleOfShapes.ixx			\
	GEOMAlgo_CoupleOfShapes.jxx			\
	GEOMAlgo_DataMapIteratorOfDataMapOfOrientedShapeShape.hxx	\
	GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyInteger.hxx		\
	GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyShapeShape.hxx	\
	GEOMAlgo_DataMapIteratorOfDataMapOfRealListOfShape.hxx		\
	GEOMAlgo_DataMapIteratorOfDataMapOfShapeReal.hxx		\
	GEOMAlgo_DataMapIteratorOfDataMapOfShapeShapeSet.hxx		\
	GEOMAlgo_DataMapNodeOfDataMapOfOrientedShapeShape.hxx	\
	GEOMAlgo_DataMapNodeOfDataMapOfPassKeyInteger.hxx	\
	GEOMAlgo_DataMapNodeOfDataMapOfPassKeyShapeShape.hxx	\
	GEOMAlgo_DataMapNodeOfDataMapOfRealListOfShape.hxx	\
	GEOMAlgo_DataMapNodeOfDataMapOfShapeReal.hxx		\
	GEOMAlgo_DataMapNodeOfDataMapOfShapeShapeSet.hxx	\
	GEOMAlgo_DataMapOfOrientedShapeShape.hxx	\
	GEOMAlgo_DataMapOfPassKeyInteger.hxx		\
	GEOMAlgo_DataMapOfPassKeyShapeShape.hxx		\
	GEOMAlgo_DataMapOfRealListOfShape.hxx		\
	GEOMAlgo_DataMapOfShapeReal.hxx			\
	GEOMAlgo_DataMapOfShapeShapeSet.hxx		\
	GEOMAlgo_FinderShapeOn1.ixx		\
	GEOMAlgo_FinderShapeOn1.hxx		\
	GEOMAlgo_FinderShapeOn1.jxx		\
	GEOMAlgo_FinderShapeOn2.hxx		\
	GEOMAlgo_FinderShapeOn2.ixx		\
	GEOMAlgo_FinderShapeOn2.jxx		\
	GEOMAlgo_FinderShapeOn.hxx		\
	GEOMAlgo_FinderShapeOn.ixx		\
	GEOMAlgo_FinderShapeOn.jxx		\
	GEOMAlgo_FinderShapeOnQuad.hxx		\
	GEOMAlgo_GlueAnalyser.hxx		\
	GEOMAlgo_GlueAnalyser.ixx		\
	GEOMAlgo_GlueAnalyser.jxx		\
	GEOMAlgo_Gluer.hxx			\
	GEOMAlgo_Gluer.ixx			\
	GEOMAlgo_Gluer.jxx			\
	GEOMAlgo_Gluer1.hxx			\
	GEOMAlgo_Gluer1.ixx			\
	GEOMAlgo_Gluer1.jxx			\
	GEOMAlgo_GlueDetector.hxx		\
	GEOMAlgo_Gluer2.hxx			\
	GEOMAlgo_GluerAlgo.hxx			\
	GEOMAlgo_HAlgo.hxx			\
	GEOMAlgo_HAlgo.ixx			\
	GEOMAlgo_HAlgo.jxx			\
	GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape.hxx			\
	GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfPassKeyListOfShape.hxx		\
	GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfPassKeyShapeListOfShape.hxx	\
	GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeBox.hxx			\
	GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo.hxx			\
	GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeState.hxx			\
	GEOMAlgo_IndexedDataMapOfIntegerShape.hxx					\
	GEOMAlgo_IndexedDataMapOfPassKeyListOfShape.hxx					\
	GEOMAlgo_IndexedDataMapOfPassKeyShapeListOfShape.hxx				\
	GEOMAlgo_IndexedDataMapOfShapeBox.hxx						\
	GEOMAlgo_IndexedDataMapOfShapeShapeInfo.hxx					\
	GEOMAlgo_IndexedDataMapOfShapeState.hxx						\
	GEOMAlgo_KindOfBounds.hxx		\
	GEOMAlgo_KindOfClosed.hxx		\
	GEOMAlgo_KindOfName.hxx			\
	GEOMAlgo_KindOfShape.hxx		\
	GEOMAlgo_ListIteratorOfListOfCoupleOfShapes.hxx	\
	GEOMAlgo_ListIteratorOfListOfPnt.hxx		\
	GEOMAlgo_ListNodeOfListOfCoupleOfShapes.hxx	\
	GEOMAlgo_ListNodeOfListOfPnt.hxx		\
	GEOMAlgo_ListOfCoupleOfShapes.hxx		\
	GEOMAlgo_ListOfPnt.hxx				\
	GEOMAlgo_PassKey.hxx			\
	GEOMAlgo_PassKey.ixx			\
	GEOMAlgo_PassKey.jxx			\
	GEOMAlgo_PassKeyMapHasher.hxx		\
	GEOMAlgo_PassKeyMapHasher.ixx		\
	GEOMAlgo_PassKeyMapHasher.jxx		\
	GEOMAlgo_PassKeyShape.hxx		\
	GEOMAlgo_PassKeyShape.ixx		\
	GEOMAlgo_PassKeyShape.jxx		\
	GEOMAlgo_PassKeyShapeMapHasher.hxx	\
	GEOMAlgo_PassKeyShapeMapHasher.ixx	\
	GEOMAlgo_PassKeyShapeMapHasher.jxx	\
	GEOMAlgo_PWireEdgeSet.hxx		\
	GEOMAlgo_ShapeAlgo.hxx			\
	GEOMAlgo_ShapeAlgo.ixx			\
	GEOMAlgo_ShapeAlgo.jxx			\
	GEOMAlgo_ShapeInfo.hxx			\
	GEOMAlgo_ShapeInfo.ixx			\
	GEOMAlgo_ShapeInfo.jxx			\
	GEOMAlgo_ShapeInfoFiller.hxx		\
	GEOMAlgo_ShapeInfoFiller.ixx		\
	GEOMAlgo_ShapeInfoFiller.jxx		\
	GEOMAlgo_ShapeSet.hxx			\
	GEOMAlgo_ShapeSet.ixx			\
	GEOMAlgo_ShapeSet.jxx			\
	GEOMAlgo_ShapeSolid.hxx			\
	GEOMAlgo_ShapeSolid.ixx			\
	GEOMAlgo_ShapeSolid.jxx			\
	GEOMAlgo_ShellSolid.hxx			\
	GEOMAlgo_ShellSolid.ixx			\
	GEOMAlgo_ShellSolid.jxx			\
	GEOMAlgo_SolidSolid.hxx			\
	GEOMAlgo_SolidSolid.ixx			\
	GEOMAlgo_SolidSolid.jxx			\
	GEOMAlgo_Splitter.hxx			\
	GEOMAlgo_Splitter.ixx			\
	GEOMAlgo_Splitter.jxx			\
	GEOMAlgo_StateCollector.hxx		\
	GEOMAlgo_StateCollector.ixx		\
	GEOMAlgo_StateCollector.jxx		\
	GEOMAlgo_State.hxx			\
	GEOMAlgo_SurfaceTools.hxx		\
	GEOMAlgo_SurfaceTools.ixx		\
	GEOMAlgo_SurfaceTools.jxx		\
	GEOMAlgo_Tools3D.hxx			\
	GEOMAlgo_Tools3D.ixx			\
	GEOMAlgo_Tools3D.jxx			\
	GEOMAlgo_Tools.hxx			\
	GEOMAlgo_Tools.ixx			\
	GEOMAlgo_Tools.jxx			\
	GEOMAlgo_VertexSolid.hxx		\
	GEOMAlgo_VertexSolid.ixx		\
	GEOMAlgo_VertexSolid.jxx		\
	GEOMAlgo_WESCorrector.hxx		\
	GEOMAlgo_WESCorrector.ixx		\
	GEOMAlgo_WESCorrector.jxx		\
	GEOMAlgo_WESScaler.hxx			\
	GEOMAlgo_WESScaler.ixx			\
	GEOMAlgo_WESScaler.jxx			\
	GEOMAlgo_WireEdgeSet.hxx		\
	GEOMAlgo_WireEdgeSet.ixx		\
	GEOMAlgo_WireEdgeSet.jxx		\
	GEOMAlgo_WireSolid.hxx			\
	GEOMAlgo_WireSolid.ixx			\
	GEOMAlgo_WireSolid.jxx			\
	GEOMAlgo_WireSplitter.hxx		\
	GEOMAlgo_WireSplitter.ixx		\
	GEOMAlgo_WireSplitter.jxx		\
	Handle_GEOMAlgo_ClsfBox.hxx		\
	Handle_GEOMAlgo_Clsf.hxx		\
	Handle_GEOMAlgo_ClsfSolid.hxx		\
	Handle_GEOMAlgo_ClsfSurf.hxx		\
	Handle_GEOMAlgo_DataMapNodeOfDataMapOfOrientedShapeShape.hxx			\
	Handle_GEOMAlgo_DataMapNodeOfDataMapOfPassKeyInteger.hxx			\
	Handle_GEOMAlgo_DataMapNodeOfDataMapOfPassKeyShapeShape.hxx			\
	Handle_GEOMAlgo_DataMapNodeOfDataMapOfRealListOfShape.hxx			\
	Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapeReal.hxx				\
	Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapeShapeSet.hxx				\
	Handle_GEOMAlgo_HAlgo.hxx							\
	Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfIntegerShape.hxx		\
	Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfPassKeyListOfShape.hxx	\
	Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfPassKeyShapeListOfShape.hxx	\
	Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeBox.hxx		\
	Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo.hxx		\
	Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeState.hxx		\
	Handle_GEOMAlgo_ListNodeOfListOfCoupleOfShapes.hxx				\
	Handle_GEOMAlgo_ListNodeOfListOfPnt.hxx

libGEOMAlgo_la_CPPFLAGS =	\
	$(CAS_CPPFLAGS)		\
	$(KERNEL_CXXFLAGS)	\
	-I$(srcdir)/../NMTDS	\
	-I$(srcdir)/../NMTTools

libGEOMAlgo_la_LDFLAGS  =		\
	../NMTTools/libNMTTools.la	\
	$(STDLIB)			\
	$(CAS_LDPATH) -lTKBO -lTKShHealing -lTKBool -lTKMesh \
	$(KERNEL_LDFLAGS) -lSALOMELocalTrace

# extra distributed files
CDL_FILES =					\
	BlockFix.cdl				\
	BlockFix_BlockFixAPI.cdl		\
	BlockFix_CheckTool.cdl			\
	BlockFix_PeriodicSurfaceModifier.cdl	\
	BlockFix_SphereSpaceModifier.cdl	\
	BlockFix_UnionEdges.cdl			\
	BlockFix_UnionFaces.cdl			\
	GEOMAlgo.cdl				\
	GEOMAlgo_Algo.cdl			\
	GEOMAlgo_Builder.cdl			\
	GEOMAlgo_BuilderArea.cdl		\
	GEOMAlgo_BuilderFace.cdl		\
	GEOMAlgo_BuilderShape.cdl		\
	GEOMAlgo_BuilderSolid.cdl		\
	GEOMAlgo_BuilderTools.cdl		\
	GEOMAlgo_Clsf.cdl			\
	GEOMAlgo_ClsfBox.cdl			\
	GEOMAlgo_ClsfSolid.cdl			\
	GEOMAlgo_ClsfSurf.cdl			\
	GEOMAlgo_CoupleOfShapes.cdl		\
	GEOMAlgo_FinderShapeOn.cdl		\
	GEOMAlgo_FinderShapeOn1.cdl		\
	GEOMAlgo_FinderShapeOn2.cdl		\
	GEOMAlgo_GlueAnalyser.cdl		\
	GEOMAlgo_Gluer.cdl			\
	GEOMAlgo_Gluer1.cdl			\
	GEOMAlgo_HAlgo.cdl			\
	GEOMAlgo_PassKey.cdl			\
	GEOMAlgo_PassKeyMapHasher.cdl		\
	GEOMAlgo_PassKeyShape.cdl		\
	GEOMAlgo_PassKeyShapeMapHasher.cdl	\
	GEOMAlgo_ShapeAlgo.cdl			\
	GEOMAlgo_ShapeInfo.cdl			\
	GEOMAlgo_ShapeInfoFiller.cdl		\
	GEOMAlgo_ShapeSet.cdl			\
	GEOMAlgo_ShapeSolid.cdl			\
	GEOMAlgo_ShellSolid.cdl			\
	GEOMAlgo_SolidSolid.cdl			\
	GEOMAlgo_Splitter.cdl			\
	GEOMAlgo_StateCollector.cdl		\
	GEOMAlgo_SurfaceTools.cdl		\
	GEOMAlgo_Tools.cdl			\
	GEOMAlgo_Tools3D.cdl			\
	GEOMAlgo_VertexSolid.cdl		\
	GEOMAlgo_WESCorrector.cdl		\
	GEOMAlgo_WESScaler.cdl			\
	GEOMAlgo_WireEdgeSet.cdl		\
	GEOMAlgo_WireSolid.cdl			\
	GEOMAlgo_WireSplitter.cdl

EXTRA_DIST += $(CDL_FILES)
