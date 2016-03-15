// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include "GEOM_Function.hxx"

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopAbs.hxx>
#include <ShHealOper_ModifStats.hxx>

class GEOMImpl_IHealing
{
public:

  enum {
    ARG_SHAPE_PROCESS_OPERATORS =  1,
    ARG_SHAPE_PROCESS_PARAMS    =  2,
    ARG_SHAPE_PROCESS_VALUES    =  3,
    ARG_ORIGINAL                =  4,
    ARG_LIST_ARGUMENTS          =  5,
    ARG_IS_COMMON_VERTEX        =  6,
    ARG_TOLERANCE               =  7,
    ARG_DEV_EDGE_VALUE          =  8,
    ARG_IS_BY_PARAMETER         =  9,
    ARG_SUBSHAPE_INDEX          = 10,
    ARG_LIST_SHAPES             = 11,
    ARG_TYPE                    = 12
  };

  GEOMImpl_IHealing(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetOperators( const Handle(TColStd_HArray1OfExtendedString)& arr ) {  if ( !arr.IsNull() ) _func->SetStringArray(ARG_SHAPE_PROCESS_OPERATORS, arr); }
  Handle(TColStd_HArray1OfExtendedString) GetOperators() { return _func->GetStringArray(ARG_SHAPE_PROCESS_OPERATORS); }

  void SetParameters( const Handle(TColStd_HArray1OfExtendedString)& arr ) {  if ( !arr.IsNull() ) _func->SetStringArray(ARG_SHAPE_PROCESS_PARAMS, arr); }
  Handle(TColStd_HArray1OfExtendedString) GetParameters() { return _func->GetStringArray(ARG_SHAPE_PROCESS_PARAMS); }

  void SetValues( const Handle(TColStd_HArray1OfExtendedString)& arr ) {  if ( !arr.IsNull() ) _func->SetStringArray(ARG_SHAPE_PROCESS_VALUES, arr); }
  Handle(TColStd_HArray1OfExtendedString) GetValues() { return _func->GetStringArray(ARG_SHAPE_PROCESS_VALUES); }

  void SetOriginal( Handle(GEOM_Function) f ) { _func->SetReference(ARG_ORIGINAL, f); }
  Handle(GEOM_Function) GetOriginal() { return _func->GetReference(ARG_ORIGINAL); }

  void SetFaces( const Handle(TColStd_HArray1OfInteger)& arr ) { if ( !arr.IsNull() ) _func->SetIntegerArray(ARG_LIST_ARGUMENTS, arr); }
  Handle(TColStd_HArray1OfInteger) GetFaces() { return _func->GetIntegerArray(ARG_LIST_ARGUMENTS); }

  void SetWires( const Handle(TColStd_HArray1OfInteger)& arr ) { if ( !arr.IsNull() ) _func->SetIntegerArray(ARG_LIST_ARGUMENTS, arr); }
  Handle(TColStd_HArray1OfInteger) GetWires() { return _func->GetIntegerArray(ARG_LIST_ARGUMENTS); }

  void SetIsCommonVertex( Standard_Boolean val ) { _func->SetInteger(ARG_IS_COMMON_VERTEX, val ? 1 : 0 ); }
  Standard_Boolean GetIsCommonVertex() { return (_func->GetInteger(ARG_IS_COMMON_VERTEX) != 0 ); }

  void SetIsByParameter( Standard_Boolean val ) { _func->SetInteger(ARG_IS_BY_PARAMETER, val ? 1 : 0 ); }
  Standard_Boolean GetIsByParameter() { return (_func->GetInteger(ARG_IS_BY_PARAMETER) != 0 ); }

  void SetTolerance( Standard_Real val ) { _func->SetReal(ARG_TOLERANCE, val); }
  Standard_Real GetTolerance() { return _func->GetReal(ARG_TOLERANCE); }

  void SetType( TopAbs_ShapeEnum val ) { _func->SetInteger(ARG_TYPE, (Standard_Integer)val); }
  TopAbs_ShapeEnum GetType() { TopAbs_ShapeEnum type = (TopAbs_ShapeEnum)(_func->GetInteger(ARG_TYPE)); return _func->IsDone() ? type : TopAbs_SHAPE; }

  void SetDevideEdgeValue( Standard_Real val ) { _func->SetReal(ARG_DEV_EDGE_VALUE, val); }
  Standard_Real GetDevideEdgeValue() { return _func->GetReal(ARG_DEV_EDGE_VALUE); }

  void SetIndex( Standard_Integer val ) { _func->SetInteger(ARG_SUBSHAPE_INDEX, val); }
  Standard_Integer GetIndex() { return _func->GetInteger(ARG_SUBSHAPE_INDEX); }

  void SetShapes(const Handle(TColStd_HSequenceOfTransient)& theShapes)
  { _func->SetReferenceList(ARG_LIST_SHAPES, theShapes); }
  Handle(TColStd_HSequenceOfTransient) GetShapes()
  { return _func->GetReferenceList(ARG_LIST_SHAPES); }

  Handle(TColStd_HSequenceOfTransient) GetOriginalAndShapes()
  {
    Handle(TColStd_HSequenceOfTransient) funs = GetShapes();
    if ( funs.IsNull() ) funs = new TColStd_HSequenceOfTransient;
    funs->Prepend( GetOriginal() );
    return funs;
  }

  void SetStatistics( ShHealOper_ModifStats* ms )
  {
    if ( ms ) ms->Clear();
    _func->SetCallBackData( (void*) ms );
  }
  ShHealOper_ModifStats* GetStatistics()
  {
    return (ShHealOper_ModifStats*) _func->GetCallBackData();
  }

private:
  Handle(GEOM_Function) _func;
};
