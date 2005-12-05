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
// See http://www.salome-platform.org/
//
#include <Standard_OStream.hxx>

#include <GEOM_Object_i.hh>
//#include <GEOM_Gen_i.hh>
#include <GEOM_ISubShape.hxx>
#include <GEOMImpl_Types.hxx>

#include "utilities.h"
#include <fstream>
#include <strstream>

#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <TCollection_AsciiString.hxx>

#include <BRepTools_ShapeSet.hxx>
#include <BRepTools.hxx>
#include <TopAbs.hxx>

#ifdef WNT
#pragma warning( disable:4786 )
#endif

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================

GEOM_Object_i::GEOM_Object_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
			      Handle(GEOM_Object) theImpl)
: SALOME::GenericObj_i( thePOA ), _engine(theEngine), _impl(theImpl)
{
  thePOA->activate_object(this);
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================

GEOM_Object_i::~GEOM_Object_i()
{}


//=============================================================================
/*!
 *  GetEntry
 */
//=============================================================================
char* GEOM_Object_i::GetEntry()
{
  const TDF_Label& aLabel = _impl->GetEntry();
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(aLabel, anEntry);
  return CORBA::string_dup(anEntry.ToCString());
}

//=============================================================================
/*!
 *  GetStudyID
 */
//=============================================================================
CORBA::Long GEOM_Object_i::GetStudyID()
{
   return _impl->GetDocID();
}


//=============================================================================
/*!
 *  GetType
 */
//=============================================================================
CORBA::Long GEOM_Object_i::GetType()
{
  return _impl->GetType();
}

//=============================================================================
/*!
 *  GetShapeType
 */
//=============================================================================
GEOM::shape_type GEOM_Object_i::GetShapeType()
{
  TopoDS_Shape _geom = _impl->GetValue();
  if(_geom.IsNull()) return GEOM::SHAPE;
  return (GEOM::shape_type)_geom.ShapeType();
}

//=============================================================================
/*!
 *  SetName
 */
//=============================================================================
void GEOM_Object_i::SetName(const char* theName)
{
  _impl->SetName(theName);
}


//=============================================================================
/*!
 *  GetName
 */
//=============================================================================
char* GEOM_Object_i::GetName()
{
  char* aName = _impl->GetName();
  if(aName) return strdup(aName);
  return strdup("");
}

//=============================================================================
/*!
 *  SetStudyEntry
 */
//=============================================================================
void GEOM_Object_i::SetStudyEntry(const char* theEntry)
{
  _impl->SetAuxData(theEntry);
}


//=============================================================================
/*!
 *  GetStudyEntry
 */
//=============================================================================
char* GEOM_Object_i::GetStudyEntry()
{
  TCollection_AsciiString anEntry = _impl->GetAuxData();
  if(!anEntry.IsEmpty()) return CORBA::string_dup(anEntry.ToCString());
  return CORBA::string_dup("");
}


//=============================================================================
/*!
 *  GetDependency
 */
//=============================================================================
GEOM::ListOfGO* GEOM_Object_i::GetDependency()
{
  GEOM::ListOfGO_var aList = new GEOM::ListOfGO();
  aList->length(0);

  Handle(TColStd_HSequenceOfTransient) aSeq = _impl->GetAllDependency();
  if (aSeq.IsNull()) return aList._retn();
  int aLength = aSeq->Length();
  if (aLength == 0) return aList._retn();

  aList->length(aLength);

  TCollection_AsciiString anEntry;

  for (int i = 1; i<=aLength; i++) {
    Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast(aSeq->Value(i));
    if (anObj.IsNull()) continue;
    TDF_Tool::Entry(anObj->GetEntry(), anEntry);
    GEOM::GEOM_Object_var obj = GEOM::GEOM_Object::_duplicate(_engine->GetObject(anObj->GetDocID(), anEntry.ToCString()));
    aList[i-1] = obj;
  }

  return aList._retn();
}

//=============================================================================
/*!
 * GetLastDependency
 */
//=============================================================================
GEOM::ListOfGO* GEOM_Object_i::GetLastDependency()
{
  GEOM::ListOfGO_var aList = new GEOM::ListOfGO();
  aList->length(0);

  Handle(TColStd_HSequenceOfTransient) aSeq = _impl->GetLastDependency();
  if (aSeq.IsNull()) return aList._retn();
  int aLength = aSeq->Length();
  if (aLength == 0) return aList._retn();

  aList->length(aLength);

  TCollection_AsciiString anEntry;

  for (int i = 1; i<=aLength; i++) {
     Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast(aSeq->Value(i));
     if (anObj.IsNull()) continue;
     TDF_Tool::Entry(anObj->GetEntry(), anEntry);
     GEOM::GEOM_Object_var obj = GEOM::GEOM_Object::_duplicate(_engine->GetObject(anObj->GetDocID(), anEntry.ToCString()));
     aList[i-1] = obj;
  }

  return aList._retn();
}

//=================================================================================
// function : GetShapeStream
// Transfer resulting shape to client as sequence of bytes
//client can extract shape from stream using BrepTools::Read function
//=================================================================================
SALOMEDS::TMPFile* GEOM_Object_i::GetShapeStream()
{
  TopoDS_Shape aShape = _impl->GetValue();

  if(aShape.IsNull()) return NULL;

  ostrstream streamShape;
  //Write TopoDS_Shape in ASCII format to the stream
  BRepTools::Write(aShape, streamShape);
  //Returns the number of bytes that have been stored in the stream's buffer.
  int size = streamShape.pcount();
  char* buf = new char [size];
  //Get pointer on internal character array in ostrstream
  char* valueOfStream = streamShape.str();
  //Create copy of ostrstream content
  memcpy(buf, valueOfStream, size);
  //Allow automatic deletion of ostrstream content
  streamShape.rdbuf()->freeze(0);

  CORBA::Octet* OctetBuf =  (CORBA::Octet*)buf;
  SALOMEDS::TMPFile_var SeqFile = new SALOMEDS::TMPFile(size,size,OctetBuf,1);
  return SeqFile._retn();

}


//=======================================================================
//function : getShape
//purpose  : return the TopoDS_Shape when client and servant are colocated, be careful
//=======================================================================
long GEOM_Object_i::getShape() {
  _geom = _impl->GetValue();
  return((long)(&_geom));
}

//=============================================================================
/*!
 *  GetSubShapeIndices
 */
//=============================================================================
GEOM::ListOfLong* GEOM_Object_i::GetSubShapeIndices()
{
  GEOM::ListOfLong_var anIndices = new GEOM::ListOfLong;

  if(!_impl->IsMainShape()) {
    Handle(GEOM_Function) aFunction = _impl->GetFunction(1); //Get SubShape function (always the first (and last)  one)
    if(aFunction.IsNull()) return anIndices._retn();
    GEOM_ISubShape ISS(aFunction);
    Handle(TColStd_HArray1OfInteger) anArray = ISS.GetIndices();
    if(anArray.IsNull() || anArray->Length() < 1) return anIndices._retn();
    anIndices->length(anArray->Length());
    for(int i=1; i<=anArray->Length(); i++) anIndices[i-1] = anArray->Value(i);
  }
  else {
    anIndices->length(0);
  }

  return anIndices._retn();
}


//=============================================================================
/*!
 *  GetMainShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Object_i::GetMainShape()
{
  GEOM::GEOM_Object_var obj;
  if(!_impl->IsMainShape()) {
    Handle(GEOM_Function) aFunction = _impl->GetFunction(1); //Get SubShape function (always the first (and last)  one)
    if(aFunction.IsNull()) return obj._retn();
    GEOM_ISubShape ISS(aFunction);

    aFunction = ISS.GetMainShape();
    if(aFunction.IsNull()) return obj._retn();
    TDF_Label aLabel  = aFunction->GetOwnerEntry();
    if(aLabel.IsNull()) return obj._retn();
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aLabel, anEntry);
    return GEOM::GEOM_Object::_duplicate(_engine->GetObject(_impl->GetDocID(), anEntry.ToCString()));
  }

  return obj._retn();
}

bool GEOM_Object_i::IsShape()
{
  return !_impl->GetValue().IsNull() && _impl->GetType() != GEOM_MARKER;
}
