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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Standard_Stream.hxx>

#include <GEOMImpl_BooleanDriver.hxx>
#include <GEOMImpl_IBoolean.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <BRepAlgo.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <TopoDS_Shape.hxx>

#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_BooleanDriver::GetID()
{
  static Standard_GUID aBooleanDriver("FF1BBB21-5D14-4df2-980B-3A668264EA16");
  return aBooleanDriver;
}


//=======================================================================
//function : GEOMImpl_BooleanDriver
//purpose  :
//=======================================================================
GEOMImpl_BooleanDriver::GEOMImpl_BooleanDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_BooleanDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IBoolean aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  Handle(GEOM_Function) aRefShape1 = aCI.GetShape1();
  Handle(GEOM_Function) aRefShape2 = aCI.GetShape2();
  TopoDS_Shape aShape1 = aRefShape1->GetValue();
  TopoDS_Shape aShape2 = aRefShape2->GetValue();
  if (!aShape1.IsNull() && !aShape2.IsNull()) {
    if (aType == BOOLEAN_COMMON) {
      BRepAlgoAPI_Common BO (aShape1, aShape2);
//      BO.Build();
      if (!BO.IsDone()) {
        StdFail_NotDone::Raise("Requested boolean operation can not be performed on the given shapes");
      }
      aShape = BO.Shape();
    } else if (aType == BOOLEAN_CUT) {
      BRepAlgoAPI_Cut BO (aShape1, aShape2);
      if (!BO.IsDone()) {
        StdFail_NotDone::Raise("Requested boolean operation can not be performed on the given shapes");
      }
      aShape = BO.Shape();
    } else if (aType == BOOLEAN_FUSE) {
      BRepAlgoAPI_Fuse BO (aShape1, aShape2);
      if (!BO.IsDone()) {
        StdFail_NotDone::Raise("Requested boolean operation can not be performed on the given shapes");
      }
      aShape = BO.Shape();
    } else if (aType == BOOLEAN_SECTION) {
      BRepAlgoAPI_Section BO (aShape1, aShape2);
      if (!BO.IsDone()) {
        StdFail_NotDone::Raise("Requested boolean operation can not be performed on the given shapes");
      }
      aShape = BO.Shape();
    } else {
    }
  }

  if (aShape.IsNull()) return 0;
  if (!BRepAlgo::IsValid(aShape)) {
    Standard_ConstructionError::Raise("Boolean aborted : non valid shape result");
  }

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}


//=======================================================================
//function :  GEOMImpl_BooleanDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_BooleanDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_BooleanDriver",
			                                 sizeof(GEOMImpl_BooleanDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_BooleanDriver) Handle(GEOMImpl_BooleanDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_BooleanDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_BooleanDriver))) {
       _anOtherObject = Handle(GEOMImpl_BooleanDriver)((Handle(GEOMImpl_BooleanDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
