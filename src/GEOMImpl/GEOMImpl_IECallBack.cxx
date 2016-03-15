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
#include "GEOMImpl_IECallBack.hxx"
#include "GEOMImpl_Gen.hxx"

std::map<TCollection_AsciiString, GEOMImpl_IECallBack*> GEOMImpl_IECallBack::myCallBacks;

//=============================================================================
/*!
 *  constructor
 */
//=============================================================================
GEOMImpl_IECallBack::GEOMImpl_IECallBack()
{
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_IECallBack::~GEOMImpl_IECallBack()
{
}

//=============================================================================
/*!
 *  GetEngine
 */
//=============================================================================
GEOMImpl_Gen* GEOMImpl_IECallBack::GetEngine()
{
  return dynamic_cast<GEOMImpl_Gen*>( GEOM_Engine::GetEngine() );
}

//=============================================================================
/*!
 *  Export
 */
//=============================================================================
bool GEOMImpl_IECallBack::Export( int                            /*theDocId*/,
				  const Handle(GEOM_Object)      /*theOriginal*/,
                                  const TCollection_AsciiString& /*theFileName*/,
                                  const TCollection_AsciiString& /*theFormatName*/ )
{
  return false;
}

//=============================================================================
/*!
 *  Import
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
GEOMImpl_IECallBack::Import( int                            /*theDocId*/,
			     const TCollection_AsciiString& /*theFormatName*/,
                             const TCollection_AsciiString& /*theFileName*/ )
{
  return NULL;
}

//=============================================================================
/*!
 *  ReadValue
 */
//=============================================================================
TCollection_AsciiString
GEOMImpl_IECallBack::ReadValue( int                            /*theDocId*/,
				const TCollection_AsciiString& /*theFileName*/,
                                const TCollection_AsciiString& /*theFormatName*/,
                                const TCollection_AsciiString& /*theParameterName*/ )
{
  return "";
}

//=============================================================================
/*!
 *  Register
 */
//=============================================================================
void GEOMImpl_IECallBack::Register( const TCollection_AsciiString& theFormatName,
                                    GEOMImpl_IECallBack* theCallBack )
{
  myCallBacks[theFormatName] = theCallBack;
}

//=============================================================================
/*!
 *  GetCallBack
 */
//=============================================================================
GEOMImpl_IECallBack* GEOMImpl_IECallBack::GetCallBack( const TCollection_AsciiString& theFormatName )
{
  return myCallBacks.find(theFormatName) == myCallBacks.end() ?
    new GEOMImpl_IECallBack() : myCallBacks[theFormatName];
}

