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

#include <GEOMImpl_PipePathDriver.hxx>
#include <GEOMImpl_ShapeDriver.hxx>
#include <GEOMImpl_IPipePath.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <BRepOffsetAPI_MiddlePath.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Precision.hxx>
#include <Standard_NullObject.hxx>

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
Standard_Integer GEOMImpl_PipePathDriver::Execute(LOGBOOK& log) const
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

    BRepOffsetAPI_MiddlePath aMPB (aShape, aBase1, aBase2);
    aMPB.Build();
    if (aMPB.IsDone()) {
      aRes = aMPB.Shape();
    }
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

    BRepOffsetAPI_MiddlePath aMPB (aShape, aBase1, aBase2);
    aMPB.Build();
    if (aMPB.IsDone()) {
      aRes = aMPB.Shape();
    }
  }
  else {
  }

  if (aRes.IsNull()) return 0;

  aFunction->SetValue(aRes);
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

bool GEOMImpl_PipePathDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPipePath aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "PIPE_PATH";

  switch ( aType ) {
  case PIPE_PATH_TWO_BASES:
    AddParam( theParams, "Pipe-like object", aCI.GetShape() );
    AddParam( theParams, "First base", aCI.GetBase1() );
    AddParam( theParams, "Second base", aCI.GetBase2() );
    break;
  case PIPE_PATH_TWO_SEQS:
    AddParam( theParams, "Pipe-like object", aCI.GetShape() );
    AddParam( theParams, "First bases", aCI.GetBaseSeq1() );
    AddParam( theParams, "Second bases", aCI.GetBaseSeq2() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_PipePathDriver,GEOM_BaseDriver);
