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

#ifdef WIN32
#pragma warning( disable:4786 )
#endif

#include "GEOM_Engine.hxx"

#include "GEOM_Field.hxx"
#include "GEOM_Function.hxx"
#include "GEOM_ISubShape.hxx"
#include "GEOM_PythonDump.hxx"
#include "GEOM_Solver.hxx"
#include "GEOM_SubShapeDriver.hxx"
#include "Sketcher_Profile.hxx"

#include <Basics_OCCTVersion.hxx>

#include "utilities.h"

#include <Basics_Utils.hxx>

#include <TDF_Tool.hxx>
#include <TDF_Data.hxx>
#include <TDF_Reference.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_DriverTable.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDataStd_Comment.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

#include <TColStd_DataMapIteratorOfDataMapOfIntegerTransient.hxx>

#include <Resource_DataMapIteratorOfDataMapOfAsciiStringAsciiString.hxx>

#if OCC_VERSION_LARGE > 0x07000000
#include <BinDrivers.hxx>
#include <StdDrivers_DocumentRetrievalDriver.hxx>
#include <PCDM_StorageDriver.hxx>
#endif

#include <set>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

#define COMMA ','
#define O_BRACKET '('
#define C_BRACKET ')'
#define O_SQR_BRACKET '['
#define C_SQR_BRACKET ']'
#define PY_NULL "None"

#ifdef _DEBUG_
static int MYDEBUG = 0;
#else
static int MYDEBUG = 0;
#endif

typedef std::map< TCollection_AsciiString, TCollection_AsciiString > TSting2StringMap;
typedef std::map< TCollection_AsciiString, TObjectData >             TSting2ObjDataMap;
typedef std::map< TCollection_AsciiString, TObjectData* >            TSting2ObjDataPtrMap;
typedef std::map< int, std::list < int > >                           TIntToListIntMap;

static GEOM_Engine* TheEngine = NULL;


static TCollection_AsciiString BuildIDFromObject(Handle(GEOM_BaseObject)& theObject)
{
  TCollection_AsciiString anID(theObject->GetDocID()), anEntry;
  TDF_Tool::Entry(theObject->GetEntry(), anEntry);
  anID+=(TCollection_AsciiString("_")+anEntry);
  return anID;
}

static TCollection_AsciiString BuildID(Standard_Integer theDocID, const char* theEntry)
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

bool ProcessFunction(Handle(GEOM_Function)&             theFunction,
                     TCollection_AsciiString&           theScript,
                     TCollection_AsciiString&           theAfterScript,
                     const TVariablesList&              theVariables,
                     const bool                         theIsPublished,
                     TDF_LabelMap&                      theProcessed,
                     std::set<TCollection_AsciiString>& theIgnoreObjs,
                     bool&                              theIsDumpCollected);

static int GetTag(const TCollection_AsciiString &theEntry);

static void FillMapOfRef(const Handle(GEOM_Function) &theFunction,
                               TIntToListIntMap      &theRefMap);

void ReplaceVariables(TCollection_AsciiString& theCommand,
                      const TVariablesList&    theVariables);

Handle(TColStd_HSequenceOfInteger) FindEntries(TCollection_AsciiString& theString);

void ReplaceEntriesByNames (TCollection_AsciiString&                  theScript,
                            TSting2ObjDataMap&                        aEntry2ObjData,
                            const bool                                theIsPublished,
                            TColStd_SequenceOfAsciiString&            theObjListToPublish,
                            Standard_Integer&                         objectCounter,
                            Resource_DataMapOfAsciiStringAsciiString& aNameToEntry);

void AddObjectColors (int                      theDocID,
                      TCollection_AsciiString& theScript,
                      const TSting2ObjDataMap& theEntry2ObjData);

void AddTextures (int theDocID, TCollection_AsciiString& theScript);

void PublishObject (TObjectData&                              theObjectData,
                    TSting2ObjDataMap&                        theEntry2ObjData,
                    const TSting2ObjDataPtrMap&               theStEntry2ObjDataPtr,
                    Resource_DataMapOfAsciiStringAsciiString& theNameToEntry,
                    std::map< int, TCollection_AsciiString >& theEntryToCmdMap,
                    std::set<TCollection_AsciiString>&        theMapOfPublished);

static TCollection_AsciiString GetPublishCommands
                   (const int                                       theTag,
                    const std::map< int, TCollection_AsciiString > &theEntryToCmdMap,
                    const TIntToListIntMap                         &theMapRefs,
                          std::set< int >                          &thePublished);

//================================================================================
/*!
 * \brief Fix up the name of python variable
 */
//================================================================================

void GEOM_Engine::healPyName( TCollection_AsciiString&                  pyName,
                              const TCollection_AsciiString&            anEntry,
                              Resource_DataMapOfAsciiStringAsciiString& aNameToEntry)
{
  const TCollection_AsciiString allowedChars
    ("qwertyuioplkjhgfdsazxcvbnmQWERTYUIOPLKJHGFDSAZXCVBNM0987654321_");

  if ( pyName.IsIntegerValue() ) { // pyName must not start with a digit
    pyName.Insert( 1, 'a' );
  }
  int p, p2=1; // replace not allowed chars
  while ((p = pyName.FirstLocationNotInSet(allowedChars, p2, pyName.Length()))) {
    pyName.SetValue(p, '_');
    p2=p;
  }
  if ( aNameToEntry.IsBound( pyName ) && anEntry != aNameToEntry( pyName ))
    {  // diff objects have same name - make a new name by appending a digit
      TCollection_AsciiString aName2;
      Standard_Integer i = 0;
      do {
        aName2 = pyName + "_" + ++i;
      } while ( aNameToEntry.IsBound( aName2 ) && anEntry != aNameToEntry( aName2 ));
      pyName = aName2;
    }
}

//=======================================================================
//function : GetTextureGUID
//purpose  :
//=======================================================================
const Standard_GUID& GEOM_Engine::GetTextureGUID()
{
  static Standard_GUID anID("FF1BBB01-5D14-4df2-980B-3A668264EA17");
  return anID;
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
#if OCC_VERSION_LARGE > 0x07000000
  _OCAFApp->DefineFormat("SALOME_GEOM", "GEOM Document Version 1.0", "sgd",
                         new StdDrivers_DocumentRetrievalDriver, 0);
  BinDrivers::DefineFormat(_OCAFApp);
#endif
  _UndoLimit = 0;
}

/*!
 *  Destructor
 */
GEOM_Engine::~GEOM_Engine()
{
  GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient It(_objects);
  std::list< Handle(GEOM_Object) > objs;
  for(; It.More(); It.Next())
    objs.push_back( Handle(GEOM_Object)::DownCast(It.Value()) );
  std::list< Handle(GEOM_Object) >::iterator objit;
  for(objit = objs.begin(); objit != objs.end(); ++objit)
    RemoveObject(*objit);

  //Close all documents not closed
  TColStd_DataMapIteratorOfDataMapOfIntegerTransient anItr (_mapIDDocument);
  for (; anItr.More(); anItr.Next())
  {
    Close(anItr.Key());
    anItr.Initialize( _mapIDDocument ); // anItr becomes invalid at _mapIDDocument.UnBind(docId)
  }
  _mapIDDocument.Clear();
  _objects.Clear();
}

//=============================================================================
/*!
 *  GetDocument
 */
//=============================================================================
Handle(TDocStd_Document) GEOM_Engine::GetDocument(int theDocID, bool force)
{
  Handle(TDocStd_Document) aDoc;
  if(_mapIDDocument.IsBound(theDocID)) {
    aDoc = Handle(TDocStd_Document)::DownCast(_mapIDDocument(theDocID));
  }
  else if (force) {
#if OCC_VERSION_MAJOR > 6
    _OCAFApp->NewDocument("BinOcaf", aDoc);
#else
    _OCAFApp->NewDocument("SALOME_GEOM", aDoc);
#endif
    aDoc->SetUndoLimit(_UndoLimit);
    _mapIDDocument.Bind(theDocID, aDoc);
    TDataStd_Integer::Set(aDoc->Main(), theDocID);
  }
  return aDoc;
}

//=============================================================================
/*!
 *  GetDocID
 */
//=============================================================================
int GEOM_Engine::GetDocID(Handle(TDocStd_Document) theDocument)
{
  if (theDocument.IsNull()) return -1;
  TColStd_DataMapIteratorOfDataMapOfIntegerTransient anItr (_mapIDDocument);
  for (; anItr.More(); anItr.Next())
    if (anItr.Value() == theDocument) return anItr.Key();

  return -1;
}

//=============================================================================
/*!
 *  GetObject
 */
//=============================================================================

Handle(GEOM_BaseObject) GEOM_Engine::GetObject(int theDocID, const char* theEntry, bool force)
{
  Handle(GEOM_BaseObject) anObject;

  TCollection_AsciiString anID = BuildID(theDocID, theEntry);

  if (_objects.IsBound(anID)) {
    anObject = Handle(GEOM_BaseObject)::DownCast(_objects(anID));
  }
  else if (force) {
    Handle(TDocStd_Document) aDoc = GetDocument(theDocID, force);
    if ( !aDoc.IsNull()) {
      TDF_Label aLabel;
      TDF_Tool::Label(aDoc->Main().Data(), theEntry, aLabel, Standard_True);
      if ( !aLabel.IsNull() ) {
        int objType = GEOM_BaseObject::GetType( aLabel );
        switch ( objType ) {
        case GEOM_FIELD_OBJTYPE:      anObject = new GEOM_Field    (aLabel); break;
        case GEOM_FIELD_STEP_OBJTYPE: anObject = new GEOM_FieldStep(aLabel); break;
        default:                      anObject = new GEOM_Object   (aLabel);
        }
        _objects.Bind(anID, anObject);
      }
    }
  }

  return anObject;
}

//=============================================================================
/*!
 *  AddBaseObject
 */
//=============================================================================

Handle(GEOM_BaseObject) GEOM_Engine::AddBaseObject(int theDocID, int theType)
{
  Handle(TDocStd_Document) aDoc = GetDocument(theDocID);
  Handle(TDataStd_TreeNode) aRoot = TDataStd_TreeNode::Set(aDoc->Main());

  // NPAL18604: use existing label to decrease memory usage,
  //            if this label has been freed (object deleted)
  bool useExisting = false;
  TDF_Label aChild;
  if (_freeLabels.find(theDocID) != _freeLabels.end()) {
    std::list<TDF_Label>& aFreeLabels = _freeLabels[theDocID];
    if (!aFreeLabels.empty()) {
      useExisting = true;
      aChild = aFreeLabels.front();
      aFreeLabels.pop_front();
    }
  }
  if (!useExisting) {
    // create new label
    aChild = TDF_TagSource::NewChild(aDoc->Main());
  }

  Handle(GEOM_BaseObject) anObject;
  switch ( theType ) {
  case GEOM_FIELD_OBJTYPE:      anObject = new GEOM_Field    (aChild, theType); break;
  case GEOM_FIELD_STEP_OBJTYPE: anObject = new GEOM_FieldStep(aChild, theType); break;
  default:                      anObject = new GEOM_Object   (aChild, theType);
  }

  //Put an object in the map of created objects
  TCollection_AsciiString anID = BuildIDFromObject(anObject);
  if(_objects.IsBound(anID)) _objects.UnBind(anID);
  _objects.Bind(anID, anObject);

  return anObject;
}

//================================================================================
/*!
 * \brief Adds a new object of the type theType in the OCAF document
 */
//================================================================================

Handle(GEOM_Object) GEOM_Engine::AddObject(int theDocID, int theType)
{
  return Handle(GEOM_Object)::DownCast( AddBaseObject(theDocID, theType) );
}

//=============================================================================
/*!
 *  AddSubShape
 */
//=============================================================================

Handle(GEOM_Object) GEOM_Engine::AddSubShape(Handle(GEOM_Object)              theMainShape,
                                             Handle(TColStd_HArray1OfInteger) theIndices,
                                             bool isStandaloneOperation)
{
  if (theMainShape.IsNull() || theIndices.IsNull()) return NULL;

  Handle(TDocStd_Document) aDoc = GetDocument(theMainShape->GetDocID());
  Handle(TDataStd_TreeNode) aRoot = TDataStd_TreeNode::Set(aDoc->Main());

  // NPAL18604: use existing label to decrease memory usage,
  //            if this label has been freed (object deleted)
  bool useExisting = false;
  TDF_Label aChild;
  int aDocID = theMainShape->GetDocID();
  if (_freeLabels.find(aDocID) != _freeLabels.end()) {
    std::list<TDF_Label>& aFreeLabels = _freeLabels[aDocID];
    if (!aFreeLabels.empty()) {
      useExisting = true;
      aChild = aFreeLabels.front();
      aFreeLabels.pop_front();
    }
  }
  if (!useExisting) {
    // create new label
    aChild = TDF_TagSource::NewChild(aDoc->Main());
  }

  Handle(GEOM_Function) aMainShape = theMainShape->GetLastFunction();
  Handle(GEOM_Object) anObject = new GEOM_Object (aChild, 28); //28 is SUBSHAPE type
  Handle(GEOM_Function) aFunction = anObject->AddFunction(GEOM_Object::GetSubShapeID(), 1);

  GEOM_ISubShape aSSI (aFunction);
  aSSI.SetMainShape(aMainShape);
  aSSI.SetIndices(theIndices);

  try {
    OCC_CATCH_SIGNALS;
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

  // Put an object in the map of created objects
  TCollection_AsciiString anID = BuildIDFromObject(anObject);
  if (_objects.IsBound(anID)) _objects.UnBind(anID);
  _objects.Bind(anID, anObject);

  // Put this sub-shape in the list of sub-shapes of theMainShape
  aMainShape->AddSubShapeReference(aFunction);

  GEOM::TPythonDump pd (aFunction);

  if (isStandaloneOperation) {
    pd << anObject << " = geompy.GetSubShape(" << theMainShape << ", [";
    Standard_Integer i = theIndices->Lower(), up = theIndices->Upper();
    for (; i <= up - 1; i++) {
      pd << theIndices->Value(i) << ", ";
    }
    pd << theIndices->Value(up) << "])";
  }
  else
    pd << "None";

  return anObject;
}

//=============================================================================
/*!
 *  RemoveObject
 */
//=============================================================================
bool GEOM_Engine::RemoveObject(Handle(GEOM_BaseObject)& theObject)
{
  if (theObject.IsNull()) return false;

  int aDocID = theObject->GetDocID();
  if(!_mapIDDocument.IsBound(aDocID))
    return false;  // document is closed...

  //Remove an object from the map of available objects
  TCollection_AsciiString anID = BuildIDFromObject(theObject);
  if (_objects.IsBound(anID)) {
    Handle(GEOM_BaseObject) anObject = Handle(GEOM_BaseObject)::DownCast(_objects(anID));
    if ( anObject != theObject )
      anObject->_label = anObject->_label.Root();
    _objects.UnBind(anID);
  }

  // If sub-shape, remove it from the list of sub-shapes of its main shape
  Handle(GEOM_Object) aGO = Handle(GEOM_Object)::DownCast( theObject );
  if ( !aGO.IsNull() && !aGO->IsMainShape()) {
    Handle(GEOM_Function) aFunction = theObject->GetFunction(1);
    GEOM_ISubShape aSSI (aFunction);
    Handle(GEOM_Function) aMainShape = aSSI.GetMainShape();
    //If main shape is not null, then remove
    if(!aMainShape.IsNull())
      aMainShape->RemoveSubShapeReference(aFunction);
  }

  int nb = theObject->GetNbFunctions();
  Handle(TDataStd_TreeNode) aNode;
  for (int i = 1; i <= nb; i++) {
    Handle(GEOM_Function) aFunction = theObject->GetFunction(i);
    if (aFunction->GetEntry().FindAttribute(GEOM_Function::GetFunctionTreeID(), aNode))
      aNode->Remove();
  }

  TDF_Label aLabel = theObject->GetEntry();
  aLabel.ForgetAllAttributes(Standard_True);

  // Remember the label to reuse it then
  std::list<TDF_Label>& aFreeLabels = _freeLabels[aDocID];
  if ( aFreeLabels.empty() || aFreeLabels.back() != aLabel )
    aFreeLabels.push_back(aLabel);

  // we can't explicitely delete theObject. At least prevent its functioning
  // as an alive object when aLabel is reused for a new object
  theObject->_label = aLabel.Root();
  theObject->_ior.Clear();
  theObject->_parameters.Clear();
  theObject->_docID = -1;

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
bool GEOM_Engine::Save(int theDocID, const char* theFileName)
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
bool GEOM_Engine::Load(int theDocID, const char* theFileName)
{
  Handle(TDocStd_Document) aDoc;
  if (_OCAFApp->Open(theFileName, aDoc) != PCDM_RS_OK) {
    return false;
  }

#if OCC_VERSION_MAJOR > 6
  // Replace old document format by the new one.
  if (aDoc->StorageFormat().IsEqual("SALOME_GEOM")) {
    aDoc->ChangeStorageFormat("BinOcaf");
  }
#endif

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
  if (_mapIDDocument.IsBound(theDocID)) {
    Handle(TDocStd_Document) aDoc = Handle(TDocStd_Document)::DownCast(_mapIDDocument(theDocID));

    //Remove all GEOM Objects associated to the given document
    TColStd_SequenceOfAsciiString aSeq;
    GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient It (_objects);
    for (; It.More(); It.Next()) {
      TCollection_AsciiString anObjID (It.Key());
      Standard_Integer anID = ExtractDocID(anObjID);
      if (theDocID == anID) aSeq.Append(It.Key());
    }
    for (Standard_Integer i=1; i<=aSeq.Length(); i++) _objects.UnBind(aSeq.Value(i));

    // Forget free labels for this document
    TFreeLabelsList::iterator anIt = _freeLabels.find(theDocID);
    if (anIt != _freeLabels.end()) {
      _freeLabels.erase(anIt);
    }

    _mapIDDocument.UnBind(theDocID);
    _OCAFApp->Close(aDoc);
    aDoc.Nullify();
  }
}

//=============================================================================
/*!
 *  DumpPython
 */
//=============================================================================
TCollection_AsciiString GEOM_Engine::DumpPython(int theDocID,
                                                std::vector<TObjectData>& theObjectData,
                                                TVariablesList theVariables,
                                                bool isPublished,
                                                bool isMultiFile, 
                                                bool& aValidScript)
{
  // Set "C" numeric locale to save numbers correctly
  Kernel_Utils::Localizer loc;

  TCollection_AsciiString aScript;
  Handle(TDocStd_Document) aDoc = GetDocument(theDocID);

  if (aDoc.IsNull())
  {
    TCollection_AsciiString anEmptyScript;
    if( isMultiFile )
      anEmptyScript = "def RebuildData(theStudy): pass\n";
    return anEmptyScript;
  }

  aScript  = "import GEOM\n";
  aScript += "from salome.geom import geomBuilder\n";
  aScript += "import math\n";
  aScript += "import SALOMEDS\n\n";
  if( isMultiFile )
    aScript += "def RebuildData(theStudy):";

  aScript += "\n\tgeompy = geomBuilder.New(theStudy)\n";

  AddTextures(theDocID, aScript);

  Standard_Integer posToInsertGlobalVars = aScript.Length() + 1;

  // a map containing copies of TObjectData from theObjectData
  TSting2ObjDataMap    aEntry2ObjData;
  // contains pointers to TObjectData of either aEntry2ObjData or theObjectData; the latter
  // occures when several StudyEntries correspond to one Entry
  TSting2ObjDataPtrMap aStEntry2ObjDataPtr;

  //Resource_DataMapOfAsciiStringAsciiString aEntry2StEntry, aStEntry2Entry, theObjectNames;
  for (unsigned i = 0; i < theObjectData.size(); ++i )
  {
    TObjectData& data = theObjectData[i];
    // look for an object by entry
    TDF_Label L;
    TDF_Tool::Label( aDoc->GetData(), data._entry, L );
    if ( L.IsNull() ) continue;
    Handle(GEOM_BaseObject) obj = GEOM_BaseObject::GetObject( L );
    // fill maps
    if ( !obj.IsNull() ) {
      TSting2ObjDataMap::iterator ent2Data =
        aEntry2ObjData.insert( std::make_pair( data._entry, data )).first;

      if ( ent2Data->second._studyEntry == data._studyEntry ) // Entry encounters 1st time
        aStEntry2ObjDataPtr.insert( std::make_pair( data._studyEntry, & ent2Data->second ));
      else
        aStEntry2ObjDataPtr.insert( std::make_pair( data._studyEntry, & data ));
    }
  }

  // collect objects entries to be published
  TColStd_SequenceOfAsciiString aObjListToPublish;

  // iterates on functions till critical (that requiers publication of objects)
  Handle(TDataStd_TreeNode) aNode, aRoot;
  Handle(GEOM_Function) aFunction;
  TDF_LabelMap aCheckedFuncMap;
  std::set< TCollection_AsciiString > anIgnoreObjMap;

  TCollection_AsciiString aFuncScript;

  // Mantis issue 0020768
  Standard_Integer objectCounter = 0;
  Resource_DataMapOfAsciiStringAsciiString aNameToEntry;
  TIntToListIntMap                         aRefMap;

  if (aDoc->Main().FindAttribute(GEOM_Function::GetFunctionTreeID(), aRoot)) {
    TDataStd_ChildNodeIterator Itr(aRoot);
    for (; Itr.More(); Itr.Next()) {
      aNode = Itr.Value();
      aFunction = GEOM_Function::GetFunction(aNode->Label());
      if (aFunction.IsNull()) {
        MESSAGE ( "Null function !!!!" );
        continue;
      }
      bool isDumpCollected = false;
      TCollection_AsciiString aCurScript, anAfterScript;
      if (!ProcessFunction(aFunction, aCurScript, anAfterScript, theVariables,
                           isPublished, aCheckedFuncMap, anIgnoreObjMap,
                           isDumpCollected ))
        continue;
      // add function description before dump
      if (!aCurScript.IsEmpty())
      {
        if ( aFunction->GetDriverGUID() == GEOM_Object::GetSubShapeID() &&
             aFuncScript.Length() > aCurScript.Length() )
          // avoid repeated SubShape...() command at the end
          if (aFuncScript.Location( aCurScript,
                                    aFuncScript.Length() - aCurScript.Length(),
                                    aFuncScript.Length()))
            continue; // aCurScript is already at the end of aFuncScript
        aFuncScript += aCurScript;
      }

      // Fill the map of references.
      FillMapOfRef(aFunction, aRefMap);

      if (isDumpCollected ) {
        // Replace entries by the names
        ReplaceEntriesByNames( aFuncScript, aEntry2ObjData, isPublished,
                               aObjListToPublish, objectCounter, aNameToEntry );

        // publish collected objects
        std::map< int, TCollection_AsciiString > anEntryToCmdMap; // sort publishing commands by study entry
        int i = 1, n = aObjListToPublish.Length();
        for ( ; i <= n; i++ )
        {
          const TCollection_AsciiString& aEntry = aObjListToPublish.Value(i);
          PublishObject( aEntry2ObjData[aEntry], aEntry2ObjData, aStEntry2ObjDataPtr,
                         aNameToEntry, anEntryToCmdMap, anIgnoreObjMap );
        }
        // add publishing commands to the script
        std::set< int >                                    aPublished;
        std::map< int, TCollection_AsciiString >::iterator anEntryToCmd = anEntryToCmdMap.begin();

        for ( ; anEntryToCmd != anEntryToCmdMap.end(); ++anEntryToCmd ) {
          const TCollection_AsciiString aPublishCmds =
              GetPublishCommands(anEntryToCmd->first, anEntryToCmdMap,
                                 aRefMap, aPublished);

          aFuncScript += aPublishCmds;
        }

        // PTv, 0020001 add result objects from RestoreGivenSubShapes into ignore list,
        //  because they will be published during command execution
        int indx = anAfterScript.Search( "RestoreGivenSubShapes" );
        if ( indx == -1 )
          indx = anAfterScript.Search( "RestoreSubShapes" );
        if ( indx != -1 ) {
          TCollection_AsciiString aSubStr = anAfterScript.SubString(1, indx);
          Handle(TColStd_HSequenceOfInteger) aSeq = FindEntries(aSubStr);
          i = 1, n = aSeq->Length();
          for ( ; i <= n; i+=2) {
            TCollection_AsciiString anEntry =
              aSubStr.SubString(aSeq->Value(i), aSeq->Value(i+1));
            anIgnoreObjMap.insert(anEntry.ToCString());
          }
        }

        aObjListToPublish.Clear();
        aScript += aFuncScript;
        aFuncScript.Clear();
      }
      aFuncScript += anAfterScript;
    }
  }

  // Replace entries by the names
  aObjListToPublish.Clear();
  ReplaceEntriesByNames( aFuncScript, aEntry2ObjData, isPublished, aObjListToPublish,
                         objectCounter, aNameToEntry );

  aScript += aFuncScript;

  // ouv : NPAL12872
  AddObjectColors( theDocID, aScript, aEntry2ObjData );

  // Make script to publish in study
  TSting2ObjDataPtrMap::iterator aStEntry2ObjDataPtrIt;
  if ( isPublished )
  {
    std::map< int, TCollection_AsciiString > anEntryToCmdMap; // sort publishing commands by object entry

    for (aStEntry2ObjDataPtrIt  = aStEntry2ObjDataPtr.begin();
         aStEntry2ObjDataPtrIt != aStEntry2ObjDataPtr.end();
         ++aStEntry2ObjDataPtrIt)
    {
      TObjectData* data = aStEntry2ObjDataPtrIt->second;
      if ( anIgnoreObjMap.count( data->_entry ))
        continue; // should not be dumped
      PublishObject( *data, aEntry2ObjData, aStEntry2ObjDataPtr,
                     aNameToEntry, anEntryToCmdMap, anIgnoreObjMap );
    }
    // add publishing commands to the script
    std::set< int >                                    aPublished;
    std::map< int, TCollection_AsciiString >::iterator anEntryToCmd = anEntryToCmdMap.begin();

    for ( ; anEntryToCmd != anEntryToCmdMap.end(); ++anEntryToCmd ) {
      const TCollection_AsciiString aPublishCmds =
          GetPublishCommands(anEntryToCmd->first, anEntryToCmdMap,
                             aRefMap, aPublished);

      aScript += aPublishCmds;
    }
  }

  //RNV: issue 16219: EDF PAL 469: "RemoveFromStudy" Function
  //Add unpublish command if need
  TCollection_AsciiString unpublishCmd("\n");
  if(isMultiFile)
    unpublishCmd += "\t";
  unpublishCmd += "geompy.hideInStudy(";
  
  for (aStEntry2ObjDataPtrIt  = aStEntry2ObjDataPtr.begin();
       aStEntry2ObjDataPtrIt != aStEntry2ObjDataPtr.end();
       ++aStEntry2ObjDataPtrIt)
    {
      TObjectData* data = aStEntry2ObjDataPtrIt->second;      
      if ( data->_unpublished && !data->_pyName.IsEmpty() ) {
        aScript +=  unpublishCmd + data->_pyName + ")";
      }
    }    
  
  //aScript += "\n\tpass\n";
  aScript += "\n";
  aValidScript = true;

  // fill _studyEntry2NameMap and build globalVars
  TCollection_AsciiString globalVars;
  _studyEntry2NameMap.Clear();
  for (aStEntry2ObjDataPtrIt  = aStEntry2ObjDataPtr.begin();
       aStEntry2ObjDataPtrIt != aStEntry2ObjDataPtr.end();
       ++aStEntry2ObjDataPtrIt)
  {
    const TCollection_AsciiString& studyEntry = aStEntry2ObjDataPtrIt->first;
    const TObjectData*                   data = aStEntry2ObjDataPtrIt->second;
    _studyEntry2NameMap.Bind ( studyEntry, data->_pyName );
    if ( !globalVars.IsEmpty() )
      globalVars += ", ";
    globalVars += data->_pyName;
  }
  if ( isMultiFile && !globalVars.IsEmpty() ) {
    globalVars.Insert( 1, "\n\tglobal " );
    aScript.Insert( posToInsertGlobalVars, globalVars );
  }

  return aScript;
}

//=======================================================================
//function : GetDumpName
//purpose  :
//=======================================================================

const char* GEOM_Engine::GetDumpName (const char* theStudyEntry) const
{
  if ( _studyEntry2NameMap.IsBound( (char*)theStudyEntry ))
    return _studyEntry2NameMap( (char*)theStudyEntry ).ToCString();

  return NULL;
}

//=======================================================================
//function : GetAllDumpNames
//purpose  :
//=======================================================================

Handle(TColStd_HSequenceOfAsciiString) GEOM_Engine::GetAllDumpNames() const
{
  Handle(TColStd_HSequenceOfAsciiString) aRetSeq = new TColStd_HSequenceOfAsciiString;

  Resource_DataMapIteratorOfDataMapOfAsciiStringAsciiString it (_studyEntry2NameMap);
  for (; it.More(); it.Next()) {
    aRetSeq->Append(it.Value());
  }

  return aRetSeq;
}

#define TEXTURE_LABEL_ID       1
#define TEXTURE_LABEL_FILE     2
#define TEXTURE_LABEL_WIDTH    3
#define TEXTURE_LABEL_HEIGHT   4
#define TEXTURE_LABEL_DATA     5

int GEOM_Engine::addTexture(int theDocID, int theWidth, int theHeight,
                            const Handle(TColStd_HArray1OfByte)& theTexture,
                            const TCollection_AsciiString& theFileName)
{
  Handle(TDocStd_Document) aDoc = GetDocument(theDocID);
  Handle(TDataStd_TreeNode) aRoot = TDataStd_TreeNode::Set(aDoc->Main());

  // NPAL18604: use existing label to decrease memory usage,
  //            if this label has been freed (object deleted)
  bool useExisting = false;
  TDF_Label aChild;
  if (_freeLabels.find(theDocID) != _freeLabels.end()) {
    std::list<TDF_Label>& aFreeLabels = _freeLabels[theDocID];
    if (!aFreeLabels.empty()) {
      useExisting = true;
      aChild = aFreeLabels.front();
      aFreeLabels.pop_front();
    }
  }
  if (!useExisting) {
    // create new label
    aChild = TDF_TagSource::NewChild(aDoc->Main());
  }

  aChild.ForgetAllAttributes(Standard_True);
  Handle(TDataStd_TreeNode) node;
  if ( !aChild.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), node ) )
    node = TDataStd_TreeNode::Set(aChild);
  TDataStd_UAttribute::Set(aChild, GetTextureGUID());

  static int aTextureID = 0;

  TDataStd_Integer::Set(aChild.FindChild(TEXTURE_LABEL_ID),     ++aTextureID);
  TDataStd_Comment::Set(aChild.FindChild(TEXTURE_LABEL_FILE),   theFileName);
  TDataStd_Integer::Set(aChild.FindChild(TEXTURE_LABEL_WIDTH),  theWidth);
  TDataStd_Integer::Set(aChild.FindChild(TEXTURE_LABEL_HEIGHT), theHeight);

  Handle(TDataStd_ByteArray) anAttr =
    TDataStd_ByteArray::Set(aChild.FindChild(TEXTURE_LABEL_DATA),
                            theTexture.IsNull() ? 0 : theTexture->Lower(),
                            theTexture.IsNull() ? 0 : theTexture->Upper());
  anAttr->ChangeArray(theTexture);

  return aTextureID;
}

Handle(TColStd_HArray1OfByte) GEOM_Engine::getTexture(int theDocID, int theTextureID,
                                                      int& theWidth, int& theHeight,
                                                      TCollection_AsciiString& theFileName)
{
  Handle(TColStd_HArray1OfByte) anArray;
  theWidth = theHeight = 0;

  Handle(TDocStd_Document) aDoc = GetDocument(theDocID);

  TDF_ChildIterator anIterator(aDoc->Main(), Standard_True);
  bool found = false;
  for (; anIterator.More() && !found; anIterator.Next()) {
    TDF_Label aTextureLabel = anIterator.Value();
    if (aTextureLabel.IsAttribute( GetTextureGUID())) {
      TDF_Label anIDLabel = aTextureLabel.FindChild(TEXTURE_LABEL_ID, Standard_False);
      Handle(TDataStd_Integer) anIdAttr;
      if(!anIDLabel.IsNull() && anIDLabel.FindAttribute(TDataStd_Integer::GetID(), anIdAttr) &&
         anIdAttr->Get() == theTextureID) {
        TDF_Label aFileLabel   = aTextureLabel.FindChild(TEXTURE_LABEL_FILE,    Standard_False);
        TDF_Label aWidthLabel  = aTextureLabel.FindChild(TEXTURE_LABEL_WIDTH,   Standard_False);
        TDF_Label aHeightLabel = aTextureLabel.FindChild(TEXTURE_LABEL_HEIGHT,  Standard_False);
        TDF_Label aDataLabel   = aTextureLabel.FindChild(TEXTURE_LABEL_DATA,    Standard_False);
        Handle(TDataStd_Integer) aWidthAttr, aHeightAttr;
        Handle(TDataStd_ByteArray) aTextureAttr;
        Handle(TDataStd_Comment) aFileAttr;
        if (!aWidthLabel.IsNull()  && aWidthLabel.FindAttribute(TDataStd_Integer::GetID(),  aWidthAttr) &&
            !aHeightLabel.IsNull() && aHeightLabel.FindAttribute(TDataStd_Integer::GetID(), aHeightAttr) &&
            !aDataLabel.IsNull()   && aDataLabel.FindAttribute(TDataStd_ByteArray::GetID(), aTextureAttr)) {
          theWidth = aWidthAttr->Get();
          theHeight = aHeightAttr->Get();
          anArray = aTextureAttr->InternalArray();
        }
        if (!aFileLabel.IsNull() && aFileLabel.FindAttribute(TDataStd_Comment::GetID(), aFileAttr))
          theFileName = aFileAttr->Get();
        found = true;
      }
    }
  }
  return anArray;
}

std::list<int> GEOM_Engine::getAllTextures(int theDocID)
{
  std::list<int> id_list;

  Handle(TDocStd_Document) aDoc = GetDocument(theDocID);

  TDF_ChildIterator anIterator(aDoc->Main(), Standard_True);
  for (; anIterator.More(); anIterator.Next()) {
    TDF_Label aTextureLabel = anIterator.Value();
    if (aTextureLabel.IsAttribute( GetTextureGUID())) {
      TDF_Label anIDLabel = aTextureLabel.FindChild(TEXTURE_LABEL_ID, Standard_False);
      Handle(TDataStd_Integer) anIdAttr;
      if(!anIDLabel.IsNull() && anIDLabel.FindAttribute(TDataStd_Integer::GetID(), anIdAttr))
        id_list.push_back((int)anIdAttr->Get());
    }
  }
  return id_list;
}

void GEOM_Engine::DocumentModified(const int theDocId, const bool isModified)
{
  if (isModified) _mapModifiedDocs.Add(theDocId);
  else _mapModifiedDocs.Remove(theDocId);
}
 
bool GEOM_Engine::DocumentModified(const int theDocId)
{
  return _mapModifiedDocs.Contains(theDocId);
}

//===========================================================================
//                     Internal functions
//===========================================================================

//=============================================================================
/*!
 *  MakeCommandfor3DSketcher: Make new command for 3DSketcher
 */
//=============================================================================
TCollection_AsciiString MakeCommandfor3DSketcher (const TCollection_AsciiString& theDescr )
{
    TCollection_AsciiString aNewDescr;
    int i = 1;
    TCollection_AsciiString aSubStr = theDescr.Token("\n\t", i);
    for (; !aSubStr.IsEmpty(); aSubStr = theDescr.Token("\n\t", i)) {
      if (aSubStr.Search( "Make3DSketcherCommand" ) != -1) {
        TCollection_AsciiString aResult = aSubStr.Token(" ", 1);
        // "3DSketcher:CMD[:CMD[:CMD...]]"
        TCollection_AsciiString aCommand = aSubStr.Token("\"", 2);

        // Split the command string to separate CMDs
        int icmd = 2;
        TColStd_SequenceOfAsciiString aSequence;
        if (aCommand.Length()) {
          TCollection_AsciiString aToken = aCommand.Token(":", icmd);
          while (aToken.Length() > 0) {
            aSequence.Append(aToken);
            aToken = aCommand.Token(":", ++icmd);
          }
        }

        if (aSequence.Length() > 0) {
          if (i > 1)
            aNewDescr += "\n\t";

          aNewDescr += "\nsk = geompy.Sketcher3D()";
          int nbCMDs = aSequence.Length();
          for (icmd = 1; icmd <= nbCMDs; icmd++) {
            aNewDescr += "\n\t";

            TCollection_AsciiString aCMD = aSequence.Value(icmd);

            // Split the CMD into string values
            TColStd_SequenceOfAsciiString aStrVals;
            int ival = 1;
            TCollection_AsciiString aToken = aCMD.Token(" ", ival);
            while (aToken.Length() > 0) {
              aStrVals.Append(aToken);
              aToken = aCMD.Token(" ", ++ival);
            }

            TCollection_AsciiString aCMDpref = aStrVals.Value(1);
            if (aCMDpref == "TT") {
              aNewDescr += "sk.addPointsAbsolute(";
              aNewDescr += aStrVals.Value(2) + ", " + aStrVals.Value(3) + ", " + aStrVals.Value(4) + ")";
            }
            else if (aCMDpref == "T") {
              aNewDescr += "sk.addPointsRelative(";
              aNewDescr += aStrVals.Value(2) + ", " + aStrVals.Value(3) + ", " + aStrVals.Value(4) + ")";
            }
            else if (aCMDpref == "WW") {
              aNewDescr += "sk.close()";
            }
            else if (aCMDpref.Value(1) == 'O'){
              TCollection_AsciiString aCMDtrunc = aStrVals.Value(1);
              aCMDtrunc.Trunc(3);
              if (aCMDpref.Value(4) == 'C')
                aNewDescr += "sk.addPointRadiusAngleH";
              else
                aNewDescr += "sk.addPointRadiusAngles";
              if (aCMDpref.Value(5) == 'A')
                aNewDescr += "Absolute(";
              else
                aNewDescr += "Relative(";
              aNewDescr +=  aStrVals.Value(4) + ", " +
                aStrVals.Value(2) + ", " + aStrVals.Value(3) + ", " + "\""+aCMDtrunc+"\"" + ")";
            }
          }
          aNewDescr += "\n\t";
          aNewDescr += aResult + " = sk.wire()";
        }
      } // Make3DSketcherCommand
      else if (aSubStr.Search( "Make3DSketcher" ) != -1) {
        TCollection_AsciiString aResult = aSubStr.Token(" ", 1);
        TCollection_AsciiString aCommand = aSubStr.Token("[", 2);
        aCommand = aCommand.Token("]", 1);
        if (i > 1)
          aNewDescr += "\n\t";
        aNewDescr += "\nsk = geompy.Sketcher3D()";
        aNewDescr += "\n\t";
        aNewDescr += "sk.addPointsAbsolute(";
        aNewDescr += aCommand + ")";
        aNewDescr += "\n\t";
        aNewDescr += aResult + " = sk.wire()";
      }
      else {
        if (i > 1)
          aNewDescr += "\n\t";
        aNewDescr += aSubStr;
      }
      i++;
    }
    return aNewDescr;
}

//=============================================================================
/*!
 *  ProcessFunction: Dump function description into script
 */
//=============================================================================
bool ProcessFunction(Handle(GEOM_Function)&             theFunction,
                     TCollection_AsciiString&           theScript,
                     TCollection_AsciiString&           theAfterScript,
                     const TVariablesList&              theVariables,
                     const bool                         theIsPublished,
                     TDF_LabelMap&                      theProcessed,
                     std::set<TCollection_AsciiString>& theIgnoreObjs,
                     bool&                              theIsDumpCollected)
{
  theIsDumpCollected = false;
  if (theFunction.IsNull()) return false;

  if (theProcessed.Contains(theFunction->GetEntry())) return false;

  // pass functions, that depends on nonexisting ones
  bool doNotProcess = false;
  TDF_LabelSequence aSeq;
  theFunction->GetDependency(aSeq);
  Standard_Integer aLen = aSeq.Length();
  for (Standard_Integer i = 1; i <= aLen && !doNotProcess; i++) {
    TDF_Label aRefLabel = aSeq.Value(i);
    Handle(TDF_Reference) aRef;
    if (!aRefLabel.FindAttribute(TDF_Reference::GetID(), aRef)) {
      doNotProcess = true;
    }
    else {
      if (aRef.IsNull() || aRef->Get().IsNull()) {
        doNotProcess = true;
      }
      else {
        Handle(TDataStd_TreeNode) aT;
        if (!TDataStd_TreeNode::Find(aRef->Get(), aT)) {
          doNotProcess = true;
        }
        else {
          TDF_Label aDepLabel = aT->Label();
          Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(aDepLabel);

          if (aFunction.IsNull()) doNotProcess = true;
          else if (!theProcessed.Contains(aDepLabel)) doNotProcess = true;
        }
      }
    }
  }

  if (doNotProcess) {
    TCollection_AsciiString anObjEntry;
    TDF_Tool::Entry(theFunction->GetOwnerEntry(), anObjEntry);
    theIgnoreObjs.insert(anObjEntry);
    return false;
  }
  theProcessed.Add(theFunction->GetEntry());

  TCollection_AsciiString aDescr = theFunction->GetDescription();
  if(aDescr.Length() == 0) return false;

  //Check if its internal function which doesn't requires dumping
  if(aDescr == "None") return false;

  //Check the very specific case of RestoreShape function,
  //which is not dumped, but the result can be published by the user.
  //We do not publish such objects to decrease danger of dumped script failure.
  if(aDescr.Value(1) == '#') {
    TCollection_AsciiString anObjEntry;
    TDF_Tool::Entry(theFunction->GetOwnerEntry(), anObjEntry);
    theIgnoreObjs.insert(anObjEntry);
    return false;
  }

  // 0020001 PTv, check for critical functions, which require dump of objects
  if (theIsPublished)
  {
    // currently, there is only one function "RestoreGivenSubShapes",
    // later this check could be replaced by iterations on list of such functions
    if (aDescr.Search( "RestoreGivenSubShapes" ) != -1)
      theIsDumpCollected = true;
    else if (aDescr.Search( "RestoreSubShapes" ) != -1)
      theIsDumpCollected = true;
  }

  //Replace parameter by notebook variables
  ReplaceVariables(aDescr,theVariables);

  //Process sketcher functions, replacing string command by calls to Sketcher interface
  if ( ( aDescr.Search( "MakeSketcherOnPlane" ) != -1 ) || ( aDescr.Search( "MakeSketcher" ) != -1 ) ) {
    Sketcher_Profile aProfile( aDescr.ToCString());
    // Make new command for SketcherOnPlane and for Sketcher
    aDescr = aProfile.GetDump();
  }
  if (aDescr.Search( "Make3DSketcher" ) != -1) {
    aDescr = MakeCommandfor3DSketcher ( aDescr );
  }

  if ( theIsDumpCollected ) {
    int i = 1;
    bool isBefore = true;
    TCollection_AsciiString aSubStr = aDescr.Token("\n\t", i++);
    while (!aSubStr.IsEmpty()) {
      if (isBefore &&
          aSubStr.Search( "RestoreGivenSubShapes" ) == -1 &&
          aSubStr.Search( "RestoreSubShapes" ) == -1)
        theScript += TCollection_AsciiString("\n\t") + aSubStr;
      else
        theAfterScript += TCollection_AsciiString("\n\t") + aSubStr;
      aSubStr = aDescr.Token("\n\t", i++);
    }
  }
  else {
    theScript += "\n\t";
    theScript += aDescr;
  }
  return true;
}

//=============================================================================
/*!
 *  GetTag: Returns the tag from entry
 */
//=============================================================================
int GetTag(const TCollection_AsciiString &theEntry)
{
  const int aGeomObjDepth = 3;
  const int aTag          = theEntry.Token(":", aGeomObjDepth).IntegerValue();

  return aTag;
}

//=============================================================================
/*!
 *  FillMapOfRef: Fill the map of references
 */
//=============================================================================
void FillMapOfRef(const Handle(GEOM_Function) &theFunction,
                        TIntToListIntMap      &theRefMap)
{
  TDF_LabelSequence       aSeq;
  TCollection_AsciiString anObjEntry;
  int                     anObjTag;

  TDF_Tool::Entry(theFunction->GetOwnerEntry(), anObjEntry);
  anObjTag = GetTag(anObjEntry);
  theFunction->GetDependency(aSeq);

  const Standard_Integer aLen = aSeq.Length();
  Standard_Integer       i;

  for (i = 1; i <= aLen; i++) {
    TDF_Label             aRefLabel = aSeq.Value(i);
    Handle(TDF_Reference) aRef;

    if (aRefLabel.FindAttribute(TDF_Reference::GetID(), aRef)) {
      if (!aRef.IsNull() && !aRef->Get().IsNull()) {
        Handle(TDataStd_TreeNode) aT;

        if (TDataStd_TreeNode::Find(aRef->Get(), aT)) {
          TDF_Label             aDepLabel = aT->Label();
          Handle(GEOM_Function) aRefFunct = GEOM_Function::GetFunction(aDepLabel);

          if (!aRefFunct.IsNull()) {
            // Get entry of the referenced object.
            TDF_Tool::Entry(aRefFunct->GetOwnerEntry(), anObjEntry);

            const int aRefTag = GetTag(anObjEntry);

            if (anObjTag != aRefTag) {
              // Avoid making references for operations without copy.
              theRefMap[anObjTag].push_back(aRefTag);
            }
          }
        }
      }
    }
  }
}

//=============================================================================
/*!
 *  FindEntries: Returns a sequence of start/end positions of entries in the string
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) FindEntries(TCollection_AsciiString& theString)
{
  Handle(TColStd_HSequenceOfInteger) aSeq = new TColStd_HSequenceOfInteger;
  Standard_Integer aLen = theString.Length();
  Standard_Boolean isFound = Standard_False;

  const char* arr = theString.ToCString();
  Standard_Integer i = 0, j;

  while(i < aLen) {
    int c = (int)arr[i];
    j = i+1;
    if(c >= 48 && c <= 57) { //Is digit?

      isFound = Standard_False;
      while((j < aLen) && ((c >= 48 && c <= 57) || c == 58) ) { //Check if it is an entry
        c = (int)arr[j++];
        if(c == 58) isFound = Standard_True;
      }

      if(isFound && arr[j-2] != 58) { // last char should be a diggit
        aSeq->Append(i+1); // +1 because AsciiString starts from 1
        aSeq->Append(j-1);
      }
    }

    i = j;
  }

  return aSeq;
}

//=============================================================================
/*!
 *  ReplaceVariables: Replace parameters of the function by variales from
 *                    Notebook if need
 */
//=============================================================================
void ReplaceVariables(TCollection_AsciiString& theCommand,
                      const TVariablesList&    theVariables)
{
  if (MYDEBUG)
    cout<<"Command : "<<theCommand<<endl;

  if (MYDEBUG) {
    cout<<"All Entries:"<<endl;
    TVariablesList::const_iterator it = theVariables.begin();
    for(;it != theVariables.end();it++)
      cout<<"\t'"<<(*it).first<<"'"<<endl;
  }

  //Additional case - multi-row commands
  int aCommandIndex = 1;
  while( aCommandIndex < 10 ) { // tmp check
    TCollection_AsciiString aCommand = theCommand.Token("\n",aCommandIndex);
    if( aCommand.Length() == 0 )
      break;

    if (MYDEBUG)
      cout<<"Sub-command : "<<aCommand<<endl;

    Standard_Integer aStartCommandPos = theCommand.Location(aCommand,1,theCommand.Length());
    Standard_Integer aEndCommandPos = aStartCommandPos + aCommand.Length();

    //Get Entry of the result object
    TCollection_AsciiString anEntry;
    if( aCommand.Search("=") != -1 ) // command returns an object
      anEntry = aCommand.Token("=",1);
    else { // command modifies the object
      if (int aStartEntryPos = aCommand.Location(1,'(',1,aCommand.Length()))
        if (int aEndEntryPos = aCommand.Location(1,',',aStartEntryPos,aCommand.Length()))
          anEntry = aCommand.SubString(aStartEntryPos+1, aEndEntryPos-1);
    }
    //Remove white spaces
    anEntry.RightAdjust();
    anEntry.LeftAdjust();
    if(MYDEBUG)
      cout<<"Result entry : '" <<anEntry<<"'"<<endl;

    if ( anEntry.IsEmpty() ) {
      aCommandIndex++;
      continue;
    }

    //Check if result is list of entries - enough to get the first entry in this case
    int aNbEntries = 1;
    if( anEntry.Value( 1 ) == O_SQR_BRACKET && anEntry.Value( anEntry.Length() ) == C_SQR_BRACKET ) {
      while(anEntry.Location(aNbEntries,COMMA,1,anEntry.Length()))
        aNbEntries++;
      TCollection_AsciiString aSeparator(COMMA);
      anEntry = anEntry.Token(aSeparator.ToCString(),1);
      anEntry.Remove( 1, 1 );
      anEntry.RightAdjust();
      anEntry.LeftAdjust();
      if(MYDEBUG)
        cout<<"Sub-entry : '" <<anEntry<<"'"<<endl;
    }

    //Find variables used for object construction
    ObjectStates* aStates = 0;
    TVariablesList::const_iterator it = theVariables.find(anEntry);
    if( it != theVariables.end() )
      aStates = (*it).second;

    if(!aStates) {
      if(MYDEBUG)
        cout<<"Valiables list empty!!!"<<endl;
      aCommandIndex++;
      continue;
    }

    TState aVariables = aStates->GetCurrectState();

    if(MYDEBUG) {
      cout<<"Variables from SObject:"<<endl;
      for (int i = 0; i < aVariables.size();i++)
        cout<<"\t Variable["<<i<<"] = "<<aVariables[i].myVariable<<endl;
    }

    //Calculate total number of parameters
    Standard_Integer aTotalNbParams = 1;
    while(aCommand.Location(aTotalNbParams,COMMA,1,aCommand.Length()))
      aTotalNbParams++;

    if(MYDEBUG)
      cout<<"aTotalNbParams = "<<aTotalNbParams<<endl;

    Standard_Integer aFirstParam = aNbEntries;

    //Replace parameters by variables
    Standard_Integer aStartPos = 0;
    Standard_Integer aEndPos = 0;
    int iVar = 0;
    TCollection_AsciiString aVar, aReplacedVar;
    for(Standard_Integer i=aFirstParam;i <= aTotalNbParams;i++) {
      //Replace first parameter (bettwen '(' character and first ',' character)
      if(i == aFirstParam)
      {
        aStartPos = aCommand.Location(O_BRACKET, 1, aCommand.Length()) + 1;
        if(aTotalNbParams - aNbEntries > 0 )
          aEndPos = aCommand.Location(aFirstParam, COMMA, 1, aCommand.Length());
        else
          aEndPos = aCommand.Location(C_BRACKET, 1, aCommand.Length());
      }
      //Replace last parameter (bettwen ',' character and ')' character)
      else if(i == aTotalNbParams)
      {
        aStartPos = aCommand.Location(i-1, COMMA, 1, aCommand.Length()) + 2;
        aEndPos = aCommand.Location(C_BRACKET, aStartPos , aCommand.Length());
      }
      //Replace other parameters (bettwen two ',' characters)
      else if(i != aFirstParam && i != aTotalNbParams )
      {
        aStartPos = aCommand.Location(i-1, COMMA, 1, aCommand.Length()) + 2;
        aEndPos = aCommand.Location(i, COMMA, 1, aCommand.Length());
      }

      if( aCommand.Value( aStartPos ) == O_SQR_BRACKET )
        aStartPos++;
      if( aCommand.Value( aEndPos-1 ) == C_SQR_BRACKET )
        aEndPos--;
      if ( aStartPos == aEndPos )
        continue; // PAL20889: for "[]"

      if(MYDEBUG)
        cout<<"aStartPos = "<<aStartPos<<", aEndPos = "<<aEndPos<<endl;

      aVar = aCommand.SubString(aStartPos, aEndPos-1);
      aVar.RightAdjust();
      aVar.LeftAdjust();

      if(MYDEBUG)
        cout<<"Variable: '"<< aVar <<"'"<<endl;

      // specific case for sketcher
      if(aVar.Location( TCollection_AsciiString("Sketcher:"), 1, aVar.Length() ) != 0) {
        Standard_Integer aNbSections = 1;
        while( aVar.Location( aNbSections, ':', 1, aVar.Length() ) )
          aNbSections++;
        aNbSections--;

        int aStartSectionPos = 0, aEndSectionPos = 0;
        TCollection_AsciiString aSection, aReplacedSection;
        for(Standard_Integer aSectionIndex = 1; aSectionIndex <= aNbSections; aSectionIndex++) {
          aStartSectionPos = aVar.Location( aSectionIndex, ':', 1, aVar.Length() ) + 1;
          if( aSectionIndex != aNbSections )
            aEndSectionPos = aVar.Location( aSectionIndex + 1, ':', 1, aVar.Length() );
          else
            aEndSectionPos = aVar.Length();

          aSection = aVar.SubString(aStartSectionPos, aEndSectionPos-1);
          if(MYDEBUG)
            cout<<"aSection: "<<aSection<<endl;

          Standard_Integer aNbParams = 1;
          while( aSection.Location( aNbParams, ' ', 1, aSection.Length() ) )
            aNbParams++;
          aNbParams--;

          int aStartParamPos = 0, aEndParamPos = 0;
          TCollection_AsciiString aParameter, aReplacedParameter;
          for(Standard_Integer aParamIndex = 1; aParamIndex <= aNbParams; aParamIndex++) {
            aStartParamPos = aSection.Location( aParamIndex, ' ', 1, aSection.Length() ) + 1;
            if( aParamIndex != aNbParams )
              aEndParamPos = aSection.Location( aParamIndex + 1, ' ', 1, aSection.Length() );
            else
              aEndParamPos = aSection.Length() + 1;

            if(MYDEBUG)
              cout<<"aParamIndex: "<<aParamIndex<<" aStartParamPos: " <<aStartParamPos<<" aEndParamPos: "<<aEndParamPos<<endl;

            if ( aStartParamPos == aEndParamPos)
              continue;

            aParameter = aSection.SubString(aStartParamPos, aEndParamPos-1);
            if(MYDEBUG)
              cout<<"aParameter: "<<aParameter<<endl;

            if(iVar >= aVariables.size())
              continue;

            aReplacedParameter = aVariables[iVar].myVariable;
            if(aReplacedParameter.IsEmpty()) {
              iVar++;
              continue;
            }

            if(aVariables[iVar].isVariable) {
              aReplacedParameter.InsertBefore(1,"'");
              aReplacedParameter.InsertAfter(aReplacedParameter.Length(),"'");
            }

            if(MYDEBUG)
              cout<<"aSection before : "<<aSection<<endl;
            aSection.Remove(aStartParamPos, aEndParamPos - aStartParamPos);
            aSection.Insert(aStartParamPos, aReplacedParameter);
            if(MYDEBUG)
              cout<<"aSection after  : "<<aSection<<endl<<endl;
            iVar++;
          }
          if(MYDEBUG)
            cout<<"aVar before : "<<aVar<<endl;
          aVar.Remove(aStartSectionPos, aEndSectionPos - aStartSectionPos);
          aVar.Insert(aStartSectionPos, aSection);
          if(MYDEBUG)
            cout<<"aVar after  : "<<aVar<<endl<<endl;
        }

        if(MYDEBUG)
          cout<<"aCommand before : "<<aCommand<<endl;
        aCommand.Remove(aStartPos, aEndPos - aStartPos);
        aCommand.Insert(aStartPos, aVar);
        if(MYDEBUG)
          cout<<"aCommand after  : "<<aCommand<<endl;

        break;
      } // end of specific case for sketcher

      //If parameter is entry or 'None', skip it
      if(theVariables.find(aVar) != theVariables.end() || aVar.Search(":") != -1 || aVar == PY_NULL)
        continue;

      if(iVar >= aVariables.size())
        continue;

      aReplacedVar = aVariables[iVar].myVariable;
      if(aReplacedVar.IsEmpty()) {
        iVar++;
        continue;
      }

      if(aVariables[iVar].isVariable) {
        aReplacedVar.InsertBefore(1,"\"");
        aReplacedVar.InsertAfter(aReplacedVar.Length(),"\"");
      }

      aCommand.Remove(aStartPos, aEndPos - aStartPos);
      aCommand.Insert(aStartPos, aReplacedVar);
      iVar++;
    }

    theCommand.Remove(aStartCommandPos, aEndCommandPos - aStartCommandPos);
    theCommand.Insert(aStartCommandPos, aCommand);

    aCommandIndex++;

    aStates->IncrementState();
  }

  if (MYDEBUG)
    cout<<"Command : "<<theCommand<<endl;
}

//=============================================================================
/*!
 *  ReplaceEntriesByNames: Replace object entries by their names
 */
//=============================================================================
void ReplaceEntriesByNames (TCollection_AsciiString&                  theScript,
                            TSting2ObjDataMap&                        aEntry2ObjData,
                            const bool                                theIsPublished,
                            TColStd_SequenceOfAsciiString&            theObjListToPublish,
                            Standard_Integer&                         objectCounter,
                            Resource_DataMapOfAsciiStringAsciiString& aNameToEntry)
{
  GEOM_Engine* engine = GEOM_Engine::GetEngine();
  Handle(TColStd_HSequenceOfInteger) aSeq = FindEntries(theScript);
  Standard_Integer aLen = aSeq->Length(), aStart = 1, aScriptLength = theScript.Length();

  //Replace entries by the names
  TCollection_AsciiString anUpdatedScript, anEntry, aName, aBaseName("geomObj_"),
    allowedChars ("qwertyuioplkjhgfdsazxcvbnmQWERTYUIOPLKJHGFDSAZXCVBNM0987654321_");
  if (aLen == 0) anUpdatedScript = theScript;

  for (Standard_Integer i = 1; i <= aLen; i+=2) {
    anUpdatedScript += theScript.SubString(aStart, aSeq->Value(i)-1);
    anEntry = theScript.SubString(aSeq->Value(i), aSeq->Value(i+1));
    theObjListToPublish.Append( anEntry );
    
    TObjectData& data = aEntry2ObjData[ anEntry ];
    if ( data._pyName.IsEmpty() ) { // encounted for the 1st time
      if ( !data._name.IsEmpty() ) { // published object
        data._pyName = data._name;
        engine->healPyName( data._pyName, anEntry, aNameToEntry);
      }
      else {
        do {
          data._pyName = aBaseName + TCollection_AsciiString(++objectCounter);
        } while(aNameToEntry.IsBound(data._pyName));
      }
    }
    
    aNameToEntry.Bind(data._pyName, anEntry); // to detect same name of diff objects
    
    anUpdatedScript += data._pyName;
    aStart = aSeq->Value(i+1) + 1;
  }
  
  //Add final part of the script
  if (aLen && aSeq->Value(aLen) < aScriptLength)
    anUpdatedScript += theScript.SubString(aSeq->Value(aLen)+1, aScriptLength); // mkr : IPAL11865
  
  theScript = anUpdatedScript;
}

//=============================================================================
/*!
 *  AddObjectColors: Add color to objects
 */
//=============================================================================
void AddObjectColors (int                      theDocID,
                      TCollection_AsciiString& theScript,
                      const TSting2ObjDataMap& theEntry2ObjData)
{
  GEOM_Engine* engine = GEOM_Engine::GetEngine();
  Handle(TDocStd_Document) aDoc = engine->GetDocument(theDocID);

  TSting2ObjDataMap::const_iterator anEntryToNameIt;
  for (anEntryToNameIt = theEntry2ObjData.begin();
       anEntryToNameIt!= theEntry2ObjData.end();
       ++anEntryToNameIt)
  {
    const TCollection_AsciiString& aEntry = anEntryToNameIt->first;
    const TCollection_AsciiString& aName = anEntryToNameIt->second._pyName;

    TDF_Label L;
    TDF_Tool::Label( aDoc->GetData(), aEntry, L );
    if ( L.IsNull() )
      continue;

    Handle(GEOM_Object) obj = GEOM_Object::GetObject( L );
    if ( obj.IsNull() )
      continue;

    bool anAutoColor = obj->GetAutoColor();
    if ( anAutoColor )
    {
      TCollection_AsciiString aCommand( "\n\t" );
      aCommand += aName + ".SetAutoColor(1)";
      theScript += aCommand.ToCString();
    }

    GEOM_Object::Color aColor = obj->GetColor();
    if ( aColor.R >= 0 && aColor.G >= 0 && aColor.B >= 0 )
    {
      TCollection_AsciiString aCommand( "\n\t" );
      aCommand += aName + ".SetColor(SALOMEDS.Color(" + aColor.R + "," + aColor.G + "," + aColor.B + "))";
      theScript += aCommand.ToCString();
    }

    Aspect_TypeOfMarker aMarkerType = obj->GetMarkerType();
    if (aMarkerType >= Aspect_TOM_POINT && aMarkerType < Aspect_TOM_USERDEFINED) {
      TCollection_AsciiString aCommand( "\n\t" );
      aCommand += aName + ".SetMarkerStd(";
      switch (aMarkerType) {
      case Aspect_TOM_POINT:   aCommand += "GEOM.MT_POINT";   break;
      case Aspect_TOM_PLUS:    aCommand += "GEOM.MT_PLUS";    break;
      case Aspect_TOM_STAR:    aCommand += "GEOM.MT_STAR";    break;
      case Aspect_TOM_X:       aCommand += "GEOM.MT_X";       break;
      case Aspect_TOM_O:       aCommand += "GEOM.MT_O";       break;
      case Aspect_TOM_O_POINT: aCommand += "GEOM.MT_O_POINT"; break;
      case Aspect_TOM_O_PLUS:  aCommand += "GEOM.MT_O_PLUS";  break;
      case Aspect_TOM_O_STAR:  aCommand += "GEOM.MT_O_STAR";  break;
      case Aspect_TOM_O_X:     aCommand += "GEOM.MT_O_X";     break;
      case Aspect_TOM_RING1:   aCommand += "GEOM.MT_RING1";   break;
      case Aspect_TOM_RING2:   aCommand += "GEOM.MT_RING2";   break;
      case Aspect_TOM_RING3:   aCommand += "GEOM.MT_RING3";   break;
      case Aspect_TOM_BALL:    aCommand += "GEOM.MT_BALL";    break;
      default:                 aCommand += "GEOM.MT_NONE";    break; // just for completeness, should not get here
      }
      aCommand += ", ";
      int aSize = (int)( obj->GetMarkerSize()/0.5 ) - 1;
      switch (aSize) {
      case  1: aCommand += "GEOM.MS_10";   break;
      case  2: aCommand += "GEOM.MS_15";   break;
      case  3: aCommand += "GEOM.MS_20";   break;
      case  4: aCommand += "GEOM.MS_25";   break;
      case  5: aCommand += "GEOM.MS_30";   break;
      case  6: aCommand += "GEOM.MS_35";   break;
      case  7: aCommand += "GEOM.MS_40";   break;
      case  8: aCommand += "GEOM.MS_45";   break;
      case  9: aCommand += "GEOM.MS_50";   break;
      case 10: aCommand += "GEOM.MS_55";   break;
      case 11: aCommand += "GEOM.MS_60";   break;
      case 12: aCommand += "GEOM.MS_65";   break;
      case 13: aCommand += "GEOM.MS_70";   break;
      default: aCommand += "GEOM.MS_NONE"; break;
      }
      aCommand += ")";
      theScript += aCommand.ToCString();
    }
    else if (aMarkerType == Aspect_TOM_USERDEFINED) {
      int aMarkerTextureID = obj->GetMarkerTexture();
      if (aMarkerTextureID >= 0) {
        TCollection_AsciiString aCommand( "\n\t" );
        aCommand += aName + ".SetMarkerTexture(texture_map[";
        aCommand += aMarkerTextureID;
        aCommand += "])";
        theScript += aCommand.ToCString();
      }
    }
  }
}

static TCollection_AsciiString pack_data (const Handle(TColStd_HArray1OfByte)& aData)
{
  TCollection_AsciiString stream;
  if (!aData.IsNull()) {
    for (Standard_Integer i = aData->Lower(); i <= aData->Upper(); i++) {
      Standard_Byte byte = aData->Value(i);
      TCollection_AsciiString strByte = "";
      for (int j = 0; j < 8; j++)
        strByte.Prepend((byte & (1<<j)) ? "1" : "0");
      stream += strByte;
    }
  }
  return stream;
}

void AddTextures (int theDocID, TCollection_AsciiString& theScript)
{
  GEOM_Engine* engine = GEOM_Engine::GetEngine();
  std::list<int> allTextures = engine->getAllTextures(theDocID);
  std::list<int>::const_iterator it;

  if (allTextures.size() > 0) {
    theScript += "\n\ttexture_map = {}\n";

    for (it = allTextures.begin(); it != allTextures.end(); ++it) {
      if (*it <= 0) continue;
      Standard_Integer aWidth, aHeight;
      TCollection_AsciiString aFileName;
      Handle(TColStd_HArray1OfByte) aTexture =
        engine->getTexture(theDocID, *it, aWidth, aHeight, aFileName);
      if (aWidth > 0 && aHeight > 0 && !aTexture.IsNull() && aTexture->Length() > 0 ) {
        TCollection_AsciiString aCommand = "\n\t";
        aCommand += "texture_map["; aCommand += *it; aCommand += "] = ";
        if (aFileName != "" ) {
          aCommand += "geompy.LoadTexture(\"";
          aCommand += aFileName.ToCString();
          aCommand += "\")";
        }
        else {
          aCommand += "geompy.AddTexture(";
          aCommand += aWidth; aCommand += ", "; aCommand += aHeight; aCommand += ", \"";
          aCommand += pack_data(aTexture);
          aCommand += "\")";
        }
        theScript += aCommand;
      }
    }
    theScript += "\n";
  }
}

//=============================================================================
/*!
 *  PublishObject: publish object in study script
 */
//=============================================================================
void PublishObject (TObjectData&                              theObjectData,
                    TSting2ObjDataMap&                        theEntry2ObjData,
                    const TSting2ObjDataPtrMap&               theStEntry2ObjDataPtr,
                    Resource_DataMapOfAsciiStringAsciiString& theNameToEntry,
                    std::map< int, TCollection_AsciiString >& theEntryToCmdMap,
                    std::set< TCollection_AsciiString>&       theIgnoreMap)
{
  GEOM_Engine* engine = GEOM_Engine::GetEngine();
  if ( theObjectData._studyEntry.IsEmpty() )
    return; // was not published
  if ( theIgnoreMap.count( theObjectData._entry ) )
    return; // not to publish

  TCollection_AsciiString aCommand("\n\tgeompy.");

  // find a father entry
  TObjectData* aFatherData = 0;
  TCollection_AsciiString aFatherStudyEntry =
    theObjectData._studyEntry.SubString( 1, theObjectData._studyEntry.SearchFromEnd(":") - 1 );
  TSting2ObjDataPtrMap::const_iterator stEntry2DataPtr =
    theStEntry2ObjDataPtr.find( aFatherStudyEntry );
  if ( stEntry2DataPtr != theStEntry2ObjDataPtr.end() )
    aFatherData = stEntry2DataPtr->second;

  // treat multiply published object
  if ( theObjectData._pyName.IsEmpty() )
  {
    TObjectData& data0 = theEntry2ObjData[ theObjectData._entry ];
    if ( data0._pyName.IsEmpty() ) return; // something wrong

    theObjectData._pyName = theObjectData._name;
    engine->healPyName( theObjectData._pyName, theObjectData._entry, theNameToEntry);

    TCollection_AsciiString aCreationCommand("\n\t");
    aCreationCommand += theObjectData._pyName + " = " + data0._pyName;

    // store aCreationCommand before publishing commands
    int tag = GetTag(theObjectData._entry);
    theEntryToCmdMap.insert( std::make_pair( tag + 2*theEntry2ObjData.size(), aCreationCommand ));
  }

  // make a command
  if ( aFatherData && !aFatherData->_pyName.IsEmpty() ) {
    aCommand += "addToStudyInFather( ";
    aCommand += aFatherData->_pyName + ", ";
  }
  else {
    aCommand += "addToStudy( ";
  }
  aCommand += theObjectData._pyName + ", '" + theObjectData._name + "' )";

  // bind a command to the study entry
  int tag = GetTag(theObjectData._entry);
  theEntryToCmdMap.insert( std::make_pair( tag, aCommand ));

  theObjectData._studyEntry.Clear(); // not to publish any more
}

//================================================================================
/*!
 * \brief Returns the string of publishing commands. Take into account that
 *  references should be published prior to the objects refer to them.
 */
//================================================================================
TCollection_AsciiString GetPublishCommands
                   (const int                                       theTag,
                    const std::map< int, TCollection_AsciiString > &theEntryToCmdMap,
                    const TIntToListIntMap                         &theMapRefs,
                          std::set< int >                          &thePublished)
{
  TCollection_AsciiString aResult;

  if (!thePublished.count(theTag)) {
    // This object is not published yet.
    std::map< int, TCollection_AsciiString >::const_iterator anIt =
      theEntryToCmdMap.find(theTag);

    if (anIt != theEntryToCmdMap.end()) {
      // There is a pubish cmd.
      TIntToListIntMap::const_iterator aRefIt = theMapRefs.find(theTag);

      if (aRefIt != theMapRefs.end()) {
        // Recursively publish all references.
        std::list< int >::const_iterator aRefTagIt = aRefIt->second.begin();

        for(; aRefTagIt != aRefIt->second.end(); ++aRefTagIt) {
          const TCollection_AsciiString aRefCmd = GetPublishCommands
            (*aRefTagIt, theEntryToCmdMap, theMapRefs, thePublished);

          aResult += aRefCmd;
        }
      }

      // Add the object command.
      aResult += anIt->second;
    }

    thePublished.insert(theTag);
  }

  return aResult;
}

//================================================================================
/*!
 * \brief Constructor
 */
//================================================================================
ObjectStates::ObjectStates()
{
  _dumpstate = 0;
}

//================================================================================
/*!
 * \brief Destructor
 */
//================================================================================
ObjectStates::~ObjectStates()
{
}

//================================================================================
/*!
 * \brief Return current object state
 * \retval state - Object state (vector of notebook variable)
 */
//================================================================================
TState ObjectStates::GetCurrectState() const
{
  if(_states.size() > _dumpstate)
    return _states[_dumpstate];
  return TState();
}

//================================================================================
/*!
 * \brief Add new object state
 * \param theState - Object state (vector of notebook variable)
 */
//================================================================================
void ObjectStates::AddState(const TState &theState)
{
  _states.push_back(theState);
}

//================================================================================
/*!
 * \brief Increment object state
 */
//================================================================================
void ObjectStates::IncrementState()
{
  _dumpstate++;
}
