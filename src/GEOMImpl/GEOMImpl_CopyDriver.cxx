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

#include "GEOMImpl_CopyDriver.hxx"
#include "GEOMImpl_ICopy.hxx"
#include "GEOMImpl_ITransferData.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"
#include "GEOM_Object.hxx"
#include "GEOMAlgo_GetInPlace.hxx"
#include "GEOMAlgo_GetInPlaceAPI.hxx"

#include <TopoDS_Shape.hxx>
#include <TopExp.hxx>
#include <TNaming_CopyShape.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>


#define NB_DATUM             2
#define DATUM_NAME_INDEX     1
#define DATUM_MATERIAL_INDEX 2


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
Standard_Integer GEOMImpl_CopyDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  Standard_Integer aType = aFunction->GetType();

  if (aType == TRANSFER_DATA) {
    return transferData(log);
  }

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

bool GEOMImpl_CopyDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_ICopy aCI( function );
  Standard_Integer aType = function->GetType();


  switch ( aType ) {
  case COPY_WITH_REF:
    theOperationName = "MakeCopy";
    AddParam( theParams, "Original", aCI.GetOriginal() );
    break;
  case COPY_WITHOUT_REF:
  {
    theOperationName = "RestoreShape";
    TDF_Label label = Label();
    Handle(GEOM_Object) obj = GEOM_Object::GetObject(label);
    if ( !obj.IsNull() && obj->GetType() == GEOM_FREE_BOUNDS )
      theOperationName = "CHECK_FREE_BNDS";
    break;
  }
  default:
    return false;
  }
  
  return true;
}

//================================================================================
/*!
 * \brief Performs Transfer Data operation.
 */
//================================================================================
Standard_Integer GEOMImpl_CopyDriver::transferData(LOGBOOK& log) const
{
  Handle(GEOM_Function)  aFunction = GEOM_Function::GetFunction(Label());
  GEOMImpl_ITransferData aTD (aFunction);
  Handle(GEOM_Function)  aRef1     = aTD.GetRef1();
  Handle(GEOM_Function)  aRef2     = aTD.GetRef2();

  if (aRef1.IsNull() || aRef2.IsNull()) {
    return 0;
  }

  TopoDS_Shape                              aShape1     = aRef1->GetValue();
  TopoDS_Shape                              aShape2     = aRef2->GetValue();
  const int                                 aFindMethod = aTD.GetFindMethod();
  TopTools_IndexedDataMapOfShapeListOfShape aMapSoDest;
  TopTools_IndexedMapOfShape                anIndices1;

  TopExp::MapShapes(aShape1, anIndices1);

  switch (aFindMethod) {
    case TD_GET_IN_PLACE:
      if (!getInPlace(aShape1, anIndices1, aShape2, aMapSoDest)) {
        return 0;
      }
      break;
    case TD_GET_IN_PLACE_OLD:
      if (!getInPlaceOld(aRef1, anIndices1, aShape2, aMapSoDest)) {
        return 0;
      }
      break;
    case TD_GET_IN_PLACE_BY_HISTORY:
      if (!getInPlaceByHistory(aRef1, anIndices1, aShape2, aRef2, aMapSoDest)) {
        return 0;
      }
      break;
    default:
      return 0;
  }

  // Perform copying names.
  Handle(TColStd_HArray1OfExtendedString) aDatumName   =
    new TColStd_HArray1OfExtendedString(1, NB_DATUM);
  Handle(TColStd_HArray1OfInteger)        aDatumMaxVal =
    new TColStd_HArray1OfInteger(1, NB_DATUM, 0);
  Handle(TColStd_HArray1OfInteger)        aDatumVal    =
    new TColStd_HArray1OfInteger(1, NB_DATUM, 0);
  GEOMImpl_ITransferData                  aTD1(aRef1);
  GEOMImpl_ITransferData                  aTD2(aRef2);
  Standard_Integer                        i;
  Standard_Integer                        aNbShapes = anIndices1.Extent();
  TopTools_MapOfShape                     aMapFence;

  aDatumName->SetValue(DATUM_NAME_INDEX,     "GEOM_TRANSFER_DATA_NAMES");
  aDatumName->SetValue(DATUM_MATERIAL_INDEX, "GEOM_TRANSFER_DATA_MATERIALS");

  for (i = 1; i <= aNbShapes; ++i) {
    const TopoDS_Shape      &aSource   = anIndices1.FindKey(i);
    TCollection_AsciiString  aName     = aTD1.GetName(aSource);
    TCollection_AsciiString  aMaterial = aTD1.GetMaterial(aSource);

    // Transfer name
    if (!aName.IsEmpty()) {
      aDatumMaxVal->ChangeValue(DATUM_NAME_INDEX)++;

      if (aMapSoDest.Contains(aSource)) {
        aDatumVal->ChangeValue(DATUM_NAME_INDEX)++;

        // Copy name to the list of subshapes of the second shape.
        const TopTools_ListOfShape         &aListDest =
          aMapSoDest.FindFromKey(aSource);
        TopTools_ListIteratorOfListOfShape  anIt(aListDest);

        for (; anIt.More(); anIt.Next()) {
          const TopoDS_Shape &aShapeDest = anIt.Value();

          if (aMapFence.Add(aShapeDest)) {
            aTD2.SetName(aShapeDest, aName);
          }
        }
      }
    }

    // Transfer Material
    if (!aMaterial.IsEmpty()) {
      aDatumMaxVal->ChangeValue(DATUM_MATERIAL_INDEX)++;

      if (aMapSoDest.Contains(aSource)) {
        aDatumVal->ChangeValue(DATUM_MATERIAL_INDEX)++;

        // Copy material to the list of subshapes of the second shape.
        const TopTools_ListOfShape         &aListDest =
          aMapSoDest.FindFromKey(aSource);
        TopTools_ListIteratorOfListOfShape  anIt(aListDest);

        for (; anIt.More(); anIt.Next()) {
          const TopoDS_Shape &aShapeDest = anIt.Value();

          if (aMapFence.Add(aShapeDest)) {
            aTD2.SetMaterial(aShapeDest, aMaterial);
          }
        }
      }
    }
  }

  // Store results.
  aTD.SetDatumName(aDatumName);
  aTD.SetDatumMaxVal(aDatumMaxVal);
  aTD.SetDatumVal(aDatumVal);

  return 1;
}

//================================================================================
/*!
 * \brief For each subshape of the source shape compute coinsident sub-shapes
 *        of the destination shape using GetInPlace method.
 */
//================================================================================

Standard_Boolean GEOMImpl_CopyDriver::getInPlace
    (const TopoDS_Shape                              &theSourceShape,
     const TopTools_IndexedMapOfShape                &theSourceIndices,
     const TopoDS_Shape                              &theDestinationShape,
           TopTools_IndexedDataMapOfShapeListOfShape &theMapSourceDest) const
{
  // Searching for the sub-shapes inside theDestinationShape shape
  GEOMAlgo_GetInPlace aGIP;

  if (!GEOMAlgo_GetInPlaceAPI::GetInPlace
          (theDestinationShape, theSourceShape, aGIP)) {
    return Standard_False;
  }

  const GEOMAlgo_DataMapOfShapeMapOfShape &aShapesIn = aGIP.ShapesIn();
  const GEOMAlgo_DataMapOfShapeMapOfShape &aShapesOn = aGIP.ShapesOn();
  Standard_Integer                         i;
  Standard_Integer                         j;
  Standard_Integer                         aNbShapes = theSourceIndices.Extent();

  for (i = 1; i <= aNbShapes; ++i) {
    const TopoDS_Shape   &aSource = theSourceIndices.FindKey(i);
    TopTools_ListOfShape  aListShapes2;
    TopTools_MapOfShape   aMapShapes2;

    for (j = 0; j < 2; ++j) {
      const GEOMAlgo_DataMapOfShapeMapOfShape &aShapes2 =
                    j == 0 ? aShapesIn : aShapesOn;

      if (aShapes2.IsBound(aSource)) {
        const TopTools_MapOfShape &aMapShapesDest =
          aShapes2.Find(aSource);
        TopTools_MapIteratorOfMapOfShape aMapIter(aMapShapesDest);

        for (; aMapIter.More(); aMapIter.Next()) {
          const TopoDS_Shape &aShapeDest = aMapIter.Key();

          if (aMapShapes2.Add(aShapeDest)) {
            aListShapes2.Append(aShapeDest);
          }
        }
      }
    }

    if (!aListShapes2.IsEmpty()) {
      theMapSourceDest.Add(aSource, aListShapes2);
    }
  }

  return Standard_True;
}

//================================================================================
/*!
 * \brief For each subshape of the source shape compute coinsident sub-shapes
 *        of the destination shape using an old implementation
 *        of GetInPlace algorithm.
 */
//================================================================================

Standard_Boolean GEOMImpl_CopyDriver::getInPlaceOld
    (const Handle(GEOM_Function)                     &theSourceRef,
     const TopTools_IndexedMapOfShape                &theSourceIndices,
     const TopoDS_Shape                              &theDestinationShape,
           TopTools_IndexedDataMapOfShapeListOfShape &theMapSourceDest) const
{
  const Standard_Integer aNbShapes = theSourceIndices.Extent();
  Standard_Integer       i;
  Standard_Integer       iErr;
  TopTools_ListOfShape   aModifiedList;
  GEOMImpl_ITransferData aTDSource(theSourceRef);

  for (i = 1; i <= aNbShapes; ++i) {
    const TopoDS_Shape      &aSource   = theSourceIndices.FindKey(i);
    TCollection_AsciiString  aName     = aTDSource.GetName(aSource);
    TCollection_AsciiString  aMaterial = aTDSource.GetMaterial(aSource);

    if (aName.IsEmpty() && aMaterial.IsEmpty()) {
      continue;
    }

    // Call old GetInPlace.
    iErr = GEOMAlgo_GetInPlaceAPI::GetInPlaceOld
      (theDestinationShape, aSource, aModifiedList);

    if (iErr == 3) {
      // Nothing is found. Skip.
      continue;
    }

    if (iErr) {
      // Error.
      return Standard_False;
    }

    theMapSourceDest.Add(aSource, aModifiedList);
  }

  return Standard_True;
}

//================================================================================
/*!
 * \brief For each subshape of the source shape compute coinsident sub-shapes
 *        of the destination shape using GetInPlaceByHistory algorithm.
 */
//================================================================================

Standard_Boolean GEOMImpl_CopyDriver::getInPlaceByHistory
    (const Handle(GEOM_Function)                     &theSourceRef,
     const TopTools_IndexedMapOfShape                &theSourceIndices,
     const TopoDS_Shape                              &theDestinationShape,
     const Handle(GEOM_Function)                     &theDestinationRef,
           TopTools_IndexedDataMapOfShapeListOfShape &theMapSourceDest) const
{
  const Standard_Integer aNbShapes = theSourceIndices.Extent();
  Standard_Integer       i;
  GEOMImpl_ITransferData aTDSource(theSourceRef);
  TopTools_IndexedMapOfShape aDestIndices;

  TopExp::MapShapes(theDestinationShape, aDestIndices);

  for (i = 1; i <= aNbShapes; ++i) {
    const TopoDS_Shape      &aSource   = theSourceIndices.FindKey(i);
    TCollection_AsciiString  aName     = aTDSource.GetName(aSource);
    TCollection_AsciiString  aMaterial = aTDSource.GetMaterial(aSource);

    if (aName.IsEmpty() && aMaterial.IsEmpty()) {
      continue;
    }

    // Call GetInPlaceByHistory.
    TopTools_ListOfShape aModifiedList;
    const Standard_Boolean isFound = GEOMAlgo_GetInPlaceAPI::GetInPlaceByHistory
      (theDestinationRef, aDestIndices, aSource, aModifiedList);

    if (isFound && !aModifiedList.IsEmpty()) {
      theMapSourceDest.Add(aSource, aModifiedList);
    }
  }

  return Standard_True;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_CopyDriver,GEOM_BaseDriver);
