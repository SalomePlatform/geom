// Copyright (C) 2014-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include "STEPPlugin_ImportDriver.hxx"
#include "STEPPlugin_IImport.hxx"

// KERNEL includes
#include <utilities.h>
#include <Basics_Utils.hxx>

// GEOM includes
#include "GEOM_Function.hxx"
#include "GEOMImpl_Types.hxx"

// OOCT includes
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Comment.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Static.hxx>
#include <STEPControl_Reader.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_Representation.hxx>
#include <TransferBRep.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>
#include <BRep_Builder.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>

#include <StdFail_NotDone.hxx>
#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

namespace
{
  //=============================================================================
  /*!
   *  GetShape()
   */
  //=============================================================================
  
  TopoDS_Shape GetShape(const Handle(Standard_Transient)        &theEnti,
                        const Handle(Transfer_TransientProcess) &theTP)
  {
    TopoDS_Shape            aResult;
    Handle(Transfer_Binder) aBinder = theTP->Find(theEnti);
    
    if (aBinder.IsNull()) {
      return aResult;
    }
    
    aResult = TransferBRep::ShapeResult(aBinder);
    
    return aResult;
  }
  
  //=============================================================================
  /*!
   *  GetLabel()
   */
  //=============================================================================
  
  TDF_Label GetLabel(const Handle(Standard_Transient) &theEnti,
                     const TDF_Label                  &theShapeLabel,
                     const TopoDS_Shape               &aShape)
  {
    TDF_Label aResult;
    
    if (theEnti->IsKind
        (STANDARD_TYPE(StepGeom_GeometricRepresentationItem))) {
      // check all named shapes using iterator
      TDF_ChildIDIterator anIt
        (theShapeLabel, TDataStd_Name::GetID(), Standard_True);
      
      for (; anIt.More(); anIt.Next()) {
        Handle(TDataStd_Name) nameAttr =
          Handle(TDataStd_Name)::DownCast(anIt.Value());
        
        if (nameAttr.IsNull()) {
          continue;
        }
        
        TDF_Label aLab = nameAttr->Label();
        Handle(TNaming_NamedShape) shAttr;
        
        if (aLab.FindAttribute(TNaming_NamedShape::GetID(), shAttr) &&
            shAttr->Get().IsEqual(aShape)) {
          aResult = aLab;
        }
      }
    }
    
    // create label and set shape
    if (aResult.IsNull()) {
      TDF_TagSource aTag;
      
      aResult = aTag.NewChild(theShapeLabel);
      
      TNaming_Builder tnBuild (aResult);
      
      tnBuild.Generated(aShape);
    }
    
    return aResult;
  }

  //=============================================================================
  /*!
   *  StoreName()
   */
  //=============================================================================
  
  void StoreName( const Handle(Standard_Transient)        &theEnti,
                  const TopTools_IndexedMapOfShape        &theIndices,
                  const Handle(Transfer_TransientProcess) &theTP,
                  const TDF_Label                         &theShapeLabel)
  {
    Handle(TCollection_HAsciiString) aName;
    
    if (theEnti->IsKind(STANDARD_TYPE(StepShape_TopologicalRepresentationItem)) ||
        theEnti->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem))) {
      aName = Handle(StepRepr_RepresentationItem)::DownCast(theEnti)->Name();
    } else {
      Handle(StepBasic_ProductDefinition) PD =
        Handle(StepBasic_ProductDefinition)::DownCast(theEnti);
      
      if (PD.IsNull() == Standard_False) {
        Handle(StepBasic_Product) Prod = PD->Formation()->OfProduct();
        aName = Prod->Name();
      }
    }
    
    bool isValidName = false;

    if (aName.IsNull() == Standard_False) {
      isValidName = true;

      if (aName->UsefullLength() < 1) {
        isValidName = false;
      } else if (aName->UsefullLength() == 4 &&
                 toupper (aName->Value(1)) == 'N' &&
                 toupper (aName->Value(2)) == 'O' &&
                 toupper (aName->Value(3)) == 'N' &&
                 toupper (aName->Value(4)) == 'E') {
        // skip 'N0NE' name
        isValidName = false;
      } else {
        // special check to pass names like "Open CASCADE STEP translator 6.3 1"
        TCollection_AsciiString aSkipName ("Open CASCADE STEP translator");

        if (aName->Length() >= aSkipName.Length()) {
          if (aName->String().SubString
              (1, aSkipName.Length()).IsEqual(aSkipName)) {
            isValidName = false;
          }
        }
      }
    }

    if (isValidName) {
      TCollection_ExtendedString aNameExt (aName->ToCString());

      // find target shape
      TopoDS_Shape S = GetShape(theEnti, theTP);

      if (S.IsNull()) {
        return;
      }

      // as PRODUCT can be included in the main shape
      // several times, we look here for all iclusions.
      Standard_Integer isub, nbSubs = theIndices.Extent();

      for (isub = 1; isub <= nbSubs; isub++) {
        TopoDS_Shape aSub = theIndices.FindKey(isub);

        if (aSub.IsPartner(S)) {
          TDF_Label L = GetLabel(theEnti, theShapeLabel, aSub);

          // set a name
          TDataStd_Name::Set(L, aNameExt);
        }
      }
    }
  }

  //=============================================================================
  /*!
   *  StoreMaterial()
   */
  //=============================================================================

  void StoreMaterial( const Handle(Standard_Transient)        &theEnti,
                      const TopTools_IndexedMapOfShape        &theIndices,
                      const Handle(Transfer_TransientProcess) &theTP,
                      const TDF_Label                         &theShapeLabel )
  {
    // Treat Product Definition Shape only.
    Handle(StepRepr_ProductDefinitionShape) aPDS =
      Handle(StepRepr_ProductDefinitionShape)::DownCast(theEnti);
    Handle(StepBasic_ProductDefinition)     aProdDef;

    if(aPDS.IsNull() == Standard_False) {
      // Product Definition Shape ==> Product Definition
      aProdDef = aPDS->Definition().ProductDefinition();
    }

    if (aProdDef.IsNull() == Standard_False) {
      // Product Definition ==> Property Definition
      const Interface_Graph    &aGraph = theTP->Graph();
      Interface_EntityIterator  aSubs  = aGraph.Sharings(aProdDef);
      TopoDS_Shape              aShape;

      for(aSubs.Start(); aSubs.More(); aSubs.Next()) {
        Handle(StepRepr_PropertyDefinition) aPropD =
          Handle(StepRepr_PropertyDefinition)::DownCast(aSubs.Value());

        if(aPropD.IsNull() == Standard_False) {
          // Property Definition ==> Representation.
          Interface_EntityIterator aSubs1 = aGraph.Sharings(aPropD);

          for(aSubs1.Start(); aSubs1.More(); aSubs1.Next()) {
            Handle(StepRepr_PropertyDefinitionRepresentation) aPDR =
              Handle(StepRepr_PropertyDefinitionRepresentation)::
              DownCast(aSubs1.Value());

            if(aPDR.IsNull() == Standard_False) {
              // Property Definition ==> Material Name.
              Handle(StepRepr_Representation) aRepr = aPDR->UsedRepresentation();

              if(aRepr.IsNull() == Standard_False) {
                Standard_Integer ir;

                for(ir = 1; ir <= aRepr->NbItems(); ir++) {
                  Handle(StepRepr_RepresentationItem) aRI = aRepr->ItemsValue(ir);
                  Handle(StepRepr_DescriptiveRepresentationItem) aDRI =
                    Handle(StepRepr_DescriptiveRepresentationItem)::DownCast(aRI);

                  if(aDRI.IsNull() == Standard_False) {
                    // Get shape from Product Definition
                    Handle(TCollection_HAsciiString) aMatName = aDRI->Name();

                    if(aMatName.IsNull() == Standard_False) {
                      TCollection_ExtendedString
                        aMatNameExt (aMatName->ToCString());

                      if (aShape.IsNull()) {
                        // Get the shape.
                        aShape = GetShape(aProdDef, theTP);

                        if (aShape.IsNull()) {
                          return;
                        }
                      }

                      // as PRODUCT can be included in the main shape
                      // several times, we look here for all iclusions.
                      Standard_Integer isub, nbSubs = theIndices.Extent();

                      for (isub = 1; isub <= nbSubs; isub++) {
                        TopoDS_Shape aSub = theIndices.FindKey(isub);

                        if (aSub.IsPartner(aShape)) {
                          TDF_Label aLabel =
                            GetLabel(aProdDef, theShapeLabel, aSub);

                          // set a name
                          TDataStd_Comment::Set(aLabel, aMatNameExt);
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  TCollection_AsciiString ToNamedUnit( const TCollection_AsciiString& unit )
  {
    TCollection_AsciiString result = unit;
    result.LowerCase();
    if ( result == "mil" ) result = "milliinch";
    return result;
  }

  TCollection_AsciiString ToOcctUnit( const TCollection_AsciiString& unit, TCollection_AsciiString& error )
  {
    TCollection_AsciiString result = "M", u = ToNamedUnit(unit);
    u.LowerCase();

    if (u == "inch")
      result = "INCH";
    else if (u == "milliinch")
      result = "MIL";
    else if (u == "microinch")
      result = "UIN";
    else if (u == "foot")
      result = "FT";
    else if (u == "mile")
      result = "MI";
    else if (u == "metre")
      result = "M";
    else if (u == "kilometre")
      result = "KM";
    else if (u == "millimetre")
      result = "MM";
    else if (u == "centimetre")
      result = "CM";
    else if (u == "micrometre")
      result = "UM";
    else if (u.IsEmpty())
      result = "M";
    else
      error = "The file contains not supported units";
      
    // TODO (for other units)
    // else
    //  result = "??"

    return result;
  }

} // end of namespace

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& STEPPlugin_ImportDriver::GetID()
{
  static Standard_GUID aGUID("a25f88df-461b-45c0-ab6b-a82101fe6ce7");
  return aGUID;
}

//=======================================================================
//function : STEPPlugin_ImportDriver
//purpose  :
//=======================================================================
STEPPlugin_ImportDriver::STEPPlugin_ImportDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer STEPPlugin_ImportDriver::Execute( TFunction_Logbook& log ) const
{
  if( Label().IsNull() ) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction( Label() );

  STEPPlugin_IImport aData( aFunction );

  TCollection_AsciiString aFileName = aData.GetFileName().ToCString();
  bool anIsIgnoreUnits = aData.GetIsIgnoreUnits();
  TDF_Label aShapeLabel = aFunction->GetNamingEntry();

  MESSAGE("Import STEP from file " << aFileName.ToCString() );

  TopoDS_Shape aResShape;
  TCollection_AsciiString anError;

  // Set "C" numeric locale to save numbers correctly
  Kernel_Utils::Localizer loc;

  STEPControl_Reader aReader;

  //VSR: 16/09/09: Convert to METERS
  Interface_Static::SetCVal("xstep.cascade.unit","M");
  Interface_Static::SetIVal("read.step.ideas", 1);
  Interface_Static::SetIVal("read.step.nonmanifold", 1);

  BRep_Builder B;
  TopoDS_Compound compound;
  B.MakeCompound(compound);

  try
  {
    OCC_CATCH_SIGNALS;
    IFSelect_ReturnStatus status = aReader.ReadFile(aFileName.ToCString());
    if (status == IFSelect_RetDone) {
      // Regard or not the model units
      if( anIsIgnoreUnits ) {
        // set UnitFlag to units from file
        TColStd_SequenceOfAsciiString anUnitLengthNames;
        TColStd_SequenceOfAsciiString anUnitAngleNames;
        TColStd_SequenceOfAsciiString anUnitSolidAngleNames;
        aReader.FileUnits(anUnitLengthNames, anUnitAngleNames, anUnitSolidAngleNames);
        if (anUnitLengthNames.Length() > 0) {
          TCollection_AsciiString aLenUnits = ToOcctUnit(anUnitLengthNames.First(), anError);
          Interface_Static::SetCVal("xstep.cascade.unit", aLenUnits.ToCString());
        }
      }
      else {
        // Need re-scale a model (set UnitFlag to 'meter')
        Interface_Static::SetCVal("xstep.cascade.unit","M");
      }

      Standard_Boolean failsonly = Standard_False;
      aReader.PrintCheckLoad(failsonly, IFSelect_ItemsByEntity);

      // Root transfers
      Standard_Integer nbr = aReader.NbRootsForTransfer();
      aReader.PrintCheckTransfer(failsonly, IFSelect_ItemsByEntity);

      for (Standard_Integer n = 1; n <= nbr; n++) {
        Standard_Boolean ok = aReader.TransferRoot(n);
        // Collecting resulting entities
        Standard_Integer nbs = aReader.NbShapes();
        if (!ok || nbs == 0)
          continue; // skip empty root

        // For a single entity
        else if (nbr == 1 && nbs == 1) {
          aResShape = aReader.Shape(1);
          if (aResShape.ShapeType() == TopAbs_COMPOUND) {
            int nbSub1 = 0;
            TopoDS_Shape currShape;
            TopoDS_Iterator It (aResShape, Standard_True, Standard_True);
            for (; It.More(); It.Next()) {
              nbSub1++;
              currShape = It.Value();
            }
            if (nbSub1 == 1)
              aResShape = currShape;
          }
          break;
        }

        for (Standard_Integer i = 1; i <= nbs; i++) {
          TopoDS_Shape aShape = aReader.Shape(i);
          if (aShape.IsNull())
            continue;
          else
            B.Add(compound, aShape);
        }
      }
      if( aResShape.IsNull() )
        aResShape = compound;

      // Check if any BRep entity has been read, there must be at least a vertex
      if ( !TopExp_Explorer( aResShape, TopAbs_VERTEX ).More() )
        StdFail_NotDone::Raise( "No geometrical data in the imported file." );

      // BEGIN: Store names and materials of sub-shapes from file
      TopTools_IndexedMapOfShape anIndices;
      TopExp::MapShapes(aResShape, anIndices);

      Handle(Interface_InterfaceModel) Model = aReader.WS()->Model();
      Handle(XSControl_TransferReader) TR = aReader.WS()->TransferReader();
      if (!TR.IsNull()) {
        Handle(Transfer_TransientProcess) TP = TR->TransientProcess();

        Standard_Integer nb = Model->NbEntities();

        for (Standard_Integer ie = 1; ie <= nb; ie++) {
          Handle(Standard_Transient) enti = Model->Value(ie);

          // Store names.
          StoreName(enti, anIndices, TP, aShapeLabel);

          // Store materials.
          StoreMaterial(enti, anIndices, TP, aShapeLabel);
        }
      }
      // END: Store names and materials
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

  log.SetTouched( Label() );

  return 1;
}

//=======================================================================
//function : MustExecute
//purpose  :
//=======================================================================
Standard_Boolean STEPPlugin_ImportDriver::MustExecute( const TFunction_Logbook& ) const
{
  return Standard_True;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool STEPPlugin_ImportDriver::
GetCreationInformation( std::string&             theOperationName,
                        std::vector<GEOM_Param>& theParams )
{
  if( Label().IsNull() ) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction( Label() );

  STEPPlugin_IImport aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "ImportSTEP";

  switch ( aType ) {
  case IMPORT_SHAPE:
    AddParam( theParams, "File name", aCI.GetFileName() );
    if( aCI.GetIsIgnoreUnits() )
      AddParam( theParams, "Format", "STEP_SCALE" );
    break;
  default:
    return false;
  }
  return true;
}

TCollection_AsciiString
STEPPlugin_ImportDriver::GetValue( const TCollection_AsciiString& theFileName,
                                   const TCollection_AsciiString& theParameterName,
                                   TCollection_AsciiString&       theError )
{
  TCollection_AsciiString aValue;

  if (theParameterName != "LEN_UNITS") {
    theError = theParameterName + " parameter reading is not supported by STEP plugin";
    return TCollection_AsciiString();
  }

  // Set "C" numeric locale to save numbers correctly
  Kernel_Utils::Localizer loc;

  STEPControl_Reader aReader;

  Interface_Static::SetCVal( "xstep.cascade.unit","M" );
  Interface_Static::SetIVal( "read.step.ideas", 1 );
  Interface_Static::SetIVal( "read.step.nonmanifold", 1 );

  try {
    OCC_CATCH_SIGNALS;
    IFSelect_ReturnStatus status = aReader.ReadFile( theFileName.ToCString() );
    if (status == IFSelect_RetDone) {
      TColStd_SequenceOfAsciiString anUnitLengthNames;
      TColStd_SequenceOfAsciiString anUnitAngleNames;
      TColStd_SequenceOfAsciiString anUnitSolidAngleNames;
      aReader.FileUnits(anUnitLengthNames, anUnitAngleNames, anUnitSolidAngleNames);
      if (anUnitLengthNames.Length() > 0)
        aValue = ToNamedUnit( anUnitLengthNames.First() );
    }
    else {
      theError = theFileName + " reading failed";
    }
  }
  catch( Standard_Failure ) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    theError = aFail->GetMessageString();
  }
  return aValue;
}


IMPLEMENT_STANDARD_HANDLE( STEPPlugin_ImportDriver, GEOM_BaseDriver );
IMPLEMENT_STANDARD_RTTIEXT( STEPPlugin_ImportDriver, GEOM_BaseDriver );
