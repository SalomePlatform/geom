// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include <Standard_Stream.hxx>

#include <GEOMUtils.hxx>

#include <Basics_OCCTVersion.hxx>

#include <utilities.h>
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>

// OCCT Includes
#include <BRepMesh_IncrementalMesh.hxx>

#include <BRepExtrema_DistShapeShape.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>

#include <BRepClass3d_SolidClassifier.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>

#include <Bnd_Box.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_Array1OfShape.hxx>

#include <Geom_Circle.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

#include <GeomLProp_CLProps.hxx>
#include <GeomLProp_SLProps.hxx>

#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>

#include <TColStd_Array1OfReal.hxx>

#include <gp_Pln.hxx>
#include <gp_Lin.hxx>

#include <ShapeAnalysis.hxx>
#include <ShapeFix_Shape.hxx>

#include <ProjLib.hxx>
#include <ElSLib.hxx>

#include <vector>

#include <Standard_Failure.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

#define STD_SORT_ALGO 1

//=======================================================================
//function : GetPosition
//purpose  :
//=======================================================================
gp_Ax3 GEOMUtils::GetPosition (const TopoDS_Shape& theShape)
{
  gp_Ax3 aResult;

  if (theShape.IsNull())
    return aResult;

  // Axes
  aResult.Transform(theShape.Location().Transformation());
  if (theShape.ShapeType() == TopAbs_FACE) {
    Handle(Geom_Surface) aGS = BRep_Tool::Surface(TopoDS::Face(theShape));
    if (!aGS.IsNull() && aGS->IsKind(STANDARD_TYPE(Geom_Plane))) {
      Handle(Geom_Plane) aGPlane = Handle(Geom_Plane)::DownCast(aGS);
      gp_Pln aPln = aGPlane->Pln();
      aResult = aPln.Position();
      // In case of reverse orinetation of the face invert the plane normal
      // (the face's normal does not mathc the plane's normal in this case)
      if(theShape.Orientation() == TopAbs_REVERSED)
      {
        gp_Dir Vx =  aResult.XDirection();
        gp_Dir N  =  aResult.Direction().Mirrored(Vx);
        gp_Pnt P  =  aResult.Location();
        aResult = gp_Ax3(P, N, Vx);
      }
    }
  }

  // Origin
  gp_Pnt aPnt;

  TopAbs_ShapeEnum aShType = theShape.ShapeType();

  if (aShType == TopAbs_VERTEX) {
    aPnt = BRep_Tool::Pnt(TopoDS::Vertex(theShape));
  }
  else {
    if (aShType == TopAbs_COMPOUND) {
      aShType = GetTypeOfSimplePart(theShape);
    }

    GProp_GProps aSystem;
    if (aShType == TopAbs_EDGE || aShType == TopAbs_WIRE)
      BRepGProp::LinearProperties(theShape, aSystem);
    else if (aShType == TopAbs_FACE || aShType == TopAbs_SHELL)
      BRepGProp::SurfaceProperties(theShape, aSystem);
    else
      BRepGProp::VolumeProperties(theShape, aSystem);

    aPnt = aSystem.CentreOfMass();
  }

  aResult.SetLocation(aPnt);

  return aResult;
}

//=======================================================================
//function : GetVector
//purpose  :
//=======================================================================
gp_Vec GEOMUtils::GetVector (const TopoDS_Shape& theShape,
                             Standard_Boolean doConsiderOrientation)
{
  if (theShape.IsNull())
    Standard_NullObject::Raise("Null shape is given for a vector");

  if (theShape.ShapeType() != TopAbs_EDGE)
    Standard_TypeMismatch::Raise("Invalid shape is given, must be a vector or an edge");

  TopoDS_Edge anE = TopoDS::Edge(theShape);

  TopoDS_Vertex V1, V2;
  TopExp::Vertices(anE, V1, V2, doConsiderOrientation);

  if (V1.IsNull() || V2.IsNull())
    Standard_NullObject::Raise("Invalid edge is given, it must have two points");

  gp_Vec aV (BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
  if (aV.Magnitude() < gp::Resolution()) {
    Standard_ConstructionError::Raise("Vector of zero length is given");
  }

  return aV;
}

//=======================================================================
//function : ShapeToDouble
//purpose  : used by CompareShapes::operator()
//=======================================================================
std::pair<double, double> ShapeToDouble (const TopoDS_Shape& S, bool isOldSorting)
{
  // Computing of CentreOfMass
  gp_Pnt GPoint;
  double Len;

  if (S.ShapeType() == TopAbs_VERTEX) {
    GPoint = BRep_Tool::Pnt(TopoDS::Vertex(S));
    Len = (double)S.Orientation();
  }
  else {
    GProp_GProps GPr;
    // BEGIN: fix for Mantis issue 0020842
    if (isOldSorting) {
      BRepGProp::LinearProperties(S, GPr);
    }
    else {
      if (S.ShapeType() == TopAbs_EDGE || S.ShapeType() == TopAbs_WIRE) {
        BRepGProp::LinearProperties(S, GPr);
      }
      else if (S.ShapeType() == TopAbs_FACE || S.ShapeType() == TopAbs_SHELL) {
        BRepGProp::SurfaceProperties(S, GPr);
      }
      else {
        BRepGProp::VolumeProperties(S, GPr);
      }
    }
    // END: fix for Mantis issue 0020842
    GPoint = GPr.CentreOfMass();
    Len = GPr.Mass();
  }

  double dMidXYZ = GPoint.X() * 999.0 + GPoint.Y() * 99.0 + GPoint.Z() * 0.9;
  return std::make_pair(dMidXYZ, Len);
}

//=======================================================================
//function : CompareShapes::operator()
//purpose  : used by std::sort(), called from SortShapes()
//=======================================================================
bool GEOMUtils::CompareShapes::operator() (const TopoDS_Shape& theShape1,
                                           const TopoDS_Shape& theShape2)
{
  if (!myMap.IsBound(theShape1)) {
    myMap.Bind(theShape1, ShapeToDouble(theShape1, myIsOldSorting));
  }

  if (!myMap.IsBound(theShape2)) {
    myMap.Bind(theShape2, ShapeToDouble(theShape2, myIsOldSorting));
  }

  std::pair<double, double> val1 = myMap.Find(theShape1);
  std::pair<double, double> val2 = myMap.Find(theShape2);

  double tol = Precision::Confusion();
  bool exchange = Standard_False;

  double dMidXYZ = val1.first - val2.first;
  if (dMidXYZ >= tol) {
    exchange = Standard_True;
  }
  else if (Abs(dMidXYZ) < tol) {
    double dLength = val1.second - val2.second;
    if (dLength >= tol) {
      exchange = Standard_True;
    }
    else if (Abs(dLength) < tol && theShape1.ShapeType() <= TopAbs_FACE) {
      // PAL17233
      // equal values possible on shapes such as two halves of a sphere and
      // a membrane inside the sphere
      Bnd_Box box1,box2;
      BRepBndLib::Add(theShape1, box1);
      if (!box1.IsVoid()) {
        BRepBndLib::Add(theShape2, box2);
        Standard_Real dSquareExtent = box1.SquareExtent() - box2.SquareExtent();
        if (dSquareExtent >= tol) {
          exchange = Standard_True;
        }
        else if (Abs(dSquareExtent) < tol) {
          Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, val1, val2;
          box1.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
          val1 = (aXmin+aXmax)*999.0 + (aYmin+aYmax)*99.0 + (aZmin+aZmax)*0.9;
          box2.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
          val2 = (aXmin+aXmax)*999.0 + (aYmin+aYmax)*99.0 + (aZmin+aZmax)*0.9;
          if ((val1 - val2) >= tol) {
            exchange = Standard_True;
          }
        }
      }
    }
  }

  //return val1 < val2;
  return !exchange;
}

//=======================================================================
//function : SortShapes
//purpose  :
//=======================================================================
void GEOMUtils::SortShapes (TopTools_ListOfShape& SL,
                            const Standard_Boolean isOldSorting)
{
#ifdef STD_SORT_ALGO
  std::vector<TopoDS_Shape> aShapesVec;
  aShapesVec.reserve(SL.Extent());

  TopTools_ListIteratorOfListOfShape it (SL);
  for (; it.More(); it.Next()) {
    aShapesVec.push_back(it.Value());
  }
  SL.Clear();

  CompareShapes shComp (isOldSorting);
  std::stable_sort(aShapesVec.begin(), aShapesVec.end(), shComp);
  //std::sort(aShapesVec.begin(), aShapesVec.end(), shComp);

  std::vector<TopoDS_Shape>::const_iterator anIter = aShapesVec.begin();
  for (; anIter != aShapesVec.end(); ++anIter) {
    SL.Append(*anIter);
  }
#else
  // old implementation
  Standard_Integer MaxShapes = SL.Extent();
  TopTools_Array1OfShape  aShapes (1,MaxShapes);
  TColStd_Array1OfInteger OrderInd(1,MaxShapes);
  TColStd_Array1OfReal    MidXYZ  (1,MaxShapes); //X,Y,Z;
  TColStd_Array1OfReal    Length  (1,MaxShapes); //X,Y,Z;

  // Computing of CentreOfMass
  Standard_Integer Index;
  GProp_GProps GPr;
  gp_Pnt GPoint;
  TopTools_ListIteratorOfListOfShape it(SL);
  for (Index=1;  it.More();  Index++)
  {
    TopoDS_Shape S = it.Value();
    SL.Remove( it ); // == it.Next()
    aShapes(Index) = S;
    OrderInd.SetValue (Index, Index);
    if (S.ShapeType() == TopAbs_VERTEX) {
      GPoint = BRep_Tool::Pnt( TopoDS::Vertex( S ));
      Length.SetValue( Index, (Standard_Real) S.Orientation());
    }
    else {
      // BEGIN: fix for Mantis issue 0020842
      if (isOldSorting) {
        BRepGProp::LinearProperties (S, GPr);
      }
      else {
        if (S.ShapeType() == TopAbs_EDGE || S.ShapeType() == TopAbs_WIRE) {
          BRepGProp::LinearProperties (S, GPr);
        }
        else if (S.ShapeType() == TopAbs_FACE || S.ShapeType() == TopAbs_SHELL) {
          BRepGProp::SurfaceProperties(S, GPr);
        }
        else {
          BRepGProp::VolumeProperties(S, GPr);
        }
      }
      // END: fix for Mantis issue 0020842
      GPoint = GPr.CentreOfMass();
      Length.SetValue(Index, GPr.Mass());
    }
    MidXYZ.SetValue(Index, GPoint.X()*999.0 + GPoint.Y()*99.0 + GPoint.Z()*0.9);
    //cout << Index << " L: " << Length(Index) << "CG: " << MidXYZ(Index) << endl;
  }

  // Sorting
  Standard_Integer aTemp;
  Standard_Boolean exchange, Sort = Standard_True;
  Standard_Real    tol = Precision::Confusion();
  while (Sort)
  {
    Sort = Standard_False;
    for (Index=1; Index < MaxShapes; Index++)
    {
      exchange = Standard_False;
      Standard_Real dMidXYZ = MidXYZ(OrderInd(Index)) - MidXYZ(OrderInd(Index+1));
      Standard_Real dLength = Length(OrderInd(Index)) - Length(OrderInd(Index+1));
      if ( dMidXYZ >= tol ) {
//         cout << "MidXYZ: " << MidXYZ(OrderInd(Index))<< " > " <<MidXYZ(OrderInd(Index+1))
//              << " d: " << dMidXYZ << endl;
        exchange = Standard_True;
      }
      else if ( Abs(dMidXYZ) < tol && dLength >= tol ) {
//         cout << "Length: " << Length(OrderInd(Index))<< " > " <<Length(OrderInd(Index+1))
//              << " d: " << dLength << endl;
        exchange = Standard_True;
      }
      else if ( Abs(dMidXYZ) < tol && Abs(dLength) < tol &&
                aShapes(OrderInd(Index)).ShapeType() <= TopAbs_FACE) {
        // PAL17233
        // equal values possible on shapes such as two halves of a sphere and
        // a membrane inside the sphere
        Bnd_Box box1,box2;
        BRepBndLib::Add( aShapes( OrderInd(Index) ), box1 );
        if ( box1.IsVoid() ) continue;
        BRepBndLib::Add( aShapes( OrderInd(Index+1) ), box2 );
        Standard_Real dSquareExtent = box1.SquareExtent() - box2.SquareExtent();
        if ( dSquareExtent >= tol ) {
//           cout << "SquareExtent: " << box1.SquareExtent()<<" > "<<box2.SquareExtent() << endl;
          exchange = Standard_True;
        }
        else if ( Abs(dSquareExtent) < tol ) {
          Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, val1, val2;
          box1.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
          val1 = (aXmin+aXmax)*999 + (aYmin+aYmax)*99 + (aZmin+aZmax)*0.9;
          box2.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
          val2 = (aXmin+aXmax)*999 + (aYmin+aYmax)*99 + (aZmin+aZmax)*0.9;
          //exchange = val1 > val2;
          if ((val1 - val2) >= tol) {
            exchange = Standard_True;
          }
          //cout << "box: " << val1<<" > "<<val2 << endl;
        }
      }

      if (exchange)
      {
//         cout << "exchange " << Index << " & " << Index+1 << endl;
        aTemp = OrderInd(Index);
        OrderInd(Index) = OrderInd(Index+1);
        OrderInd(Index+1) = aTemp;
        Sort = Standard_True;
      }
    }
  }

  for (Index=1; Index <= MaxShapes; Index++)
    SL.Append( aShapes( OrderInd(Index) ));
#endif
}

//=======================================================================
//function : CompsolidToCompound
//purpose  :
//=======================================================================
TopoDS_Shape GEOMUtils::CompsolidToCompound (const TopoDS_Shape& theCompsolid)
{
  if (theCompsolid.ShapeType() != TopAbs_COMPSOLID) {
    return theCompsolid;
  }

  TopoDS_Compound aCompound;
  BRep_Builder B;
  B.MakeCompound(aCompound);

  TopTools_MapOfShape mapShape;
  TopoDS_Iterator It (theCompsolid, Standard_True, Standard_True);

  for (; It.More(); It.Next()) {
    TopoDS_Shape aShape_i = It.Value();
    if (mapShape.Add(aShape_i)) {
      B.Add(aCompound, aShape_i);
    }
  }

  return aCompound;
}

//=======================================================================
//function : AddSimpleShapes
//purpose  :
//=======================================================================
void GEOMUtils::AddSimpleShapes (const TopoDS_Shape& theShape, TopTools_ListOfShape& theList)
{
  if (theShape.ShapeType() != TopAbs_COMPOUND &&
      theShape.ShapeType() != TopAbs_COMPSOLID) {
    theList.Append(theShape);
    return;
  }

  TopTools_MapOfShape mapShape;
  TopoDS_Iterator It (theShape, Standard_True, Standard_True);

  for (; It.More(); It.Next()) {
    TopoDS_Shape aShape_i = It.Value();
    if (mapShape.Add(aShape_i)) {
      if (aShape_i.ShapeType() == TopAbs_COMPOUND ||
          aShape_i.ShapeType() == TopAbs_COMPSOLID) {
        AddSimpleShapes(aShape_i, theList);
      } else {
        theList.Append(aShape_i);
      }
    }
  }
}

//=======================================================================
//function : CheckTriangulation
//purpose  :
//=======================================================================
bool GEOMUtils::CheckTriangulation (const TopoDS_Shape& aShape)
{
  bool isTriangulation = true;

  TopExp_Explorer exp (aShape, TopAbs_FACE);
  if (exp.More())
  {
    TopLoc_Location aTopLoc;
    Handle(Poly_Triangulation) aTRF;
    aTRF = BRep_Tool::Triangulation(TopoDS::Face(exp.Current()), aTopLoc);
    if (aTRF.IsNull()) {
      isTriangulation = false;
    }
  }
  else // no faces, try edges
  {
    TopExp_Explorer expe (aShape, TopAbs_EDGE);
    if (!expe.More()) {
      return false;
    }
    TopLoc_Location aLoc;
    Handle(Poly_Polygon3D) aPE = BRep_Tool::Polygon3D(TopoDS::Edge(expe.Current()), aLoc);
    if (aPE.IsNull()) {
      isTriangulation = false;
    }
  }

  if (!isTriangulation) {
    // calculate deflection
    Standard_Real aDeviationCoefficient = 0.001;

    Bnd_Box B;
    BRepBndLib::Add(aShape, B);
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

    Standard_Real dx = aXmax - aXmin, dy = aYmax - aYmin, dz = aZmax - aZmin;
    Standard_Real aDeflection = Max(Max(dx, dy), dz) * aDeviationCoefficient * 4;
    Standard_Real aHLRAngle = 0.349066;

    BRepMesh_IncrementalMesh Inc (aShape, aDeflection, Standard_False, aHLRAngle);
  }

  return true;
}

//=======================================================================
//function : GetTypeOfSimplePart
//purpose  :
//=======================================================================
TopAbs_ShapeEnum GEOMUtils::GetTypeOfSimplePart (const TopoDS_Shape& theShape)
{
  TopAbs_ShapeEnum aType = theShape.ShapeType();
  if      (aType == TopAbs_VERTEX)                             return TopAbs_VERTEX;
  else if (aType == TopAbs_EDGE  || aType == TopAbs_WIRE)      return TopAbs_EDGE;
  else if (aType == TopAbs_FACE  || aType == TopAbs_SHELL)     return TopAbs_FACE;
  else if (aType == TopAbs_SOLID || aType == TopAbs_COMPSOLID) return TopAbs_SOLID;
  else if (aType == TopAbs_COMPOUND) {
    // Only the iType of the first shape in the compound is taken into account
    TopoDS_Iterator It (theShape, Standard_False, Standard_False);
    if (It.More()) {
      return GetTypeOfSimplePart(It.Value());
    }
  }
  return TopAbs_SHAPE;
}

//=======================================================================
//function : GetEdgeNearPoint
//purpose  :
//=======================================================================
TopoDS_Shape GEOMUtils::GetEdgeNearPoint (const TopoDS_Shape& theShape,
                                          const TopoDS_Vertex& thePoint)
{
  TopoDS_Shape aResult;

  // 1. Explode the shape on edges
  TopTools_MapOfShape mapShape;
  Standard_Integer nbEdges = 0;
  TopExp_Explorer exp (theShape, TopAbs_EDGE);
  for (; exp.More(); exp.Next()) {
    if (mapShape.Add(exp.Current())) {
      nbEdges++;
    }
  }

  if (nbEdges == 0)
    Standard_NullObject::Raise("Given shape contains no edges");

  mapShape.Clear();
  Standard_Integer ind = 1;
  TopTools_Array1OfShape anEdges (1, nbEdges);
  TColStd_Array1OfReal aDistances (1, nbEdges);
  for (exp.Init(theShape, TopAbs_EDGE); exp.More(); exp.Next()) {
    if (mapShape.Add(exp.Current())) {
      TopoDS_Shape anEdge = exp.Current();
      anEdges(ind) = anEdge;

      // 2. Classify the point relatively each edge
      BRepExtrema_DistShapeShape aDistTool (thePoint, anEdges(ind));
      if (!aDistTool.IsDone())
        Standard_ConstructionError::Raise("Cannot find a distance from the given point to one of edges");

      aDistances(ind) = aDistTool.Value();
      ind++;
    }
  }

  // 3. Define edge, having minimum distance to the point
  Standard_Real nearest = RealLast(), nbFound = 0;
  Standard_Real prec = Precision::Confusion();
  for (ind = 1; ind <= nbEdges; ind++) {
    if (Abs(aDistances(ind) - nearest) < prec) {
      nbFound++;
    }
    else if (aDistances(ind) < nearest) {
      nearest = aDistances(ind);
      aResult = anEdges(ind);
      nbFound = 1;
    }
    else {
    }
  }
  if (nbFound > 1) {
    Standard_ConstructionError::Raise("Multiple edges near the given point are found");
  }
  else if (nbFound == 0) {
    Standard_ConstructionError::Raise("There are no edges near the given point");
  }
  else {
  }

  return aResult;
}

//=======================================================================
//function : PreciseBoundingBox
//purpose  : 
//=======================================================================
Standard_Boolean GEOMUtils::PreciseBoundingBox
                          (const TopoDS_Shape &theShape, Bnd_Box &theBox)
{
  Standard_Real aBound[6];

  theBox.Get(aBound[0], aBound[2], aBound[4], aBound[1], aBound[3], aBound[5]);

  Standard_Integer i;
  const gp_Pnt aMid(0.5*(aBound[1] + aBound[0]),  // XMid
                    0.5*(aBound[3] + aBound[2]),  // YMid
                    0.5*(aBound[5] + aBound[4])); // ZMid
  const gp_XYZ aSize(aBound[1] - aBound[0],       // DX
                     aBound[3] - aBound[2],       // DY
                     aBound[5] - aBound[4]);      // DZ
  const gp_Pnt aPnt[6] =
    {
      gp_Pnt(aBound[0] - (aBound[1] - aBound[0]), aMid.Y(), aMid.Z()), // XMin
      gp_Pnt(aBound[1] + (aBound[1] - aBound[0]), aMid.Y(), aMid.Z()), // XMax
      gp_Pnt(aMid.X(), aBound[2] - (aBound[3] - aBound[2]), aMid.Z()), // YMin
      gp_Pnt(aMid.X(), aBound[3] + (aBound[3] - aBound[2]), aMid.Z()), // YMax
      gp_Pnt(aMid.X(), aMid.Y(), aBound[4] - (aBound[5] - aBound[4])), // ZMin
      gp_Pnt(aMid.X(), aMid.Y(), aBound[5] + (aBound[5] - aBound[4]))  // ZMax
    };
  const gp_Dir aDir[3] = { gp::DX(), gp::DY(), gp::DZ() };
  const Standard_Real aPlnSize[3] =
    {
      0.5*Max(aSize.Y(), aSize.Z()), // XMin, XMax planes
      0.5*Max(aSize.X(), aSize.Z()), // YMin, YMax planes
      0.5*Max(aSize.X(), aSize.Y())  // ZMin, ZMax planes
    };
  gp_Pnt aPMin[2];

  for (i = 0; i < 6; i++) {
    const Standard_Integer iHalf = i/2;
    const gp_Pln aPln(aPnt[i], aDir[iHalf]);
    BRepBuilderAPI_MakeFace aMkFace(aPln, -aPlnSize[iHalf], aPlnSize[iHalf],
                                          -aPlnSize[iHalf], aPlnSize[iHalf]);

    if (!aMkFace.IsDone()) {
      return Standard_False;
    }

    TopoDS_Shape aFace = aMkFace.Shape();

    // Get minimal distance between planar face and shape.
    Standard_Real aMinDist =
      GEOMUtils::GetMinDistance(aFace, theShape, aPMin[0], aPMin[1]);

    if (aMinDist < 0.) {
      return Standard_False;
    }

    aBound[i] = aPMin[1].Coord(iHalf + 1);
  }

  // Update Bounding box with the new values.
  theBox.SetVoid();
  theBox.Update(aBound[0], aBound[2], aBound[4], aBound[1], aBound[3], aBound[5]);

  return Standard_True;
}

//=======================================================================
//function : GetMinDistanceSingular
//purpose  : 
//=======================================================================
double GEOMUtils::GetMinDistanceSingular(const TopoDS_Shape& aSh1,
                                         const TopoDS_Shape& aSh2,
                                         gp_Pnt& Ptmp1, gp_Pnt& Ptmp2)
{
  TopoDS_Shape     tmpSh1;
  TopoDS_Shape     tmpSh2;
  Standard_Real    AddDist1 = 0.;
  Standard_Real    AddDist2 = 0.;
  Standard_Boolean IsChange1 = GEOMUtils::ModifyShape(aSh1, tmpSh1, AddDist1);
  Standard_Boolean IsChange2 = GEOMUtils::ModifyShape(aSh2, tmpSh2, AddDist2);

  if( !IsChange1 && !IsChange2 )
    return -2.0;

  BRepExtrema_DistShapeShape dst(tmpSh1,tmpSh2);
  if (dst.IsDone()) {
    double MinDist = 1.e9;
    gp_Pnt PMin1, PMin2, P1, P2;
    for (int i = 1; i <= dst.NbSolution(); i++) {
      P1 = dst.PointOnShape1(i);
      P2 = dst.PointOnShape2(i);
      Standard_Real Dist = P1.Distance(P2);
      if (MinDist > Dist) {
        MinDist = Dist;
        PMin1 = P1;
        PMin2 = P2;
      }
    }
    if(MinDist<1.e-7) {
      Ptmp1 = PMin1;
      Ptmp2 = PMin2;
    }
    else {
      gp_Dir aDir(gp_Vec(PMin1,PMin2));
      if( MinDist > (AddDist1+AddDist2) ) {
        Ptmp1 = gp_Pnt( PMin1.X() + aDir.X()*AddDist1,
                        PMin1.Y() + aDir.Y()*AddDist1,
                        PMin1.Z() + aDir.Z()*AddDist1 );
        Ptmp2 = gp_Pnt( PMin2.X() - aDir.X()*AddDist2,
                        PMin2.Y() - aDir.Y()*AddDist2,
                        PMin2.Z() - aDir.Z()*AddDist2 );
        return (MinDist - AddDist1 - AddDist2);
      }
      else {
        if( AddDist1 > 0 ) {
          Ptmp1 = gp_Pnt( PMin1.X() + aDir.X()*AddDist1,
                          PMin1.Y() + aDir.Y()*AddDist1,
                          PMin1.Z() + aDir.Z()*AddDist1 );
          Ptmp2 = Ptmp1;
        }
        else {
          Ptmp2 = gp_Pnt( PMin2.X() - aDir.X()*AddDist2,
                          PMin2.Y() - aDir.Y()*AddDist2,
                          PMin2.Z() - aDir.Z()*AddDist2 );
          Ptmp1 = Ptmp2;
        }
      }
    }
    double res = MinDist - AddDist1 - AddDist2;
    if(res<0.) res = 0.0;
    return res;
  }
  return -2.0;
}

//=======================================================================
//function : GetMinDistance
//purpose  : 
//=======================================================================
Standard_Real GEOMUtils::GetMinDistance
                               (const TopoDS_Shape& theShape1,
                                const TopoDS_Shape& theShape2,
                                gp_Pnt& thePnt1, gp_Pnt& thePnt2)
{
  Standard_Real aResult = 1.e9;

  // Issue 0020231: A min distance bug with torus and vertex.
  // Make GetMinDistance() return zero if a sole VERTEX is inside any of SOLIDs

  // which of shapes consists of only one vertex?
  TopExp_Explorer exp1(theShape1,TopAbs_VERTEX), exp2(theShape2,TopAbs_VERTEX);
  TopoDS_Shape V1 = exp1.More() ? exp1.Current() : TopoDS_Shape();
  TopoDS_Shape V2 = exp2.More() ? exp2.Current() : TopoDS_Shape();
  exp1.Next(); exp2.Next();
  if ( exp1.More() ) V1.Nullify();
  if ( exp2.More() ) V2.Nullify();
  // vertex and container of solids
  TopoDS_Shape V = V1.IsNull() ? V2 : V1;
  TopoDS_Shape S = V1.IsNull() ? theShape1 : theShape2;
  if ( !V.IsNull() ) {
    // classify vertex against solids
    gp_Pnt p = BRep_Tool::Pnt( TopoDS::Vertex( V ) );
    for ( exp1.Init( S, TopAbs_SOLID ); exp1.More(); exp1.Next() ) {
      BRepClass3d_SolidClassifier classifier( exp1.Current(), p, 1e-6);
      if ( classifier.State() == TopAbs_IN ) {
        thePnt1 = p;
        thePnt2 = p;
        return 0.0;
      }
    }
  }
  // End Issue 0020231

  // skl 30.06.2008
  // additional workaround for bugs 19899, 19908 and 19910 from Mantis
  double dist = GEOMUtils::GetMinDistanceSingular
      (theShape1, theShape2, thePnt1, thePnt2);

  if (dist > -1.0) {
    return dist;
  }

  BRepExtrema_DistShapeShape dst (theShape1, theShape2);
  if (dst.IsDone()) {
    gp_Pnt P1, P2;

    for (int i = 1; i <= dst.NbSolution(); i++) {
      P1 = dst.PointOnShape1(i);
      P2 = dst.PointOnShape2(i);

      Standard_Real Dist = P1.Distance(P2);
      if (aResult > Dist) {
        aResult = Dist;
        thePnt1 = P1;
        thePnt2 = P2;
      }
    }
  }

  return aResult;
}

//=======================================================================
// function : ConvertClickToPoint()
// purpose  : Returns the point clicked in 3D view
//=======================================================================
gp_Pnt GEOMUtils::ConvertClickToPoint( int x, int y, Handle(V3d_View) aView )
{
  V3d_Coordinate XEye, YEye, ZEye, XAt, YAt, ZAt;
  aView->Eye( XEye, YEye, ZEye );

  aView->At( XAt, YAt, ZAt );
  gp_Pnt EyePoint( XEye, YEye, ZEye );
  gp_Pnt AtPoint( XAt, YAt, ZAt );

  gp_Vec EyeVector( EyePoint, AtPoint );
  gp_Dir EyeDir( EyeVector );

  gp_Pln PlaneOfTheView = gp_Pln( AtPoint, EyeDir );
  Standard_Real X, Y, Z;
  aView->Convert( x, y, X, Y, Z );
  gp_Pnt ConvertedPoint( X, Y, Z );

  gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project( PlaneOfTheView, ConvertedPoint );
  gp_Pnt ResultPoint = ElSLib::Value( ConvertedPointOnPlane.X(), ConvertedPointOnPlane.Y(), PlaneOfTheView );
  return ResultPoint;
}

//=======================================================================
// function : ModifyShape
// purpose  : 
//=======================================================================
Standard_Boolean GEOMUtils::ModifyShape(const TopoDS_Shape  &theShape,
                                              TopoDS_Shape  &theModifiedShape,
                                              Standard_Real &theAddDist)
{
  Standard_Boolean isModified = Standard_False;
  TopExp_Explorer anExp;
  int nbf = 0;

  theAddDist = 0.;
  theModifiedShape.Nullify();

  for ( anExp.Init( theShape, TopAbs_FACE ); anExp.More(); anExp.Next() ) {
    nbf++;
    theModifiedShape = anExp.Current();
  }
  if(nbf==1) {
    TopoDS_Shape sh = theShape;
    while(sh.ShapeType()==TopAbs_COMPOUND) {
      TopoDS_Iterator it(sh);
      sh = it.Value();
    }
    Handle(Geom_Surface) S = BRep_Tool::Surface(TopoDS::Face(theModifiedShape));
    if( S->IsKind(STANDARD_TYPE(Geom_SphericalSurface)) ||
        S->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)) ||
        S->IsUPeriodic()) {
      const Standard_Boolean isShell =
        (sh.ShapeType()==TopAbs_SHELL || sh.ShapeType()==TopAbs_FACE);

      if( isShell || S->IsUPeriodic() ) {
        // non solid case or any periodic surface (Mantis 22454).
        double U1,U2,V1,V2;
        // changes for 0020677: EDF 1219 GEOM: MinDistance gives 0 instead of 20.88
        //S->Bounds(U1,U2,V1,V2); changed by
        ShapeAnalysis::GetFaceUVBounds(TopoDS::Face(theModifiedShape),U1,U2,V1,V2);
        // end of changes for 020677 (dmv)
        Handle(Geom_RectangularTrimmedSurface) TrS1 =
          new Geom_RectangularTrimmedSurface(S,U1,(U1+U2)/2.,V1,V2);
        Handle(Geom_RectangularTrimmedSurface) TrS2 =
          new Geom_RectangularTrimmedSurface(S,(U1+U2)/2.,U2,V1,V2);
        BRep_Builder B;
        TopoDS_Face F1,F2;
        TopoDS_Shape aMShape;

        if (isShell) {
          B.MakeCompound(TopoDS::Compound(aMShape));
        } else {
          B.MakeShell(TopoDS::Shell(aMShape));
        }

        B.MakeFace(F1,TrS1,1.e-7);
        B.Add(aMShape,F1);
        B.MakeFace(F2,TrS2,1.e-7);
        B.Add(aMShape,F2);
        Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape;

        if (!isShell) {
          // The original shape is a solid.
          TopoDS_Solid aSolid;

          B.MakeSolid(aSolid);
          B.Add(aSolid, aMShape);
          aMShape = aSolid;
        }

        sfs->Init(aMShape);
        sfs->SetPrecision(1.e-6);
        sfs->SetMaxTolerance(1.0);
        sfs->Perform();
        theModifiedShape = sfs->Shape();
        isModified = Standard_True;
      }
      else {
        if( S->IsKind(STANDARD_TYPE(Geom_SphericalSurface)) ) {
          Handle(Geom_SphericalSurface) SS = Handle(Geom_SphericalSurface)::DownCast(S);
          gp_Pnt PC = SS->Location();
          BRep_Builder B;
          TopoDS_Vertex V;
          B.MakeVertex(V,PC,1.e-7);
          theModifiedShape = V;
          theAddDist = SS->Radius();
          isModified = Standard_True;
        }
        else {
          Handle(Geom_ToroidalSurface) TS = Handle(Geom_ToroidalSurface)::DownCast(S);
          gp_Ax3 ax3 = TS->Position();
          Handle(Geom_Circle) C = new Geom_Circle(ax3.Ax2(),TS->MajorRadius());
          BRep_Builder B;
          TopoDS_Edge E;
          B.MakeEdge(E,C,1.e-7);
          theModifiedShape = E;
          theAddDist = TS->MinorRadius();
          isModified = Standard_True;
        }
      }
    } else {
      theModifiedShape = theShape;
    }
  }
  else
    theModifiedShape = theShape;

  return isModified;
}
