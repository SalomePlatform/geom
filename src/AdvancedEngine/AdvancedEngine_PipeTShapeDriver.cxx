// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "AdvancedEngine_PipeTShapeDriver.hxx"
#include "AdvancedEngine_IPipeTShape.hxx"
#include "AdvancedEngine_Types.hxx"
#include "AdvancedEngine_IOperations.hxx"

#include "GEOMImpl_Block6Explorer.hxx"
#include "GEOM_Function.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOMUtils.hxx"
#include "GEOMAlgo_FinderShapeOn2.hxx"
#include "GEOMAlgo_ClsfBox.hxx"
#include "GEOMAlgo_ClsfSurf.hxx"
#include "GEOMAlgo_Splitter.hxx"

#include "Geom_CylindricalSurface.hxx"
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pln.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <StdFail_NotDone.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <TopoDS.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <vector>

// Undefine below macro to enable workaround about problem with wrong 
// tolerances of intersection curves in MakePipeTShape and MakeQuarterPipeTShape
// VSR 30/12/2014: macro enabled
#define FIX_CURVES_TOLERANCES

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& AdvancedEngine_PipeTShapeDriver::GetID()
{
  static Standard_GUID aGUID("1C3A0F3F-729D-4E83-8232-78E74FC5637C");
  return aGUID;
}

//=======================================================================
//function : AdvancedEngine_PipeTShapeDriver
//purpose  :
//=======================================================================
AdvancedEngine_PipeTShapeDriver::AdvancedEngine_PipeTShapeDriver()
{
}

//=======================================================================
//function : getShapesOnBoxIDs
  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
    * \param theBox - the box to check state of sub-shapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
//=======================================================================
Handle(TColStd_HSequenceOfInteger)
AdvancedEngine_PipeTShapeDriver::GetShapesOnBoxIDs(const TopoDS_Shape& aBox,
                    const TopoDS_Shape& aShape,
                    const Standard_Integer theShapeType,
                    GEOMAlgo_State theState) const
{
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs;

  // Check presence of triangulation, build if need
  if (!GEOMUtils::CheckTriangulation(aShape)) {
    StdFail_NotDone::Raise("Cannot build triangulation on the shape");
    return aSeqOfIDs;
  }

  // Call algo
  GEOMAlgo_FinderShapeOn2 aFinder;
  Standard_Real aTol = 0.0001; // default value

  Handle(GEOMAlgo_ClsfBox) aClsfBox = new GEOMAlgo_ClsfBox;
  aClsfBox->SetBox(aBox);

  aFinder.SetShape(aShape);
  aFinder.SetTolerance(aTol);
  aFinder.SetClsf(aClsfBox);
  aFinder.SetShapeType( (TopAbs_ShapeEnum)theShapeType );
  aFinder.SetState(theState);
  aFinder.Perform();

  // Interprete results
  Standard_Integer iErr = aFinder.ErrorStatus();
  // the detailed description of error codes is in GEOMAlgo_FinderShapeOn2.cxx
  if (iErr) {
    TCollection_AsciiString aMsg (" iErr : ");
    aMsg += TCollection_AsciiString(iErr);
    StdFail_NotDone::Raise(aMsg.ToCString());
    return aSeqOfIDs;
  }


  const TopTools_ListOfShape& listSS = aFinder.Shapes(); // the result

  if (listSS.Extent() < 1) {
    StdFail_NotDone::Raise(NOT_FOUND_ANY); // NPAL18017
    return aSeqOfIDs;
  }

  // Fill sequence of object IDs
  aSeqOfIDs = new TColStd_HSequenceOfInteger;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);

  TopTools_ListIteratorOfListOfShape itSub (listSS);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    int id = anIndices.FindIndex(itSub.Value());
//    std::cerr << "Shape with ID " << id << " found" << std::endl;
    aSeqOfIDs->Append(id);
  }

  return aSeqOfIDs;
}

//=======================================================================
//function : GetShapesOnSurfaceIDs
  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
    * \param theSurface - the surface to check state of sub-shapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
//=======================================================================
Handle(TColStd_HSequenceOfInteger)
  AdvancedEngine_PipeTShapeDriver::GetShapesOnSurfaceIDs(const Handle(Geom_Surface)& theSurface,
                                                    const TopoDS_Shape&         theShape,
                                                    TopAbs_ShapeEnum            theShapeType,
                                                    GEOMAlgo_State              theState) const
{
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs;

  // Check presence of triangulation, build if need
  if (!GEOMUtils::CheckTriangulation(theShape)) {
    StdFail_NotDone::Raise("Cannot build triangulation on the shape");
    return aSeqOfIDs;
  }

  // Call algo
  GEOMAlgo_FinderShapeOn2   aFinder;
  Handle(GEOMAlgo_ClsfSurf) aClsfSurf = new GEOMAlgo_ClsfSurf;
  Standard_Real             aTol      = 1e-6;

  aClsfSurf->SetSurface(theSurface);
  aFinder.SetShape(theShape);
  aFinder.SetTolerance(aTol);
  aFinder.SetClsf(aClsfSurf);
  aFinder.SetShapeType(theShapeType);
  aFinder.SetState(theState);

  // Sets the minimal number of inner points for the faces that do not have own
  // inner points at all (for e.g. rectangular planar faces have just 2 triangles).
  // Default value=3
  aFinder.SetNbPntsMin(3);
  // Sets the maximal number of inner points for edges or faces.
  // It is usefull for the cases when this number is very big (e.g =2000) to improve
  // the performance. If this value =0, all inner points will be taken into account.
  // Default value=0
  aFinder.SetNbPntsMax(0);

  aFinder.Perform();

  // Interprete results
  Standard_Integer iErr = aFinder.ErrorStatus();
  // the detailed description of error codes is in GEOMAlgo_FinderShapeOn2.cxx
  if (iErr) {
//    MESSAGE(" iErr : " << iErr);
    TCollection_AsciiString aMsg (" iErr : ");
    aMsg += TCollection_AsciiString(iErr);
    StdFail_NotDone::Raise(aMsg.ToCString());
    return aSeqOfIDs;
  }
//  Standard_Integer iWrn = aFinder.WarningStatus();
  // the detailed description of warning codes is in GEOMAlgo_FinderShapeOn2.cxx
//  if (iWrn) {
//    MESSAGE(" *** iWrn : " << iWrn);
//  }

  const TopTools_ListOfShape& listSS = aFinder.Shapes(); // the result

  if (listSS.Extent() < 1) {
    //StdFail_NotDone::Raise("Not a single sub-shape of the requested type found on the given surface");
    StdFail_NotDone::Raise(NOT_FOUND_ANY); // NPAL18017
    return aSeqOfIDs;
  }

  // Fill sequence of object IDs
  aSeqOfIDs = new TColStd_HSequenceOfInteger;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(theShape, anIndices);

  TopTools_ListIteratorOfListOfShape itSub (listSS);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    int id = anIndices.FindIndex(itSub.Value());
    aSeqOfIDs->Append(id);
  }

  return aSeqOfIDs;
}

//=======================================================================
//function : GetCommonShapesOnCylinders
//purpose  : return the common shapes between 2 cylindrical surfaces
//           along OX and OZ
//=======================================================================
void AdvancedEngine_PipeTShapeDriver::GetCommonShapesOnCylinders(const TopoDS_Shape& theShape,
                                                           TopAbs_ShapeEnum theShapeType,
                                                           double r1,
                                                           double r2,
                                                           Handle(TopTools_HSequenceOfShape)& commonShapes) const
{
  gp_Pnt aP0 (0, 0, 0);
  gp_Vec aVX = gp::DX(), aVZ = gp::DZ();
  gp_Ax3 anAxis1 (aP0, aVX, aVZ), anAxis2 (aP0, aVZ, aVX);

  TopTools_IndexedMapOfShape aMapOfShapes;
  aMapOfShapes.Clear();
  TopExp::MapShapes(theShape, aMapOfShapes);

  commonShapes->Clear();

  int myID;
  bool found = false;

  // Create a cylinder surface
  Handle(Geom_Surface) aC1Ext = new Geom_CylindricalSurface(anAxis1, r1);
  if ( aC1Ext.IsNull() )
    StdFail_NotDone::Raise("Couldn't build main cylindrical surface");
  // Find object IDs
  Handle(TColStd_HSequenceOfInteger) aSeqExt1 = GetShapesOnSurfaceIDs( aC1Ext, theShape, theShapeType, GEOMAlgo_ST_ON );
  // Create a cylinder surface
  Handle(Geom_Surface) aC2Ext = new Geom_CylindricalSurface(anAxis2, r2);
  if ( aC2Ext.IsNull() )
    StdFail_NotDone::Raise("Couldn't build incident cylindrical surface");
  // Find object IDs
  Handle(TColStd_HSequenceOfInteger) aSeqExt2 = GetShapesOnSurfaceIDs( aC2Ext, theShape, theShapeType, GEOMAlgo_ST_ON );
  // # Recherche (dans le quart de Te) de l'arete d'intersection des 2 cylindres
  // # Search in theShape for common shape of type theShapeType on the intersection of 2 pipes
  found = false;
  for (int i=1; i<=aSeqExt2->Length();i++) {
//    std::cerr << "aSeqExt2->Value(i): " << aSeqExt2->Value(i) << std::endl;
    for (int j=1; j<=aSeqExt1->Length();j++) {
//      std::cerr << "aSeqExt1->Value(j): " << aSeqExt1->Value(j) << std::endl;
      if (aSeqExt1->Value(j) == aSeqExt2->Value(i)) {
        myID = aSeqExt1->Value(j);
        commonShapes->Append(aMapOfShapes.FindKey(myID));
        found = true;
      }
    }
  }
  if (!found)
    StdFail_NotDone::Raise("Common shapes couldn't be found");
}

//=======================================================================
//function : MakePipeTShape
//purpose  :
//=======================================================================
TopoDS_Shape AdvancedEngine_PipeTShapeDriver::MakePipeTShape (const double r1, const double w1, const double l1,
                                                              const double r2, const double w2, const double l2) const
{
  double r1Ext = r1 + w1;
  double r2Ext = r2 + w2;

  gp_Pnt aP0 (0, 0, 0);
  gp_Pnt aP1 (-l1, 0, 0);
  gp_Vec aVX = gp::DX(), aVY = gp::DY(), aVZ = gp::DZ();
  gp_Ax2 anAxes1 (aP1, aVX, aVZ);
  gp_Ax2 anAxes2 (aP0, aVZ, aVX);

  // Build the initial pipes
  BRepPrimAPI_MakeCylinder C1Int (anAxes1, r1, Abs(2 * l1));
  BRepPrimAPI_MakeCylinder C1Ext (anAxes1, r1Ext, Abs(2 * l1));
  BRepPrimAPI_MakeCylinder C2Int (anAxes2, r2, Abs(l2));
  BRepPrimAPI_MakeCylinder C2Ext (anAxes2, r2Ext, Abs(l2));
  C1Int.Build();
  C1Ext.Build();
  C2Int.Build();
  C2Ext.Build();
  if (!C1Int.IsDone() || !C1Ext.IsDone() || !C2Int.IsDone() || !C2Ext.IsDone()) {
    StdFail_NotDone::Raise("Cannot build cylinders");
  }

  // Fuse the 2 pipes
  BRepAlgoAPI_Fuse fuse1 (C1Ext.Shape(), C2Ext.Shape());
  if (!fuse1.IsDone()) {
    StdFail_NotDone::Raise("Cannot fuse cylinders");
  }

  // Remove small radius main pipe
  BRepAlgoAPI_Cut cut1 (fuse1.Shape(), C1Int.Shape());
  if (!cut1.IsDone()) {
    StdFail_NotDone::Raise("Coudn't cut cylinders");
  }

  // Remove small radius incident pipe => Te
  BRepAlgoAPI_Cut Te (cut1.Shape(), C2Int.Shape());
  if (!Te.IsDone()) {
    StdFail_NotDone::Raise("Coudn't cut cylinders");
  }

  TopoDS_Shape aShape = Te.Shape();

  // VSR: 30/12/2014: temporary workaround about intersection curves problem
#ifdef FIX_CURVES_TOLERANCES
  GEOMUtils::FixShapeCurves(aShape);
#endif

  return aShape;
}

//=======================================================================
//function : MakeQuarterPipeTShape
//purpose  :
//=======================================================================
TopoDS_Shape AdvancedEngine_PipeTShapeDriver::MakeQuarterPipeTShape (const double r1, const double w1, const double l1,
                                                                     const double r2, const double w2, const double l2) const
{
  TopoDS_Shape Te = MakePipeTShape(r1, w1, l1, r2, w2, l2);
  if (Te.IsNull())
    StdFail_NotDone::Raise("Couldn't build Pipe TShape");

  // Get a quarter of shape => Te2
  double r1Ext = r1 + w1;
  BRepPrimAPI_MakeBox box1 (gp_Pnt(0, -2*r1Ext, -2*r1Ext), gp_Pnt( Abs(2 * l1), 2*r1Ext, Abs(2*l2)));
  BRepPrimAPI_MakeBox box2 (gp_Pnt(0,  2*r1Ext, -2*r1Ext), gp_Pnt(-Abs(2 * l1), 0,       Abs(2*l2)));
  box1.Build();
  box2.Build();
  if (!box1.IsDone() || !box2.IsDone()) {
    StdFail_NotDone::Raise("Couldn't build boxes");
  }
  BRepAlgoAPI_Cut cut3 (Te, box1.Shape());
  if (!cut3.IsDone()) {
    StdFail_NotDone::Raise("Couldn't cut Pipe Tshape with box");
  }
  BRepAlgoAPI_Cut Te4 (cut3.Shape(), box2.Shape());
  if (!Te4.IsDone()) {
    StdFail_NotDone::Raise("Couldn't cut Pipe Tshape with box");
  }

  TopoDS_Shape aShape = Te4.Shape();

  // VSR: 30/12/2014: temporary workaround about intersection curves problem
#ifdef FIX_CURVES_TOLERANCES
  GEOMUtils::FixShapeCurves(aShape);
#endif

  return Te4.Shape();
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer AdvancedEngine_PipeTShapeDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  AdvancedEngine_IPipeTShape aData (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape, Te4, Te4Part;
  //TopoDS_Edge arete_intersect_int;
  //Handle(TopTools_HSequenceOfShape) edges_e = new TopTools_HSequenceOfShape;
  Handle(TColStd_HSequenceOfInteger) edges_e;
  //Handle(TopTools_HSequenceOfShape) edges_i = new TopTools_HSequenceOfShape;
  //gp_Pnt aP0 (0, 0, 0);
  //gp_Vec aVX = gp::DX(), aVY = gp::DY(), aVZ = gp::DZ();
  bool hexMesh = (bool) aData.GetHexMesh();

  // Useful values
  //double aSize = 2*(aData.GetL1() + aData.GetL2());
  double epsilon = Precision::Approximation();
  double aR1Ext = aData.GetR1() + aData.GetW1();
  double aR2Ext = aData.GetR2() + aData.GetW2();

  if (aData.GetR2() > aData.GetR1() + epsilon) {
    StdFail_NotDone::Raise("TShape cannot be computed if R2 > R1");
  }

  if (aR2Ext > aR1Ext + epsilon) {
    StdFail_NotDone::Raise("TShape cannot be computed if R2+W2 > R1+W1");
  }

  // external radius are equal
  if (fabs(aR2Ext - aR1Ext) < epsilon) {
    if (aType == TSHAPE_CHAMFER)
      StdFail_NotDone::Raise("TShape with chamfer cannot be computed if R2+W2 = R1+W1");
    if (aType == TSHAPE_FILLET)
      StdFail_NotDone::Raise("TShape with fillet cannot be computed if R2+W2 = R1+W1");
    // internal radius are different => not possible
    if (fabs(aData.GetR2() - aData.GetR1()) > epsilon) {
      StdFail_NotDone::Raise("TShape cannot be computed if R2+W2 = R1+W1 and R2 != R1");
    }
  }

  if (aR1Ext >= aData.GetL2() + epsilon) {
    StdFail_NotDone::Raise("TShape cannot be computed if R1+W1 >= L2");
  }
  if (aR2Ext >= aData.GetL1() + epsilon) {
    StdFail_NotDone::Raise("TShape cannot be computed if R2+W2 >= L1");
  }

  if (aType == TSHAPE_CHAMFER) {
    if (aData.GetH() >= (aData.GetL2() - aR1Ext + epsilon)) {
      StdFail_NotDone::Raise("TShape cannot be computed: height of chamfer is too high");
    }

    if (aData.GetW() >= (aData.GetL1() - aR2Ext + epsilon))
      StdFail_NotDone::Raise("TShape cannot be computed: width of chamfer is too high");
  }

  if (aType == TSHAPE_FILLET) {
    if (aData.GetRF() >= (aData.GetL2() - aR1Ext + epsilon) ||
        aData.GetRF() >= (aData.GetL1() - aR2Ext + epsilon))
      StdFail_NotDone::Raise("TShape cannot be computed: radius of fillet is too high");
  }

  if (hexMesh) {
    // Create a quarter of a basic T-Shape pipe
    //std::cerr << "Create a quarter of a basic T-Shape pipe" << std::endl;
    Te4 = MakeQuarterPipeTShape(aData.GetR1(), aData.GetW1(), aData.GetL1(),
                                aData.GetR2(), aData.GetW2(), aData.GetL2());
  }
  else {
    // No need to cut pipe t-shape
    //std::cerr << "Create a basic T-Shape pipe" << std::endl;
    Te4 = MakePipeTShape(aData.GetR1(), aData.GetW1(), aData.GetL1(),
                         aData.GetR2(), aData.GetW2(), aData.GetL2());
  }
  aShape = Te4;
/*
  if (aType == TSHAPE_BASIC) {
      aShape = Te4;
//       aShape = MakeQuarterPipeTShape(aData.GetR1(), aData.GetW1(), aData.GetL1(),
//       aData.GetR2(), aData.GetW2(), aData.GetL2());
  }
  else if (aType == TSHAPE_CHAMFER) {
    // TShape with chamfer
//     BRep_Builder BB;
//     TopoDS_Compound CC;
//     BB.MakeCompound(CC);
    // Create chamfer on the edges edges_e
    BRepFilletAPI_MakeChamfer chamfer (Te4);
    TopTools_IndexedMapOfShape anEdgesIndices;
    TopExp::MapShapes(Te4, anEdgesIndices);

    TopoDS_Shape theBox;
    if (hexMesh) {
      BRepPrimAPI_MakeBox aBox (gp_Pnt(0,0,0),gp_Pnt(-aR2Ext, -aR2Ext, aR1Ext));
      aBox.Build();
      if (!aBox.IsDone()) {
        StdFail_NotDone::Raise("Couldn't build box");
      }
      theBox = aBox.Shape();
    }
    else {
      BRepPrimAPI_MakeBox aBox (gp_Pnt(aR2Ext,aR2Ext,0),gp_Pnt(-aR2Ext, -aR2Ext, aR1Ext));
      aBox.Build();
      if (!aBox.IsDone()) {
        StdFail_NotDone::Raise("Couldn't build box");
      }
      theBox = aBox.Shape();
    }
    Handle(TColStd_HSequenceOfInteger) edges_e = new TColStd_HSequenceOfInteger;
    edges_e = GetShapesOnBoxIDs(theBox, Te4, TopAbs_EDGE, GEOMAlgo_ST_IN);
    if (edges_e.IsNull() || edges_e->Length() == 0) {
      StdFail_NotDone::Raise("Common edges not found");
    }

    TopTools_IndexedDataMapOfShapeListOfShape M;
    GEOMImpl_Block6Explorer::MapShapesAndAncestors(Te4, TopAbs_EDGE, TopAbs_FACE, M);
//     std::cerr << "Number of IDs: " << edges_e->Length() << std::endl;
    int nbEdgesInChamfer = 0;
    for (int i=1;i<=edges_e->Length();i++) {
//       std::cerr << "Get Edge with ID #" << i << std::endl;
      int theId = edges_e->Value(i);
//       std::cerr << "ID #" << i << "= " << theId << std::endl;
//       std::cerr << "Search for edge in shape" << std::endl;
      TopoDS_Edge theEdge = TopoDS::Edge(anEdgesIndices.FindKey(theId));
//       std::cerr << "Found" << std::endl;
//       std::cerr << "Keep only edges with a vertex on (x, x, re1)" << std::endl;
      TopExp_Explorer ExVertices;
      for (ExVertices.Init(theEdge,TopAbs_VERTEX); ExVertices.More(); ExVertices.Next()) {
        gp_Pnt aPt = BRep_Tool::Pnt(TopoDS::Vertex(ExVertices.Current()));
        if (aPt.Z() - aR1Ext <= epsilon) {
//           std::cerr << "aPt.Z() = aR1Ext => keep this edge" << std::endl;
          nbEdgesInChamfer ++;
          const TopTools_ListOfShape& aFL = M.FindFromKey(theEdge);
          TopoDS_Face F = TopoDS::Face( aFL.First() );
          if (hexMesh)
            chamfer.Add(aData.GetH(), aData.GetW(), theEdge, F);
          else
            chamfer.Add(aData.GetW(), aData.GetH(), theEdge, F);
          break;
        }
      }
//       std::cerr << "Test if hexMesh: ";
      if (hexMesh && nbEdgesInChamfer == 1) {
//         std::cerr << "Yes => stop after 1 edge" << std::endl;
        break;
      }
//       std::cerr << "No => continue for other edges" << std::endl;
  //  BB.Add(CC, edges_e->Value(i));
  //  const TopTools_ListOfShape& aFL = M.FindFromKey(TopoDS::Edge(edges_e->Value(i)));
  //  chamfer.Add(aData.GetW(), aData.GetH(), TopoDS::Edge(edges_e->Value(i)), F);
    }
//     std::cerr << "Build chamfer with " << nbEdgesInChamfer << " edges" << std::endl;
//     }
    chamfer.Build();
    if (!chamfer.IsDone()) {
      StdFail_NotDone::Raise("Chamfer can not be computed on the given shape with the given parameters");
    }

//     BB.Add(CC, chamfer.Shape());

//     aShape = CC;
    aShape = chamfer.Shape();
  }
  else if (aType == TSHAPE_FILLET) {
    // TShape with fillet
    // Create fillet on the edge arete_intersect_ext
    BRepFilletAPI_MakeFillet fill (Te4);

    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(Te4, anIndices);

    TopoDS_Shape theBox;
    if (hexMesh) {
      BRepPrimAPI_MakeBox aBox (gp_Pnt(0,0,0),gp_Pnt(-aR2Ext, -aR2Ext, aR1Ext));
      aBox.Build();
      if (!aBox.IsDone()) {
        StdFail_NotDone::Raise("Couldn't build box");
      }
      theBox = aBox.Shape();
    }
    else {
      BRepPrimAPI_MakeBox aBox (gp_Pnt(aR2Ext,aR2Ext,0),gp_Pnt(-aR2Ext, -aR2Ext, aR1Ext));
      aBox.Build();
      if (!aBox.IsDone()) {
        StdFail_NotDone::Raise("Couldn't build box");
      }
      theBox = aBox.Shape();
    }
    Handle(TColStd_HSequenceOfInteger) edges_e = new TColStd_HSequenceOfInteger;
    edges_e = GetShapesOnBoxIDs(theBox, Te4, TopAbs_EDGE, GEOMAlgo_ST_IN);
    if (edges_e.IsNull() || edges_e->Length() == 0) {
      StdFail_NotDone::Raise("Common edges not found");
    }

//     fill.Add(TopoDS::Edge(edges_e->Value(1)));
//     if (!hexMesh) {
    for (int i=1;i<=edges_e->Length();i++) {
      if (hexMesh && (i > 1))
        break;
      TopoDS_Edge theEdge = TopoDS::Edge(anIndices.FindKey(edges_e->Value(i)));
      fill.Add(theEdge);
//             fill.Add(TopoDS::Edge(edges_e->Value(i)));
    }
//     }
    fill.SetRadius(aData.GetRF(), 1, 1);
    fill.Build();
    if (!fill.IsDone()) {
      StdFail_NotDone::Raise("Fillet can't be computed on the given shape with the given radius");
    }

    aShape = fill.Shape();
  }
  else {
    // other construction modes here
  }
*/
  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool AdvancedEngine_PipeTShapeDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  AdvancedEngine_IPipeTShape aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "PIPETSHAPE";

  switch ( aType ) {
  case TSHAPE_BASIC:
    AddParam( theParams, "Main radius", aCI.GetR1() );
    AddParam( theParams, "Main width", aCI.GetW1() );
    AddParam( theParams, "Main half-length", aCI.GetL1() );
    AddParam( theParams, "Incident pipe radius", aCI.GetR2() );
    AddParam( theParams, "Incident pipe width", aCI.GetW2() );
    AddParam( theParams, "Incident pipe half-length", aCI.GetL2() );
    AddParam( theParams, "For hex mesh", aCI.GetHexMesh() );
    break;
  case TSHAPE_CHAMFER:
    AddParam( theParams, "Main radius", aCI.GetR1() );
    AddParam( theParams, "Main width", aCI.GetW1() );
    AddParam( theParams, "Main half-length", aCI.GetL1() );
    AddParam( theParams, "Incident pipe radius", aCI.GetR2() );
    AddParam( theParams, "Incident pipe width", aCI.GetW2() );
    AddParam( theParams, "Incident pipe half-length", aCI.GetL2() );
    AddParam( theParams, "Chamfer height", aCI.GetH() );
    AddParam( theParams, "Chamfer width", aCI.GetW() );
    AddParam( theParams, "For hex mesh", aCI.GetHexMesh() );
    break;
  case TSHAPE_FILLET:
    AddParam( theParams, "Main radius", aCI.GetR1() );
    AddParam( theParams, "Main width", aCI.GetW1() );
    AddParam( theParams, "Main half-length", aCI.GetL1() );
    AddParam( theParams, "Incident pipe radius", aCI.GetR2() );
    AddParam( theParams, "Incident pipe width", aCI.GetW2() );
    AddParam( theParams, "Incident pipe half-length", aCI.GetL2() );
    AddParam( theParams, "Fillet radius", aCI.GetRF() );
    AddParam( theParams, "For hex mesh", aCI.GetHexMesh() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (AdvancedEngine_PipeTShapeDriver,GEOM_BaseDriver);
