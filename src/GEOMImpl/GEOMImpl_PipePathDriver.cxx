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

#include <Basics_OCCTVersion.hxx>

#include <GEOMImpl_PipePathDriver.hxx>

#include <GEOMImpl_IShapesOperations.hxx>
#include <GEOMImpl_ShapeDriver.hxx>
#include <GEOMImpl_IPipePath.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Shell.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_ShapeTolerance.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>

#if OCC_VERSION_LARGE > 0x06050300
#include <BRepOffsetAPI_MiddlePath.hxx>
#endif

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <GProp_GProps.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Conic.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert.hxx>

#include <TColgp_SequenceOfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_HSequenceOfTransient.hxx>

#include <Precision.hxx>

#include <Standard_NullObject.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_ConstructionError.hxx>

#include "utilities.h"

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_PipePathDriver::GetID()
{
  static Standard_GUID aPipePathDriver ("FF1BBB19-5D14-4df2-980B-3A668264EA17");
  return aPipePathDriver;
}

//=======================================================================
//function : GEOMImpl_PipePathDriver
//purpose  :
//=======================================================================
GEOMImpl_PipePathDriver::GEOMImpl_PipePathDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_PipePathDriver::Execute (TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aRes;

  // RestorePath
  if (aType == PIPE_PATH_TWO_BASES) {
    GEOMImpl_IPipePath aPI (aFunction);

    Handle(GEOM_Function) aRefShape = aPI.GetShape();
    Handle(GEOM_Function) aRefBase1 = aPI.GetBase1();
    Handle(GEOM_Function) aRefBase2 = aPI.GetBase2();

    TopoDS_Shape aShape = aRefShape->GetValue();
    TopoDS_Shape aBase1 = aRefBase1->GetValue();
    TopoDS_Shape aBase2 = aRefBase2->GetValue();

    if (aShape.IsNull() || aBase1.IsNull() || aBase2.IsNull())
      Standard_NullObject::Raise("RestorePath aborted : null argument");

#if OCC_VERSION_LARGE > 0x06050300
    BRepOffsetAPI_MiddlePath aMPB (aShape, aBase1, aBase2);
    aMPB.Build();
    if (aMPB.IsDone()) {
      aRes = aMPB.Shape();
    }
#else
    Standard_NullObject::Raise("RestorePath is not implemented in used OCCT version");
#endif
  }
  else if (aType == PIPE_PATH_TWO_SEQS) {
    GEOMImpl_IPipePath aPI (aFunction);

    Handle(GEOM_Function) aRefShape = aPI.GetShape();
    TopoDS_Shape aShape = aRefShape->GetValue();

    Handle(TColStd_HSequenceOfTransient) aBaseSeq1 = aPI.GetBaseSeq1();
    Handle(TColStd_HSequenceOfTransient) aBaseSeq2 = aPI.GetBaseSeq2();

    TopoDS_Shape aBase1;
    TopoDS_Shape aBase2;

    if (aBaseSeq1->Length() == 1 && aBaseSeq2->Length() == 1) {
      Handle(GEOM_Function) aRefShape1 = Handle(GEOM_Function)::DownCast(aBaseSeq1->Value(1));
      Handle(GEOM_Function) aRefShape2 = Handle(GEOM_Function)::DownCast(aBaseSeq2->Value(1));
      aBase1 = aRefShape1->GetValue();
      aBase2 = aRefShape2->GetValue();
    }
    else {
      aBase1 = GEOMImpl_ShapeDriver::MakeWireFromEdges(aBaseSeq1, Precision::Confusion());
      aBase2 = GEOMImpl_ShapeDriver::MakeWireFromEdges(aBaseSeq2, Precision::Confusion());
    }

    if (aShape.IsNull() || aBase1.IsNull() || aBase2.IsNull())
      Standard_NullObject::Raise("RestorePath aborted : null argument");

#if OCC_VERSION_LARGE > 0x06050300
    BRepOffsetAPI_MiddlePath aMPB (aShape, aBase1, aBase2);
    aMPB.Build();
    if (aMPB.IsDone()) {
      aRes = aMPB.Shape();
    }
#else
    Standard_NullObject::Raise("RestorePath is not implemented in used OCCT version");
#endif
  }
  else {
  }

  if (aRes.IsNull()) return 0;

  aFunction->SetValue(aRes);
  log.SetTouched(Label());

  return 1;
}

//=======================================================================
//function :  GEOMImpl_PipePathDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_PipePathDriver_Type_()
{
  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if (aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if (aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if (aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_PipePathDriver",
                                                         sizeof(GEOMImpl_PipePathDriver),
                                                         1,
                                                         (Standard_Address)_Ancestors,
                                                         (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_PipePathDriver) Handle(GEOMImpl_PipePathDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_PipePathDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_PipePathDriver))) {
       _anOtherObject = Handle(GEOMImpl_PipePathDriver)((Handle(GEOMImpl_PipePathDriver)&)AnObject);
     }
  }

  return _anOtherObject;
}
