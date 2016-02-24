// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "CurveCreator_Utils.hxx"
#include "CurveCreator.hxx"
#include "CurveCreator_Curve.hxx"
#include "CurveCreator_Section.hxx"
#include "CurveCreator_UtilsICurve.hxx"

#include <Basics_OCCTVersion.hxx>

#include <GEOMUtils.hxx>

#include <gp_Pln.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Line.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_LocalContext.hxx>

#include <Geom_Point.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Select3D_SensitivePoint.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools_WireExplorer.hxx>

#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <GeomAPI_Interpolate.hxx>

#include <ProjLib.hxx>
#include <ElSLib.hxx>

#include <math.h>

#include "CurveCreator_ICurve.hxx"

const double LOCAL_SELECTION_TOLERANCE = 0.0001;
const int    SCENE_PIXEL_PROJECTION_TOLERANCE = 10;
const int    SCENE_PIXEL_POINT_TOLERANCE = 5;

#define PLN_FREE   0
#define PLN_ORIGIN 1
#define PLN_OX     2
#define PLN_FIXED  3

/**
 * This static function returns the curve of original type from the edge.
 *
 * \param theEdge the edge
 * \return the curve of original type. Can be null handle.
 */
static Handle(Geom_Curve) GetCurve(const TopoDS_Edge &theEdge)
{
  Handle(Geom_Curve) aResult;

  if (theEdge.IsNull()) {
    return aResult;
  }

  Standard_Real aF;
  Standard_Real aL;

  aResult = BRep_Tool::Curve(theEdge, aF, aL);

  if (aResult.IsNull()) {
    return aResult;
  }

  // Get the curve of original type
  Handle(Standard_Type) aType = aResult->DynamicType();

  while (aType == STANDARD_TYPE(Geom_TrimmedCurve)) {
    Handle(Geom_TrimmedCurve) aTrCurve =
      Handle(Geom_TrimmedCurve)::DownCast(aResult);

    aResult = aTrCurve->BasisCurve();
    aType  = aResult->DynamicType();
  }

  return aResult;
}

//=======================================================================
// function : ConvertClickToPoint()
// purpose  : Returns the point clicked in 3D view
//=======================================================================
void CurveCreator_Utils::ConvertPointToClick( const gp_Pnt& thePoint,
                                              Handle(V3d_View) theView,
                                              int& x, int& y )
{
  theView->Convert(thePoint.X(), thePoint.Y(), thePoint.Z(), x, y );
}


//=======================================================================
// function : ConvertClickToPoint()
// purpose  : Returns the point clicked in 3D view
//=======================================================================
gp_Pnt CurveCreator_Utils::ConvertClickToPoint( int x, int y, Handle(V3d_View) aView )
{
  // the 3D point, that is a projection of the pixels to the XYZ view plane
  //return GEOMUtils::ConvertClickToPoint( x, y, aView );

  // we need the projection to the XOY plane
  // 1. find a point in the plane of the eye and the normal to the plane
  Standard_Real X, Y, Z;
  Quantity_Parameter Vx, Vy, Vz;
  aView->ConvertWithProj( x, y, X, Y, Z, Vx, Vy, Vz );

  // 2. build a ray from the point by the normal to the XOY plane and intersect it
  // The ray equation is the following : p(x,y,z) = p0(x,y,z) + t*V(x,y,z)
  // X,Y,Z - defines p0(x,y,z), Vx,Vy,Vz - defines V(x,y,z)
  // p(x,y,z) - is a searched point, t - should to be calculated by the condition of XOY plane
  // The system of equations is the following:
  // p(x) = p0(x)+t*V(x)
  // p(y) = p0(y)+t*V(y)
  // p(z) = p0(z)+t*V(z)
  // p(z) = 0

  Standard_Real aXp, aYp, aZp;
  //It is not possible to use Precision::Confusion(), because it is e-0.8, but V is sometimes e-6
  Standard_Real aPrec = LOCAL_SELECTION_TOLERANCE;
  if ( fabs( Vz ) > aPrec ) {
    Standard_Real aT = -Z/Vz;
    aXp = X + aT*Vx;
    aYp = Y + aT*Vy;
    aZp = Z + aT*Vz;
  }
  else { // Vz = 0 - the eyed plane is orthogonal to Z plane - XOZ, or YOZ
    aXp = aYp = aZp = 0;
    if ( fabs( Vy ) < aPrec ) // Vy = 0 - the YOZ plane
      aYp = Y;
    else if ( fabs( Vx ) < aPrec ) // Vx = 0 - the XOZ plane
      aXp = X;
  }
  /*std::cout << "ConvertClickToPoint: " << std::endl
            << "XYZ1 = (" << X << ", " << Y << ", " << Z << "); " << std::endl
            << "Vxyz = (" << Vx << ", " << Vy << ", " << Vz << "); " << std::endl
            << "Resp = (" << aXp << ", " << aYp << ", " << aZp << "); " << std::endl;*/

  gp_Pnt ResultPoint( aXp, aYp, aZp );
  return ResultPoint;
}

//=======================================================================
// function : constructBSpline
// purpose  :
//=======================================================================
bool CurveCreator_Utils::constructBSpline(
  const Handle(TColgp_HArray1OfPnt)& thePoints,
  const Standard_Boolean theIsClosed,
  Handle(Geom_BSplineCurve)& theBSpline)
{
  const int aPointCount = thePoints->Length();
  if (aPointCount <= 1)
  {
    return false;
  }

  // Calculate the tangents.
  TColgp_Array1OfVec aTangents(1, aPointCount);
  Handle(TColStd_HArray1OfBoolean) aTangentFlags =
    new TColStd_HArray1OfBoolean(1, aPointCount);
  GeomAPI_Interpolate aInterpolator(thePoints, theIsClosed, 0);
  if (aPointCount == 2)
  {
    aTangentFlags->SetValue(1, Standard_False);
    aTangentFlags->SetValue(2, Standard_False);
  }
  else
  {
    for (Standard_Integer aPN = 1; aPN <= aPointCount; ++aPN)
    {
      gp_Vec aTangent;
      if (aPN != 1 || theIsClosed)
      {
        const Standard_Integer aPN1 = (aPN != 1) ? (aPN - 1) : aPointCount;
        aTangent = gp_Vec(thePoints->Value(aPN1),
          thePoints->Value(aPN)).Normalized();
      }
      if (aPN < aPointCount || theIsClosed)
      {
        const Standard_Integer aPN2 = (aPN != aPointCount) ? (aPN + 1) : 1;
        const gp_Vec aTangent2 = aTangent +
          gp_Vec(thePoints->Value(aPN), thePoints->Value(aPN2)).Normalized();
        if (aTangent2.SquareMagnitude() >= Precision::SquareConfusion())
        {
          aTangent = aTangent2.Normalized();
        }
        else
        {
          aTangent = -aTangent;
        }
      }
      aTangents.SetValue(aPN, aTangent);
      aTangentFlags->SetValue(aPN, Standard_True);
    }
  }

  // Interpolate.
  aInterpolator.Load(aTangents, aTangentFlags, Standard_False);
  aInterpolator.Perform();
  const bool aResult = (aInterpolator.IsDone() == Standard_True);
  if (aResult)
  {
    theBSpline = aInterpolator.Curve();
  }
  return aResult;
}

//=======================================================================
// function : constructWire
// purpose  :
//=======================================================================
TopoDS_Wire CurveCreator_Utils::ConstructWire(
  Handle(TColgp_HArray1OfPnt) thePoints,
  const bool theIsPolyline,
  const bool theIsClosed)
{
  TopoDS_Wire aWire;
  BRep_Builder aBuilder;
  aBuilder.MakeWire(aWire);
  const int aPointCount = thePoints->Length();
  if (theIsPolyline)
  {
    const TopoDS_Vertex aFirstVertex =
      BRepBuilderAPI_MakeVertex(thePoints->Value(1));
    TopoDS_Vertex aVertex = aFirstVertex;
    for (Standard_Integer aPN = 1; aPN < aPointCount; ++aPN)
    {
      const TopoDS_Vertex aVertex2 =
        BRepBuilderAPI_MakeVertex(thePoints->Value(aPN + 1));
      aBuilder.Add(aWire, BRepBuilderAPI_MakeEdge(aVertex, aVertex2));
      aVertex = aVertex2;
    }
    if (theIsClosed && aPointCount > 1)
    {
      aBuilder.Add(aWire, BRepBuilderAPI_MakeEdge(aVertex, aFirstVertex));
    }
  }
  else
  {
    Handle(Geom_BSplineCurve) aBSpline;
    if (constructBSpline(thePoints, theIsClosed, aBSpline))
    {
      aBuilder.Add(aWire, BRepBuilderAPI_MakeEdge(aBSpline));
    }
  }
  return aWire;
}

//=======================================================================
// function : constructShape
// purpose  :
//=======================================================================
void CurveCreator_Utils::constructShape(
  const CurveCreator_ICurve* theCurve, TopoDS_Shape& theShape)
{
  BRep_Builder aBuilder;
  TopoDS_Compound aShape;
  aBuilder.MakeCompound(aShape);
  const int aSectionCount = theCurve->getNbSections();
  for (int aSectionI = 0; aSectionI < aSectionCount; ++aSectionI)
  {
    const int aTmpPointCount = theCurve->getNbPoints(aSectionI);
    if (aTmpPointCount == 0)
    {
      continue;
    }

    // Get the different points.
    const CurveCreator_ISection* aSection = theCurve->getSection(aSectionI);
    Handle(TColgp_HArray1OfPnt) aPoints;
    aSection->GetDifferentPoints(theCurve->getDimension(), aPoints);
    const int aPointCount = aPoints->Length();
    const bool isClosed = theCurve->isClosed(aSectionI);

    // Add the vertices to the shape.
    for (Standard_Integer aPN = 1; aPN <= aPointCount; ++aPN)
    {
      aBuilder.Add(aShape, BRepBuilderAPI_MakeVertex(aPoints->Value(aPN)));
    }

    // Add the wire to the shape.
    const bool isPolyline =
      (theCurve->getSectionType(aSectionI) == CurveCreator::Polyline);
    const TopoDS_Wire aWire = ConstructWire(aPoints, isPolyline, isClosed);
    if (!aWire.IsNull())
    {
      aBuilder.Add(aShape, aWire);
    }
  }
  theShape = aShape;
}

/**
 * This is an intermediate structure for curve construction.
 */
struct Section3D
{
  Section3D() : myIsClosed(false), myIsBSpline(false)
  { }

  bool                        myIsClosed;
  bool                        myIsBSpline;
  Handle(TColgp_HArray1OfPnt) myPoints;
};

//=======================================================================
// function : constructCurve
// purpose  : 
//=======================================================================
bool CurveCreator_Utils::constructCurve
                      (const TopoDS_Shape        theShape,
                             CurveCreator_Curve *theCurve,
                             gp_Ax3             &theLocalCS)
{
  if (theShape.IsNull()) {
    return false;
  }

  // Collect wires or vertices from shape.
  TopTools_ListOfShape aWOrV;
  TopAbs_ShapeEnum     aType = theShape.ShapeType();

  if (aType == TopAbs_WIRE || aType == TopAbs_VERTEX) {
    aWOrV.Append(theShape);
  } else if (aType == TopAbs_COMPOUND) {
    TopoDS_Iterator aShIter(theShape);

    for (; aShIter.More(); aShIter.Next()) {
      const TopoDS_Shape &aSubShape = aShIter.Value();

      aType = aSubShape.ShapeType();

      if (aType == TopAbs_WIRE || aType == TopAbs_VERTEX) {
        aWOrV.Append(aSubShape);
      } else {
        // Only subshapes of types wire or vertex are supported.
        return false;
      }
    }
  } else {
    // Only wire (vertex) or compound of wires (vertices) are supported.
    return false;
  }

  // Treat each wire or vertex. Get points, compute the working plane.
  gp_Pln                             aPlane;
  Standard_Integer                   aPlaneStatus = PLN_FREE;
  TopTools_ListIteratorOfListOfShape anIter(aWOrV);
  std::list<Section3D>               aListSec;

  for (; anIter.More(); anIter.Next()) {
    Section3D aSec3D;

    aSec3D.myPoints = CurveCreator_Utils::getPoints
      (anIter.Value(), aSec3D.myIsClosed, aSec3D.myIsBSpline);

    if (aSec3D.myPoints.IsNull()) {
      return false;
    }

    aListSec.push_back(aSec3D);

    if (aPlaneStatus != PLN_FIXED) {
      // Compute plane
      CurveCreator_Utils::FindPlane(aSec3D.myPoints, aPlane, aPlaneStatus);
    }
  }

  // Check if it is possible to change a computed coordinate system by
  // XOY, XOZ or YOZ or parallel to them.
  gp_Pnt        aO(0., 0., 0.);
  gp_Dir        aNDir(0., 0., 1.);
  gp_Dir        aXDir(1., 0., 0.);
  gp_Ax3        anAxis;
  Standard_Real aTolAng = Precision::Confusion(); // Angular() is too small.

  switch (aPlaneStatus) {
    case PLN_ORIGIN:
      {
        // Change the location.
        aO.SetZ(aPlane.Location().Z());
        anAxis.SetLocation(aO);
        aPlane.SetPosition(anAxis);
      }
      break;
    case PLN_OX:
      {
        // Fixed origin + OX axis
        const gp_Dir &aPlnX = aPlane.Position().XDirection();

        if (Abs(aPlnX.Z()) <= aTolAng) {
          // Make a coordinate system parallel to XOY.
          aO.SetZ(aPlane.Location().Z());
          anAxis.SetLocation(aO);
          aPlane.SetPosition(anAxis);
        } else if (Abs(aPlnX.Y()) <= aTolAng) {
          // Make a coordinate system parallel to XOZ.
          aO.SetY(aPlane.Location().Y());
          aNDir.SetCoord(0., 1., 0.);
          aXDir.SetCoord(0., 0., 1.);
          anAxis = gp_Ax3(aO, aNDir, aXDir);
          aPlane.SetPosition(anAxis);
        } else if (Abs(aPlnX.X()) <= aTolAng) {
          // Make a coordinate system parallel to YOZ.
          aO.SetX(aPlane.Location().X());
          aNDir.SetCoord(1., 0., 0.);
          aXDir.SetCoord(0., 1., 0.);
          anAxis = gp_Ax3(aO, aNDir, aXDir);
          aPlane.SetPosition(anAxis);
        }
      }
      break;
    case PLN_FIXED:
      {
        const gp_Dir &aPlnN = aPlane.Position().Direction();
        gp_Dir        aYDir(0., 1., 0.);

        if (aPlnN.IsParallel(aNDir, aTolAng)) {
          // Make a coordinate system parallel to XOY.
          aO.SetZ(aPlane.Location().Z());
          anAxis.SetLocation(aO);
          aPlane.SetPosition(anAxis);
        } else if (aPlnN.IsParallel(aYDir, aTolAng)) {
          // Make a coordinate system parallel to XOZ.
          aO.SetY(aPlane.Location().Y());
          aNDir.SetCoord(0., 1., 0.);
          aXDir.SetCoord(0., 0., 1.);
          anAxis = gp_Ax3(aO, aNDir, aXDir);
          aPlane.SetPosition(anAxis);
        } else if (aPlnN.IsParallel(aXDir, aTolAng)) {
          // Make a coordinate system parallel to YOZ.
          aO.SetX(aPlane.Location().X());
          aNDir.SetCoord(1., 0., 0.);
          aXDir.SetCoord(0., 1., 0.);
          anAxis = gp_Ax3(aO, aNDir, aXDir);
          aPlane.SetPosition(anAxis);
        }
      }
      break;
    case PLN_FREE:
    default:
      // Use XOY plane.
      aPlane.SetPosition(anAxis);
      break;
  }

  // Compute 2d points.
  std::list<Section3D>::const_iterator aSecIt = aListSec.begin();
  Standard_Real                        aTolConf2 =
    Precision::Confusion()*Precision::Confusion();
  Standard_Real                        aX;
  Standard_Real                        aY;

  for (; aSecIt != aListSec.end(); ++aSecIt) {
    Standard_Integer          i;
    CurveCreator::Coordinates aCoords;

    for (i = aSecIt->myPoints->Lower(); i <= aSecIt->myPoints->Upper(); ++i) {
      const gp_Pnt &aPnt = aSecIt->myPoints->Value(i);

      if (aPlane.SquareDistance(aPnt) > aTolConf2) {
        // The point doesn't lie on the plane.
        return false;
      }

      ElSLib::Parameters(aPlane, aPnt, aX, aY);
      aCoords.push_back(aX);
      aCoords.push_back(aY);
    }

    // Add a new section to the curve.
    const std::string               aSecName =
      CurveCreator_UtilsICurve::getUniqSectionName(theCurve);
    const CurveCreator::SectionType aSecType = aSecIt->myIsBSpline ?
      CurveCreator::Spline : CurveCreator::Polyline;

    theCurve->addSectionInternal(aSecName, aSecType,
                                 aSecIt->myIsClosed, aCoords);
  }

  // Set the local coordinate system.
  theLocalCS = aPlane.Position();

  return true;
}

class CompareSectionToPoint
{
public:
  CompareSectionToPoint( const int theISection = -1, const int theIPoint = -1 )
    : mySectionId( theISection ), myPointId( theIPoint ) {};
  ~CompareSectionToPoint() {}

  bool operator < ( const CompareSectionToPoint& theOther ) const
  {
    bool isLess = mySectionId < theOther.mySectionId;
    if ( !isLess && mySectionId == theOther.mySectionId )
      isLess = myPointId < theOther.myPointId;
    return isLess;
  }

private:
  int mySectionId;
  int myPointId;
};


void CurveCreator_Utils::getSelectedPoints( Handle(AIS_InteractiveContext) theContext,
                                            const CurveCreator_ICurve* theCurve,
                                            CurveCreator_ICurve::SectionToPointList& thePoints )
{
  thePoints.clear();

  std::list<double> aSelectedPoints;
  gp_Pnt aPnt;
  std::map<CompareSectionToPoint, int> aPointsMap;

  CurveCreator_ICurve::SectionToPointList aPoints;
  for ( theContext->InitSelected(); theContext->MoreSelected(); theContext->NextSelected() ) {
    TopoDS_Vertex aVertex;
    TopoDS_Shape aShape = theContext->SelectedShape();
    if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
      aVertex = TopoDS::Vertex( theContext->SelectedShape() );

    if ( aVertex.IsNull() )
      continue;
    aPnt = BRep_Tool::Pnt( aVertex );

    CurveCreator_UtilsICurve::findSectionsToPoints( theCurve, aPnt.X(), aPnt.Y(), aPoints );
    CurveCreator_ICurve::SectionToPointList::const_iterator anIt = aPoints.begin(),
                                                            aLast = aPoints.end();
    CompareSectionToPoint aPoint;
    for ( ; anIt != aLast; anIt++ ) {
      aPoint = CompareSectionToPoint( (*anIt).first, (*anIt).second );
      if ( aPointsMap.find( aPoint ) != aPointsMap.end() )
        continue;
      aPointsMap[aPoint] = 0;

      thePoints.push_back( *anIt );
    }
  }
}

void CurveCreator_Utils::setSelectedPoints( Handle(AIS_InteractiveContext) theContext,
                                            const CurveCreator_ICurve* theCurve,
                                            const CurveCreator_ICurve::SectionToPointList& thePoints )
{
  if ( !theCurve )
    return;

  Handle(AIS_InteractiveObject) anAIS = theCurve->getAISObject();
  if ( anAIS.IsNull() )
    return;
  Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast( anAIS );
  if ( anAISShape.IsNull() )
    return;

  //ASL: we convert list of point indices to list of points coordinates
  int aSize = thePoints.size();
  std::vector<gp_Pnt> aPntsToSelect( aSize );

  CurveCreator_ICurve::SectionToPointList::const_iterator
                     aPIt = thePoints.begin(), aPLast = thePoints.end();
  CurveCreator_ICurve::SectionToPoint aSToPoint;
  for( int i=0; aPIt != aPLast; aPIt++, i++ )
  {
    gp_Pnt aPntToSelect;
    CurveCreator_UtilsICurve::getPoint( theCurve, aPIt->first, aPIt->second, aPntToSelect );
    aPntsToSelect[i] = aPntToSelect;
  }

  theContext->ClearSelected( Standard_False );
  //ASL: we switch off automatic highlight to improve performance of selection
  theContext->SetAutomaticHilight( Standard_False );

  Handle(SelectMgr_Selection) aSelection = anAISShape->Selection( AIS_Shape::SelectionMode( TopAbs_VERTEX ) );
  for( aSelection->Init(); aSelection->More(); aSelection->Next() )
  {    
#if OCC_VERSION_LARGE > 0x06080100
    const Handle(SelectMgr_SensitiveEntity) aHSenEntity = aSelection->Sensitive();
    if( aHSenEntity.IsNull() )
      continue;
    Handle(SelectBasics_SensitiveEntity) aSenEntity = aHSenEntity->BaseSensitive();
#else
    Handle(SelectBasics_SensitiveEntity) aSenEntity = aSelection->Sensitive();
#endif

    Handle(Select3D_SensitivePoint) aSenPnt = Handle(Select3D_SensitivePoint)::DownCast( aSenEntity );

    gp_Pnt anOwnerPnt = aSenPnt->Point();
    Handle(SelectMgr_EntityOwner) anOwner = Handle(SelectMgr_EntityOwner)::DownCast( aSenPnt->OwnerId() );


    CurveCreator_ICurve::SectionToPointList::const_iterator anIt = thePoints.begin(),
                                                                   aLast = thePoints.end();
    bool isFound = false;
    for( int i=0; i<aSize; i++ )
    {
      bool isIntersect = fabs( aPntsToSelect[i].X() - anOwnerPnt.X() ) < LOCAL_SELECTION_TOLERANCE &&
                         fabs( aPntsToSelect[i].Y() - anOwnerPnt.Y() ) < LOCAL_SELECTION_TOLERANCE;
      if( isIntersect )
      {
        theContext->AddOrRemoveSelected( anOwner, Standard_False );
        break;
      }
    }
  }

  //ASL: we switch on again automatic highlight (otherwise selection will not be shown)
  //     and call HilightPicked to draw selected owners
  theContext->SetAutomaticHilight( Standard_True );
  theContext->LocalContext()->HilightPicked( Standard_True );
}

//=======================================================================
// function : setLocalPointContext
// purpose  : Open/close the viewer local context
//=======================================================================
void CurveCreator_Utils::setLocalPointContext( const CurveCreator_ICurve* theCurve,
                                               Handle(AIS_InteractiveContext) theContext,
                                               const bool theOpen )
{
  if ( !theContext )
    return;

  if ( theOpen ) {
    // Open local context if there is no one
    if ( !theContext->HasOpenedContext() ) {
      theContext->ClearCurrents( false );
      theContext->OpenLocalContext( false/*use displayed objects*/, true/*allow shape decomposition*/ );
    }
    // load the curve AIS object to the local context with the point selection
    Handle(AIS_InteractiveObject) anAIS = theCurve->getAISObject();
    if ( !anAIS.IsNull() )
    {
      if ( anAIS->IsKind( STANDARD_TYPE( AIS_Shape ) ) )
      {
        theContext->Load( anAIS, -1/*selection mode*/, true/*allow decomposition*/ );
        theContext->Activate( anAIS, AIS_Shape::SelectionMode( (TopAbs_ShapeEnum)TopAbs_VERTEX ) );
      }
    }
  }
  else {
    if ( theContext->HasOpenedContext() )
      theContext->CloseAllContexts();
  }
}

bool CurveCreator_Utils::pointOnObject( Handle(V3d_View) theView,
                                        Handle(AIS_InteractiveObject) theObject,
                                        const int theX, const int theY,
                                        gp_Pnt& thePoint,
                                        gp_Pnt& thePoint1, gp_Pnt& thePoint2 )
{
  bool isFullFound = false;

  if ( theObject.IsNull() || theView.IsNull() )
    return isFullFound;
  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast( theObject );
  if ( aShape.IsNull() )
    return isFullFound;
  const TopoDS_Compound& aCompound = TopoDS::Compound( aShape->Shape() );
  if ( aCompound.IsNull() )
    return isFullFound;

  gp_Pnt aCurPoint, aCurPoint1, aCurPoint2;
  gp_Pnt aFoundPoint, aFoundPnt1, aFoundPnt2;
  Standard_Real aParameter;
  bool isFound = false;
  int aDelta, aMinDelta = 2*SCENE_PIXEL_PROJECTION_TOLERANCE*SCENE_PIXEL_PROJECTION_TOLERANCE;
  TopExp_Explorer anExp( aCompound, TopAbs_EDGE );
  for ( ; anExp.More(); anExp.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anExp.Current());
    if ( anEdge.IsNull() )
      continue;
    Standard_Real aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve( anEdge, aFirst, aLast );
    if ( aCurve->IsKind( STANDARD_TYPE(Geom_BSplineCurve) ) ) {
      Handle(Geom_BSplineCurve) aBSplineCurve =
                          Handle(Geom_BSplineCurve)::DownCast( aCurve );
      if ( !aBSplineCurve.IsNull() ) {
        isFound = hasProjectPointOnCurve( theView, theX, theY, aBSplineCurve,
                                          aParameter, aDelta );
        if ( isFound ) {
          aCurPoint = aBSplineCurve->Value( aParameter );
          Standard_Integer anI1, anI2;
          aBSplineCurve->LocateU( aParameter, LOCAL_SELECTION_TOLERANCE, anI1, anI2 );
          aCurPoint1 = aBSplineCurve->Value( aBSplineCurve->Knot( anI1 ) );
          aCurPoint2 = aBSplineCurve->Value( aBSplineCurve->Knot( anI2 ) );
        }
      }
    }
    else { // a curve built on a polyline edge
      Handle(Geom_Line) aGLine = Handle(Geom_Line)::DownCast( aCurve );
      if ( aGLine.IsNull() )
        continue;
      isFound = hasProjectPointOnCurve( theView, theX, theY, aGLine, aParameter,
                                        aDelta );
      if ( isFound ) {
        aCurPoint = aGLine->Value( aParameter );
        TopoDS_Vertex V1, V2;
        TopExp::Vertices( anEdge, V1, V2, Standard_True );
        if ( V1.IsNull() || V2.IsNull() )
          continue;
        aCurPoint1 = BRep_Tool::Pnt(V1);
        aCurPoint2 = BRep_Tool::Pnt(V2);

        // check that the projected point is on the bounded curve
        gp_Vec aVec1( aCurPoint1, aCurPoint );
        gp_Vec aVec2( aCurPoint2, aCurPoint );
        isFound = fabs( aVec1.Angle( aVec2 ) - M_PI ) < LOCAL_SELECTION_TOLERANCE;
      }
    }
    if ( isFound && aMinDelta >= aDelta ) {
      aMinDelta = aDelta;

      isFullFound = true;
      aFoundPnt1 = aCurPoint1;
      aFoundPnt2 = aCurPoint2;
      aFoundPoint = aCurPoint;
    }
  }
  if ( isFullFound ) {
    int aX, anY, aX1, anY1, aX2, anY2;
    int aDelta;
    CurveCreator_Utils::ConvertPointToClick( aFoundPoint, theView, aX, anY );
    CurveCreator_Utils::ConvertPointToClick( aFoundPnt1, theView, aX1, anY1 );
    CurveCreator_Utils::ConvertPointToClick( aFoundPnt2, theView, aX2, anY2 );

    isFullFound = !isEqualPixels( aX, anY, aX1, anY1, SCENE_PIXEL_POINT_TOLERANCE, aDelta ) &&
                  !isEqualPixels( aX, anY, aX2, anY2, SCENE_PIXEL_POINT_TOLERANCE, aDelta );
    if ( isFullFound ) {
      thePoint = aFoundPoint;
      thePoint1 = aFoundPnt1;
      thePoint2 = aFoundPnt2;
    }
  }
  return isFullFound;
}

bool CurveCreator_Utils::hasProjectPointOnCurve( Handle(V3d_View) theView,
                                                 const int theX, const int theY,
                                                 const Handle(Geom_Curve)& theCurve,
                                                 Standard_Real& theParameter,
                                                 int& theDelta )
{
  bool isFound = false;
  if ( theView.IsNull() )
    return isFound;

  gp_Pnt aPoint = CurveCreator_Utils::ConvertClickToPoint( theX, theY, theView );

  GeomAPI_ProjectPointOnCurve aProj( aPoint, theCurve );
  Standard_Integer aNbPoint = aProj.NbPoints();
  if (aNbPoint > 0) {
    for (Standard_Integer j = 1; j <= aNbPoint && !isFound; j++) {
      gp_Pnt aNewPoint = aProj.Point( j );
      theParameter = aProj.Parameter( j );

      int aX, anY;
      CurveCreator_Utils::ConvertPointToClick( aNewPoint, theView, aX, anY );

      isFound = isEqualPixels( aX, anY, theX, theY, SCENE_PIXEL_PROJECTION_TOLERANCE, theDelta );
    }
  }
  return isFound;
}

bool CurveCreator_Utils::isEqualPixels( const int theX, const int theY, const int theOtherX,
                                        const int theOtherY, const double theTolerance, int& theDelta )
{
  int aXDelta = abs( theX - theOtherX );
  int anYDelta = abs( theY - theOtherY );

  theDelta = aXDelta*aXDelta + anYDelta*anYDelta;

  return aXDelta < theTolerance && anYDelta < theTolerance;
}

bool CurveCreator_Utils::isEqualPoints( const gp_Pnt& thePoint, const gp_Pnt& theOtherPoint )
{
  return theOtherPoint.IsEqual( thePoint, LOCAL_SELECTION_TOLERANCE );
}

//=======================================================================
// function : getPoints
// purpose  : 
//=======================================================================
Handle(TColgp_HArray1OfPnt) CurveCreator_Utils::getPoints
                  (const TopoDS_Shape &theShape,
                         bool         &IsClosed,
                         bool         &IsBSpline)
{
  Handle(TColgp_HArray1OfPnt) aResult;

  IsClosed  = false;
  IsBSpline = false;

  if (theShape.IsNull()) {
    return aResult;
  }

  const TopAbs_ShapeEnum aShType = theShape.ShapeType();

  if (aShType == TopAbs_VERTEX) {
    // There is a single point.
    gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(theShape));

    aResult = new TColgp_HArray1OfPnt(1, 1, aPnt);

    return aResult;
  } else if (aShType != TopAbs_WIRE) {
    // The shape is neither a vertex nor a wire.
    return aResult;
  }

  // Treat wire.
  BRepTools_WireExplorer anExp(TopoDS::Wire(theShape));

  if (!anExp.More()) {
    // Empty wires are not allowed.
    return aResult;
  }

  // Treat the first edge.
  TopoDS_Edge        anEdge = anExp.Current();
  Handle(Geom_Curve) aCurve = GetCurve(anEdge);

  if (aCurve.IsNull()) {
    return aResult;
  }

  // Check the curve type.
  Handle(Standard_Type) aType     = aCurve->DynamicType();

  if (aType == STANDARD_TYPE(Geom_BSplineCurve)) {
    IsBSpline = true;
  } else if (aType != STANDARD_TYPE(Geom_Line)) {
    // The curve is neither a line or a BSpline. It is not valid.
    return aResult;
  }

  // Go to the next edge.
  TopoDS_Vertex aFirstVtx = anExp.CurrentVertex();

  anExp.Next();

  if (IsBSpline)
  {
    // There should be a single BSpline curve in the wire.
    if (anExp.More()) {
      return aResult;
    }

    // Construct a section from poles of BSpline.
    Handle(Geom_BSplineCurve) aBSplCurve =
      Handle(Geom_BSplineCurve)::DownCast(aCurve);

    // Check if the edge is valid. It should not be based on trimmed curve.
    gp_Pnt aCP[2] = { aBSplCurve->StartPoint(), aBSplCurve->EndPoint() };
    TopoDS_Vertex aV[2];
    Standard_Integer i;

    TopExp::Vertices(anEdge, aV[0], aV[1]);

    for (i = 0; i < 2; i++) {
      gp_Pnt        aPnt = BRep_Tool::Pnt(aV[i]);
      Standard_Real aTol = BRep_Tool::Tolerance(aV[i]);

      if (!aPnt.IsEqual(aCP[i], aTol)) {
        return aResult;
      }
    }

    IsClosed = aV[0].IsSame(aV[1]) ? true : false;
    
    Standard_Integer aNbPoints = aBSplCurve->NbKnots();
    TColStd_Array1OfReal aKnots(1, aNbPoints);
    aBSplCurve->Knots(aKnots);

    // Don't consider the last point as it coincides with the first
    if (IsClosed)
      --aNbPoints;

    aResult = new TColgp_HArray1OfPnt(1, aNbPoints);
    for (i = 1; i <= aNbPoints; ++i)
      aResult->SetValue(i, aBSplCurve->Value( aKnots.Value(i) ));
  }
  else
  {
    // This is a polyline.
    TopTools_ListOfShape aVertices;
    Standard_Integer     aNbVtx = 1;


    aVertices.Append(aFirstVtx);

    for (; anExp.More(); anExp.Next(), ++aNbVtx) {
      anEdge = anExp.Current();
      aCurve = GetCurve(anEdge);

      if (aCurve.IsNull()) {
        return aResult;
      }

      aType = aCurve->DynamicType();

      if (aType != STANDARD_TYPE(Geom_Line)) {
        // The curve is not a line. It is not valid.
        return aResult;
      }

      // Add the current vertex to the list.
      aVertices.Append(anExp.CurrentVertex());
    }

    // Check if the section is closed.
    TopoDS_Vertex aLastVtx = TopExp::LastVertex(anEdge, Standard_True);

    IsClosed = aFirstVtx.IsSame(aLastVtx) ? true : false;

    // Store a last vertex
    if (!IsClosed)
    {
      aVertices.Append(aLastVtx);
      aNbVtx++;
    }

    // Fill the array of points.
    aResult = new TColgp_HArray1OfPnt(1, aNbVtx);

    Standard_Integer i;
    TopTools_ListIteratorOfListOfShape aVtxIter(aVertices);

    for (i = 1; aVtxIter.More(); aVtxIter.Next(), ++i) {
      gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aVtxIter.Value()));

      aResult->SetValue(i, aPnt);
    }
  }

  return aResult;
}
//=======================================================================
// function : FindPlane
// purpose  : 
//=======================================================================
void CurveCreator_Utils::FindPlane
                       (const Handle(TColgp_HArray1OfPnt) &thePoints,
                              gp_Pln                     &thePlane,
                              Standard_Integer           &thePlnStatus)
{
  if (thePoints.IsNull() || thePlnStatus == PLN_FIXED) {
    // The plane can't be defined or is fixed. Nothing to change.
    return;
  }

  Standard_Integer    i;
  const Standard_Real aTolConf = Precision::Confusion();

  for (i = thePoints->Lower(); i <= thePoints->Upper(); ++i) {
    const gp_Pnt &aPnt = thePoints->Value(i);

    switch (thePlnStatus) {
      case PLN_FREE:
        // Fix the origin.
        thePlane.SetLocation(aPnt);
        thePlnStatus = PLN_ORIGIN;
        break;
      case PLN_ORIGIN:
        {
          // Fix origin + OX axis
          const gp_Pnt &aPlnLoc = thePlane.Location();

          if (!aPnt.IsEqual(aPlnLoc, aTolConf)) {
            // Set the X axis.
            gp_Dir aXDir(aPnt.XYZ().Subtracted(aPlnLoc.XYZ()));
            gp_Ax3 aXNorm(aPlnLoc, aXDir);
            gp_Ax3 aNewPlnPos(aPlnLoc, aXNorm.XDirection(), aXNorm.Direction());

            thePlane.SetPosition(aNewPlnPos);
            thePlnStatus = PLN_OX;
          }
        }
        break;
      case PLN_OX:
        {
          // Fix OY axis
          gp_Lin aXLin(thePlane.XAxis());
          Standard_Real aSqrDist = aXLin.SquareDistance(aPnt);

          if (aSqrDist > aTolConf*aTolConf) {
            // Compute main axis.
            const gp_Pnt &aPlnLoc = thePlane.Location();
            gp_Dir        aDir(aPnt.XYZ().Subtracted(aPlnLoc.XYZ()));
            gp_Ax3        aXNorm(aPlnLoc, aXLin.Direction(), aDir);
            gp_Ax3        aNewPlnPos(aPlnLoc, aXNorm.YDirection(),
                                     aXNorm.Direction());

            thePlane.SetPosition(aNewPlnPos);
            thePlnStatus = PLN_FIXED;
            return;
          }
        }
        break;
      default:
        return;
    }
  }
}
