// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

#include <Standard_Stream.hxx>

#include <GEOMImpl_GlueDriver.hxx>
#include <GEOMImpl_IGlue.hxx>
#include <GEOMImpl_Types.hxx>

#include <GEOM_Object.hxx>
#include <GEOM_Function.hxx>

#include "GEOMAlgo_Gluer2.hxx"
#include "GEOMAlgo_ListIteratorOfListOfCoupleOfShapes.hxx"
#include "GEOMAlgo_CoupleOfShapes.hxx"
#include "GEOMAlgo_ListOfCoupleOfShapes.hxx"

#include "utilities.h"

#include <BRep_Builder.hxx>
#include <ShapeFix_Shape.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TopExp.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Shape.hxx>

#include <Standard_NullObject.hxx>
#include <Standard_Failure.hxx>

#define MSG_BAD_TOLERANCE "Tolerance is too big"
#define MSG_BAD_ARG_SHAPE "Argument shape is not a compound of hexahedral solids"

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
/*
TopoDS_Shape GEOMImpl_GlueDriver::GlueFacesWithWarnings (const TopoDS_Shape& theShape,
                                                         const Standard_Real theTolerance,
                                                         const Standard_Boolean doKeepNonSolids,
                                                         TCollection_AsciiString& theWarning) const
{
  Standard_Integer iErr, iWrn;
  TopoDS_Shape aRes;
  GEOMAlgo_Gluer aGluer;

  aGluer.SetShape(theShape);
  aGluer.SetTolerance(theTolerance);
  aGluer.SetCheckGeometry(Standard_True);
  aGluer.SetKeepNonSolids(doKeepNonSolids);

  aGluer.Perform();

  iErr = aGluer.ErrorStatus();
  if (iErr) {
    switch (iErr) {
    case 2:
      Standard_Failure::Raise("No vertices found in source shape");
      break;
    case 3:
    case 4:
      Standard_Failure::Raise(MSG_BAD_TOLERANCE " or " MSG_BAD_ARG_SHAPE);
      break;
    case 5:
      Standard_Failure::Raise("Source shape is Null");
      break;
    case 6:
      Standard_Failure::Raise("Result shape is Null");
      break;
    case 100:
      Standard_Failure::Raise(MSG_BAD_TOLERANCE);
      break;
    case 101:
    case 102:
      Standard_Failure::Raise(MSG_BAD_ARG_SHAPE);
      break;
    case 200:
      Standard_Failure::Raise("Error occurred during check of geometric coincidence");
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

  // SKL 18.01.2010 - patch for 20662
  Handle(ShapeFix_Shape) aSfs = new ShapeFix_Shape(aRes);
  aSfs->SetPrecision(Precision::Confusion());
  aSfs->Perform();
  aRes = aSfs->Shape();

  // Fill history to be used by GetInPlace functionality
  TopTools_IndexedMapOfShape aResIndices;
  TopExp::MapShapes(aRes, aResIndices);

  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  // history for all argument shapes
  TDF_LabelSequence aLabelSeq;
  aFunction->GetDependency(aLabelSeq);
  Standard_Integer nbArg = aLabelSeq.Length();

  for (Standard_Integer iarg = 1; iarg <= nbArg; iarg++) {

    TDF_Label anArgumentRefLabel = aLabelSeq.Value(iarg);

    Handle(GEOM_Object) anArgumentObject = GEOM_Object::GetReferencedObject(anArgumentRefLabel);
    TopoDS_Shape anArgumentShape = anArgumentObject->GetValue();

    TopTools_IndexedMapOfShape anArgumentIndices;
    TopExp::MapShapes(anArgumentShape, anArgumentIndices);
    Standard_Integer nbArgumentEntities = anArgumentIndices.Extent();

    // Find corresponding label in history
    TDF_Label anArgumentHistoryLabel =
      aFunction->GetArgumentHistoryEntry(anArgumentRefLabel, Standard_True);

    for (Standard_Integer ie = 1; ie <= nbArgumentEntities; ie++) {
      TopoDS_Shape anEntity = anArgumentIndices.FindKey(ie);
      const TopTools_ListOfShape& aModified = aGluer.Modified(anEntity);
      Standard_Integer nbModified = aModified.Extent();

      if (nbModified > 0) {
        TDF_Label aWhatHistoryLabel = anArgumentHistoryLabel.FindChild(ie, Standard_True);
        Handle(TDataStd_IntegerArray) anAttr =
          TDataStd_IntegerArray::Set(aWhatHistoryLabel, 1, nbModified);

        TopTools_ListIteratorOfListOfShape itM (aModified);
        for (int im = 1; itM.More(); itM.Next(), ++im) {
          int id = aResIndices.FindIndex(itM.Value());
          anAttr->SetValue(im, id);
        }
      }
    }
  }

  return aRes;
}

//=======================================================================
//function : GlueFaces
//purpose  :
//=======================================================================
TopoDS_Shape GEOMImpl_GlueDriver::GlueFaces (const TopoDS_Shape& theShape,
                                             const Standard_Real theTolerance,
                                             const Standard_Boolean doKeepNonSolids)
{
  TopoDS_Shape aRes;
  GEOMAlgo_Gluer aGluer;

  aGluer.SetShape(theShape);
  aGluer.SetTolerance(theTolerance);
  aGluer.SetCheckGeometry(Standard_True);
  aGluer.SetKeepNonSolids(doKeepNonSolids);

  aGluer.Perform();

  Standard_Integer iErr = aGluer.ErrorStatus();
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
      Standard_Failure::Raise("Error occurred during check of geometric coincidence");
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

  Standard_Integer iWrn = aGluer.WarningStatus();
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
//function : GlueFacesByList
//purpose  :
//=======================================================================
TopoDS_Shape GEOMImpl_GlueDriver::GlueFacesByList (const TopoDS_Shape& theShape,
                                                   const Standard_Real theTolerance,
                                                   const Standard_Boolean doKeepNonSolids,
                                                   const TopTools_MapOfShape& aFaces)
{
  TopoDS_Shape aRes;

  GEOMAlgo_Gluer1 aGluer;
  GEOMAlgo_ListIteratorOfListOfCoupleOfShapes aItCS;
  GEOMAlgo_CoupleOfShapes aCS;
  GEOMAlgo_ListOfCoupleOfShapes aLCS;

  aGluer.SetShape(theShape);
  aGluer.SetTolerance(theTolerance);
  aGluer.SetKeepNonSolids(doKeepNonSolids);
  aGluer.Perform();
  Standard_Integer iErr = aGluer.ErrorStatus();
  if (iErr) return aRes;

  TopTools_ListOfShape listShape;
  const GEOMAlgo_ListOfCoupleOfShapes& aLCSG = aGluer.GluedFaces();
  // Access to faces
  aItCS.Initialize(aLCSG);
  for (; aItCS.More(); aItCS.Next()) {
    const GEOMAlgo_CoupleOfShapes& aCSG = aItCS.Value();
    const TopoDS_Shape& aF1 = aCSG.Shape1();
    const TopoDS_Shape& aF2 = aCSG.Shape2();
    if (aFaces.Contains(aF1) || aFaces.Contains(aF2))
      continue;
    aCS.SetShapes(aF1,aF2);
    aLCS.Append(aCS);
  }

  //cout<<"aLCS.Extent() = "<<aLCS.Extent()<<endl;
  if (aLCS.Extent()>0) {
    aGluer.SetFacesToUnglue(aLCS);
    aGluer.UnglueFaces();
    iErr = aGluer.ErrorStatus();
    if (iErr) return aRes;
  }

  aRes = aGluer.Result();

  return aRes;
}
*/

//=======================================================================
//function : GlueFaces
//purpose  :
//=======================================================================
TopoDS_Shape GEOMImpl_GlueDriver::GlueFaces
          (const TopoDS_Shape                       &theShape,
           const Standard_Real                       theTolerance,
           const Standard_Boolean                    doKeepNonSolids,
                 TopTools_DataMapOfShapeListOfShape *pMapModif)
{
  TopoDS_Shape aRes;

  GEOMAlgo_Gluer2 aGA;

  // 1. Initialization
  aGA.SetArgument(theShape);
  aGA.SetTolerance(theTolerance);
  aGA.SetKeepNonSolids(doKeepNonSolids);

  // 2. Detect interfered shapes
  aGA.Detect();

  //Standard_Integer iWrnDetect = aGA.WarningStatus();
  //if (iWrnDetect == 2) {
  //  Standard_Failure::Raise("GLUE_ERROR_STICKED_SHAPES");
  //}

  Standard_Integer iErr = aGA.ErrorStatus();
  if (iErr) {
    switch (iErr) {
    case 11:
      Standard_Failure::Raise("GEOMAlgo_GlueDetector failed");
      break;
    case 13:
    case 14:
      Standard_Failure::Raise("PerformImagesToWork failed");
      break;
    default:
      {
        // description of all errors see in GEOMAlgo_Gluer2.cxx
        TCollection_AsciiString aMsg ("Error in GEOMAlgo_Gluer2 with code ");
        aMsg += TCollection_AsciiString(iErr);
        Standard_Failure::Raise(aMsg.ToCString());
        break;
      }
    }
    return aRes;
  }

  // 3. Set shapes to glue. If the operator is absent, the whole gluing will be done
  //aGA.SetShapesToGlue(aMSG);

  // 4. Gluing
  aGA.Perform();
  iErr = aGA.ErrorStatus();
  if (iErr) {
    switch (iErr) {
    case 11:
      Standard_Failure::Raise("GEOMAlgo_GlueDetector failed");
      break;
    case 13:
    case 14:
      Standard_Failure::Raise("PerformImagesToWork failed");
      break;
    default:
      {
        // description of all errors see in GEOMAlgo_Gluer2.cxx
        TCollection_AsciiString aMsg ("Error in GEOMAlgo_Gluer2 with code ");
        aMsg += TCollection_AsciiString(iErr);
        Standard_Failure::Raise(aMsg.ToCString());
        break;
      }
    }
    return aRes;
  }

  Standard_Integer iWrn = aGA.WarningStatus();
  if (iWrn) {
    switch (iWrn) {
    case 1:
      MESSAGE("No shapes to glue");
      break;
    default:
      // description of all warnings see in GEOMAlgo_Gluer2.cxx
      MESSAGE("Warning in GEOMAlgo_Gluer2 with code " << iWrn);
      break;
    }
  }

  // 5. Result
  aRes = aGA.Shape();

  if (pMapModif) {
    // Fill the map of modified shapes.
    TopTools_IndexedMapOfShape aMapSubShapes;

    TopExp::MapShapes(theShape, aMapSubShapes);
    pMapModif->Clear();

    const Standard_Integer aNbShapes = aMapSubShapes.Extent();
    Standard_Integer       i;

    for (i = 1; i <= aNbShapes; ++i) {
      const TopoDS_Shape         &aSubShape = aMapSubShapes.FindKey(i);
      const TopTools_ListOfShape &aModif    = aGA.Modified(aSubShape);

      if (!aModif.IsEmpty()) {
        pMapModif->Bind(aSubShape, aModif);
      }
    }
  }

  return aRes;
}

//=======================================================================
//function : GlueWithWarnings
//purpose  :
//=======================================================================
TopoDS_Shape GEOMImpl_GlueDriver::GlueWithWarnings (const TopoDS_Shape& theShape,
                                                    const Standard_Real theTolerance,
                                                    const TopAbs_ShapeEnum theShapeType,
                                                    const Standard_Boolean doKeepNonSolids,
                                                    TCollection_AsciiString& theWarning) const
{
  TopoDS_Shape aRes;

  GEOMAlgo_Gluer2 aGA;

  // 1. Initialization
  aGA.SetArgument(theShape);
  aGA.SetTolerance(theTolerance);
  aGA.SetKeepNonSolids(doKeepNonSolids);

  // 2. Detect interfered shapes
  aGA.Detect();

  //modified by NIZNHY-PKV Tue Mar 13 14:07:12 2012f
  Standard_Integer iWrnDetect = aGA.WarningStatus();
  if (iWrnDetect == 2) {
    Standard_Failure::Raise("GLUE_ERROR_STICKED_SHAPES");
  }
  //modified by NIZNHY-PKV Tue Mar 13 14:07:14 2012t

  Standard_Integer iErr = aGA.ErrorStatus();
  if (iErr) {
    switch (iErr) {
    case 11:
      Standard_Failure::Raise("GEOMAlgo_GlueDetector failed");
      break;
    case 13:
    case 14:
      Standard_Failure::Raise("PerformImagesToWork failed");
      break;
    default:
      {
        // description of all errors see in GEOMAlgo_Gluer2.cxx
        TCollection_AsciiString aMsg ("Error in GEOMAlgo_Gluer2 with code ");
        aMsg += TCollection_AsciiString(iErr);
        Standard_Failure::Raise(aMsg.ToCString());
        break;
      }
    }
    return aRes;
  }

  if (theShapeType != TopAbs_FACE) {
    // 3. Fill shapes to glue aMSG
    TopTools_DataMapOfShapeListOfShape aMSG;
    const TopTools_DataMapOfShapeListOfShape& aMSD = aGA.ShapesDetected();
    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItMSD;
    aItMSD.Initialize(aMSD);
    for (; aItMSD.More(); aItMSD.Next()) {
      const TopoDS_Shape& aSx = aItMSD.Key();
      const TopTools_ListOfShape& aLSD = aItMSD.Value();
      if (aSx.ShapeType() == theShapeType) {
        aMSG.Bind(aSx, aLSD);
      }
    }

    // 4. Set shapes to glue. If the operator is absent, the whole gluing will be done
    aGA.SetShapesToGlue(aMSG);
  }

  // 5. Gluing
  aGA.Perform();
  iErr = aGA.ErrorStatus();
  if (iErr) {
    switch (iErr) {
    case 11:
      Standard_Failure::Raise("GEOMAlgo_GlueDetector failed");
      break;
    case 13:
    case 14:
      Standard_Failure::Raise("PerformImagesToWork failed");
      break;
    default:
      {
        // description of all errors see in GEOMAlgo_Gluer2.cxx
        TCollection_AsciiString aMsg ("Error in GEOMAlgo_Gluer2 with code ");
        aMsg += TCollection_AsciiString(iErr);
        Standard_Failure::Raise(aMsg.ToCString());
        break;
      }
    }
    return aRes;
  }

  Standard_Integer iWrn = aGA.WarningStatus();
  if (iWrn) {
    switch (iWrn) {
    case 1:
      theWarning = "No shapes to glue";
      break;
    default:
      // description of all warnings see in GEOMAlgo_Gluer2.cxx
      theWarning = "Warning in GEOMAlgo_Gluer2 with code ";
      theWarning += TCollection_AsciiString(iWrn);
      break;
    }
  }

  // 6. Result
  aRes = aGA.Shape();

  // 7. Fill history to be used by GetInPlace functionality
  TopTools_IndexedMapOfShape aResIndices;
  TopExp::MapShapes(aRes, aResIndices);

  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  // history for all argument shapes
  TDF_LabelSequence aLabelSeq;
  aFunction->GetDependency(aLabelSeq);
  Standard_Integer nbArg = aLabelSeq.Length();

  for (Standard_Integer iarg = 1; iarg <= nbArg; iarg++) {

    TDF_Label anArgumentRefLabel = aLabelSeq.Value(iarg);

    Handle(GEOM_Object) anArgumentObject = GEOM_Object::GetReferencedObject(anArgumentRefLabel);
    TopoDS_Shape anArgumentShape = anArgumentObject->GetValue();

    TopTools_IndexedMapOfShape anArgumentIndices;
    TopExp::MapShapes(anArgumentShape, anArgumentIndices);
    Standard_Integer nbArgumentEntities = anArgumentIndices.Extent();

    // Find corresponding label in history
    TDF_Label anArgumentHistoryLabel =
      aFunction->GetArgumentHistoryEntry(anArgumentRefLabel, Standard_True);

    for (Standard_Integer ie = 1; ie <= nbArgumentEntities; ie++) {
      TopoDS_Shape anEntity = anArgumentIndices.FindKey(ie);
      const TopTools_ListOfShape& aModified = aGA.Modified(anEntity);
      Standard_Integer nbModified = aModified.Extent();

      if (nbModified > 0) {
        TDF_Label aWhatHistoryLabel = anArgumentHistoryLabel.FindChild(ie, Standard_True);
        Handle(TDataStd_IntegerArray) anAttr =
          TDataStd_IntegerArray::Set(aWhatHistoryLabel, 1, nbModified);

        TopTools_ListIteratorOfListOfShape itM (aModified);
        for (int im = 1; itM.More(); itM.Next(), ++im) {
          int id = aResIndices.FindIndex(itM.Value());
          anAttr->SetValue(im, id);
        }
      }
    }
  }

  return aRes;
}

//=======================================================================
//function : GlueByList
//purpose  :
//=======================================================================
TopoDS_Shape GEOMImpl_GlueDriver::GlueByList (const TopoDS_Shape& theShape,
                                              const Standard_Real theTolerance,
                                              const Standard_Boolean doKeepNonSolids,
                                              const TopTools_MapOfShape& theShapesList,
                                              const Standard_Boolean doGlueAllEdges)
{
  TopoDS_Shape aRes;

  GEOMAlgo_Gluer2 aGA;

  // 1. Initialization
  aGA.SetArgument(theShape);
  aGA.SetTolerance(theTolerance);
  aGA.SetKeepNonSolids(doKeepNonSolids);

  // 2. Detect interfered shapes
  aGA.Detect();

  //modified by NIZNHY-PKV Tue Mar 13 14:07:12 2012f
  Standard_Integer iWrnDetect = aGA.WarningStatus();
  if (iWrnDetect == 2) {
    /*
    TopTools_ListIteratorOfListOfShape aItLS;

    // Sticked shapes are detected
    const TopTools_IndexedDataMapOfShapeListOfShape& aIDMSS = pGluer2->StickedShapes();

    Standard_Integer i, aNb = aIDMSS.Extent();
    for (i = 1; i <= aNb; ++i) {
      // ancestor aSa (edge, wire face,..)  
      const TopoDS_Shape& aSa = aIDMSS.FindKey(i); 

      // successors aSs (vertex, edge, ...)
      // of the ancestor that are sticked 
      // for given value of the tolerance
      const TopTools_ListOfShape& aLSS = aIDMSS.FindFromIndex(i);
      aItLS.Initialize(aLSS);
      for (; aItLS.More(); aItLS.Next()) {
        const TopoDS_Shape& aSs = aItLS.Value();
      }
    }
    */
    Standard_Failure::Raise("GLUE_ERROR_STICKED_SHAPES");
  }
  //modified by NIZNHY-PKV Tue Mar 13 14:07:14 2012t

  Standard_Integer iErr = aGA.ErrorStatus();
  if (iErr) {
    switch (iErr) {
    case 11:
      Standard_Failure::Raise("GEOMAlgo_GlueDetector failed");
      break;
    case 13:
    case 14:
      Standard_Failure::Raise("PerformImagesToWork failed");
      break;
    default:
      {
        // description of all errors see in GEOMAlgo_Gluer2.cxx
        TCollection_AsciiString aMsg ("Error in GEOMAlgo_Gluer2 with code ");
        aMsg += TCollection_AsciiString(iErr);
        Standard_Failure::Raise(aMsg.ToCString());
        break;
      }
    }
    return aRes;
  }

  // 3. Fill shapes to glue aMSG
  TopTools_DataMapOfShapeListOfShape aMSG;
  const TopTools_DataMapOfShapeListOfShape& aMSD = aGA.ShapesDetected();
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItMSD;
  aItMSD.Initialize(aMSD);
  for (; aItMSD.More(); aItMSD.Next()) {
    const TopoDS_Shape& aSx = aItMSD.Key();
    const TopTools_ListOfShape& aLSD = aItMSD.Value();
    TopTools_ListIteratorOfListOfShape anItLSD (aLSD);
    bool isToGlue = false;
    if (doGlueAllEdges && aSx.ShapeType() == TopAbs_EDGE) {
      isToGlue = true;
    }
    else {
      for (; anItLSD.More() && !isToGlue; anItLSD.Next()) {
        if (theShapesList.Contains(anItLSD.Value())) {
          isToGlue = true;
        }
      }
    }
    if (isToGlue) {
      aMSG.Bind(aSx, aLSD);
    }
  }

  // 4. Set shapes to glue. If the operator is absent, the whole gluing will be done
  aGA.SetShapesToGlue(aMSG);

  // 5. Gluing
  aGA.Perform();
  iErr = aGA.ErrorStatus();
  if (iErr) {
    switch (iErr) {
    case 11:
      Standard_Failure::Raise("GEOMAlgo_GlueDetector failed");
      break;
    case 13:
    case 14:
      Standard_Failure::Raise("PerformImagesToWork failed");
      break;
    default:
      {
        // description of all errors see in GEOMAlgo_Gluer2.cxx
        TCollection_AsciiString aMsg ("Error in GEOMAlgo_Gluer2 with code ");
        aMsg += TCollection_AsciiString(iErr);
        Standard_Failure::Raise(aMsg.ToCString());
        break;
      }
    }
    return aRes;
  }

  Standard_Integer iWrn = aGA.WarningStatus();
  if (iWrn) {
    switch (iWrn) {
    case 1:
      MESSAGE("No shapes to glue");
      break;
    default:
      // description of all warnings see in GEOMAlgo_Gluer2.cxx
      MESSAGE("Warning in GEOMAlgo_Gluer2 with code " << iWrn);
      break;
    }
  }

  // 6. Result
  aRes = aGA.Shape();

  return aRes;
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_GlueDriver::Execute(Handle(TFunction_Logbook)& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IGlue aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;
  TCollection_AsciiString aWrn;

  TopoDS_Shape aShapeBase;
  Handle(TColStd_HSequenceOfTransient) aRefBases = aCI.GetBase();
  if ( aRefBases && !aRefBases->IsEmpty() )
  {
    if ( aRefBases->Length() == 1 )
    {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aRefBases->Value(1));
      if ( !aRefShape.IsNull() )
        aShapeBase = aRefShape->GetValue();
    }
    else
    {
      TopoDS_Compound compound;
      BRep_Builder builder;
      builder.MakeCompound( compound );
      int ind = 1;
      for ( ; ind <= aRefBases->Length(); ind++)
      {
        Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aRefBases->Value(ind));
        if ( aRefShape.IsNull() ) break;
        TopoDS_Shape aShape = aRefShape->GetValue();
        if (aShape.IsNull()) break;
        builder.Add( compound, aShape );
      }
      if ( ind > aRefBases->Length() )
        aShapeBase = compound;
      else
        aShapeBase.Nullify();
    }
  }
  if (aShapeBase.IsNull()) {
    Standard_NullObject::Raise("Shape for gluing is null");
  }

  Standard_Real tol3d = aCI.GetTolerance();

  Standard_Boolean aKeepNonSolids = aCI.GetKeepNonSolids();

  if (aType == GLUE_FACES) {
    //aShape = GlueFacesWithWarnings(aShapeBase, tol3d, aKeepNonSolids, aWrn);
    aShape = GlueWithWarnings(aShapeBase, tol3d, TopAbs_FACE, aKeepNonSolids, aWrn);
  }
  else if (aType == GLUE_EDGES) {
    aShape = GlueWithWarnings(aShapeBase, tol3d, TopAbs_EDGE, aKeepNonSolids, aWrn);
  }
  else if (aType == GLUE_FACES_BY_LIST || aType == GLUE_EDGES_BY_LIST) {
    Handle(TColStd_HSequenceOfTransient) SF = aCI.GetFaces();
    TopTools_MapOfShape aFaces;
    int i = 1;
    for (; i <= SF->Length(); i++) {
      Handle(Standard_Transient) anItem = SF->Value(i);
      if (anItem.IsNull())
        continue;
      Handle(GEOM_Function) aRefSh = Handle(GEOM_Function)::DownCast(anItem);
      if (aRefSh.IsNull())
        continue;
      TopoDS_Shape aFace = aRefSh->GetValue();
      if (aFace.IsNull())
        continue;
      aFaces.Add(aFace);
    }

    Standard_Boolean aGlueAllEdges = Standard_False;
    if (aType == GLUE_FACES_BY_LIST)
      aGlueAllEdges = aCI.GetGlueAllEdges();

    //aShape = GlueFacesByList(aShapeBase, tol3d, aKeepNonSolids, aFaces);
    aShape = GlueByList(aShapeBase, tol3d, aKeepNonSolids, aFaces, aGlueAllEdges);
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log->SetTouched(Label());

  if (!aWrn.IsEmpty()) {
    Standard_Failure::Raise(aWrn.ToCString());
  }

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_GlueDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IGlue aCI( function );
  Standard_Integer aType = function->GetType();

  switch ( aType ) {
  case GLUE_FACES:
    theOperationName = "GLUE_FACES";
    AddParam( theParams, "Selected shapes", aCI.GetBase() );
    AddParam( theParams, "Tolerance", aCI.GetTolerance() );
    AddParam( theParams, "To keep non solids", aCI.GetKeepNonSolids() );
    break;
  case GLUE_EDGES:
    theOperationName = "GLUE_EDGES";
    AddParam( theParams, "Selected shapes", aCI.GetBase() );
    AddParam( theParams, "Tolerance", aCI.GetTolerance() );
    break;
  case GLUE_FACES_BY_LIST:
    theOperationName = "GLUE_FACES";
    AddParam( theParams, "Selected shapes", aCI.GetBase() );
    AddParam( theParams, "Tolerance", aCI.GetTolerance() );
    AddParam( theParams, "Faces", aCI.GetFaces() );
    AddParam( theParams, "To keep non solids", aCI.GetKeepNonSolids() );
    AddParam( theParams, "To glue all edges", aCI.GetGlueAllEdges() );
    break;
  case GLUE_EDGES_BY_LIST:
    theOperationName = "GLUE_EDGES";
    AddParam( theParams, "Selected shapes", aCI.GetBase() );
    AddParam( theParams, "Tolerance", aCI.GetTolerance() );
    AddParam( theParams, "Edges", aCI.GetFaces() );
    break;
  default:
    return false;
  }
  
  return true;
}

IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_GlueDriver,GEOM_BaseDriver)
