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
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
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
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
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
   *  GetAllParents()
   */
  //=============================================================================

  Standard_Boolean GetAllParents(const TopoDS_Shape         &theShape,
                                 const TopoDS_Shape         &theSubShape,
                                       TopTools_ListOfShape &theParents)
  {
    const TopAbs_ShapeEnum aSubShType = theSubShape.ShapeType();
    Standard_Boolean       aResult    = Standard_False;

    if (theShape.ShapeType() >= aSubShType) {
      return aResult; // NULL shape
    }

    TopoDS_Iterator     anIt(theShape);
    TopTools_MapOfShape aMapFence;

    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape &aSubShape = anIt.Value();

      if (aMapFence.Add(aSubShape)) {
        if (theSubShape.IsSame(aSubShape)) {
          // The sub-shape is found. theShape is its parent.
          theParents.Append(theShape);
          aResult = Standard_True;
          break;
        }

        if (aSubShape.ShapeType() < aSubShType) {
          if (GetAllParents(aSubShape, theSubShape, theParents)) {
            // The sub-shape is found.
            theParents.Append(theShape);
            aResult = Standard_True;
            break;
          }
        }
      }
    }

    return aResult;
  }

  //=============================================================================
  /*!
   *  BuildModifiedShape()
   */
  //=============================================================================

  TopoDS_Shape BuildModifiedShape
              (const TopoDS_Shape                 &theShape,
                     TopTools_DataMapOfShapeShape &theMapModified)
  {
    // Check if the shape is modified.
    TopoDS_Shape     aFwdShape  = theShape.Oriented(TopAbs_FORWARD);
    TopoDS_Iterator  anIt(aFwdShape);
    Standard_Boolean isModified = Standard_False;

    for (; anIt.More(); anIt.Next()) {
      if (theMapModified.IsBound(anIt.Value())) {
        isModified = Standard_True;
        break;
      }
    }

    TopoDS_Shape aResult;

    if (isModified) {
      BRep_Builder aBuilder;

      aResult = aFwdShape.EmptyCopied();

      for (anIt.Initialize(aFwdShape); anIt.More(); anIt.Next()) {
        const TopoDS_Shape &aSubShape = anIt.Value();

        if (theMapModified.IsBound(aSubShape)) {
          TopoDS_Shape aModifSubShape = theMapModified.Find(aSubShape);

          if (aModifSubShape.IsNull()) {
            // Recursively compute the sub-shape.
            aModifSubShape = BuildModifiedShape(aSubShape, theMapModified);
          }

          aBuilder.Add(aResult, aModifSubShape);
        } else {
          aBuilder.Add(aResult, aSubShape);
        }
      }

      // Set the result shape orienation.
      aResult.Orientation(theShape.Orientation());
      theMapModified.Bind(theShape, aResult);
    } else {
      aResult = theShape;
    }

    return aResult;
  }

  //=============================================================================
  /*!
   *  CreateAssemblies()
   */
  //=============================================================================

  TopoDS_Shape CreateAssemblies
              (const STEPControl_Reader           &theReader,
               const TopoDS_Shape                 &theShape,
                     TopTools_DataMapOfShapeShape &theMapShapeAssembly)
  {
    TopoDS_Shape                     aResult = theShape;
    Handle(XSControl_TransferReader) aTR     = theReader.WS()->TransferReader();
    TopTools_ListOfShape             aListAssemblies;

    if (!aTR.IsNull()) {
      Handle(Interface_InterfaceModel)  aModel      = theReader.WS()->Model();
      Handle(Transfer_TransientProcess) aTP         = aTR->TransientProcess();
      Standard_Integer                  aNbEntities = aModel->NbEntities();
      Standard_Integer                  i;
      Handle(Standard_Type)             aNAUOType   =
              STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence);

      for (i = 1; i <= aNbEntities; i++) {
        Handle(Standard_Transient) anEnti = aModel->Value(i);

        if (anEnti->IsKind(aNAUOType)) {
          // This is an assembly. Find target shape
          TopoDS_Shape aShape = GetShape(anEnti, aTP);

          if (aShape.IsNull()) {
            continue;
          }

          if (aShape.ShapeType() != TopAbs_COMPOUND) {
            aListAssemblies.Append(aShape);
          }
        }
      }
    }

    // Create assemblies.
    if (!aListAssemblies.IsEmpty()) {
      TopTools_ListIteratorOfListOfShape anIter(aListAssemblies);
      BRep_Builder                       aBuilder;

      for (; anIter.More(); anIter.Next()) {
        const TopoDS_Shape   &aShape  = anIter.Value();
        TopTools_ListOfShape  aParents;

        if (GetAllParents(theShape, aShape, aParents) &&
            aParents.First().ShapeType() == TopAbs_COMPOUND) {
          TopoDS_Compound                    aComp;
          TopTools_ListIteratorOfListOfShape aParentIter(aParents);

          // Fill theMapShapeAssembly.
          for (; aParentIter.More(); aParentIter.Next()) {
            theMapShapeAssembly.Bind(aParentIter.Value(), TopoDS_Shape());
          }

          aBuilder.MakeCompound(aComp);
          aBuilder.Add(aComp, aShape);
          theMapShapeAssembly.Bind(aShape, aComp);
        }
      }

      // Build a new shape.
      aResult = BuildModifiedShape(theShape, theMapShapeAssembly);
    }

    return aResult;
  }

  //=============================================================================
  /*!
   *  StoreName()
   */
  //=============================================================================
  
  void StoreName(const Handle(Standard_Transient)        &theEnti,
                 const TopTools_IndexedMapOfShape        &theIndices,
                 const Handle(XSControl_WorkSession)     &theWS,
                 const Handle(Transfer_TransientProcess) &theTP,
                 const TDF_Label                         &theShapeLabel,
                       TopTools_DataMapOfShapeShape      &theMapShapeAssembly)
  {
    Handle(TCollection_HAsciiString) aName;
    
    if (theEnti->IsKind(STANDARD_TYPE(StepShape_TopologicalRepresentationItem)) ||
        theEnti->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem))) {
      aName = Handle(StepRepr_RepresentationItem)::DownCast(theEnti)->Name();
    } else if (theEnti->IsKind(STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence))) {
      Handle(StepRepr_NextAssemblyUsageOccurrence) aNAUO = 
        Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(theEnti);

      Interface_EntityIterator aSubs = theWS->Graph().Sharings(aNAUO);

      for (aSubs.Start(); aSubs.More(); aSubs.Next()) {
        Handle(StepRepr_ProductDefinitionShape) aPDS = 
          Handle(StepRepr_ProductDefinitionShape)::DownCast(aSubs.Value());

        if(aPDS.IsNull()) {
          continue;
        }

        Handle(StepBasic_ProductDefinitionRelationship) aPDR =
          aPDS->Definition().ProductDefinitionRelationship();

        if (aPDR.IsNull()) {
          continue;
        }

        if (aPDR->HasDescription() && aPDR->Description()->UsefullLength() >0) {
          aName = aPDR->Description();
        } else if (!aPDR->Name().IsNull() && aPDR->Name()->UsefullLength() >0 ) {
          aName = aPDR->Name();
        } else if (!aPDR->Id().IsNull()) {
          aName = aPDR->Id();
        }
      }
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

      if (theMapShapeAssembly.IsBound(S)) {
        S = theMapShapeAssembly.Find(S);
      }

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
Standard_Integer STEPPlugin_ImportDriver::Execute(LOGBOOK& log) const
{
  if( Label().IsNull() ) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction( Label() );

  STEPPlugin_IImport aData( aFunction );

  TCollection_AsciiString aFileName = aData.GetFileName().ToCString();
  bool anIsIgnoreUnits = aData.GetIsIgnoreUnits();
  bool isCreateAssemblies = aData.GetIsCreateAssemblies();
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
      Standard_Integer aNbRoots = aReader.NbRootsForTransfer();
      Standard_Integer i;

      aReader.PrintCheckTransfer(failsonly, IFSelect_ItemsByEntity);

      for (i = 1; i <= aNbRoots; i++) {
        aReader.TransferRoot(i);
      }

      // Create result shape
      const Standard_Integer aNbShapes = aReader.NbShapes();
      TopTools_ListOfShape   aListResShapes;

      if (isCreateAssemblies) {
        for (i = 1; i <= aNbShapes; i++) {
          TopoDS_Shape aShape = aReader.Shape(i);

          if (aShape.IsNull()) {
            continue;
          }

          aListResShapes.Append(aShape);
        }
      } else {
        for (i = 1; i <= aNbShapes; i++) {
          TopoDS_Shape aShape = aReader.Shape(i);

          if (aShape.IsNull()) {
            continue;
          }

          if (aShape.ShapeType() == TopAbs_COMPOUND) {
            int             aNbSub = 0;
            TopoDS_Shape    aSubShape;
            TopoDS_Iterator anIt (aShape, Standard_True, Standard_True);

            for (; anIt.More(); anIt.Next()) {
              aNbSub++;
              aSubShape = anIt.Value();
            }

            if (aNbSub == 1) {
              // Use the single sub-shape
              aListResShapes.Append(aSubShape);
            } else if (aNbSub > 1) {
              // Use the shape
              aListResShapes.Append(aShape);
            }
          } else {
            // Use the shape itself
            aListResShapes.Append(aShape);
          }
        }
      }

      // Construct result shape.
      if (!aListResShapes.IsEmpty()) {
        if (aListResShapes.Extent() == 1) {
          // Use the single shape.
          aResShape = aListResShapes.First();
        } else {
          // Make a compound of result shapes.
          TopTools_ListIteratorOfListOfShape anIt(aListResShapes);

          for (; anIt.More(); anIt.Next()) {
            B.Add(compound, anIt.Value());
          }

          aResShape = compound;
        }
      }

      if( aResShape.IsNull() ) {
        StdFail_NotDone::Raise("Null result shape");
        return 0;
      }

      // Check if any BRep entity has been read, there must be at least a vertex
      if ( !TopExp_Explorer( aResShape, TopAbs_VERTEX ).More() )
        StdFail_NotDone::Raise( "No geometrical data in the imported file." );

      // Create assemblies in the shape, if they are not created yet.
      TopTools_DataMapOfShapeShape aMapShapeAssembly;

      if (isCreateAssemblies) {
        aResShape = CreateAssemblies(aReader, aResShape, aMapShapeAssembly);
      }

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
          StoreName(enti, anIndices, aReader.WS(),
                    TP, aShapeLabel, aMapShapeAssembly);

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
    AddParam( theParams, "Create Assemblies", aCI.GetIsCreateAssemblies() );
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

OCCT_IMPLEMENT_STANDARD_RTTIEXT( STEPPlugin_ImportDriver, GEOM_BaseDriver );
