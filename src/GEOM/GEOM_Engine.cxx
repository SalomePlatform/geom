// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifdef WNT
#pragma warning( disable:4786 )
#endif

#include "GEOM_Engine.hxx"

#include "GEOM_Solver.hxx"
#include "GEOM_Function.hxx"
#include "GEOM_ISubShape.hxx"
#include "GEOM_SubShapeDriver.hxx"
#include "GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient.hxx"
#include "GEOM_PythonDump.hxx"

#include "utilities.h"

#include <TDF_Tool.hxx>
#include <TDF_Data.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_DriverTable.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

#include <Interface_DataMapIteratorOfDataMapOfIntegerTransient.hxx>
#include <Resource_DataMapIteratorOfDataMapOfAsciiStringAsciiString.hxx>

#include <map>
#include <string>

#include <Standard_Failure.hxx>
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

void ProcessFunction(Handle(GEOM_Function)& theFunction, 
		     TCollection_AsciiString& theScript,
		     TColStd_MapOfTransient& theProcessed);

Handle(TColStd_HSequenceOfInteger) FindEntries(TCollection_AsciiString& theString);

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
Handle(GEOM_Object) GEOM_Engine::AddSubShape(Handle(GEOM_Object) theMainShape, 
					     Handle(TColStd_HArray1OfInteger) theIndices,
					     bool isStandaloneOperation)
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
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
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
bool GEOM_Engine::RemoveObject(Handle(GEOM_Object) theObject)
{
  if(!theObject) return false;

  //Remove an object from the map of available objects
  TCollection_AsciiString anID = BuildIDFromObject(theObject);
  if(_objects.IsBound(anID)) _objects.UnBind(anID);

  int nb = theObject->GetNbFunctions();
  Handle(TDataStd_TreeNode) aNode;
  for(int i = 1; i<=nb; i++) {
    Handle(GEOM_Function) aFunction = theObject->GetFunction(i);
    if(aFunction->GetEntry().FindAttribute(GEOM_Function::GetFunctionTreeID(), aNode)) 
      aNode->Remove();
  }

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
  if(_OCAFApp->Open(theFileName, aDoc) != CDF_RS_OK) {
    return false;
  }

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

//=============================================================================
/*!
 *  DumpPython
 */
//=============================================================================
TCollection_AsciiString GEOM_Engine::DumpPython(int theDocID, 
						Resource_DataMapOfAsciiStringAsciiString& theObjectNames,
						bool isPublished, 
						bool& aValidScript)
{
  TCollection_AsciiString aScript;
  Handle(TDocStd_Document) aDoc = GetDocument(theDocID);
  
  if(aDoc.IsNull()) return TCollection_AsciiString("def RebuildData(theStudy): pass\n");
 
  aScript = "import geompy\n";
  aScript += "import math\n\n";
  aScript += "def RebuildData(theStudy):";
  aScript += "\n\tgeompy.init_geom(theStudy)";
  
  Standard_Integer posToInertGlobalVars = aScript.Length() + 1;

  Handle(TDataStd_TreeNode) aNode, aRoot;
  Handle(GEOM_Function) aFunction;
  TColStd_MapOfTransient aMap;

  if(aDoc->Main().FindAttribute(GEOM_Function::GetFunctionTreeID(), aRoot)) {
    TDataStd_ChildNodeIterator Itr(aRoot);
    for(; Itr.More(); Itr.Next()) {
      aNode = Itr.Value();
      aFunction = GEOM_Function::GetFunction(aNode->Label());
      if(aFunction.IsNull()) {
	cout << "Null function !!!!" << endl;
	continue;
      }
      ProcessFunction(aFunction, aScript, aMap);
    }
  }

  Resource_DataMapOfAsciiStringAsciiString aEntry2StEntry, aStEntry2Entry;
  Resource_DataMapIteratorOfDataMapOfAsciiStringAsciiString anEntryToNameIt;
  // build maps entry <-> studyEntry
  for (anEntryToNameIt.Initialize( theObjectNames );
       anEntryToNameIt.More();
       anEntryToNameIt.Next())
  {
    const TCollection_AsciiString& aEntry = anEntryToNameIt.Key();
    // look for an object by entry
    TDF_Label L;
    TDF_Tool::Label( aDoc->GetData(), aEntry, L );
    if ( L.IsNull() ) continue;
    Handle(GEOM_Object) obj = GEOM_Object::GetObject( L );
    // fill maps
    if ( !obj.IsNull() ) {
      TCollection_AsciiString aStudyEntry (obj->GetAuxData());
      aEntry2StEntry.Bind( aEntry,  aStudyEntry);
      aStEntry2Entry.Bind( aStudyEntry, aEntry );
    }
  }

  Handle(TColStd_HSequenceOfInteger) aSeq = FindEntries(aScript);
  Standard_Integer aLen = aSeq->Length(), objectCounter = 0, aStart = 1, aScriptLength = aScript.Length();
  Resource_DataMapOfAsciiStringAsciiString aNameToEntry, anEntryToBadName;

  //Replace entries by the names
  TCollection_AsciiString anUpdatedScript, anEntry, aName, aBaseName("geomObj_"),
    allowedChars ("qwertyuioplkjhgfdsazxcvbnmQWERTYUIOPLKJHGFDSAZXCVBNM0987654321_");
  if(aLen == 0) anUpdatedScript = aScript;

  for(Standard_Integer i = 1; i <= aLen; i+=2) {
    anUpdatedScript += aScript.SubString(aStart, aSeq->Value(i)-1);
    anEntry = aScript.SubString(aSeq->Value(i), aSeq->Value(i+1));
    if(theObjectNames.IsBound(anEntry)) {
      aName = theObjectNames.Find(anEntry);
      // check validity of aName
      bool isValidName = true;
      if ( aName.IsIntegerValue() ) { // aName must not start with a digit
        aName.Insert( 1, 'a' );
        isValidName = false;
      }
      int p, p2=1; // replace not allowed chars
      while ((p = aName.FirstLocationNotInSet(allowedChars, p2, aName.Length()))) {
        aName.SetValue(p, '_');
        p2=p;
        isValidName = false;
      }
      if ( aNameToEntry.IsBound( aName ) && anEntry != aNameToEntry( aName ))
      {  // diff objects have same name - make a new name by appending a digit
        TCollection_AsciiString aName2;
        Standard_Integer i = 0;
        do {
          aName2 = aName + "_" + ++i;
        } while ( aNameToEntry.IsBound( aName2 ) && anEntry != aNameToEntry( aName2 ));
        aName = aName2;
        isValidName = false;
      }
      if ( !isValidName ) {
        if ( isPublished )
          anEntryToBadName.Bind( anEntry, theObjectNames.Find(anEntry) );
        theObjectNames( anEntry ) = aName;
      }
    }
    else {
      do {
        aName = aBaseName + TCollection_AsciiString(++objectCounter);
      } while(aNameToEntry.IsBound(aName));
      theObjectNames.Bind(anEntry, aName);
    }
    aNameToEntry.Bind(aName, anEntry); // to detect same name of diff objects

    anUpdatedScript += aName;
    aStart = aSeq->Value(i+1) + 1;
  }

  //Add final part of the script
  if(aLen && aSeq->Value(aLen) < aScriptLength)  anUpdatedScript += aScript.SubString(aSeq->Value(aLen)+1, aScriptLength); // mkr : IPAL11865
 
  // Make script to publish in study
  if ( isPublished )
  {
    map< int, string > anEntryToCommandMap; // sort publishing commands by object entry
    for (anEntryToNameIt.Initialize( theObjectNames );
         anEntryToNameIt.More();
         anEntryToNameIt.Next())
    {
      const TCollection_AsciiString& aEntry = anEntryToNameIt.Key();
      const TCollection_AsciiString& aName = anEntryToNameIt.Value();
      if ( !aEntry2StEntry.IsBound( aEntry ))
        continue; // was not published
      TCollection_AsciiString aCommand("\n\tgeompy."), aFatherEntry;

      // find a father entry
      const TCollection_AsciiString& aStudyEntry = aEntry2StEntry( aEntry );
      TCollection_AsciiString aFatherStudyEntry =
        aStudyEntry.SubString( 1, aStudyEntry.SearchFromEnd(":") - 1 );
      if ( aStEntry2Entry.IsBound( aFatherStudyEntry ))
        aFatherEntry = aStEntry2Entry( aFatherStudyEntry );

      // make a command
      if ( !aFatherEntry.IsEmpty() && theObjectNames.IsBound( aFatherEntry )) {
        aCommand += "addToStudyInFather( ";
        aCommand += theObjectNames( aFatherEntry ) + ", ";
      }
      else
        aCommand += "addToStudy( ";
      if ( anEntryToBadName.IsBound( aEntry ))
        aCommand += aName + ", \"" + anEntryToBadName( aEntry ) + "\" )";
      else 
        aCommand += aName + ", \"" + aName + "\" )";

      // bind a command to the last digit of the entry
      int tag =
        aEntry.SubString( aEntry.SearchFromEnd(":")+1, aEntry.Length() ).IntegerValue();
      anEntryToCommandMap.insert( make_pair( tag, aCommand.ToCString() ));
    }

    // add publishing commands to the script
    map< int, string >::iterator anEntryToCommand = anEntryToCommandMap.begin();
    for ( ; anEntryToCommand != anEntryToCommandMap.end(); ++anEntryToCommand ) {
      anUpdatedScript += (char*)anEntryToCommand->second.c_str();
    }
  }

  anUpdatedScript += "\n\tpass\n";
  aValidScript = true;

  // fill _studyEntry2NameMap and build globalVars
  TCollection_AsciiString globalVars;
  _studyEntry2NameMap.Clear();
  Resource_DataMapIteratorOfDataMapOfAsciiStringAsciiString aStEntryToEntryIt;
  for (aStEntryToEntryIt.Initialize( aStEntry2Entry );
       aStEntryToEntryIt.More();
       aStEntryToEntryIt.Next() )
  {
    const TCollection_AsciiString & name = theObjectNames( aStEntryToEntryIt.Value() );
    _studyEntry2NameMap.Bind (aStEntryToEntryIt.Key(), name );
    if ( !globalVars.IsEmpty() )
      globalVars += ", ";
    globalVars += name;
  }
  if ( !globalVars.IsEmpty() ) {
    globalVars.Insert( 1, "\n\tglobal " );
    anUpdatedScript.Insert( posToInertGlobalVars, globalVars );
  }
  
  return anUpdatedScript;
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


//===========================================================================
//                     Internal functions
//===========================================================================
void ProcessFunction(Handle(GEOM_Function)& theFunction, 
		     TCollection_AsciiString& theScript,
		     TColStd_MapOfTransient& theProcessed)
{
  if(theFunction.IsNull() || theProcessed.Contains(theFunction)) return;

/*
  TDF_LabelSequence aSeq;
  theFunction->GetDependency(aSeq);
  Standard_Integer aLen = aSeq.Length();
  for(Standard_Integer i = 1; i<= aLen; i++) {
    Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(aSeq.Value(i));
    if(aFunction.IsNull()) continue;
    ProcessFunction(aFunction, theScript, theProcessed);
  }
*/

  TCollection_AsciiString aDescr = theFunction->GetDescription();
  if(aDescr.Length() == 0) {
    //cout << "Warning: the function has no description" << endl;
    return;
  }
  //Check if its internal function which doesn't requires dumping
  if(aDescr == "None") return;

  theScript += "\n\t";
  theScript += aDescr;
 
  theProcessed.Add(theFunction);
  return;
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

  char* arr = theString.ToCString();
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
