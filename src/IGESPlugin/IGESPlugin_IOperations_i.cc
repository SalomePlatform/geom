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
#include "IGESPlugin_IOperations_i.hh"
#include "IGESPlugin_IOperations.hxx"

// KERNEL includes
#include <utilities.h>

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
IGESPlugin_IOperations_i::IGESPlugin_IOperations_i( PortableServer::POA_ptr thePOA,
						    GEOM::GEOM_Gen_ptr theEngine,
						    IGESPlugin_IOperations* theImpl )
:GEOM_IOperations_i( thePOA, theEngine, theImpl )
{
  MESSAGE( "IGESPlugin_IOperations_i::IGESPlugin_IOperations_i" );
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
IGESPlugin_IOperations_i::~IGESPlugin_IOperations_i()
{
  MESSAGE( "IGESPlugin_IOperations_i::~IGESPlugin_IOperations_i" );
}

//=============================================================================
/*!
 *  ExportIGES
 *  Export a shape to IGES format
 *  \param theOriginal The shape to export
 *  \param theFileName The name of the exported file
 *  \param theVersion  The version of IGES format which defines, whether to write
 *                     only faces (5.1 IGES format) or shells and solids also (5.3 IGES format).
 */
//=============================================================================
void IGESPlugin_IOperations_i::ExportIGES( GEOM::GEOM_Object_ptr theOriginal,
					   const char*           theFileName,
					   const char*           theVersion )
{
  // duplicate the original shape
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate( theOriginal );

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) anOriginal = GetObjectImpl( theOriginal );
  if (anOriginal.IsNull()) return;

  GetOperations()->ExportIGES( anOriginal, theFileName, theVersion );
}

//=============================================================================
/*!
 *  ImportIGES
 *  Import a shape from IGES format
 *  \param theFileName The name of the file to import
 *  \param theIsIgnoreUnits If True, file length units will be ignored (set to 'meter')
 *                          and result model will be scaled, if its units are not meters.
 *                          If False (default), file length units will be taken into account.
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* IGESPlugin_IOperations_i::ImportIGES( const char* theFileName,
						      const bool  theIsIgnoreUnits = false )
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Import the shape from the file
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->ImportIGES( theFileName, theIsIgnoreUnits );

  if( !GetOperations()->IsDone() || aHSeq.IsNull() )
    return aSeq._retn();

  // Copy created objects.
  Standard_Integer aLength = aHSeq->Length();
  aSeq->length( aLength );
  for( Standard_Integer i = 1; i <= aLength; i++ )
    aSeq[i-1] = GetObject( Handle(GEOM_Object)::DownCast( aHSeq->Value(i) ) );

  return aSeq._retn();
}

//=============================================================================
/*!
 *  ReadValue
 */
//=============================================================================
char* IGESPlugin_IOperations_i::ReadValue( const char* theFileName,
					   const char* theParameterName )
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  TCollection_AsciiString aParam = GetOperations()->ReadValue( theFileName, theParameterName );

  return CORBA::string_dup( aParam.ToCString() );
}

IGESPlugin_IOperations* IGESPlugin_IOperations_i::GetOperations()
{
  return (IGESPlugin_IOperations*)GetImpl();
}
