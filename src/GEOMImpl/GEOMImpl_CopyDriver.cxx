
using namespace std;
#include "GEOMImpl_CopyDriver.hxx"
#include "GEOMImpl_ICopy.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs.hxx>
#include <TNaming_CopyShape.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_CopyDriver::GetID()
{
  static Standard_GUID aCopyDriver("FF1BBB53-5D14-4df2-980B-3A668264EA16");
  return aCopyDriver; 
}


//=======================================================================
//function : GEOMImpl_CopyDriver
//purpose  : 
//=======================================================================
GEOMImpl_CopyDriver::GEOMImpl_CopyDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_CopyDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  Standard_Integer aType = aFunction->GetType();

  GEOMImpl_ICopy aCI (aFunction);
  TopoDS_Shape aCopy;
  
  if(aType == COPY_WITH_REF) {
  
    Handle(GEOM_Function) aRefFunction = aCI.GetOriginal();
    if (aRefFunction.IsNull()) return 0;
    TopoDS_Shape anOriginal = aRefFunction->GetValue();

    TColStd_IndexedDataMapOfTransientTransient aMap;
  
    TNaming_CopyShape::CopyTool(anOriginal, aMap, aCopy);
  }
  else if(aType == COPY_WITHOUT_REF) {
    aCopy = aFunction->GetValue();
  }
  
  if (aCopy.IsNull()) return 0;

  aFunction->SetValue(aCopy);

  log.SetTouched(Label()); 

  return 1;    
}


//=======================================================================
//function :  GEOMImpl_CopyDriver_Type_
//purpose  :
//======================================================================= 
Standard_EXPORT Handle_Standard_Type& GEOMImpl_CopyDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared); 
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_CopyDriver",
			                                 sizeof(GEOMImpl_CopyDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//======================================================================= 
const Handle(GEOMImpl_CopyDriver) Handle(GEOMImpl_CopyDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_CopyDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_CopyDriver))) {
       _anOtherObject = Handle(GEOMImpl_CopyDriver)((Handle(GEOMImpl_CopyDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
