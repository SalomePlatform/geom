using namespace std;
//  File      : GEOM_Sketcher.cxx
//  Created   : Wed Jul 5 10:12:09 2000
//  Author    : Martine LANGLOIS

//  Modified  : Tue Dec 11 21:28:07 2001
//  Author    : Nicolas REJNERI
//  Project   : SALOME
//  Module    : SALOMEGUI
//  Copyright : Open CASCADE
//  $Header$

#include "GEOM_Sketcher.h"
#include "utilities.h"

#include <qstring.h> 

#include <Geom_Axis1Placement.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_CartesianPoint.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <BRepLib.hxx>
#include <TopExp.hxx>
#include <ProjLib.hxx>
#include <Precision.hxx>
#include <ElSLib.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <GccAna_Pnt2dBisec.hxx>
#include <GeomAPI.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <gce_MakeCirc2d.hxx>
#include <GccAna_Circ2d2TanRad.hxx>
#include <GccAna_Circ2d2TanOn.hxx>
#include <GccEnt.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccAna_Lin2dTanPer.hxx>
#include <GccAna_Lin2dTanObl.hxx>
#include <gce_MakeLin2d.hxx>
#include <GCE2d_MakeArcOfCircle.hxx>
#include <Precision.hxx>
#include <ElCLib.hxx>
#include <AIS_Point.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <DsgPrs_ArrowSide.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <AIS_Drawer.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Graphic3d_NameOfFont.hxx>

#include <TopoDS_Wire.hxx>

/*!
  \class GEOM_Sketcher GEOM_Sketcher.h
  \brief ...
*/

Standard_Real resol = 1.0;    

/*!
  Constructor.
*/
Sketch::Sketch()
{

}

/*!
  Destructor.
*/
Sketch::~Sketch()
{

}


/*!
  Constructor.

  \param V3d_Viewer
*/
Sketch::Sketch(const Handle(V3d_Viewer)& aViewer) :
myInteractiveContext(new AIS_InteractiveContext(aViewer)),
myAxisColor(Quantity_Color(Quantity_NOC_YELLOW)),
myCurrentColor(Quantity_Color(Quantity_NOC_GREEN)),
myWireColor(Quantity_Color(Quantity_NOC_RED))
{
	Init();
}

/*!
  Constructor.

  \param V3d_Viewer
  \param Quantity_Color
  \param Quantity_Color
  \param Quantity_Color
*/
Sketch::Sketch(const Handle(V3d_Viewer)& aViewer,
	       const Quantity_Color& anAxisColor,
	       const Quantity_Color& aCurrentColor,
	       const Quantity_Color& aWireColor):
myInteractiveContext(new AIS_InteractiveContext(aViewer)),
myAxisColor(anAxisColor),
myCurrentColor(aCurrentColor),
myWireColor(aWireColor)
{
	Init();
}

/*!
  Build the current edge in a graphic mode.
  The first signature with view coordinates is used to connect to the move event from the user interface.
  The second signature is used when the current point is known by 2d real coordinates.

  \param Xp     
  \param Yp     
  \param V3d_View       
*/
void Sketch::MakeCurrentEdge(const Standard_Integer  Xp   ,
			     const Standard_Integer  Yp   ,
			     const Handle(V3d_View)& aView )
{
  /* 3d coordinates of the picked point */
  Standard_Real Xv,Yv,Zv;
  aView->Convert(Xp,Yp,Xv,Yv,Zv);
  /* computation of real 2d coordinates in plane of sketch */
  Standard_Real Vx,Vy,Vz;
  aView->Proj(Vx,Vy,Vz);
  gp_Dir D(Vx,Vy,Vz);
  gp_Pnt P(Xv,Yv,Zv);
  gp_Lin L(P,D);
  Standard_Real X,Y;
  gp_Pnt Sol;
  IntAna_IntConicQuad Int(L,myPlane->Pln(),Precision::Angular());
  if (Int.IsDone()) {
    if (!Int.IsParallel()) {
      if (Int.NbPoints() > 0 ) {
	Sol = Int.Point(1);
	ElSLib::Parameters(myPlane->Pln(),Sol,X,Y);
      }
    }
  }
  MakeCurrentEdge(X,Y);
}

/*!
  Build the current edge in a graphic mode.
  The first signature with view coordinates is used to connect to the move event from the user interface.
  The second signature is used when the current point is known by 2d real coordinates.

  \param X      
  \param Y      
*/
void Sketch::MakeCurrentEdge(const Standard_Real  X, const Standard_Real Y)
{
  /* Create the current edge depending on the active mode */
  switch (myCurrentStatus) {
  case BEGIN_SKETCH:
    myCurrentEdge = BRepBuilderAPI_MakeVertex(ElCLib::To3d(myPlane->Pln().Position().Ax2(),gp_Pnt2d(X,Y)));
    break;
  case SEGMENT:
    MakeCurrentSegment(X,Y);
    break;
  case ARC_CHORD:
    MakeCurrentSegment(X,Y);
    break;
  case ARC_CHORD_END:
    MakeCurrentArc(X,Y);
    break;
  }
  DisplayCurrentEdge();
}

/*!
  Build the current edge in a graphic mode.
  Function to connect to the input event from the user interface.
*/
void Sketch::ValidateEdge()
{
  gp_Pnt pt;
  gp_Pnt2d pt2d;
  switch (myCurrentStatus) {
  case BEGIN_SKETCH:
    {
      myFirstPointSketch = TopoDS::Vertex(myCurrentEdge);
      myPresentableWire = new AIS_Shape(myFirstPointSketch);
      myPresentableWire->SetColor(myWireColor.Name());
      myInteractiveContext->Display(myPresentableWire);
      pt = BRep_Tool::Pnt(myFirstPointSketch);
      pt2d = ProjLib::Project(myPlane->Pln(),pt);
      myLastX = pt2d.X();
      myLastY = pt2d.Y();
      myCurrentStatus = SEGMENT;
      break;	
    }
  case SEGMENT:
    {
      Standard_Real first,last;
      TopLoc_Location L;
      Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(TopoDS::Edge(myCurrentEdge),myPlane,L,first,last);
      myCurrentEdge = BRepBuilderAPI_MakeEdge2d(Handle(Geom2d_Line)::DownCast(C)->Lin2d(),0.,myLengthDimension->Value());
      if (myTransitionStatus == ANGLE || 
	  myTransitionStatus == LENGTH_FIXED ||
	  myTransitionStatus == X_FIXED ||
	  myTransitionStatus == Y_FIXED) 
	myTransitionStatus = NOCONSTRAINT;
      AddEdgeToWire();
      break;
    }
  case ARC_CHORD:
    {
      myInteractiveContext->CloseLocalContext();
      myInteractiveContext->OpenLocalContext();
      gp_Pnt2d p1 (myLastX,myLastY);
      pt = BRep_Tool::Pnt(TopExp::LastVertex(TopoDS::Edge(myCurrentEdge)));
      gp_Pnt2d p2 = ProjLib::Project(myPlane->Pln(),pt);
      GccAna_Pnt2dBisec ComputeMediatrice(p1,p2);
      if (ComputeMediatrice.HasSolution()) {
	myMediatrice = new Geom2d_Line(ComputeMediatrice.ThisSolution());
	Handle(Geom_Curve) aMediatrice3d = GeomAPI::To3d(myMediatrice,myPlane->Pln());
	myPresentableMediatrice = new AIS_Axis(Handle(Geom_Line)::DownCast(aMediatrice3d));
	myInteractiveContext->Display(myPresentableMediatrice);
      }
      TopoDS_Edge e = BRepBuilderAPI_MakeEdge2d(gce_MakeCirc2d(gp_Pnt2d(0.,0),1.));
      BRepLib::BuildCurve3d(e);
      myLengthDimension->SetText(TCollection_ExtendedString());
      myInteractiveContext->Redisplay(myLengthDimension,Standard_False);
      if (myEdgesNumber == 0)
	myPreviousEdge = TopoDS::Edge(myCurrentEdge);
      pt2d = ProjLib::Project(myPlane->Pln(),pt);
      myLastX = pt2d.X();
      myLastY = pt2d.Y();
      myTransitionStatus = NOCONSTRAINT;
      myCurrentStatus = ARC_CHORD_END;
      break;
    }
  case ARC_CHORD_END:
    myCurrentStatus = ARC_CHORD;
    AddEdgeToWire();	
    break;
  }
}

/*!
  Add edge to current wire on an edge validation .
*/
void Sketch::AddEdgeToWire()
{
  myPreviousEdge = TopoDS::Edge(myCurrentEdge);
  BRepLib::BuildCurve3d(myPreviousEdge);
  myCurrentWire.Add(TopoDS::Edge(myPreviousEdge));
  myEdgesNumber++;
  myPresentableWire->Set( myCurrentWire.Wire() );
  myInteractiveContext->Redisplay(myPresentableWire);
  myConstructionMode.Append(myCurrentStatus);
  myConstraintMode.Append(myTransitionStatus);
  myInteractiveContext->CloseLocalContext();
  gp_Pnt pt;
  if (myPreviousEdge.Orientation() == TopAbs_FORWARD )
    pt = BRep_Tool::Pnt(TopExp::LastVertex(TopoDS::Edge(myPreviousEdge)));
  else 
    pt = BRep_Tool::Pnt(TopExp::FirstVertex(TopoDS::Edge(myPreviousEdge)));
  gp_Pnt2d pt2d= ProjLib::Project(myPlane->Pln(),pt);
  myLastX = pt2d.X();
  myLastY = pt2d.Y();
}

/*!
  Set the numeric dimension for the current edge and validate creation.

  \param aValue 
  \return Standard_Boolean
*/
Standard_Boolean Sketch::SetDimension(Standard_Real& aValue)
{
  fitInResol(aValue); 
  if (myCurrentStatus == SEGMENT ||
      myCurrentStatus == ARC_CHORD){
    Standard_Real first,last;
    TopLoc_Location L;
    Handle(Geom2d_Curve) C = 
      BRep_Tool::CurveOnSurface(TopoDS::Edge(myCurrentEdge),myPlane,L,first,last);
    myCurrentEdge = 
      BRepBuilderAPI_MakeEdge2d(Handle(Geom2d_Line)::DownCast(C)->Lin2d(),0.,aValue);
    DisplayCurrentEdge();
    if (myTransitionStatus == NOCONSTRAINT) {
      mySegmentLength = aValue;
      myTransitionStatus = LENGTH_FIXED;
    }
    else
      ValidateEdge();
    return Standard_True;
  }
  
  else if( myCurrentStatus == ARC_CHORD_END){
    if (myTransitionStatus == TANGENT) return Standard_False;
    gp_Pnt2d p;
    if (myEdgesNumber > 0) {
      if (myPreviousEdge.Orientation() == TopAbs_FORWARD)
	p = ProjLib::Project(myPlane->Pln(),BRep_Tool::Pnt(TopExp::LastVertex(myPreviousEdge)));
      else
	p = ProjLib::Project(myPlane->Pln(),BRep_Tool::Pnt(TopExp::FirstVertex(myPreviousEdge)));
    }
    else
      p = ProjLib::Project(myPlane->Pln(),BRep_Tool::Pnt(myFirstPointSketch));
    GccAna_Circ2d2TanRad aSol(p, gp_Pnt2d(myLastX,myLastY),aValue,Precision::Confusion());
    Standard_Real dist = RealLast();
    if (aSol.NbSolutions() > 0) {
      gp_Circ2d CirSol;
      gp_Pnt2d pc = ProjLib::Project(myPlane->Pln(),BRep_Tool::Pnt(TopoDS::Vertex(myCenterCircle->Shape())));
      for (Standard_Integer i =1; i<= aSol.NbSolutions(); i++) {
	if (pc.Distance(aSol.ThisSolution(i).Location()) < dist)
	  CirSol = aSol.ThisSolution(i);
      }
      if (myCurrentEdge.Orientation() == TopAbs_FORWARD)
	myCurrentEdge = BRepBuilderAPI_MakeEdge2d(aSol.ThisSolution(1),p,gp_Pnt2d(myLastX,myLastY));
      else {	
	myCurrentEdge = BRepBuilderAPI_MakeEdge2d(aSol.ThisSolution(1),gp_Pnt2d(myLastX,myLastY),p);
	myCurrentEdge.Reverse();
      }	
      DisplayCurrentEdge();
      ValidateEdge();	
      return Standard_True;
    }
  }
  
  return Standard_False;
}

/*!
  Set the numeric dimension for the current edge and validate creation.
  
  \param deltaX 
  \param deltaY 
*/
void Sketch::SetDimension(Standard_Real& deltaX, Standard_Real& deltaY)
{
  fitInResol(deltaX); 
  fitInResol(deltaY);
  Standard_Real X = myLastX + deltaX;
  Standard_Real Y = myLastY + deltaY;
  MakeCurrentEdge(X,Y);
  ValidateEdge();
}

/*!
  Set the numeric value of the X coordinate of current point giving a deltaX relative to previous point.

  \param deltaX 
*/
void Sketch::SetXDimension(Standard_Real& deltaX)
{
  fitInResol(deltaX); 
  Standard_Real X = myLastX + deltaX;
  Standard_Real Y = myLastY;
  if ( deltaX == 0. )
    Y = Y + 100.0 * Precision::Confusion();

  if (myTransitionStatus == NOCONSTRAINT) {
    MakeCurrentEdge(X,Y);
    myTransitionStatus = X_FIXED;
    mySegmentX = X;
  }
  else if (myTransitionStatus == Y_FIXED) {
    myTransitionStatus = NOCONSTRAINT;
    MakeCurrentEdge(X,mySegmentY);
    ValidateEdge();
  }
  else if (myTransitionStatus == ANGLE) {
    myTransitionStatus = NOCONSTRAINT;
    Standard_Real angle;
    if (0 <= mySegmentAngle &&  mySegmentAngle<= PI )
      angle = PI - mySegmentAngle;
    else
      angle = mySegmentAngle - PI;
    Y = X*Tan(angle);
    MakeCurrentEdge(X,Y);
    ValidateEdge();
  }
  else
    myTransitionStatus = NOCONSTRAINT;
}

/*!
  Set the numeric value of the Y coordinate of current point giving a deltaY relative to previous point.

  \param deltaY 
*/
void Sketch::SetYDimension(Standard_Real& deltaY)
{
  fitInResol(deltaY); 
  Standard_Real X = myLastX;
  Standard_Real Y = myLastY + deltaY;

  if ( deltaY == 0. )
    X = X + 100.0 * Precision::Confusion();

  if (myTransitionStatus == NOCONSTRAINT) {
    MakeCurrentEdge(X,Y);
    myTransitionStatus = Y_FIXED;
    mySegmentY = Y;
  }
  else if (myTransitionStatus == X_FIXED) {
    myTransitionStatus = NOCONSTRAINT;
    MakeCurrentEdge(mySegmentX,Y);
    ValidateEdge();
  }
  else if (myTransitionStatus == ANGLE) {
    myTransitionStatus = NOCONSTRAINT;
    Standard_Real angle;
    if (0 <= mySegmentAngle &&  mySegmentAngle<= PI )
      angle = PI - mySegmentAngle;
    else
      angle = mySegmentAngle - PI;
    X = Y/Tan(angle);
    MakeCurrentEdge(X,Y);
    ValidateEdge();
  }
  else
    myTransitionStatus = NOCONSTRAINT;
}

/*!
  Set the numeric value of angle between 2 segments.

  \param aValue 
*/
void Sketch::SetSegmentAngle(Standard_Real& aValue)
{
  if (myEdgesNumber > 0) {
    Standard_Real First,Last;
    TopLoc_Location L;
    Standard_Real angle;
    if (0 <= aValue &&  aValue<= PI )
      angle = PI - aValue;
    else
      angle = aValue - PI;
    Handle (Geom2d_Curve) PreviousCurve = BRep_Tool::CurveOnSurface(myPreviousEdge,myPlane,L,First,Last);
    if (PreviousCurve->IsKind(STANDARD_TYPE(Geom2d_Line))) {
      Handle (Geom2d_Curve) PreviousCurve = BRep_Tool::CurveOnSurface(TopoDS::Edge(myPreviousEdge),myPlane,L,First,Last);
      
      GccAna_Lin2dTanObl aSol(gp_Pnt2d(myLastX,myLastY),Handle(Geom2d_Line)::DownCast(PreviousCurve)->Lin2d(),angle);
      myCurrentEdge = BRepBuilderAPI_MakeEdge2d(aSol.ThisSolution(1),0.,myLengthDimension->Value());
    }
    if (myTransitionStatus == LENGTH_FIXED)  {
      ValidateEdge();
    }
    else if (myTransitionStatus == X_FIXED)  {
      Standard_Real length = mySegmentX/Cos(angle);
      SetDimension(length);
      ValidateEdge();
    }
    else if (myTransitionStatus == Y_FIXED)  {
      Standard_Real length = mySegmentY/Sin(angle);
      SetDimension(length);
      ValidateEdge();
    }
    else {
      mySegmentAngle = aValue;
      myTransitionStatus = ANGLE;
    }
  }
}

/*!
  Get the angle value between 2 segments.

  \return Standard_Real
*/
Standard_Real Sketch::GetSegmentAngle()
{
  return mySegmentAngle;
}

/*!
  Close automatically an open sketch.

  \return TopoDS_Wire.  Return null shape if not possible.
*/
TopoDS_Wire Sketch::Close()
{ 
  myCurrentStatus = END_SKETCH;
  myInteractiveContext->CloseAllContexts();
  myInteractiveContext->EraseAll(Standard_False);
  if (myEdgesNumber >= 2) {
    BRepTools_WireExplorer Ex(myCurrentWire.Wire());
    TopoDS_Vertex V1;
    if (myPreviousEdge.Orientation() == TopAbs_FORWARD)
      V1 = TopExp::LastVertex(myPreviousEdge);
    else
      V1 = TopExp::FirstVertex(myPreviousEdge);
    myCurrentWire.Add(BRepBuilderAPI_MakeEdge(V1,myFirstPointSketch).Edge());
    myEdgesNumber++;
    return myCurrentWire.Wire();
  }
  else
    return TopoDS_Wire();
}



/*!
  Clear sketch presentation.
*/
void Sketch::Clear()
{
  myCurrentStatus = END_SKETCH;
  myInteractiveContext->CloseAllContexts();
  myInteractiveContext->EraseAll(Standard_False);
}

/*!
  Terminate sketch without closing.

  \return TopoDS_Wire. Return null shape if not possible.
*/
TopoDS_Wire Sketch::End()
{
  myCurrentStatus = END_SKETCH;
  myInteractiveContext->CloseAllContexts();
  myInteractiveContext->EraseAll(Standard_False);
  if (myCurrentWire.IsDone()) {
    return myCurrentWire.Wire();
  }
  else
    return TopoDS_Wire();
}


/*!
  Delete current edge.
*/
Standard_Boolean Sketch::Delete()
{
  myInteractiveContext->Erase(myAngleDimension,Standard_True,Standard_False);
  myInteractiveContext->Erase(myLengthDimension,Standard_True,Standard_False);
  myInteractiveContext->Erase(myRadiusDimension,Standard_True,Standard_False);
  myInteractiveContext->Erase(myCenterCircle,Standard_True,Standard_False);
  myInteractiveContext->Erase(myXDimension,Standard_True,Standard_False);
  myInteractiveContext->Erase(myYDimension,Standard_True,Standard_False);
  
  if (myCurrentStatus == BEGIN_SKETCH) {
    myCurrentStatus = END_SKETCH;
    myInteractiveContext->CloseAllContexts();
    myInteractiveContext->EraseAll(Standard_False);
    return true;
  }
  else if(myCurrentStatus == SEGMENT   ||
	  myCurrentStatus == ARC_CHORD ) {
    RemoveLastEdge();
  }
  else if(myCurrentStatus ==  ARC_CHORD_END) {
    myCurrentStatus = ARC_CHORD;
    myInteractiveContext->CloseAllContexts();
  }
  gp_Pnt pt;
  if (myEdgesNumber == 0) {
    //myInteractiveContext->EraseAll(Standard_False);
    ChangeMode(BEGIN_SKETCH); // DCQ
    pt = BRep_Tool::Pnt(myFirstPointSketch);
  }
  else {
    if (myPreviousEdge.Orientation() == TopAbs_FORWARD )
      pt = BRep_Tool::Pnt(TopExp::LastVertex(TopoDS::Edge(myPreviousEdge)));
    else 
      pt = BRep_Tool::Pnt(TopExp::FirstVertex(TopoDS::Edge(myPreviousEdge)));
  }
  gp_Pnt2d pt2d= ProjLib::Project(myPlane->Pln(),pt);
  myLastX = pt2d.X();
  myLastY = pt2d.Y();

  return false;
}

/*!
  Set a specific plane for sketch.

  \param GeomyPlane     
*/
void Sketch::SetPlane(const Handle(Geom_Plane)& aPlane)
{
  myPlane = aPlane;
}

/*!
  Set display parameters.

  \param aColor 
*/
void Sketch::SetWireColor(const Quantity_Color& aColor)
{
  myWireColor = aColor;
}

/*!
  Set display parameters.

  \param aColor 
*/
void Sketch::SetCurrentColor(const Quantity_Color& aColor)
{
  myCurrentColor = aColor;
}

/*!
  Set display parameters.
  
  \param aColor 
*/
void Sketch::SetAxisColor(const Quantity_Color& aColor)
{
  myAxisColor = aColor;
}


/*!
  Change mode of construction line.

  \param aMode : SEGMENT, ARC_CHORD.
*/
void Sketch::ChangeMode(const SketchStatus aMode)
{
  gp_Pnt2d p;
  if (myEdgesNumber > 0) {
    if (myPreviousEdge.Orientation() == TopAbs_FORWARD)
      p = ProjLib::Project(myPlane->Pln(),BRep_Tool::Pnt(TopExp::LastVertex(myPreviousEdge)));
    else
      p = ProjLib::Project(myPlane->Pln(),BRep_Tool::Pnt(TopExp::FirstVertex(myPreviousEdge)));
    myLastX = p.X();
    myLastY = p.Y();
    myInteractiveContext->CloseLocalContext(myInteractiveContext->IndexOfCurrentLocal());
  }
  if (!myCurrentStatus == BEGIN_SKETCH) 
    myCurrentStatus = aMode; /* change the mode only when the sketch is not in state BEGIN_SKETCH, i.d. fist point has been fixed */

}

/*!
  Set transition constraint between consecutive edges.

  \param aStatus : NOCONSTRAINT, TANGENT, PERPENDICULAR, ANGLE, LENGTH_FIXED, X_FIXED, Y_FIXED.
*/
void Sketch::SetTransitionStatus(const TransitionStatus aStatus)
{
  myTransitionStatus = aStatus;
}

/*!
  Set or unset the display of dimensions.

  \param atype  
  \param OnOff  
*/
void Sketch::SetParameterVisibility(const TypeOfParameter atype, const Standard_Boolean OnOff)
{
  switch (atype) {
  case ANGLE_PARAMETER:
    myIsAngleDimensionVisible = OnOff;
    if (!myIsAngleDimensionVisible && !myAngleDimension.IsNull())
      myInteractiveContext->Erase(myAngleDimension,Standard_True,Standard_False);
    //else DCQ
    //  DisplayCurrentEdge();
    break;
  case LENGTH_PARAMETER:
    myIsLengthDimensionVisible = OnOff;
    if (!myIsLengthDimensionVisible&& !myLengthDimension.IsNull())
      myInteractiveContext->Erase(myLengthDimension,Standard_True,Standard_False);
    //else DCQ
    //  DisplayCurrentEdge();
    break;
  case RADIUS_PARAMETER:
    myIsRadiusDimensionVisible = OnOff;
    if (!myIsRadiusDimensionVisible&& !myRadiusDimension.IsNull()){
      myInteractiveContext->Erase(myRadiusDimension,Standard_True,Standard_False);
      myInteractiveContext->Erase(myCenterCircle,Standard_True,Standard_False);
    }
    //else DCQ
    //  DisplayCurrentEdge();
    break;
  case XVALUE_PARAMETER:
    myIsXDimensionVisible = OnOff;
    if (!myIsXDimensionVisible&& !myXDimension.IsNull())
      myInteractiveContext->Erase(myXDimension,Standard_True,Standard_False);
    break;
  case YVALUE_PARAMETER:
    myIsYDimensionVisible = OnOff;
    if (!myIsYDimensionVisible&& !myYDimension.IsNull())
      myInteractiveContext->Erase(myYDimension,Standard_True,Standard_False);
    break;
  }
}

/*!
  Hilight parameters.

  \param atype  
  \param acolor 
*/
void Sketch::HiligthWithColor(const TypeOfParameter atype, const Quantity_NameOfColor acolor)
{
  switch (atype) {
  case ANGLE_PARAMETER:
    myInteractiveContext->HilightWithColor(myAngleDimension, acolor);
    break;
  case LENGTH_PARAMETER:
    myInteractiveContext->HilightWithColor(myLengthDimension, acolor);
    break;
  case RADIUS_PARAMETER:
    myInteractiveContext->HilightWithColor(myRadiusDimension, acolor);
    break;
  case XVALUE_PARAMETER:
    myInteractiveContext->HilightWithColor(myXDimension, acolor);
    break;
  case YVALUE_PARAMETER:
    myInteractiveContext->HilightWithColor(myYDimension, acolor);
    break;
  }
}

	
/*!
  Unhilight parameters.

  \param atype  
*/
void Sketch::Unhiligth(const TypeOfParameter atype)
{
  switch (atype) {
  case ANGLE_PARAMETER:
    myInteractiveContext->Unhilight(myAngleDimension);
    break;
  case LENGTH_PARAMETER:
    myInteractiveContext->Unhilight(myLengthDimension);
    break;
  case RADIUS_PARAMETER:
    myInteractiveContext->Unhilight(myRadiusDimension);
    break;
  case XVALUE_PARAMETER:
    myInteractiveContext->Unhilight(myXDimension);
    break;
  case YVALUE_PARAMETER:
    myInteractiveContext->Unhilight(myYDimension);
    break;
  }
}

/*!
  Check if the edition of a type of parameter is relevant depending on sketch current status.

  \param atype  
  \return Standard_Boolean
*/
Standard_Boolean Sketch::IsValidCurrentParameter(const TypeOfParameter atype)
{
  switch (atype) {
  case ANGLE_PARAMETER:
    if (myCurrentStatus != SEGMENT && myCurrentStatus != ARC_CHORD) 
      return Standard_False;
    else if (myTransitionStatus == TANGENT || myTransitionStatus == PERPENDICULAR)  
      return Standard_False;
    else if (myEdgesNumber < 1)
      return Standard_False;
    else  {
      TopLoc_Location L;
      Standard_Real First,Last;
      Handle (Geom2d_Curve) PreviousCurve = BRep_Tool::CurveOnSurface(myPreviousEdge,myPlane,L,First,Last);
      if (!PreviousCurve->IsKind(STANDARD_TYPE(Geom2d_Line))) 
	return Standard_False;
    }
    break;
  case LENGTH_PARAMETER:
    if (myCurrentStatus != SEGMENT   && 
	myCurrentStatus != ARC_CHORD /*&&
					  myCurrentStatus != CURVE_POINTS*/
	) 
      return Standard_False;
    else if (myTransitionStatus == LENGTH_FIXED) 
      return Standard_False;
    break;
  case RADIUS_PARAMETER:
    if (myCurrentStatus != ARC_CHORD_END)
      return Standard_False;
    break;
  case XVALUE_PARAMETER:
    if (myCurrentStatus != SEGMENT   && 
	myCurrentStatus != ARC_CHORD /*&&
					  myCurrentStatus != CURVE_POINTS*/
	) 
      return Standard_False;
    else if (myTransitionStatus == X_FIXED) 
      return Standard_False;
    break;
  case YVALUE_PARAMETER:
    if (myCurrentStatus != SEGMENT   && 
	myCurrentStatus != ARC_CHORD /*&&
					  myCurrentStatus != CURVE_POINTS*/
	) 
      return Standard_False;
    else if (myTransitionStatus == Y_FIXED) 
      return Standard_False;
    break;
  }
  return Standard_True;
}

/*!
  Set a parameter value.

  \param atype  
  \param aValue 
*/
void Sketch::SetParameterValue(const TypeOfParameter atype, Standard_Real aValue)
{
  switch (atype) {
  case ANGLE_PARAMETER:
    SetSegmentAngle(aValue);
    break;
  case LENGTH_PARAMETER:
    SetDimension(aValue);
    break;
  case RADIUS_PARAMETER:
    SetDimension(aValue);
    break;
  case XVALUE_PARAMETER:
    SetXDimension(aValue);
    break;
  case YVALUE_PARAMETER:
    SetYDimension(aValue);
    break;
  }
}

/*!
  Initialisation of sketch parameters or options.
*/
void Sketch::Init()
{
  myPlane = new Geom_Plane (0.,0.,1.,0.);
  CreateConstraints();
  BRepLib::Plane(myPlane);
  myEdgesNumber = 0;
  myCurrentStatus = BEGIN_SKETCH;
  /* In order to update the visulisation of current objects by using Redisplay method from InteractiveContext */
  myCurrentEdge = BRepBuilderAPI_MakeVertex(gp_Pnt(0.,0.,0.));		
  myPresentableEdge = new AIS_Shape(myCurrentEdge);
  myPresentableEdge->SetColor(myCurrentColor.Name());
  myInteractiveContext->Display(myPresentableEdge);
  myTransitionStatus = NOCONSTRAINT;
  /* Init for display objects */
  TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(0.,0.,0.));
  TopoDS_Vertex V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(10.,0.,0.));
  myLengthDimension = new AIS_LengthDimension (V1,V2,myPlane,0.,TCollection_ExtendedString());
  myXDimension = new AIS_LengthDimension (V1,V2,myPlane,0.,TCollection_ExtendedString(),gp_Pnt(),DsgPrs_AS_NONE,
					  AIS_TOD_Horizontal);
  myYDimension = new AIS_LengthDimension (V1,V2,myPlane,0.,TCollection_ExtendedString(),gp_Pnt(),DsgPrs_AS_NONE,
					  AIS_TOD_Vertical);
  myRadiusDimension = new AIS_LengthDimension (V1,V2,myPlane,0.,TCollection_ExtendedString());
  myCenterCircle = new AIS_Shape(V1);
  myIsLengthDimensionVisible = Standard_False;
  myIsXDimensionVisible = Standard_False;
  myIsYDimensionVisible = Standard_False;
  myIsRadiusDimensionVisible = Standard_False;
}

/*!
  Build the current segment.

  \param X      
  \param Y      
*/
void Sketch::MakeCurrentSegment(Standard_Real X, Standard_Real Y)
{
  if (myTransitionStatus == NOCONSTRAINT) 
    myCurrentEdge = BRepBuilderAPI_MakeEdge2d(gp_Pnt2d(myLastX,myLastY), gp_Pnt2d(X,Y));
  else if (myTransitionStatus == X_FIXED) 
    myCurrentEdge = BRepBuilderAPI_MakeEdge2d(gp_Pnt2d(myLastX,myLastY),gp_Pnt2d(mySegmentX,Y));
  else if (myTransitionStatus == Y_FIXED) 
    myCurrentEdge = BRepBuilderAPI_MakeEdge2d(gp_Pnt2d(myLastX,myLastY),gp_Pnt2d(X,mySegmentY));
  else if (myTransitionStatus == TANGENT && myEdgesNumber > 0) {
    Standard_Real first,last;
    TopLoc_Location L;
    Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(myPreviousEdge,myPlane,L,first,last);
    gp_Pnt2d p1;
    gp_Vec2d Vt;
    if (myPreviousEdge.Orientation() == TopAbs_FORWARD)
      C->D1(last,p1,Vt);
    else 
      C->D1(first,p1,Vt);
    gp_Lin2d aline(p1,Vt);
    Geom2dAPI_ProjectPointOnCurve proj(gp_Pnt2d(X,Y),new Geom2d_Line(aline));
    if (proj.NbPoints() > 0)
      myCurrentEdge = BRepBuilderAPI_MakeEdge2d(p1,proj.Point(1));
  }
  else if (myTransitionStatus == PERPENDICULAR && myEdgesNumber > 0) {
    Standard_Real first,last;
    TopLoc_Location L;
    Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(myPreviousEdge,myPlane,L,first,last);
    gp_Pnt2d p1;
    gp_Lin2d perpen;
    if (myPreviousEdge.Orientation() == TopAbs_FORWARD)
      C->D0(last,p1);
    else 
      C->D0(first,p1);
    if (C->IsKind(STANDARD_TYPE(Geom2d_Line))) {
      GccAna_Lin2dTanPer aSol(p1,Handle(Geom2d_Line)::DownCast(C)->Lin2d());
      perpen = aSol.ThisSolution(1);
    }
    else if (C->IsKind(STANDARD_TYPE(Geom2d_Circle))) {
      GccAna_Lin2dTanPer aSol(p1,Handle(Geom2d_Circle)::DownCast(C)->Circ2d());
      perpen = aSol.ThisSolution(1);
    }
    Geom2dAPI_ProjectPointOnCurve proj(gp_Pnt2d(X,Y),new Geom2d_Line(perpen));
    if (proj.NbPoints() > 0)
      myCurrentEdge = BRepBuilderAPI_MakeEdge2d(p1,proj.Point(1));
  }
  else if (myTransitionStatus == ANGLE && myEdgesNumber > 0) {
    Standard_Real First,Last;
    TopLoc_Location L;
    Handle (Geom2d_Curve) PreviousCurve = BRep_Tool::CurveOnSurface(myPreviousEdge,myPlane,L,First,Last);
    if (PreviousCurve->IsKind(STANDARD_TYPE(Geom2d_Line))) {
      Handle (Geom2d_Curve) PreviousCurve = BRep_Tool::CurveOnSurface(TopoDS::Edge(myPreviousEdge),myPlane,L,First,Last);
      Standard_Real angle;
      if (0 <= mySegmentAngle &&  mySegmentAngle<= PI )
	angle = PI - mySegmentAngle;
      else
	angle = mySegmentAngle - PI;
      GccAna_Lin2dTanObl aSol(gp_Pnt2d(myLastX,myLastY),Handle(Geom2d_Line)::DownCast(PreviousCurve)->Lin2d(),angle);
      Standard_Real dist = RealLast();
      gp_Pnt2d pt(X,Y),ptproj;
      for (Standard_Integer i =1; i<=aSol.NbSolutions(); i++) {
	Geom2dAPI_ProjectPointOnCurve proj(pt,new Geom2d_Line(aSol.ThisSolution(i)));
	if (pt.Distance(proj.Point(1)) < dist) {
	  dist = pt.Distance(proj.Point(1));
	  ptproj = proj.Point(1);
	}
      }
      myCurrentEdge = BRepBuilderAPI_MakeEdge2d(gp_Pnt2d(myLastX,myLastY),ptproj);
    }
  }
  else if (myTransitionStatus == LENGTH_FIXED && myEdgesNumber > 0) {
    Standard_Real First,Last;
    TopLoc_Location L;
    Handle (Geom2d_Curve) PreviousCurve = BRep_Tool::CurveOnSurface(myPreviousEdge,myPlane,L,First,Last);
    if (PreviousCurve->IsKind(STANDARD_TYPE(Geom2d_Line))) {
      Handle (Geom2d_Curve) PreviousCurve = BRep_Tool::CurveOnSurface(TopoDS::Edge(myPreviousEdge),myPlane,L,First,Last);
      gp_Lin2d aline = gce_MakeLin2d(gp_Pnt2d(myLastX,myLastY), gp_Pnt2d(X,Y));
      myCurrentEdge = BRepBuilderAPI_MakeEdge2d(aline,0.,mySegmentLength);
    }
  }
}

/*!
  Build the current arc.

  \param X      
  \param Y      
*/
void Sketch::MakeCurrentArc(Standard_Real X, Standard_Real Y)
{
  gp_Circ2d CircSol;
  Standard_Boolean OK(Standard_False);

  if (myTransitionStatus == NOCONSTRAINT) {
    GccAna_Circ2d2TanOn aSol(gp_Pnt2d(myLastX,myLastY), gp_Pnt2d(X,Y),myMediatrice->Lin2d(),Precision::Confusion());
    if (aSol.NbSolutions() > 0){
      CircSol = aSol.ThisSolution(1);
      OK = Standard_True;
    }
  }
  /* Tangency with previous edge */
  else if (myTransitionStatus == TANGENT && myEdgesNumber > 0) {
    Standard_Real first,last;
    TopLoc_Location L;
    Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(myPreviousEdge,myPlane,L,first,last);
    if (C->IsKind(STANDARD_TYPE(Geom2d_Line))){
      GccAna_Circ2d2TanOn aSol(GccEnt::Unqualified(Handle(Geom2d_Line)::DownCast(C)->Lin2d()), gp_Pnt2d(myLastX,myLastY),myMediatrice->Lin2d(),Precision::Confusion());
      if (aSol.NbSolutions() > 0){
	CircSol = aSol.ThisSolution(1);
	OK = Standard_True;
      }
    }
    else if (C->IsKind(STANDARD_TYPE(Geom2d_Circle))) {
      GccAna_Circ2d2TanOn aSol(GccEnt::Unqualified(Handle(Geom2d_Circle)::DownCast(C)->Circ2d()), gp_Pnt2d(myLastX,myLastY),myMediatrice->Lin2d(),Precision::Confusion());
      if (aSol.NbSolutions() > 0){
	CircSol = aSol.ThisSolution(1);
	OK = Standard_True;
      }
    }
    else if(C->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
      gp_Pnt2d pc;
      gp_Vec2d Vt;
      C->D1(last,pc,Vt);
      gp_Lin2d alin2d(pc,gp_Dir2d(Vt));
      GccAna_Circ2d2TanOn aSol(GccEnt::Unqualified(alin2d), gp_Pnt2d(myLastX,myLastY),myMediatrice->Lin2d(),Precision::Confusion());
      if (aSol.NbSolutions() > 0){
	CircSol = aSol.ThisSolution(1);
	OK = Standard_True;
      }
    }
  }
  /* Tangency with the perpendicular to the previous edge */
  else if (myTransitionStatus == PERPENDICULAR && myEdgesNumber > 0) {
    Standard_Real first,last;
    TopLoc_Location L;
    Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(myPreviousEdge,myPlane,L,first,last);
    gp_Pnt2d p1;
    gp_Lin2d perpen;
    if (myPreviousEdge.Orientation() == TopAbs_FORWARD)
      C->D0(last,p1);
    else 
      C->D0(first,p1);
    if (C->IsKind(STANDARD_TYPE(Geom2d_Line))) {
      GccAna_Lin2dTanPer aSol(p1,Handle(Geom2d_Line)::DownCast(C)->Lin2d());
      perpen = aSol.ThisSolution(1);
    }
    else if (C->IsKind(STANDARD_TYPE(Geom2d_Circle))) {
      GccAna_Lin2dTanPer aSol(p1,Handle(Geom2d_Circle)::DownCast(C)->Circ2d());
      perpen = aSol.ThisSolution(1);
    }
    GccAna_Circ2d2TanOn aSol(GccEnt::Unqualified(perpen), gp_Pnt2d(myLastX,myLastY),myMediatrice->Lin2d(),Precision::Confusion());
    if (aSol.NbSolutions() > 0){
      CircSol = aSol.ThisSolution(1);
      OK = Standard_True;
    }
  }

  gp_Pnt2d p;
  if (myEdgesNumber > 0) {
    if (myPreviousEdge.Orientation() == TopAbs_FORWARD)
      p = ProjLib::Project(myPlane->Pln(),BRep_Tool::Pnt(TopExp::LastVertex(myPreviousEdge)));
    else
      p = ProjLib::Project(myPlane->Pln(),BRep_Tool::Pnt(TopExp::FirstVertex(myPreviousEdge)));
  }
  else
    p =  ProjLib::Project(myPlane->Pln(),BRep_Tool::Pnt(myFirstPointSketch));
  if (OK){
    gp_Vec2d V1(p,gp_Pnt2d(X,Y));
    gp_Vec2d V2(p,gp_Pnt2d(myLastX,myLastY));
    if (V1.Angle(V2) > 0 )
      myCurrentEdge = BRepBuilderAPI_MakeEdge2d(CircSol,p,gp_Pnt2d(myLastX,myLastY));
    else {	
      myCurrentEdge = BRepBuilderAPI_MakeEdge2d(CircSol,gp_Pnt2d(myLastX,myLastY),p);
      myCurrentEdge.Reverse();
    }
  }
  else {
    myCurrentStatus = ARC_CHORD;
    myLastX = p.X();
    myLastY = p.Y();
    myInteractiveContext->CloseLocalContext();
  }

}

/*!
  Display the current edge under construction with it's dimension.
*/
void Sketch::DisplayCurrentEdge()
{
  myPresentableEdge->Set(myCurrentEdge);
  myInteractiveContext->Redisplay(myPresentableEdge);
  if (myCurrentStatus == SEGMENT ||
      myCurrentStatus == ARC_CHORD ) {
    /* Length dimension */
    TopoDS_Vertex V1 = TopExp::FirstVertex(TopoDS::Edge(myCurrentEdge));
    TopoDS_Vertex V2 = TopExp::LastVertex(TopoDS::Edge(myCurrentEdge));
    DisplayLengthDimension(V1,V2);
    /* Angular dimension */
    DisplayAngleDimension();
    DisplayXDimension(V1,V2);
    DisplayYDimension(V1,V2);
  }
  else if (myCurrentStatus == ARC_CHORD_END ) 
    DisplayRadiusDimension();
  else {
    TopoDS_Vertex V1 = TopoDS::Vertex(myCurrentEdge);
    TopoDS_Vertex V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(0.,0.,0.));
    DisplayXDimension(V1,V2);
    DisplayYDimension(V1,V2);
  }
    
}

/*!
  Display the current length dimension.

  \param V1     
  \param V2     
*/
void Sketch::DisplayLengthDimension(const TopoDS_Vertex& V1,const TopoDS_Vertex& V2)
{
  gp_Pnt p1 = BRep_Tool::Pnt(TopoDS::Vertex(V1));
  gp_Pnt p2 = BRep_Tool::Pnt(TopoDS::Vertex(V2));
  Standard_Real length = p1.Distance(p2);
  if (length <= Precision::Confusion()) return;
  myLengthDimension->SetFirstShape(V1);
  myLengthDimension->SetSecondShape(V2);
  fitInResol(length);
  myLengthDimension->SetValue(length);
  QString S;
  S.sprintf("%.1f",length);
  myLengthDimension->SetText(TCollection_ExtendedString(strdup(S)));
  if (myIsLengthDimensionVisible) {
    if (myInteractiveContext->IsDisplayed(myLengthDimension))
      myInteractiveContext->Redisplay(myLengthDimension);
    else
      myInteractiveContext->Display(myLengthDimension);
  }
}

/*!
  Display the current X dimension.

  \param V1     
  \param V2     
*/
void Sketch::DisplayXDimension(const TopoDS_Vertex& V1,const TopoDS_Vertex& V2)
{
  if (myTransitionStatus != X_FIXED) {
  gp_Pnt p1 = BRep_Tool::Pnt(TopoDS::Vertex(V1));
  gp_Pnt p2 = BRep_Tool::Pnt(TopoDS::Vertex(V2));

  gp_Lin aline(p1,myPlane->Pln().XAxis().Direction());
  GeomAPI_ProjectPointOnCurve proj(p2,new Geom_Line(aline));

  gp_Pnt pb = p1;

  if (proj.NbPoints() > 0) {
    Standard_Real length = p1.Distance(proj.Point(1));
    if (length <= Precision::Confusion()) return;
    myXDimension->SetFirstShape(V1);
    myXDimension->SetSecondShape(V2);
    fitInResol(length);
    myXDimension->SetValue(length);
    QString S;
    S.sprintf("%.1f",length);
    myXDimension->SetText(TCollection_ExtendedString(strdup(S)));
    //    myXDimension->SetPosition(proj.Point(1));
    pb.BaryCenter(5,proj.Point(1),5);
    myXDimension->SetPosition(pb);
    if (myIsXDimensionVisible) {
      if (myInteractiveContext->IsDisplayed(myXDimension))
	myInteractiveContext->Redisplay(myXDimension);
      else
	myInteractiveContext->Display(myXDimension);
    }
  }
  } else
    myInteractiveContext->Erase(myXDimension,Standard_True,Standard_False);
}

/*!
  Display the current Y dimension.

  \param V1     
  \param V2     
*/
void Sketch::DisplayYDimension(const TopoDS_Vertex& V1,const TopoDS_Vertex& V2)
{
  if (myTransitionStatus != Y_FIXED) {

  gp_Pnt p1 = BRep_Tool::Pnt(TopoDS::Vertex(V1));
  gp_Pnt p2 = BRep_Tool::Pnt(TopoDS::Vertex(V2));
  gp_Lin aline(p2 /*p1*/, myPlane->Pln().YAxis().Direction());
  gp_Pnt pb = p2 /*p1*/;
  GeomAPI_ProjectPointOnCurve proj(p1 /*p2*/,new Geom_Line(aline));
  if (proj.NbPoints() > 0) {
    Standard_Real length = /*p1*/ p2.Distance(proj.Point(1));
    if (length <= Precision::Confusion()) return;
    myYDimension->SetFirstShape(V1);
    myYDimension->SetSecondShape(V2);
    fitInResol(length);
    myYDimension->SetValue(length);
    QString S;
    S.sprintf("%.1f",length);
    myYDimension->SetText(TCollection_ExtendedString(strdup(S)));
    pb.BaryCenter(5,proj.Point(1),5);
    myYDimension->SetPosition(pb);
    //    myYDimension->SetPosition(p2);
    if (myIsYDimensionVisible) {
      if (myInteractiveContext->IsDisplayed(myYDimension))
	myInteractiveContext->Redisplay(myYDimension);
      else
	myInteractiveContext->Display(myYDimension);
    }
  }
  } else
    myInteractiveContext->Erase(myYDimension,Standard_True,Standard_False);
}

/*!
  Display the current angle dimension.
*/
void Sketch::DisplayAngleDimension()
{
  if (!myIsAngleDimensionVisible)
    return;
  if (myEdgesNumber > 0) {
    Standard_Real First,Last;
    TopLoc_Location L;
    Handle (Geom2d_Curve) PreviousCurve = BRep_Tool::CurveOnSurface(myPreviousEdge,myPlane,L,First,Last);
    Handle (Geom2d_Curve) CurrentCurve = BRep_Tool::CurveOnSurface(TopoDS::Edge(myCurrentEdge),myPlane,L,First,Last);
    if (PreviousCurve->IsKind(STANDARD_TYPE(Geom2d_Line)) && CurrentCurve->IsKind(STANDARD_TYPE(Geom2d_Line))) {
      Standard_Real angle = Handle(Geom2d_Line)::DownCast(PreviousCurve)->Lin2d().Angle(Handle(Geom2d_Line)::DownCast(CurrentCurve)->Lin2d());
      gp_Pnt2d apos;
      if (0 <= angle && angle<= PI) 
	angle = PI - angle;
      else 
	angle = PI + angle;
      CurrentCurve->D0((First+Last)/5.,apos);
      gp_Pnt apos3d = ElCLib::To3d(myPlane->Pln().Position().Ax2(),apos);
      Standard_Real angtext = angle*180./PI;
      mySegmentAngle = angle;
      BRepLib::BuildCurve3d(TopoDS::Edge(myCurrentEdge));
      fitInResol(angtext);
			
      QString S;
      S.sprintf("%.1f",angtext);
      if (myInteractiveContext->IndexOfCurrentLocal() == 0) {
	myInteractiveContext->OpenLocalContext();
	myAngleDimension = new AIS_AngleDimension(myPreviousEdge,TopoDS::Edge(myCurrentEdge),myPlane,angle,
						  TCollection_ExtendedString(strdup(S)));
	myInteractiveContext->Display(myAngleDimension);
      }
      else {
	myAngleDimension->SetSecondShape(myCurrentEdge);
	myAngleDimension->SetValue(angle);
	myAngleDimension->SetPosition(apos3d);
	myAngleDimension->SetText(TCollection_ExtendedString(strdup(S)));
	myInteractiveContext->Redisplay(myAngleDimension);
      }
    }
  }
}

/*!
  Display the current radius dimension.
*/
void Sketch::DisplayRadiusDimension()
{
  if (! myIsRadiusDimensionVisible)
    return;
  BRepLib::BuildCurve3d(TopoDS::Edge(myCurrentEdge));
  Standard_Real First,Last;
  Handle (Geom_Circle) C = Handle (Geom_Circle)::DownCast(BRep_Tool::Curve(TopoDS::Edge(myCurrentEdge),First,Last));
  if (!C.IsNull()) {
    Standard_Real R = C->Radius();
    TopoDS_Shape V1 = BRepBuilderAPI_MakeVertex(C->Location());
    gp_Pnt MidlePoint ;
    C->D0((First+Last)/2.,MidlePoint);
    TopoDS_Vertex V2 = BRepBuilderAPI_MakeVertex(MidlePoint);
    myRadiusDimension->SetFirstShape(V1);
    myRadiusDimension->SetSecondShape(V2);
    myRadiusDimension->SetValue(R);
    fitInResol(R);
    QString S;
    S.sprintf("%.1f",R);
    myRadiusDimension->SetText(TCollection_ExtendedString(strdup(S)));
    if (myInteractiveContext->IsDisplayed(myRadiusDimension))
      myInteractiveContext->Redisplay(myRadiusDimension);
    else
      myInteractiveContext->Display(myRadiusDimension);
    myCenterCircle->Set(V1);
    if (myInteractiveContext->IsDisplayed(myCenterCircle))
      myInteractiveContext->Redisplay(myCenterCircle);
    else
      myInteractiveContext->Display(myCenterCircle);
  }
}

/*!
  Remove last edge from the current wire.
*/
void Sketch::RemoveLastEdge()
{
  if (myEdgesNumber == 0) {
    myCurrentStatus = END_SKETCH;
    myInteractiveContext->CloseAllContexts();
    myInteractiveContext->EraseAll(Standard_False);
    return;
  }
  else {
    BRepTools_WireExplorer Ex;
    BRepBuilderAPI_MakeWire MW;
    Standard_Integer index = 1;
    myCurrentEdge = myPreviousEdge;
    for (Ex.Init(myCurrentWire.Wire());Ex.More();Ex.Next()){
      if (index <= myEdgesNumber-1) {
	MW.Add(Ex.Current());
	myPreviousEdge = Ex.Current();
	index++;
      }
    }
    myCurrentWire = MW;
    myCurrentStatus = (SketchStatus)myConstructionMode(myEdgesNumber);
    myTransitionStatus = (TransitionStatus)myConstraintMode(myEdgesNumber);
    myEdgesNumber--;
    myConstructionMode.Remove(index);
    myConstraintMode.Remove(index);
    if (myEdgesNumber == 0) 
      myPresentableWire->Set(myFirstPointSketch);
    else
      myPresentableWire->Set(myCurrentWire.Wire());
    myInteractiveContext->Redisplay(myPresentableWire);
    myInteractiveContext->CloseLocalContext();
    myPresentableEdge->Set(myCurrentEdge);
    myInteractiveContext->Redisplay(myPresentableEdge);
  }
}

/*!
  Create initial constraints.
*/
void Sketch::CreateConstraints()
{
  Handle(Geom_Axis1Placement) xAxis = new Geom_Axis1Placement(myPlane->Pln().XAxis());
  Handle(Geom_Axis1Placement) yAxis = new Geom_Axis1Placement(myPlane->Pln().YAxis());
  myHAxis = new AIS_Axis(xAxis);
  myVAxis = new AIS_Axis(yAxis);
  myAngularAxis = myVAxis;
  myHAxis->SetColor(myAxisColor.Name());
  myVAxis->SetColor(myAxisColor.Name());
  myAngularAxis->SetColor(myAxisColor.Name());
}

/*!
  fitInResol.

  \param toFit  
  \param minIsResol     
*/
void Sketch::fitInResol(Standard_Real &toFit, Standard_Boolean minIsResol)
{ 
  Standard_Real sign =  (toFit < 0) ? -1. : +1.;
  Standard_Real value = toFit + sign * resol/2.0; /* why "+ resol/2.0" ? because if resol = 0.5, 3.3 is rounded to 3.5 */
  int nTimesResol = int(value/resol);
  if ((nTimesResol == 0) &&  (minIsResol)) nTimesResol = 1;
  toFit = nTimesResol*resol;
}

SketchStatus Sketch::GetCurrentStatus()
{
  return myCurrentStatus;
}

Standard_Integer Sketch::GetmyEdgesNumber()
{
  return myEdgesNumber;
}



