// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>

#include <GeomLProp_CLProps.hxx>
#include <GeomLProp_SLProps.hxx>

#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>

#include <TColStd_Array1OfReal.hxx>

#include <gp_Pln.hxx>
#include <gp_Lin.hxx>

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
gp_Vec GEOMUtils::GetVector (const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
    Standard_NullObject::Raise("Null shape is given for a vector");

  if (theShape.ShapeType() != TopAbs_EDGE)
    Standard_TypeMismatch::Raise("Invalid shape is given, must be a vector or an edge");

  TopoDS_Edge anE = TopoDS::Edge(theShape);

  TopoDS_Vertex V1, V2;
  TopExp::Vertices(anE, V1, V2, Standard_True);

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
