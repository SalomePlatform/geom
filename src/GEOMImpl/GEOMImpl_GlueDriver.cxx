
using namespace std;
#include "GEOMImpl_GlueDriver.hxx"
#include "GEOMImpl_IGlue.hxx"
#include "GEOMImpl_Types.hxx"

#include "GEOM_Function.hxx"

#include "GEOMAlgo_Gluer.hxx"

#include "utilities.h"

#include <TopoDS_Shape.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Failure.hxx>

//=======================================================================
//function : GEOMImpl_GlueDriver
//purpose  :
//=======================================================================
GEOMImpl_GlueDriver::GEOMImpl_GlueDriver()
{
}

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_GlueDriver::GetID()
{
  static Standard_GUID aGlueDriver("FF1BBB63-5D14-4df2-980B-3A668264EA16");
  return aGlueDriver;
}

//=======================================================================
//function : GlueFacesWithWarnings
//purpose  :
//=======================================================================
TopoDS_Shape GEOMImpl_GlueDriver::GlueFacesWithWarnings (const TopoDS_Shape& theShape,
                                                         const Standard_Real theTolerance,
                                                         TCollection_AsciiString& theWarning) const
{
  Standard_Integer iErr, iWrn;
  TopoDS_Shape aRes;
  GEOMAlgo_Gluer aGluer;

  aGluer.SetShape(theShape);
  aGluer.SetTolerance(theTolerance);
  aGluer.SetCheckGeometry(Standard_True);

  aGluer.Perform();

  iErr = aGluer.ErrorStatus();
  if (iErr) {
    switch (iErr) {
    case 2:
      Standard_Failure::Raise("No vertices found in source shape");
      break;
    case 5:
      Standard_Failure::Raise("Source shape is Null");
      break;
    case 6:
      Standard_Failure::Raise("Result shape is Null");
      break;
    case 200:
      Standard_Failure::Raise("Error occured during check of geometric coincidence");
      break;
    default:
      {
        // description of all errors see in GEOMAlgo_Gluer.cxx
        TCollection_AsciiString aMsg ("Error in GEOMAlgo_Gluer with code ");
        aMsg += TCollection_AsciiString(iErr);
        Standard_Failure::Raise(aMsg.ToCString());
        break;
      }
    }
    return aRes;
  }

  iWrn = aGluer.WarningStatus();
  if (iWrn) {
    switch (iWrn) {
    case 1:
      {
        Standard_Integer nbAlone = aGluer.AloneShapes();
        theWarning = TCollection_AsciiString(nbAlone);
        theWarning += " solid(s) can not be glued by faces";
      }
      break;
    default:
      // description of all warnings see in GEOMAlgo_Gluer.cxx
      theWarning = "Warning in GEOMAlgo_Gluer with code ";
      theWarning += TCollection_AsciiString(iWrn);
      break;
    }
  }

  aRes = aGluer.Result();

  return aRes;
}

//=======================================================================
//function : GlueFaces
//purpose  :
//=======================================================================
TopoDS_Shape GEOMImpl_GlueDriver::GlueFaces (const TopoDS_Shape& theShape,
                                             const Standard_Real theTolerance)
{
  Standard_Integer iErr, iWrn;
  TopoDS_Shape aRes;
  GEOMAlgo_Gluer aGluer;

  aGluer.SetShape(theShape);
  aGluer.SetTolerance(theTolerance);
  aGluer.SetCheckGeometry(Standard_True);

  aGluer.Perform();

  iErr = aGluer.ErrorStatus();
  if (iErr) {
    switch (iErr) {
    case 2:
      Standard_Failure::Raise("No vertices found in source shape");
      break;
    case 5:
      Standard_Failure::Raise("Source shape is Null");
      break;
    case 6:
      Standard_Failure::Raise("Result shape is Null");
      break;
    case 200:
      Standard_Failure::Raise("Error occured during check of geometric coincidence");
      break;
    default:
      {
        // description of all errors see in GEOMAlgo_Gluer.cxx
        TCollection_AsciiString aMsg ("Error in GEOMAlgo_Gluer with code ");
        aMsg += TCollection_AsciiString(iErr);
        Standard_Failure::Raise(aMsg.ToCString());
        break;
      }
    }
    return aRes;
  }

  iWrn = aGluer.WarningStatus();
  if (iWrn) {
    switch (iWrn) {
    case 1:
      MESSAGE("Some shapes can not be glued by faces");
      break;
    default:
      // description of all warnings see in GEOMAlgo_Gluer.cxx
      MESSAGE("Warning in GEOMAlgo_Gluer with code " << iWrn);
      break;
    }
  }

  aRes = aGluer.Result();

  return aRes;
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_GlueDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IGlue aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;
  TCollection_AsciiString aWrn;

  if (aType == GLUE_FACES) {
    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    if (aShapeBase.IsNull()) {
      Standard_NullObject::Raise("Shape for gluing is null");
    }

    Standard_Real tol3d = aCI.GetTolerance();
    aShape = GlueFacesWithWarnings(aShapeBase, tol3d, aWrn);
  } else {
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  if (!aWrn.IsEmpty()) {
    Standard_Failure::Raise(aWrn.ToCString());
  }

  return 1;
}

//=======================================================================
//function :  GEOMImpl_GlueDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_GlueDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_GlueDriver",
			                                 sizeof(GEOMImpl_GlueDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_GlueDriver) Handle(GEOMImpl_GlueDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_GlueDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_GlueDriver))) {
       _anOtherObject = Handle(GEOMImpl_GlueDriver)((Handle(GEOMImpl_GlueDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
