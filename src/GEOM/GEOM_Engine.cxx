#include "GEOM_Engine.hxx"

#include "GEOM_Solver.hxx"
#include "GEOM_Function.hxx"
#include "GEOM_ISubShape.hxx"
#include "GEOM_SubShapeDriver.hxx"
#include "GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient.hxx"

#include "utilities.h"

#include <Interface_DataMapIteratorOfDataMapOfIntegerTransient.hxx>

#include <TDF_Tool.hxx>
#include <TDF_Data.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_DriverTable.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

static GEOM_Engine* TheEngine = NULL;

static TCollection_AsciiString BuildIDFromObject(Handle(GEOM_Object)& theObject)
{
  TCollection_AsciiString anID(theObject->GetDocID()), anEntry;
  TDF_Tool::Entry(theObject->GetEntry(), anEntry);
  anID+=(TCollection_AsciiString("_")+anEntry);
  return anID;
}

static TCollection_AsciiString BuildID(Standard_Integer theDocID, char* theEntry)
{
  TCollection_AsciiString anID(theDocID);
  anID+=(TCollection_AsciiString("_")+theEntry);
  return anID;
}

static Standard_Integer ExtractDocID(TCollection_AsciiString& theID)
{
  TCollection_AsciiString aDocID = theID.Token("_");
  if(aDocID.Length() < 1) return -1;
  return aDocID.IntegerValue();
}

//=============================================================================
/*!
 *  GetEngine
 */
//=============================================================================
GEOM_Engine* GEOM_Engine::GetEngine() { return TheEngine; }


//=============================================================================
/*!
 *  SetEngine
 */
//=============================================================================
void GEOM_Engine::SetEngine(GEOM_Engine* theEngine) { TheEngine = theEngine; }

//=============================================================================
/*!
 *  Constructor
 */
//=============================================================================
GEOM_Engine::GEOM_Engine()
{
  TFunction_DriverTable::Get()->AddDriver(GEOM_Object::GetSubShapeID(), new GEOM_SubShapeDriver());

  _OCAFApp = new GEOM_Application();
  _UndoLimit = 10;
}

//=============================================================================
/*!
 *  GetDocument
 */
//=============================================================================
Handle(TDocStd_Document) GEOM_Engine::GetDocument(int theDocID)
{
  Handle(TDocStd_Document) aDoc;
  if(!_mapIDDocument.IsBound(theDocID)) {
    _OCAFApp->NewDocument("SALOME_GEOM", aDoc);
    aDoc->SetUndoLimit(_UndoLimit);
    _mapIDDocument.Bind(theDocID, aDoc);
    TDataStd_Integer::Set(aDoc->Main(), theDocID);
  }

  return Handle(TDocStd_Document)::DownCast(_mapIDDocument(theDocID));
}

//=============================================================================
/*!
 *  GetDocID
 */
//=============================================================================
int GEOM_Engine::GetDocID(Handle(TDocStd_Document) theDocument)
{
  if(theDocument.IsNull()) return -1;
  for(Interface_DataMapIteratorOfDataMapOfIntegerTransient anItr(_mapIDDocument); anItr.More(); anItr.Next())
    if(anItr.Value() == theDocument) return anItr.Key();

  return -1;

}

//=============================================================================
/*!
 *  GetObject
 */
//=============================================================================
Handle(GEOM_Object) GEOM_Engine::GetObject(int theDocID, char* theEntry)
{
  TCollection_AsciiString anID = BuildID(theDocID, theEntry);
  if(_objects.IsBound(anID)) return Handle(GEOM_Object)::DownCast(_objects(anID));

  TDF_Label aLabel;
  Handle(TDocStd_Document) aDoc = GetDocument(theDocID);
  TDF_Tool::Label(aDoc->Main().Data(), theEntry, aLabel, Standard_True);
  Handle(GEOM_Object) anObject = new GEOM_Object(aLabel);

  _objects.Bind(anID, anObject);

  return anObject;
}

//=============================================================================
/*!
 *  AddObject
 */
//=============================================================================
Handle(GEOM_Object) GEOM_Engine::AddObject(int theDocID, int theType)
{
    Handle(TDocStd_Document) aDoc = GetDocument(theDocID);
    Handle(TDataStd_TreeNode) aRoot = TDataStd_TreeNode::Set(aDoc->Main());

    TDF_Label aChild = TDF_TagSource::NewChild(aDoc->Main());
    Handle(GEOM_Object) anObject = new GEOM_Object(aChild, theType);

    //Put an object in the map of created objects
    TCollection_AsciiString anID = BuildIDFromObject(anObject);
    if(_objects.IsBound(anID)) _objects.UnBind(anID);
    _objects.Bind(anID, anObject);

    return anObject;
}

//=============================================================================
/*!
 *  AddSubShape
 */
//=============================================================================
Handle(GEOM_Object) GEOM_Engine::AddSubShape(Handle(GEOM_Object) theMainShape, Handle(TColStd_HArray1OfInteger) theIndices)
{
  if(theMainShape.IsNull() || theIndices.IsNull()) return NULL;

  Handle(TDocStd_Document) aDoc = GetDocument(theMainShape->GetDocID());
  Handle(TDataStd_TreeNode) aRoot = TDataStd_TreeNode::Set(aDoc->Main());

  TDF_Label aChild = TDF_TagSource::NewChild(aDoc->Main());

  Handle(GEOM_Function) aMainShape = theMainShape->GetLastFunction();
  Handle(GEOM_Object) anObject = new GEOM_Object(aChild, 28); //28 is SUBSHAPE type
  Handle(GEOM_Function) aFunction = anObject->AddFunction(GEOM_Object::GetSubShapeID(), 1);

  GEOM_ISubShape aSSI(aFunction);
  aSSI.SetMainShape(aMainShape);
  aSSI.SetIndices(theIndices);

  try {
    GEOM_Solver aSolver (GEOM_Engine::GetEngine());
    if (!aSolver.ComputeFunction(aFunction)) {
      MESSAGE("GEOM_Engine::AddSubShape Error: Can't build a sub shape");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    MESSAGE("GEOM_Engine::AddSubShape Error: " << aFail->GetMessageString());
    return NULL;
  }

  //Put an object in the map of created objects
  TCollection_AsciiString anID = BuildIDFromObject(anObject);
  if(_objects.IsBound(anID)) _objects.UnBind(anID);
  _objects.Bind(anID, anObject);

  TCollection_AsciiString anEntry, aDescr("");
  TDF_Tool::Entry(anObject->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = geom.AddSubShape(";
  TDF_Tool::Entry(theMainShape->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  aDescr += (", [");
  for(Standard_Integer i=theIndices->Lower(); i<=theIndices->Upper(); i++) {
    aDescr += (TCollection_AsciiString(theIndices->Value(i))+", ");
  }
  aDescr.Trunc(aDescr.Length()-1);
  aDescr += "])";
  aFunction->SetDescription(aDescr);

  return anObject;
}

//=============================================================================
/*!
 *  RemoveObject
 */
//=============================================================================
bool GEOM_Engine::RemoveObject(Handle(GEOM_Object) theObject)
{
  if(!theObject) return false;

  //Remove an object from the map of available objects
  TCollection_AsciiString anID = BuildIDFromObject(theObject);
  if(_objects.IsBound(anID)) _objects.UnBind(anID);

  TDF_Label aLabel = theObject->GetEntry();
  aLabel.ForgetAllAttributes(Standard_True);

  theObject.Nullify();

  return true;
}

//=============================================================================
/*!
 *  Undo
 */
//=============================================================================
void GEOM_Engine::Undo(int theDocID)
{
  GetDocument(theDocID)->Undo();
}

//=============================================================================
/*!
 *  Redo
 */
//=============================================================================
void GEOM_Engine::Redo(int theDocID)
{
  GetDocument(theDocID)->Redo();
}

//=============================================================================
/*!
 *  Save
 */
//=============================================================================
bool GEOM_Engine::Save(int theDocID, char* theFileName)
{
  if(!_mapIDDocument.IsBound(theDocID)) return false;
  Handle(TDocStd_Document) aDoc = Handle(TDocStd_Document)::DownCast(_mapIDDocument(theDocID));

  _OCAFApp->SaveAs(aDoc, theFileName);

  return true;
}

//=============================================================================
/*!
 *  Load
 */
//=============================================================================
bool GEOM_Engine::Load(int theDocID, char* theFileName)
{
  Handle(TDocStd_Document) aDoc;
  if(_OCAFApp->Open(theFileName, aDoc) != CDF_RS_OK) return false;

  aDoc->SetUndoLimit(_UndoLimit);

  if(_mapIDDocument.IsBound(theDocID)) _mapIDDocument.UnBind(theDocID);
  _mapIDDocument.Bind(theDocID, aDoc);

  TDataStd_Integer::Set(aDoc->Main(), theDocID);

  return true;
}

//=============================================================================
/*!
 *  Close
 */
//=============================================================================
void GEOM_Engine::Close(int theDocID)
{
  if(_mapIDDocument.IsBound(theDocID)) {
    Handle(TDocStd_Document) aDoc = Handle(TDocStd_Document)::DownCast(_mapIDDocument(theDocID));

    //Remove all GEOM Objects associated to the given document
    TColStd_SequenceOfAsciiString aSeq;
    GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient It(_objects);
    for(; It.More(); It.Next()) {
      TCollection_AsciiString anObjID(It.Key());
      Standard_Integer anID = ExtractDocID(anObjID);
      if(theDocID == anID) aSeq.Append(It.Key());
    }
    for(Standard_Integer i=1; i<=aSeq.Length(); i++) _objects.UnBind(aSeq.Value(i));

   _mapIDDocument.UnBind(theDocID);
    _OCAFApp->Close(aDoc);
    aDoc.Nullify();
  }
}
