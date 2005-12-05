// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//

#include <Standard_Stream.hxx>

#include <GEOMImpl_FillingDriver.hxx>
#include <GEOM_Function.hxx>
#include <GEOMImpl_IFilling.hxx>
#include <GEOMImpl_Types.hxx>

#include <BRep_Tool.hxx>
#include <BRepAlgo.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_SectionGenerator.hxx>

#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_FillingDriver::GetID()
{
  static Standard_GUID aFillingDriver ("FF1BBB62-5D14-4df2-980B-3A668264EA16");
  return aFillingDriver;
}


//=======================================================================
//function : GEOMImpl_FillingDriver
//purpose  :
//=======================================================================

GEOMImpl_FillingDriver::GEOMImpl_FillingDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_FillingDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());
  if (aFunction.IsNull()) return 0;

  if (aFunction->GetType() != BASIC_FILLING) return 0;

  GEOMImpl_IFilling IF (aFunction);
  Handle(GEOM_Function) aShapeFunction = IF.GetShape();
  if (aShapeFunction.IsNull()) return 0;
  TopoDS_Shape aShape = aShapeFunction->GetValue();
  if (aShape.IsNull() || aShape.ShapeType() != TopAbs_COMPOUND) return 0;

  Standard_Integer mindeg = IF.GetMinDeg();
  Standard_Integer maxdeg = IF.GetMaxDeg();
  Standard_Real tol3d = IF.GetTol2D();
  Standard_Real tol2d = IF.GetTol3D();
  Standard_Integer nbiter = IF.GetNbIter();

  if (mindeg > maxdeg) {
    Standard_RangeError::Raise("Minimal degree can not be more than maximal degree");
  }

  /* we verify the contents of the shape */
  TopExp_Explorer Ex;
  TopoDS_Shape Scurrent;
  Standard_Real First, Last;
  Handle(Geom_Curve) C;
  GeomFill_SectionGenerator Section;

  Standard_Integer i = 0;
  for (Ex.Init(aShape, TopAbs_EDGE); Ex.More(); Ex.Next()) {
    Scurrent = Ex.Current() ;
    if (Scurrent.IsNull() || Scurrent.ShapeType() != TopAbs_EDGE) return 0;
    C = BRep_Tool::Curve(TopoDS::Edge(Scurrent), First, Last);
    C = new Geom_TrimmedCurve(C, First, Last);
    Section.AddCurve(C);
    i++;
  }

  /* a 'tolerance' is used to compare 2 knots : see GeomFill_Generator.cdl */
  Section.Perform(Precision::Confusion());
  Handle(GeomFill_Line) Line = new GeomFill_Line(i);

  GeomFill_AppSurf App (mindeg, maxdeg, tol3d, tol2d, nbiter); /* user parameters */
  App.Perform(Line, Section);

  if (!App.IsDone()) return 0;
  Standard_Integer UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots;
  App.SurfShape(UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots);
  Handle(Geom_BSplineSurface) GBS = new Geom_BSplineSurface
    (App.SurfPoles(), App.SurfWeights(), App.SurfUKnots(), App.SurfVKnots(),
     App.SurfUMults(), App.SurfVMults(), App.UDegree(), App.VDegree());

  if (GBS.IsNull()) return 0;
  aShape = BRepBuilderAPI_MakeFace(GBS);

  /* We test the validity of resulting shape */
  if (!BRepAlgo::IsValid((aShape))) {
    Standard_ConstructionError::Raise("Algorithm have produced an invalid shape result");
    return 0;
  }

  aFunction->SetValue(aShape);

  log.SetTouched(Label());
  return 1;
}


//=======================================================================
//function :  GEOMImpl_FillingDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_FillingDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_FillingDriver",
			                                 sizeof(GEOMImpl_FillingDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================

const Handle(GEOMImpl_FillingDriver) Handle(GEOMImpl_FillingDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_FillingDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_FillingDriver))) {
       _anOtherObject = Handle(GEOMImpl_FillingDriver)((Handle(GEOMImpl_FillingDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}


