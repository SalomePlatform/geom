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
#include "STEPPlugin_IOperations_i.hh"
#include "STEPPlugin_IOperations.hxx"

// KERNEL includes
#include <utilities.h>

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
STEPPlugin_IOperations_i::STEPPlugin_IOperations_i( PortableServer::POA_ptr thePOA,
						    GEOM::GEOM_Gen_ptr theEngine,
						    STEPPlugin_IOperations* theImpl )
:GEOM_IOperations_i( thePOA, theEngine, theImpl )
{
  MESSAGE( "STEPPlugin_IOperations_i::STEPPlugin_IOperations_i" );
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
STEPPlugin_IOperations_i::~STEPPlugin_IOperations_i()
{
  MESSAGE( "STEPPlugin_IOperations_i::~STEPPlugin_IOperations_i" );
}

//=============================================================================
/*!
 *  ExportSTEP
 *  Export a shape to STEP format
 *  \param theOriginal The shape to export
 *  \param theFileName The name of the exported file
 *  \param theUnit the length unit.
 */
//=============================================================================
void STEPPlugin_IOperations_i::ExportSTEP(GEOM::GEOM_Object_ptr theOriginal,
                                          const char*           theFileName,
                                          GEOM::length_unit     theUnit)
{
  // duplicate the original shape
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate( theOriginal );

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) anOriginal = GetObjectImpl( theOriginal );
  if (anOriginal.IsNull()) return;

  STEPPlugin_IOperations::LengthUnit aUnit;

  switch (theUnit) {
  case GEOM::LU_INCH:
    aUnit = STEPPlugin_IOperations::LengthUnit_Inch;
    break;
  case GEOM::LU_MILLIMETER:
    aUnit = STEPPlugin_IOperations::LengthUnit_Millimeter;
    break;
  case GEOM::LU_FOOT:
    aUnit = STEPPlugin_IOperations::LengthUnit_Foot;
    break;
  case GEOM::LU_MILE:
    aUnit = STEPPlugin_IOperations::LengthUnit_Mile;
    break;
  case GEOM::LU_METER:
    aUnit = STEPPlugin_IOperations::LengthUnit_Meter;
    break;
  case GEOM::LU_KILOMETER:
    aUnit = STEPPlugin_IOperations::LengthUnit_Kilometer;
    break;
  case GEOM::LU_MILLIINCH:
    aUnit = STEPPlugin_IOperations::LengthUnit_Milliinch;
    break;
  case GEOM::LU_MICROMETER:
    aUnit = STEPPlugin_IOperations::LengthUnit_Micrometer;
    break;
  case GEOM::LU_CENTIMETER:
    aUnit = STEPPlugin_IOperations::LengthUnit_Centimeter;
    break;
  case GEOM::LU_MICROINCH:
    aUnit = STEPPlugin_IOperations::LengthUnit_Microinch;
    break;
  default:
    return;
  }

  //Export the shape to the file
  GetOperations()->ExportSTEP( anOriginal, theFileName, aUnit );
}

//=============================================================================
/*!
 *  ImportSTEP
 *  Import a shape from STEP format
 *  \param theFileName The name of the file to import
 *  \param theIsIgnoreUnits If True, file length units will be ignored (set to 'meter')
 *                          and result model will be scaled, if its units are not meters.
 *                          If False (default), file length units will be taken into account.
 *  \param IsCreateAssemblies If True, for each assembly compound is created
 *                          in the result. If False Compounds that contain a
 *                          single shape are eliminated from the result.
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* STEPPlugin_IOperations_i::ImportSTEP
                          (const char *theFileName,
                           const bool  theIsIgnoreUnits,
                           const bool  IsCreateAssemblies)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Import the shape from the file
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->ImportSTEP
    (theFileName, theIsIgnoreUnits, IsCreateAssemblies);

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
char* STEPPlugin_IOperations_i::ReadValue( const char* theFileName,
					   const char* theParameterName )
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  TCollection_AsciiString aParam = GetOperations()->ReadValue( theFileName, theParameterName );

  return CORBA::string_dup( aParam.ToCString() );
}

STEPPlugin_IOperations* STEPPlugin_IOperations_i::GetOperations()
{
  return (STEPPlugin_IOperations*)GetImpl();
}
