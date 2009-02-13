//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//#include <Standard_Stream.hxx>
//
#include <GEOMImpl_PositionDriver.hxx>
#include <GEOMImpl_IPosition.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <GEOMImpl_IMeasureOperations.hxx>

// OCCT Includes
#include <BRepBuilderAPI_Transform.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <ShHealOper_EdgeDivide.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <gp_Pln.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Curve.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <BRepGProp.hxx>
#include <ShapeFix_Wire.hxx>

#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TopExp.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_PositionDriver::GetID()
{
  static Standard_GUID aPositionDriver("FF1BBB69-5D14-4df2-980B-3A668264EA16");
  return aPositionDriver;
}


//=======================================================================
//function : GEOMImpl_PositionDriver
//purpose  :
//=======================================================================
GEOMImpl_PositionDriver::GEOMImpl_PositionDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_PositionDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPosition aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == POSITION_SHAPE || aType == POSITION_SHAPE_COPY) {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    Handle(GEOM_Function) aRefStartLCS = aCI.GetStartLCS();
    Handle(GEOM_Function) aRefEndLCS = aCI.GetEndLCS();

    TopoDS_Shape aShapeBase = aRefShape->GetValue();
    TopoDS_Shape aShapeStartLCS = aRefStartLCS->GetValue();
    TopoDS_Shape aShapeEndLCS = aRefEndLCS->GetValue();

    if (aShapeBase.IsNull() || aShapeStartLCS.IsNull() ||
	aShapeEndLCS.IsNull() || aShapeEndLCS.ShapeType() != TopAbs_FACE)
      return 0;

    gp_Trsf aTrsf;
    gp_Ax3 aStartAx3, aDestAx3;

    // End LCS
    aDestAx3 = GEOMImpl_IMeasureOperations::GetPosition(aShapeEndLCS);

    // Start LCS
    aStartAx3 = GEOMImpl_IMeasureOperations::GetPosition(aShapeStartLCS);

    // Set transformation
    aTrsf.SetDisplacement(aStartAx3, aDestAx3);

    // Perform transformation
    BRepBuilderAPI_Transform aBRepTrsf (aShapeBase, aTrsf, Standard_False);
    aShape = aBRepTrsf.Shape();
  }
  else if (aType == POSITION_SHAPE_FROM_GLOBAL ||
           aType == POSITION_SHAPE_FROM_GLOBAL_COPY) {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    Handle(GEOM_Function) aRefEndLCS = aCI.GetEndLCS();

    TopoDS_Shape aShapeBase = aRefShape->GetValue();
    TopoDS_Shape aShapeEndLCS = aRefEndLCS->GetValue();

    if (aShapeBase.IsNull() || aShapeEndLCS.IsNull() ||
        aShapeEndLCS.ShapeType() != TopAbs_FACE)
      return 0;

    gp_Trsf aTrsf;
    gp_Ax3 aStartAx3, aDestAx3;

    // End LCS
    aDestAx3 = GEOMImpl_IMeasureOperations::GetPosition(aShapeEndLCS);

    // Set transformation
    aTrsf.SetDisplacement(aStartAx3, aDestAx3);

    // Perform transformation
    BRepBuilderAPI_Transform aBRepTrsf (aShapeBase, aTrsf, Standard_False);
    aShape = aBRepTrsf.Shape();
  }
  else if (aType == POSITION_ALONG_PATH) {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    Handle(GEOM_Function) aPathShape = aCI.GetPath();
    double aValue = aCI.GetDistance();
    bool aReversed = aCI.GetReverse();

    TopoDS_Shape aShapeBase = aRefShape->GetValue();
    TopoDS_Shape aPath = aPathShape->GetValue();

    if (aShapeBase.IsNull() || aPath.IsNull())
      return 0;

    //Get a Center Of Mass Of Base Object
    GProp_GProps aSystem;
    gp_Pnt aCenterMass;
    if (aShapeBase.ShapeType() == TopAbs_VERTEX) {
      aCenterMass = BRep_Tool::Pnt(TopoDS::Vertex(aShapeBase));
    } else if (aShapeBase.ShapeType() == TopAbs_EDGE || aShapeBase.ShapeType() == TopAbs_WIRE) {
      BRepGProp::LinearProperties(aShapeBase, aSystem);
      aCenterMass = aSystem.CentreOfMass();
    } else if (aShapeBase.ShapeType() == TopAbs_FACE || aShapeBase.ShapeType() == TopAbs_SHELL) {
      BRepGProp::SurfaceProperties(aShapeBase, aSystem);
      aCenterMass = aSystem.CentreOfMass();
    } else {
      BRepGProp::VolumeProperties(aShapeBase, aSystem);
      aCenterMass = aSystem.CentreOfMass();
    }

    TopoDS_Shape aTrimmedPath;
    gp_Trsf aTrsf;
    Handle(Geom_Curve) aCurve;    
    Standard_Real aFirst =0.,aLast=0.;
    Standard_Real aParam = 0.;
    Standard_Real aLength = 0.;

    gp_Pnt aPFirst, aPLast;
   
    if ( aPath.ShapeType() == TopAbs_EDGE ) { // The Path is Edge
      TopoDS_Edge anEdge = TopoDS::Edge(aPath);

      BRep_Tool::Range(anEdge,aFirst,aLast);
      aCurve = BRep_Tool::Curve(anEdge,aFirst,aLast);
      if (aReversed)
	aCurve = aCurve->Reversed();

      aCurve->D0(aFirst, aPFirst);
      aCurve->D0(aLast, aPLast);

      // Translate a CenterMass of Base Shape to the start of the path
      if ( !aPFirst.IsEqual(aCenterMass, gp::Resolution()) ) {
	gp_Trsf aCurTrsf;
	aCurTrsf.SetTranslation(aCenterMass, aPFirst);
	aTrsf.PreMultiply(aCurTrsf);
      }
      aParam = aFirst + aValue*(aLast - aFirst); // Calculate parameter
    } else if ( aPath.ShapeType() == TopAbs_WIRE ) { // The  path Shape is Wire
      TopoDS_Wire aWire = TopoDS::Wire(aPath);

     // fix edges order
      Handle(ShapeFix_Wire) aFixWire = new ShapeFix_Wire;
      aFixWire->Load(aWire);
      aFixWire->FixReorder();
      aWire = aFixWire->Wire();
      
      TopExp_Explorer ex;
      TopTools_SequenceOfShape Edges;
      Standard_Real nbEdges = 0.;
      BRepTools_WireExplorer aWE (aWire);
      for (; aWE.More(); aWE.Next(), nbEdges++) // Explore a Wire on Edges
	Edges.Append(aWE.Current());

      Standard_Real aSummOfLen =0.;
      Standard_Real aCurLen =0.;
      GeomAdaptor_Curve aAdC;

      for(int i=1; i<=Edges.Length(); i++) { // Calculate summary Lenght of edges
	TopoDS_Edge anEdge = TopoDS::Edge(Edges.Value(i));
	BRep_Tool::Range(anEdge,aFirst,aLast);
	aCurve = BRep_Tool::Curve(anEdge,aFirst,aLast);
	aAdC.Load(aCurve,aFirst,aLast);
	aCurLen = GCPnts_AbscissaPoint::Length(aAdC,aFirst,aLast); 
	aSummOfLen += aCurLen;
      }

      // Move BaseShape to the Start Of the Curve
      TopoDS_Edge anEdge;
      if (!aReversed)
	anEdge = TopoDS::Edge(Edges.Value(1));
      else
	anEdge = TopoDS::Edge(Edges.Value(Edges.Length()));

      BRep_Tool::Range(anEdge,aFirst,aLast);
      aCurve = BRep_Tool::Curve(anEdge,aFirst,aLast);
      aCurve->D0(aFirst, aPFirst);
      aCurve->D0(aLast, aPLast);
      if ( !aPFirst.IsEqual(aCenterMass, gp::Resolution()) ) {
	gp_Trsf aCurTrsf;
	if (aReversed && anEdge.Orientation() == TopAbs_FORWARD)
	  aPFirst = aPLast;

	aCurTrsf.SetTranslation(aCenterMass, aPFirst);
	aTrsf.PreMultiply(aCurTrsf);
      }

      Standard_Real aWireLen = aSummOfLen*aValue;
      aSummOfLen = 0;
      for(int i=1; i<=Edges.Length(); i++) {
	TopoDS_Edge anEdge;
	if (!aReversed)
	  anEdge = TopoDS::Edge(Edges.Value(i));
	else
	  anEdge = TopoDS::Edge(Edges.Value(Edges.Length() - i + 1));

	aCurve = BRep_Tool::Curve(anEdge,aFirst,aLast);
	BRep_Tool::Range(anEdge,aFirst,aLast);

	if (!aReversed && anEdge.Orientation() == TopAbs_REVERSED)
	  aCurve = aCurve->Reversed();

	if (aReversed && anEdge.Orientation() == TopAbs_FORWARD)
	  aCurve = aCurve->Reversed();

	aAdC.Load(aCurve,aFirst,aLast);
	aCurLen = GCPnts_AbscissaPoint::Length(aAdC,aFirst,aLast);

	if ( aWireLen > (aSummOfLen + aCurLen) ) { // Transform a Base object along this Edge
	  aSummOfLen += aCurLen;
	  gp_Pnt aP1, aP2;
	  gp_Vec aStartVec1, aStartVec2, aDestVec1, aDestVec2;
	  aCurve->D2(aFirst, aP1, aStartVec1, aStartVec2 );
	  aCurve->D2(aLast, aP2, aDestVec1, aDestVec2 );
	  gp_Trsf aCurTrsf;
	  if (aStartVec2.Magnitude() > gp::Resolution() && aDestVec2.Magnitude() > gp::Resolution()) {
	    gp_Ax3 aStartAx3(aP1, aStartVec1, aStartVec2);
	    gp_Ax3 aDestAx3(aP2, aDestVec1, aDestVec2);
	    aCurTrsf.SetDisplacement(aStartAx3, aDestAx3);
	  } else
	    aCurTrsf.SetTranslation(aP1, aP2);

	  aTrsf.PreMultiply(aCurTrsf);
	}
	else {
	  aLength = aWireLen - aSummOfLen;
	  GCPnts_AbscissaPoint anAbsc(aAdC,aLength,aFirst);
	  if(anAbsc.IsDone()) 
	    aParam = anAbsc.Parameter();
	  break;
	}
      }
    } else
      return 0; // Unknown Type

    gp_Trsf aCurTrsf;
    gp_Pnt aP1, aP2;
    gp_Vec aStartVec1, aStartVec2, aDestVec1, aDestVec2;
    aCurve->D2(aFirst, aP1, aStartVec1, aStartVec2 );
    aCurve->D2(aParam, aP2, aDestVec1, aDestVec2 );

    if (aStartVec2.Magnitude() > gp::Resolution() && aDestVec2.Magnitude() > gp::Resolution()) {
      gp_Ax3 aStartAx3(aP1, aStartVec1, aStartVec2);
      gp_Ax3 aDestAx3(aP2, aDestVec1, aDestVec2);
      aCurTrsf.SetDisplacement(aStartAx3, aDestAx3);
    } else
      aCurTrsf.SetTranslation(aP1, aP2);

    aTrsf.PreMultiply(aCurTrsf);
    
    if ( !aPFirst.IsEqual(aCenterMass, gp::Resolution()) ) {
      gp_Trsf aCurTrsf;
      aCurTrsf.SetTranslation(aPFirst, aCenterMass);
      aTrsf.PreMultiply(aCurTrsf);
    }

    // Perform transformation
    BRepBuilderAPI_Transform aBRepTrsf (aShapeBase, aTrsf, Standard_False);
    aShape = aBRepTrsf.Shape();
  }
  else
    return 0;

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}


//=======================================================================
//function :  GEOMImpl_PositionDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_PositionDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_PositionDriver",
			                                 sizeof(GEOMImpl_PositionDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_PositionDriver) Handle(GEOMImpl_PositionDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_PositionDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_PositionDriver))) {
       _anOtherObject = Handle(GEOMImpl_PositionDriver)((Handle(GEOMImpl_PositionDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
