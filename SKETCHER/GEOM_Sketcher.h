//  GEOM SKETCHER : basic sketcher
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOM_Sketcher.h
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#include "GEOM_SketcherStatus.h"

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Geom_Plane.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Axis.hxx>
#include <TopoDS_Edge.hxx>
#include <AIS_LengthDimension.hxx>
#include <AIS_RadiusDimension.hxx>
#include <AIS_AngleDimension.hxx>
#include <Geom2d_Line.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <gp_Vec2d.hxx>

class Sketch  
{
public:
        Sketch();
	~Sketch();
	Sketch(const Handle(V3d_Viewer)& aViewer);
        Sketch(const Handle(V3d_Viewer)& aViewer, 
		       const Quantity_Color& anAxisColor,
		       const Quantity_Color& aCurrentColor,
		       const Quantity_Color& aWireColor);

	static void fitInResol(Standard_Real &toFit, 
			       Standard_Boolean minIsResol = Standard_False);

	void MakeCurrentEdge(const Standard_Integer  Xp   ,
				     const Standard_Integer  Yp   ,
				     const Handle(V3d_View)& aView );
	
	void MakeCurrentEdge(const Standard_Real  X, const Standard_Real Y);

	Standard_Boolean SetDimension(Standard_Real& aValue); 
	void SetDimension(Standard_Real& deltaX,Standard_Real& deltaY);

	void SetXDimension(Standard_Real& deltaX);
	void SetYDimension(Standard_Real& deltaY);

	void SetSegmentAngle(Standard_Real& aValue); 
	Standard_Real GetSegmentAngle();

	void ValidateEdge();

	TopoDS_Wire Close();
	TopoDS_Wire End();

	void Clear();

	Standard_Boolean Delete();

	void SetPlane(const Handle(Geom_Plane)& aPlane);

	void SetWireColor(const Quantity_Color& aColor);
	void SetCurrentColor(const Quantity_Color& aColor);
	void SetAxisColor(const Quantity_Color& aColor);

	void SetParameterVisibility(const TypeOfParameter atype, 
					    const Standard_Boolean OnOff);

	void HiligthWithColor(const TypeOfParameter atype, 
				      const Quantity_NameOfColor acolor);
	void Unhiligth(const TypeOfParameter atype);

	Standard_Boolean IsValidCurrentParameter(const TypeOfParameter atype);

	void SetParameterValue(const TypeOfParameter atype, Standard_Real aValue);

	void ChangeMode(const SketchStatus aMode);

	void SetTransitionStatus(const TransitionStatus aStatus);

	SketchStatus GetCurrentStatus(); 
	Standard_Integer GetmyEdgesNumber();

private :
	
	void Init();

	void MakeCurrentSegment(Standard_Real X, Standard_Real Y);

	void MakeCurrentArc(Standard_Real X, Standard_Real Y);

	void DisplayCurrentEdge();

	void DisplayLengthDimension(const TopoDS_Vertex& V1,const TopoDS_Vertex& V2);
	void DisplayXDimension(const TopoDS_Vertex& V1,const TopoDS_Vertex& V2);
	void DisplayYDimension(const TopoDS_Vertex& V1,const TopoDS_Vertex& V2);
	void DisplayAngleDimension();
	void DisplayRadiusDimension();

	void AddEdgeToWire();

	void RemoveLastEdge();

	void CreateConstraints();

private:
	// Current status of construction
	SketchStatus myCurrentStatus; 
	// Transition status between arcs and segments
	TransitionStatus myTransitionStatus;  
	// Current wire
	BRepBuilderAPI_MakeWire myCurrentWire;
	// Current presentable wire
	Handle_AIS_Shape myPresentableWire;   
	// Number of edges into the current wire
	Standard_Integer myEdgesNumber;       
	// Store for each edge how it has been built i.d the value of myCurrentStatus
	TColStd_SequenceOfInteger myConstructionMode; 
	// Store for each edge how it has been built i.d the value of myTransitionStatus
	TColStd_SequenceOfInteger myConstraintMode;  
	// Current edge 
	TopoDS_Shape myCurrentEdge; 
	// Previous edge
	TopoDS_Edge myPreviousEdge; 
	// List of points to interpolate when computing a curve by interpolation
	TColgp_SequenceOfPnt2d myPointsToInterpolate; 
	// Tangent vector at beginning of curve
	gp_Vec2d myTangentVector;         
	// Tangent flag at beginning og curve
	Standard_Boolean myTangentFlag;   
	
	// Origin of sketch
	TopoDS_Vertex myFirstPointSketch; 
	// Last x coordinate of end point of previous edge or picked point
	Standard_Real myLastX; 
	// Last y coordinate of end point of previous edge or picked point
	Standard_Real myLastY; 
	// angle between 2 segments when an ANGLE TransitionStatus is set.
	Standard_Real mySegmentAngle;  
	// Length of segment when the length of the segment is fixed before the angle
	Standard_Real mySegmentLength; 
	// Value of X fixed coordinate 
	Standard_Real mySegmentX;                    
	// Value of Y fixed Coordinate
	Standard_Real mySegmentY;                    
	Handle_AIS_Shape myPresentableEdge;          // Current presentable edge
	Handle_AIS_LengthDimension myLengthDimension;// For display of current length segment dimension
	Handle_AIS_LengthDimension myXDimension;     // For display of current X segment dimension
	Handle_AIS_LengthDimension myYDimension;     // For display of current Y segment dimension
	Handle_AIS_LengthDimension myRadiusDimension;// For display of current radius dimension
	Handle_AIS_AngleDimension myAngleDimension;  // For display of current angle dimension
	Standard_Boolean myIsLengthDimensionVisible; // Visibility flag for LengthDimension
	Standard_Boolean myIsXDimensionVisible;      // Visibility flag for X Dimension
	Standard_Boolean myIsYDimensionVisible;      // Visibility flag for Y Dimension
	Standard_Boolean myIsRadiusDimensionVisible; // Visibility flag for RadiusDimension
	Standard_Boolean myIsAngleDimensionVisible;  // Visibility flag for AngleDimension
	Handle_AIS_Axis myPresentableMediatrice;     // Chord mediatrice of current circle as a presentable object
	Handle_Geom2d_Line myMediatrice; // Chord mediatrice of current circle as a 2d line
	Handle_AIS_Shape myCenterCircle; // to visualise center of current arc

	Handle_AIS_Axis myHAxis;         // Horizontal axis
	Handle_AIS_Axis myVAxis;         // Vertical axis
	Handle_AIS_Axis myAngularAxis;   // Axis making a predefined angle with the previous edge

	Quantity_Color myWireColor;      // Color of wire and of build edges
	Quantity_Color myCurrentColor;   // Color of edge under construction
	Quantity_Color myAxisColor;      // Color for axis
	Handle_AIS_InteractiveContext myInteractiveContext; // Interactive context for display management
	
	Handle_Geom_Plane myPlane;       // Plane of sketch

	BRepBuilderAPI_MakeWire myPasteWire;						
	Standard_Integer myPasteEdgesNumber;
	TColStd_SequenceOfInteger myPasteConstructionMode;
	TColStd_SequenceOfInteger myPasteConstraintMode;	
};
