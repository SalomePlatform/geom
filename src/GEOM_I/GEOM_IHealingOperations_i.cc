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
#include <Standard_Stream.hxx>

#include <list>

#include "GEOM_IHealingOperations_i.hh"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include <TColStd_HSequenceOfTransient.hxx>

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================

GEOM_IHealingOperations_i::GEOM_IHealingOperations_i (PortableServer::POA_ptr thePOA,
						      GEOM::GEOM_Gen_ptr theEngine,
						      ::GEOMImpl_IHealingOperations* theImpl)
:GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IHealingOperations_i::GEOM_IHealingOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================

GEOM_IHealingOperations_i::~GEOM_IHealingOperations_i()
{
  MESSAGE("GEOM_IHealingOperations_i::~GEOM_IHealingOperations_i");
}

//=============================================================================
/*!
 *  Convert
 */
//=============================================================================
Handle(TColStd_HArray1OfInteger) GEOM_IHealingOperations_i::Convert
                                          (const GEOM::short_array& theInArray)
{
  Handle(TColStd_HArray1OfInteger) anOutArray;
  int n = theInArray.length();
  if ( n <= 0 )
    return anOutArray;
  anOutArray = new TColStd_HArray1OfInteger( 1, n );
  for (int i = 0; i < n; i++)
    anOutArray->SetValue( i+1, theInArray[i] );
  return anOutArray;
}

//=============================================================================
/*!
 *  Convert
 */
//=============================================================================
Handle(TColStd_HArray1OfExtendedString) GEOM_IHealingOperations_i::Convert
                                         (const GEOM::string_array& theInArray)
{
  Handle(TColStd_HArray1OfExtendedString) anOutArray;
  int n = theInArray.length();
  if ( n <= 0 )
    return anOutArray;
  anOutArray = new TColStd_HArray1OfExtendedString( 1, n );
  char* str;
  for ( int i = 0; i < n; i++ )
  {
    str = CORBA::string_dup( theInArray[i] );
    anOutArray->SetValue( i+1, TCollection_ExtendedString( str ) );
  }
  return anOutArray;
}

//=============================================================================
/*!
 *  ProcessShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::ProcessShape (GEOM::GEOM_Object_ptr theObject,
							       const GEOM::string_array& theOperations,
							       const GEOM::string_array& theParams,
							       const GEOM::string_array& theValues)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Check parameters
  if ( CORBA::is_nil(theObject) )
    return aGEOMObject._retn();

  // Check if theOperations has more than 0 elements and theParams and theValues have the same length
//  if ( theOperations.length() <= 0 || theParams.length() != theValues.length() )
//    return aGEOMObject._retn();

  // Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if ( anObject.IsNull() )
    return aGEOMObject._retn();

  // Perform
  Handle(GEOM_Object) aNewObject = GetOperations()->ShapeProcess( anObject,
    Convert( theOperations ), Convert( theParams ), Convert( theValues ) );
  if ( !GetOperations()->IsDone() || aNewObject.IsNull() )
    return aGEOMObject._retn();

  return GetObject( aNewObject );
}

//=============================================================================
/*!
 *  GetShapeProcessParameters
 */
//=============================================================================
void GEOM_IHealingOperations_i::GetShapeProcessParameters(GEOM::string_array_out theOperations,
                                                          GEOM::string_array_out theParams,
                                                          GEOM::string_array_out theValues)
{
  GEOM::string_array_var anOpArray = new GEOM::string_array();
  GEOM::string_array_var aParArray = new GEOM::string_array();
  GEOM::string_array_var aValArray = new GEOM::string_array();

  // retrieve the values as stl-lists
  list<string> operationsList, paramsList, valuesList;
  GetOperations()->GetShapeProcessParameters( operationsList, paramsList, valuesList );
  const int opSize = operationsList.size(),
  parSize = paramsList.size(),
  valSize = valuesList.size();

  if ( opSize >= 0 && parSize >= 0 && parSize == valSize ) {
    // allocate the CORBA arrays, sizes == returned lists' sizes
    anOpArray->length(opSize);
    aParArray->length(parSize);
    aValArray->length(valSize);

    // fill the local CORBA arrays with values from lists
    list<string>::iterator opIt, parIt, valIt;
    int i = 0;
    for ( opIt = operationsList.begin(); opIt != operationsList.end(); i++,++opIt )
      anOpArray[i] = CORBA::string_dup( (*opIt).c_str() );

    for ( i = 0, parIt = paramsList.begin(), valIt = valuesList.begin();
	  parIt != paramsList.end(); i++, ++parIt,++valIt ) {
      aParArray[i] = CORBA::string_dup( (*parIt).c_str() );
      aValArray[i] = CORBA::string_dup( (*valIt).c_str() );
    }
  }

  // initialize out-parameters with local arrays
  theOperations = anOpArray._retn();
  theParams = aParArray._retn();
  theValues = aValArray._retn();
}

//=============================================================================
/*!
 *  GetOperatorParameters
 */
//=============================================================================
void GEOM_IHealingOperations_i::GetOperatorParameters (const char* theOperator,  
						       GEOM::string_array_out theParams, 
						       GEOM::string_array_out theValues)
{
  GEOM::string_array_var aParArray = new GEOM::string_array();
  GEOM::string_array_var aValArray = new GEOM::string_array();

  // retrieve the values as stl-lists
  list<string> paramsList, valuesList;
  if ( GetOperations()->GetOperatorParameters( theOperator, paramsList, valuesList ) ) {
    const int parSize = paramsList.size(), valSize = valuesList.size();

    if ( parSize == valSize ) {
      aParArray->length(parSize);
      aValArray->length(valSize);

      // fill the local CORBA arrays with values from lists
      list<string>::iterator parIt, valIt;
      int i;
      for ( i = 0, parIt = paramsList.begin(), valIt = valuesList.begin();
            parIt != paramsList.end(); i++, ++parIt,++valIt ) {
	aParArray[i] = CORBA::string_dup( (*parIt).c_str() );
	aValArray[i] = CORBA::string_dup( (*valIt).c_str() );
      }
    }
  }

  // initialize out-parameters with local arrays
  theParams = aParArray._retn();
  theValues = aValArray._retn();
}

//=============================================================================
/*!
 *  SuppressFaces
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::SuppressFaces (GEOM::GEOM_Object_ptr theObject,
								const GEOM::short_array& theFaces)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Check parameters
  if ( CORBA::is_nil(theObject) ) // if theFaces is empty - it's OK, it means that ALL faces must be removed
    return aGEOMObject._retn();

  // Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
  Handle(GEOM_Object) aNewObject =
    GetOperations()->SuppressFaces( anObject, Convert( theFaces ) );
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return  GetObject( aNewObject );
}

//=============================================================================
/*!
 *  CloseContour
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::CloseContour (GEOM::GEOM_Object_ptr theObject,
							       const GEOM::short_array& theWires,
							       CORBA::Boolean isCommonVertex)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Check parameters
  if ( CORBA::is_nil(theObject) )
    return aGEOMObject._retn();

  // Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
  Handle(GEOM_Object) aNewObject =
    GetOperations()->CloseContour( anObject, Convert( theWires ), isCommonVertex );
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aNewObject);
}

//=============================================================================
/*!
 *  RemoveIntWires
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::RemoveIntWires (GEOM::GEOM_Object_ptr theObject,
								 const GEOM::short_array& theWires)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Check parameters
  if ( CORBA::is_nil(theObject) ) // if theWires is empty - it's OK, it means that ALL wires should be removed
    return aGEOMObject._retn();

  // Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
  Handle(GEOM_Object) aNewObject =
    GetOperations()->RemoveIntWires( anObject, Convert( theWires ) );
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aNewObject);
}

//=============================================================================
/*!
 *  FillHoles
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::FillHoles (GEOM::GEOM_Object_ptr theObject,
							    const GEOM::short_array& theWires)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Check parameters
  if ( CORBA::is_nil(theObject) ) // if theWires is empty - it's OK, it means that ALL wires should be removed
    return aGEOMObject._retn();

  // Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
  Handle(GEOM_Object) aNewObject =
    GetOperations()->FillHoles( anObject, Convert( theWires ) );
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aNewObject);
}

//=============================================================================
/*!
 *  Sew
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::Sew (GEOM::GEOM_Object_ptr theObject,
						      CORBA::Double theTolerance)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Check parameters
  if ( CORBA::is_nil(theObject) || theTolerance < 0 )
    return aGEOMObject._retn();

  // Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
  Handle(GEOM_Object) aNewObject =
    GetOperations()->Sew( anObject, theTolerance );
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aNewObject);
}

//=============================================================================
/*!
 *  DivideEdge
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::DivideEdge (GEOM::GEOM_Object_ptr theObject,
                                                             CORBA::Short theIndex,
							     CORBA::Double theValue,
							     CORBA::Boolean isByParameter)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Check parameters
  if ( CORBA::is_nil(theObject) || theValue < 0 || theValue > 1 )
    return aGEOMObject._retn();

  // Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
  Handle(GEOM_Object) aNewObject =
    GetOperations()->DivideEdge( anObject, theIndex, theValue, isByParameter );
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aNewObject);
}

//=============================================================================
/*!
 *  GetFreeBoundary
 */
//=============================================================================
CORBA::Boolean GEOM_IHealingOperations_i::GetFreeBoundary ( GEOM::GEOM_Object_ptr theObject,
							    GEOM::ListOfGO_out theClosedWires,
							    GEOM::ListOfGO_out theOpenWires )
{
  theClosedWires = new GEOM::ListOfGO;
  theOpenWires = new GEOM::ListOfGO;

  // Set a not done flag
  GetOperations()->SetNotDone();

  if ( CORBA::is_nil(theObject) )
  	return false;

  // Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull())
    return false;

  Handle(TColStd_HSequenceOfTransient) aClosed = new TColStd_HSequenceOfTransient();
  Handle(TColStd_HSequenceOfTransient) anOpen  = new TColStd_HSequenceOfTransient();
  bool res = GetOperations()->GetFreeBoundary( anObject, aClosed, anOpen );

  if ( !GetOperations()->IsDone() || !res )
  	return false;

  int i, n = aClosed->Length();
  theClosedWires->length( n );
  for ( i = 1; i <= n; i++ )
    (*theClosedWires)[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aClosed->Value(i)));

  n = anOpen->Length();
  theOpenWires->length( n );
  for ( i = 1, n = anOpen->Length(); i <= n; i++ )
    (*theOpenWires)[i-1] = GetObject(Handle(GEOM_Object)::DownCast(anOpen->Value(i)));

  return true;
}


//=============================================================================
/*!
 *  ChangeOrientation
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::ChangeOrientation (GEOM::GEOM_Object_ptr theObject)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Check parameters
  if ( CORBA::is_nil(theObject) )
    return aGEOMObject._retn();

  aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  // Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
//  Handle(GEOM_Object) aNewObject =
    GetOperations()->ChangeOrientation( anObject );
//  if (!GetOperations()->IsDone() || aNewObject.IsNull())
//    return aGEOMObject._retn();

  //return GetObject(aNewObject);
  return aGEOMObject._retn();  
}


//=============================================================================
/*!
 *  ChangeOrientationCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::ChangeOrientationCopy (GEOM::GEOM_Object_ptr theObject)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Check parameters
  if ( CORBA::is_nil(theObject) )
    return aGEOMObject._retn();

  // Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
  Handle(GEOM_Object) aNewObject =
    GetOperations()->ChangeOrientationCopy( anObject );
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aNewObject);
}

