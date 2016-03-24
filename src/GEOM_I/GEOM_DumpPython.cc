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

#include <Standard_Stream.hxx>

#include "GEOM_Gen_i.hh"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <Resource_DataMapOfAsciiStringAsciiString.hxx>

#include <vector>
#include <string>

//=======================================================================
//function : RemoveTabulation
//purpose  : 
//=======================================================================
void RemoveTabulation( TCollection_AsciiString& theScript )
{
  std::string aString( theScript.ToCString() );
  std::string::size_type aPos = 0;
  while( aPos < aString.length() )
  {
    aPos = aString.find( "\n\t", aPos );
    if( aPos == std::string::npos )
      break;
    aString.replace( aPos, 2, "\n" );
    aPos++;
  }
  theScript = aString.c_str();
}

//=======================================================================
//function : ConvertV6toV7
//purpose  : dump elements are stored when study is saved,
//           and kept when study is reloaded.
//           When a study from an older version is loaded,
//           the dump must be post processed in case of syntax modification.
//           In V7.2, geompy.GEOM --> GEOM
//=======================================================================
void ConvertV6toV7( TCollection_AsciiString& theScript )
{
  std::string aString( theScript.ToCString() );
  std::string::size_type aPos = 0;
  while( aPos < aString.length() )
  {
    aPos = aString.find( "geompy.GEOM", aPos );
    if( aPos == std::string::npos )
      break;
    aString.replace( aPos, 11, "GEOM" );
    aPos++;
  }
  theScript = aString.c_str();
}

//=======================================================================
//function : DumpPython
//purpose  : 
//=======================================================================

Engines::TMPFile* GEOM_Gen_i::DumpPython(CORBA::Object_ptr theStudy, 
					 CORBA::Boolean isPublished, 
					 CORBA::Boolean isMultiFile,
                                         CORBA::Boolean& isValidScript)
{
  SALOMEDS::Study_var aStudy = SALOMEDS::Study::_narrow(theStudy);
  if(CORBA::is_nil(aStudy))
    return new Engines::TMPFile(0);   

  SALOMEDS::SObject_var aSO = aStudy->FindComponent(ComponentDataType());
  if(CORBA::is_nil(aSO))
    return new Engines::TMPFile(0);  

  TObjectData objData;
  std::vector<TObjectData> objectDataVec;

  TVariablesList aVariableMap;

  SALOMEDS::ChildIterator_var Itr = aStudy->NewChildIterator(aSO);
  for(Itr->InitEx(true); Itr->More(); Itr->Next()) {
    SALOMEDS::SObject_var aValue = Itr->Value();
    CORBA::String_var IOR = aValue->GetIOR();
    if(strlen(IOR.in()) > 0) {
      CORBA::Object_var obj = _orb->string_to_object(IOR);
      GEOM::GEOM_BaseObject_var GO = GEOM::GEOM_BaseObject::_narrow(obj);
      if(!CORBA::is_nil(GO)) {
        CORBA::String_var aName       = aValue->GetName();
        CORBA::String_var anEntry     = GO->GetEntry();
        CORBA::String_var aStudyEntry = aValue->GetID();
        objData._name       = aName.in();
        objData._entry      = anEntry.in();
        objData._studyEntry = aStudyEntry.in();

	//Find Drawable Attribute
	SALOMEDS::GenericAttribute_var aGenAttr;
	if(aValue->FindAttribute(aGenAttr, "AttributeDrawable") ) {
	  SALOMEDS::AttributeDrawable_var aDrw = SALOMEDS::AttributeDrawable::_narrow(aGenAttr);
	  objData._unpublished = !aDrw->IsDrawable();
	} else {
	  objData._unpublished = false;
	}

	objectDataVec.push_back( objData );

	//Find attribute with list of used notebook variables
	SALOMEDS::GenericAttribute_var anAttr;
	SALOMEDS::AttributeString_var anAttrStr;
	if(aValue->FindAttribute(anAttr,"AttributeString")){
	  anAttrStr = SALOMEDS::AttributeString::_narrow(anAttr);
	  SALOMEDS::ListOfListOfStrings_var aSections = aStudy->ParseVariables(anAttrStr->Value());
	  ObjectStates* aStates = new ObjectStates();
	  for(int i = 0; i < aSections->length(); i++) {
	    TState aState;
	    SALOMEDS::ListOfStrings aListOfVars = aSections[i];
	    for(int j = 0; j < aListOfVars.length(); j++) {
	      bool isVar = aStudy->IsVariable(aListOfVars[j].in());
	      TVariable aVar = TVariable( (char*)aListOfVars[j].in(), isVar );
	      aState.push_back(aVar);
	    }
	    aStates->AddState(aState);
	  }
	  aVariableMap.insert(std::make_pair(TCollection_AsciiString(anEntry),aStates));
	}
      }
    }
  }
  
  TCollection_AsciiString aScript;
  aScript += _impl->DumpPython(aStudy->StudyId(), objectDataVec, aVariableMap, isPublished, isMultiFile, isValidScript);

  if (isPublished)
  {

    SALOMEDS::AttributeParameter_var ap = aStudy->GetModuleParameters("Interface Applicative", 
								      ComponentDataType(),
								      -1);
    if(!CORBA::is_nil(ap)) {
      //Add the id parameter of the object
      std::vector<TObjectData>::iterator it = objectDataVec.begin();
      for( ;it != objectDataVec.end(); it++ ) {
      
	//1. Encode entry
        if ( (*it)._studyEntry.Length() < 7 ) continue;
	std::string tail( (*it)._studyEntry.ToCString(), 6, (*it)._studyEntry.Length()-1 );
	std::string newEntry(ComponentDataType());
	newEntry+=("_"+tail);
	
	CORBA::String_var anEntry = CORBA::string_dup(newEntry.c_str());
	
	if( ap->IsSet(anEntry, 6) ) { //6 Means string array, see SALOMEDS_Attributes.idl AttributeParameter interface
	  if ( GetDumpName((*it)._studyEntry.ToCString()) == NULL ) 
	    continue;
	  std::string idCommand = std::string("geompy.getObjectID(") + GetDumpName((*it)._studyEntry.ToCString()) + std::string(")");
	  SALOMEDS::StringSeq_var aSeq= ap->GetStrArray(anEntry);
	  int oldLenght = aSeq->length();	
	  aSeq->length(oldLenght+2);
	  aSeq[oldLenght] = CORBA::string_dup("_PT_OBJECT_ID_");
	  aSeq[oldLenght + 1] = CORBA::string_dup(idCommand.c_str());
	  ap->SetStrArray( anEntry, aSeq );
	}	 
      }
    }

    TCollection_AsciiString aDirScript, aNodeName, aNodeEntry, aFatherName, aFatherEntry;
    Resource_DataMapOfAsciiStringAsciiString aNameToEntry;
    SALOMEDS::UseCaseBuilder_var useCaseBuilder = aStudy->GetUseCaseBuilder();
    SALOMEDS::UseCaseIterator_var Itr = useCaseBuilder->GetUseCaseIterator(aSO);
    SALOMEDS::SObject_var aNodeSO;
    SALOMEDS::SObject_var aFatherSO;
    SALOMEDS::GenericAttribute_var anIDAttr;
    for(Itr->Init(true); Itr->More(); Itr->Next()) {
      aFatherName.Clear();
      aFatherEntry.Clear();
      aNodeSO = Itr->Value();
      // get father info
      aFatherSO = useCaseBuilder->GetFather( aNodeSO );
      if ( aFatherSO->FindAttribute(anIDAttr, "AttributeLocalID") ) {
	SALOMEDS::AttributeLocalID_var aLocalID = SALOMEDS::AttributeLocalID::_narrow(anIDAttr);
	if ( aLocalID->Value() == 999 ) {
	  aFatherName = aFatherSO->GetName();
	  aFatherEntry = aFatherSO->GetID();
	  _impl->healPyName( aFatherName, aFatherEntry, aNameToEntry);
	}
	aLocalID->UnRegister();
      }
      // get node info
      if ( aNodeSO->FindAttribute(anIDAttr, "AttributeLocalID") ) {
	SALOMEDS::AttributeLocalID_var aLocalID = SALOMEDS::AttributeLocalID::_narrow(anIDAttr);
	if ( aLocalID->Value() == 999 ) {
	  // the node is folder
	  aNodeName = aNodeSO->GetName();
	  aNodeEntry = aNodeSO->GetID();
	  _impl->healPyName( aNodeName, aNodeEntry, aNameToEntry);
	  aDirScript += aNodeName;
	  aDirScript += " = geompy.NewFolder('";
	  aDirScript += aNodeSO->GetName();
	  aDirScript += "'";
	  if ( !aFatherName.IsEmpty() && !aFatherEntry.IsEmpty() ) {
	    // the folder takes place under another folder
	    aDirScript += ", ";
	    aDirScript += aFatherName;
	  }
	  aDirScript += ")\n";
	  aNameToEntry.Bind( aNodeName, aNodeEntry );
	}
	aLocalID->UnRegister();
      } else if ( !aFatherName.IsEmpty() && !aFatherEntry.IsEmpty() ) {
	// the node is Geom object under folder
	aDirScript += "geompy.PutToFolder(";
	if ( GetDumpName( aNodeSO->GetID() ) == NULL )
	  continue;
	aDirScript += GetDumpName( aNodeSO->GetID() );
	aDirScript += ", ";
	aDirScript += aFatherName;
	aDirScript += ")\n";
      }
    }
    if ( !aDirScript.IsEmpty() )
    {
      aScript += "\n\t### Folders and it's content\n";
      aScript += aDirScript;
    }
    //Output the script that sets up the visual parameters.
    char* script = aStudy->GetDefaultScript(ComponentDataType(), "\t");
    if (script && strlen(script) > 0) {
      aScript += "\n\t### Store presentation parameters of displayed objects\n";
      aScript += script;
      CORBA::string_free(script);
    }
  }

  if( isMultiFile )
    aScript += "\n\tpass";
  aScript += "\n";

  if( !isMultiFile ) // remove unnecessary tabulation
    RemoveTabulation( aScript );

  ConvertV6toV7( aScript ); //convert scripts related to studies saved in SALOME V6 and older
  
  int aLen = aScript.Length(); 
  unsigned char* aBuffer = new unsigned char[aLen+1];
  strcpy((char*)aBuffer, aScript.ToCString());

  CORBA::Octet* anOctetBuf =  (CORBA::Octet*)aBuffer;
  Engines::TMPFile_var aStreamFile = new Engines::TMPFile(aLen+1, aLen+1, anOctetBuf, 1); 

  return aStreamFile._retn(); 
}

//=======================================================================
//function : GetDumpName
//purpose  : 
//=======================================================================

char* GEOM_Gen_i::GetDumpName (const char* theStudyEntry)
{
  const char* name = _impl->GetDumpName( theStudyEntry );
  if ( name && strlen( name ) > 0 )
    return strdup( name );

  return NULL;
}

//=======================================================================
//function : GetAllDumpNames
//purpose  : 
//=======================================================================

GEOM::string_array* GEOM_Gen_i::GetAllDumpNames()
{
  Handle(TColStd_HSequenceOfAsciiString) aHSeq = _impl->GetAllDumpNames();
  int i = 0, aLen = aHSeq->Length();

  GEOM::string_array_var seq = new GEOM::string_array();
  seq->length(aLen);

  for (; i < aLen; i++) {
    seq[i] = CORBA::string_dup(aHSeq->Value(i + 1).ToCString());
  }

  return seq._retn();
}
