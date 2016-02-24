// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// internal includes
#include "IGESPlugin_ImportDriver.hxx"
#include "IGESPlugin_IImport.hxx"

// KERNEL includes
#include <utilities.h>
#include <Basics_Utils.hxx>

// GEOM includes
#include "GEOM_Function.hxx"
#include "GEOMImpl_Types.hxx"

// OOCT includes
#include <IFSelect_ReturnStatus.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_IGESEntity.hxx>

#include <Interface_Static.hxx>
#include <Interface_InterfaceModel.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Transfer_Binder.hxx>
#include <TransferBRep.hxx>

#include <TNaming_Builder.hxx>
#include <TDF_TagSource.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Builder.hxx>
#include <gp_Pnt.hxx>

#include <StdFail_NotDone.hxx>
#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& IGESPlugin_ImportDriver::GetID()
{
  static Standard_GUID aGUID("3dbb09b8-659f-4a0f-88b9-d9968c43a448");
  return aGUID;
}

//=======================================================================
//function : IGESPlugin_ImportDriver
//purpose  :
//=======================================================================
IGESPlugin_ImportDriver::IGESPlugin_ImportDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer IGESPlugin_ImportDriver::Execute(LOGBOOK& log) const
{
  if( Label().IsNull() ) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction( Label() );

  IGESPlugin_IImport aData( aFunction );

  TCollection_AsciiString aFileName = aData.GetFileName().ToCString();
  bool anIsIgnoreUnits = aData.GetIsIgnoreUnits();
  TDF_Label aShapeLabel = aFunction->GetNamingEntry();

  TopoDS_Shape aResShape;
  TCollection_AsciiString anError;

  // Set "C" numeric locale to save numbers correctly
  Kernel_Utils::Localizer loc;

  IGESControl_Reader aReader;

  Interface_Static::SetCVal( "xstep.cascade.unit", "M" );

  try {
    OCC_CATCH_SIGNALS;

    IFSelect_ReturnStatus status = aReader.ReadFile( aFileName.ToCString() );

    if (status == IFSelect_RetDone) {
      if( anIsIgnoreUnits ) {
        // need re-scale a model, set UnitFlag to 'meter'
        Handle(IGESData_IGESModel) aModel =
          Handle(IGESData_IGESModel)::DownCast( aReader.Model() );
        if( !aModel.IsNull() ) {
          IGESData_GlobalSection aGS = aModel->GlobalSection();
          aGS.SetUnitFlag(6);
          aModel->SetGlobalSection(aGS);
        }
      }

      MESSAGE("ImportIGES : all Geometry Transfer");
      //OCC 5.1.2 porting
      //     aReader.Clear();
      //     aReader.TransferRoots(false);
      aReader.ClearShapes();
      aReader.TransferRoots();

      MESSAGE("ImportIGES : count of shapes produced = " << aReader.NbShapes());
      aResShape = aReader.OneShape();

      // BEGIN: Store names of sub-shapes from file
      Handle(Interface_InterfaceModel) Model = aReader.WS()->Model();
      Handle(XSControl_TransferReader) TR = aReader.WS()->TransferReader();
      if (!TR.IsNull()) {
        Handle(Transfer_TransientProcess) TP = /*TransientProcess();*/TR->TransientProcess();
        Standard_Integer nb = Model->NbEntities();
        for (Standard_Integer i = 1; i <= nb; i++) {
          Handle(IGESData_IGESEntity) ent = Handle(IGESData_IGESEntity)::DownCast(Model->Value(i));
          if (ent.IsNull() || ! ent->HasName()) continue;

          // find target shape
          Handle(Transfer_Binder) binder = TP->Find( ent );
          if( binder.IsNull() ) continue;
          TopoDS_Shape S = TransferBRep::ShapeResult( binder );
          if( S.IsNull() ) continue;

          // create label and set shape
          TDF_Label L;
          TDF_TagSource aTag;
          L = aTag.NewChild( aShapeLabel );
          TNaming_Builder tnBuild (L);
          tnBuild.Generated(S);

          // set a name
          TCollection_AsciiString string = ent->NameValue()->String();
          string.LeftAdjust();
          string.RightAdjust();
          TCollection_ExtendedString str (string);
          TDataStd_Name::Set(L, str);
        }
      }
      // END: Store names
    }
    else {
      switch (status) {
        case IFSelect_RetVoid:
          anError = "Nothing created or No data to process";
          break;
        case IFSelect_RetError:
          anError = "Error in command or input data";
          break;
        case IFSelect_RetFail:
          anError = "Execution was run, but has failed";
          break;
        case IFSelect_RetStop:
          anError = "Execution has been stopped. Quite possible, an exception was raised";
          break;
        default:
          break;
        }
      anError = "Wrong format of the imported file. Can't import file.";
      aResShape.Nullify();
    }
  }
  catch( Standard_Failure ) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    anError = aFail->GetMessageString();
    aResShape.Nullify();
  }

  if( aResShape.IsNull() ) {
    StdFail_NotDone::Raise( anError.ToCString() );
    return 0;
  }

  aFunction->SetValue( aResShape );

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

bool IGESPlugin_ImportDriver::
GetCreationInformation( std::string&             theOperationName,
                        std::vector<GEOM_Param>& theParams )
{
  if( Label().IsNull() ) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction( Label() );

  IGESPlugin_IImport aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "ImportIGES";

  switch ( aType ) {
  case IMPORT_SHAPE:
    AddParam( theParams, "File name", aCI.GetFileName() );
    if( aCI.GetIsIgnoreUnits() )
      AddParam( theParams, "Format", "IGES_SCALE" );
    break;
  default:
    return false;
  }
  return true;
}

TCollection_AsciiString
IGESPlugin_ImportDriver::GetValue( const TCollection_AsciiString& theFileName,
                                     const TCollection_AsciiString& theParameterName,
                                     TCollection_AsciiString&       theError )
{
  Handle(TCollection_HAsciiString) aValue;

  if (theParameterName != "LEN_UNITS") {
    theError = theParameterName + " parameter reading is not supported by IGES plugin";
    return TCollection_AsciiString();
  }

  // Set "C" numeric locale to save numbers correctly
  Kernel_Utils::Localizer loc;

  IGESControl_Reader aReader;

  Interface_Static::SetCVal("xstep.cascade.unit","M");

  try {
    OCC_CATCH_SIGNALS;

    IFSelect_ReturnStatus status = aReader.ReadFile(theFileName.ToCString());
    if (status == IFSelect_RetDone) {
      Handle(IGESData_IGESModel) aModel =
        Handle(IGESData_IGESModel)::DownCast(aReader.Model());
      if (!aModel.IsNull()) {
        aValue = aModel->GlobalSection().UnitName();
        //if (!aValue.IsNull()) {
        //  Handle(TCollection_HAsciiString) aPrefix = new TCollection_HAsciiString ("UNIT_");
        //  aValue->Prepend(aPrefix);
        //}
      }
    }
    else {
      theError = theFileName + " reading failed";
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    theError = aFail->GetMessageString();
  }
  if (!aValue.IsNull())
    return aValue->String();
  else
    return TCollection_AsciiString();
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT( IGESPlugin_ImportDriver, GEOM_BaseDriver );
