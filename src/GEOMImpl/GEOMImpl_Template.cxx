
using namespace std;
#include "GEOMImpl_Template.hxx"
#include "GEOM_Function.hxx"
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_Template::GetID()
{
  static Standard_GUID aTemplate("Insert the correct GUID here");
  return aTemplate; 
}


//=======================================================================
//function : GEOMImpl_Template
//purpose  : 
//=======================================================================

GEOMImpl_Template::GEOMImpl_Template() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_Template::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label()); 
  if(aFunction.IsNull()) return 0;
  
  //Add implemetation here
  
  log.SetTouched(Label());
  return 1;    
}


//=======================================================================
//function :  GEOMImpl_Template_Type_
//purpose  :
//======================================================================= 
Standard_EXPORT Handle_Standard_Type& GEOMImpl_Template_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared); 
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_Template",
			                                 sizeof(GEOMImpl_Template),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//======================================================================= 

const Handle(GEOMImpl_Template) Handle(GEOMImpl_Template)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_Template) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_Template))) {
       _anOtherObject = Handle(GEOMImpl_Template)((Handle(GEOMImpl_Template)&)AnObject);
     }
  }

  return _anOtherObject ;
}


