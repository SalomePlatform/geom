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

#include "GEOM_Object_i.hh"

#include "GEOM_ISubShape.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_BaseDriver.hxx"

#include <utilities.h>
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>

#include <BRepTools.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <Standard_OStream.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Iterator.hxx>

#include <fstream>
#include <sstream>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

#ifdef _DEBUG_
#include <typeinfo>
#endif

#ifdef WIN32
#pragma warning( disable:4786 )
#endif

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================

GEOM_Object_i::GEOM_Object_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
			      HANDLE_NAMESPACE(GEOM_Object) theImpl) :
  SALOME::GenericObj_i( thePOA ),
  GEOM_BaseObject_i( thePOA, theEngine, theImpl ),
  _impl( theImpl )
{
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================

GEOM_Object_i::~GEOM_Object_i()
{
  //MESSAGE("GEOM_Object_i::~GEOM_Object_i");
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
 *  GetTopologyType
 */
//=============================================================================
GEOM::shape_type GEOM_Object_i::GetTopologyType()
{
  TopoDS_Shape shape = _impl->GetValue();
  if(shape.IsNull()) return GEOM::SHAPE;

  if ( shape.ShapeType() == TopAbs_COMPOUND || shape.ShapeType() == TopAbs_COMPSOLID ) {
    TopoDS_Shape shape_i;
    TopoDS_Iterator It (shape, Standard_True, Standard_False);
    for (; It.More(); It.Next()) {
      if ( !shape_i.IsNull() ) return (GEOM::shape_type)shape.ShapeType();
      shape_i = It.Value();
    }
    if ( !shape_i.IsNull() )
      return (GEOM::shape_type) shape_i.ShapeType();
  }

  return (GEOM::shape_type)shape.ShapeType();
}

static GEOM::shape_type getMinMaxShapeType( const TopoDS_Shape& shape, bool ismin )
{
  if ( shape.IsNull() )
    return GEOM::SHAPE;

  GEOM::shape_type ret = (GEOM::shape_type)shape.ShapeType();

  if ( shape.ShapeType() == TopAbs_COMPOUND || shape.ShapeType() == TopAbs_COMPSOLID ) {
    TopoDS_Iterator it(shape, Standard_True, Standard_False);
    for (; it.More(); it.Next()) {
      TopoDS_Shape sub_shape = it.Value();
      if ( sub_shape.IsNull() ) continue;
      GEOM::shape_type stype = (GEOM::shape_type)getMinMaxShapeType( sub_shape, ismin );
      if ( stype == GEOM::SHAPE ) continue;
      if ( ismin && stype > ret )
	ret = stype;
      else if ( !ismin && ( ret < GEOM::SOLID || stype < ret ) )
	ret = stype;
    }
  }

  return ret;
}

//=============================================================================
/*!
 *  GetMinShapeType
 */
//=============================================================================
GEOM::shape_type GEOM_Object_i::GetMinShapeType()
{
  return getMinMaxShapeType( _impl->GetValue(), true );
}

//=============================================================================
/*!
 *  GetMaxShapeType
 */
//=============================================================================
GEOM::shape_type GEOM_Object_i::GetMaxShapeType()
{
  return getMinMaxShapeType( _impl->GetValue(), false );
}

//================================================================================
/*!
 * GetSubShapeName
 */
//================================================================================

char* GEOM_Object_i::GetSubShapeName(CORBA::Long subID)
{
  CORBA::String_var name("");

  HANDLE_NAMESPACE(GEOM_Function) aMainFun = _impl->GetLastFunction();
  if ( aMainFun.IsNull() ) return name._retn();

  const TDataStd_ListOfExtendedString& aListEntries = aMainFun->GetSubShapeReferences();
  TDataStd_ListIteratorOfListOfExtendedString anIt( aListEntries );
  for (; anIt.More(); anIt.Next())
  {
    TCollection_AsciiString anEntry = anIt.Value();
    HANDLE_NAMESPACE(GEOM_BaseObject) anObj =
      GEOM_Engine::GetEngine()->GetObject( _impl->GetDocID(), anEntry.ToCString(), false );
    if ( anObj.IsNull() ) continue;

    TCollection_AsciiString aSubName = anObj->GetName();
    if ( aSubName.IsEmpty() ) continue;

    HANDLE_NAMESPACE(GEOM_Function) aFun = anObj->GetLastFunction();
    if ( aFun.IsNull() ) continue;
  
    GEOM_ISubShape ISS( aFun );
    Handle(TColStd_HArray1OfInteger) subIDs = ISS.GetIndices();
    if ( subIDs.IsNull() || subIDs->Length() != 1 ) continue;

    if ( subIDs->Value( subIDs->Lower() ) == subID )
    {
      name = aSubName.ToCString();
      break;
    }
  }
  return name._retn();
}

//=============================================================================
/*!
 *  SetColor
 */
//=============================================================================
void GEOM_Object_i::SetColor(const SALOMEDS::Color& theColor)
{
  ::GEOM_Object::Color aColor;
  aColor.R = theColor.R;
  aColor.G = theColor.G;
  aColor.B = theColor.B;
  _impl->SetColor(aColor);
}


//=============================================================================
/*!
 *  GetColor
 */
//=============================================================================
SALOMEDS::Color GEOM_Object_i::GetColor()
{
  SALOMEDS::Color aColor;
  aColor.R = _impl->GetColor().R;
  aColor.G = _impl->GetColor().G;
  aColor.B = _impl->GetColor().B;
  return aColor;
}


//=============================================================================
/*!
 *  SetAutoColor
 */
//=============================================================================
void GEOM_Object_i::SetAutoColor(CORBA::Boolean theAutoColor)
{
  _impl->SetAutoColor(theAutoColor);
}


//=============================================================================
/*!
 *  GetAutoColor
 */
//=============================================================================
CORBA::Boolean GEOM_Object_i::GetAutoColor()
{
  return _impl->GetAutoColor();
}


//=============================================================================
/*!
 *  SetMarkerStd
 */
//=============================================================================
void GEOM_Object_i::SetMarkerStd(GEOM::marker_type theType, GEOM::marker_size theSize)
{
  if ( theType == GEOM::MT_NONE || theSize == GEOM::MS_NONE ) {
    _impl->UnsetMarker();
  }
  else {
    Aspect_TypeOfMarker aType = (Aspect_TypeOfMarker)( (int)theType-1 );
    double aSize = ((int)theSize+1)*0.5;
    _impl->SetMarkerStd( aType, aSize );
  }
}


//=============================================================================
/*!
 *  SetMarkerTexture
 */
//=============================================================================
void GEOM_Object_i::SetMarkerTexture(CORBA::Long theTextureId)
{
  _impl->SetMarkerTexture( theTextureId );
}


//=============================================================================
/*!
 *  GetMarkerType
 */
//=============================================================================
GEOM::marker_type GEOM_Object_i::GetMarkerType()
{
  return (GEOM::marker_type)( (int)_impl->GetMarkerType()+1 );
}


//=============================================================================
/*!
 *  GetMarkerSize
 */
//=============================================================================
GEOM::marker_size GEOM_Object_i::GetMarkerSize()
{
  int aSize = (int)( _impl->GetMarkerSize()/0.5 ) - 1;
  return aSize < GEOM::MS_10 || aSize > GEOM::MS_70 ? GEOM::MS_NONE : (GEOM::marker_size)aSize;
}


//=============================================================================
/*!
 *  GetMarkerTexture
 */
//=============================================================================
CORBA::Long GEOM_Object_i::GetMarkerTexture()
{
  return _impl->GetMarkerTexture();
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

  std::ostringstream streamShape;
  //Write TopoDS_Shape in ASCII format to the stream
  BRepTools::Write(aShape, streamShape);
  //Returns the number of bytes that have been stored in the stream's buffer.
  int size = streamShape.str().size();
  //Allocate octect buffer of required size
  CORBA::Octet* OctetBuf = SALOMEDS::TMPFile::allocbuf(size);
  //Copy ostrstream content to the octect buffer
  memcpy(OctetBuf, streamShape.str().c_str(), size);
  //Create and return TMPFile
  SALOMEDS::TMPFile_var SeqFile = new SALOMEDS::TMPFile(size,size,OctetBuf,1);
  return SeqFile._retn();
}


//=======================================================================
//function : getShape
//purpose  : return the TopoDS_Shape when client and servant are colocated, be careful
//=======================================================================
CORBA::LongLong GEOM_Object_i::getShape() {
  _geom = _impl->GetValue();
  return ((CORBA::LongLong)(&_geom));
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
    HANDLE_NAMESPACE(GEOM_Function) aFunction = _impl->GetLastFunction(); //Get Sub-shape function (always the first (and last)  one)
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
    HANDLE_NAMESPACE(GEOM_Function) aFunction = _impl->GetFunction(1); //Get Sub-shape function (always the first (and last)  one)
    if(aFunction.IsNull()) return obj._retn();
    GEOM_ISubShape ISS(aFunction);

    aFunction = ISS.GetMainShape();
    if(aFunction.IsNull()) return obj._retn();
    TDF_Label aLabel  = aFunction->GetOwnerEntry();
    if(aLabel.IsNull()) return obj._retn();
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aLabel, anEntry);
    return GEOM::GEOM_Object::_narrow
      ( _engine->GetObject(_impl->GetDocID(), anEntry.ToCString()) );
  }

  return obj._retn();
}

bool GEOM_Object_i::IsShape()
{
  return !_impl->GetValue().IsNull() && _impl->GetType() != GEOM_MARKER;
}

bool GEOM_Object_i::IsSame(GEOM::GEOM_BaseObject_ptr other)
{
  bool result = false;

  GEOM::GEOM_Object_var shapePtr = GEOM::GEOM_Object::_narrow( other );
  if ( !CORBA::is_nil( shapePtr ) ) {
    CORBA::String_var entry = shapePtr->GetEntry();
    HANDLE_NAMESPACE(GEOM_Object) otherObject = HANDLE_NAMESPACE(GEOM_Object)::DownCast
      ( GEOM_Engine::GetEngine()->GetObject( shapePtr->GetStudyID(), entry, false ));
    if ( !otherObject.IsNull() ) {
      TopoDS_Shape thisShape  = _impl->GetValue();
      TopoDS_Shape otherShape = otherObject->GetValue();
      result = !thisShape.IsNull() && !otherShape.IsNull() && thisShape.IsSame( otherShape );
    }
  }
  return result;
}
