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

#include <Standard_Stream.hxx>

#include <list>

#include "GEOM_IHealingOperations_i.hh"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include "ShHealOper_ModifStats.hxx"

#include <utilities.h>
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>
#include <Basics_Utils.hxx>

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
 *  ProcessShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::ProcessShape (GEOM::GEOM_Object_ptr theObject,
                                                               const GEOM::string_array& theOperations,
                                                               const GEOM::string_array& theParams,
                                                               const GEOM::string_array& theValues)
{
  Kernel_Utils::Localizer loc;

  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Check if theOperations has more than 0 elements and theParams and theValues have the same length
  //if (theOperations.length() <= 0 || theParams.length() != theValues.length())
  //  return aGEOMObject._retn();

  // Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
  HANDLE_NAMESPACE(GEOM_Object) aNewObject = GetOperations()->ShapeProcess( anObject,
    ConvertStringArray( theOperations ), ConvertStringArray( theParams ),
    ConvertStringArray( theValues ) );
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
  std::list<std::string> operationsList, paramsList, valuesList;
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
    std::list<std::string>::iterator opIt, parIt, valIt;
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
  std::list<std::string> paramsList, valuesList;
  if ( GetOperations()->GetOperatorParameters( theOperator, paramsList, valuesList ) ) {
    const int parSize = paramsList.size(), valSize = valuesList.size();

    if ( parSize == valSize ) {
      aParArray->length(parSize);
      aValArray->length(valSize);

      // fill the local CORBA arrays with values from lists
      std::list<std::string>::iterator parIt, valIt;
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

  // Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // if theFaces is empty - it's OK, it means that ALL faces must be removed

  // Perform
  HANDLE_NAMESPACE(GEOM_Object) aNewObject =
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

  // Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
  HANDLE_NAMESPACE(GEOM_Object) aNewObject =
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

  // Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // if theWires is empty - it's OK, it means that ALL wires should be removed

  // Perform
  HANDLE_NAMESPACE(GEOM_Object) aNewObject =
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

  // Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // if theWires is empty - it's OK, it means that ALL wires should be removed

  // Perform
  HANDLE_NAMESPACE(GEOM_Object) aNewObject =
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
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::Sew (const GEOM::ListOfGO& theObjects,
                                                      CORBA::Double         theTolerance)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Check parameters
  if (theTolerance < 0)
    return aGEOMObject._retn();

  //Get the shapes
  std::list< HANDLE_NAMESPACE(GEOM_Object) > aShapes;
  if (! GetListOfObjectsImpl( theObjects, aShapes ))
    return aGEOMObject._retn();

  // Perform
  HANDLE_NAMESPACE(GEOM_Object) aNewObject = GetOperations()->Sew( aShapes, theTolerance, false );
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aNewObject);
}

//=============================================================================
/*!
 *  SewAllowNonManifold
 */
//=============================================================================
GEOM::GEOM_Object_ptr
GEOM_IHealingOperations_i::SewAllowNonManifold (const GEOM::ListOfGO& theObjects,
                                                CORBA::Double         theTolerance)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Check parameters
  if (theTolerance < 0)
    return aGEOMObject._retn();

  //Get the shapes
  std::list< HANDLE_NAMESPACE(GEOM_Object) > aShapes;
  if (! GetListOfObjectsImpl( theObjects, aShapes ))
    return aGEOMObject._retn();

  // Perform
  HANDLE_NAMESPACE(GEOM_Object) aNewObject = GetOperations()->Sew( aShapes, theTolerance, true );
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aNewObject);
}

//=============================================================================
/*!
 *  RemoveInternalFaces
 */
//=============================================================================
GEOM::GEOM_Object_ptr
GEOM_IHealingOperations_i::RemoveInternalFaces (const GEOM::ListOfGO& theSolids)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Get the objects
  std::list< HANDLE_NAMESPACE(GEOM_Object) > aShapes;
  if (! GetListOfObjectsImpl( theSolids, aShapes ))
    return aGEOMObject._retn();

  // Perform
  HANDLE_NAMESPACE(GEOM_Object) aNewObject = GetOperations()->RemoveInternalFaces(aShapes);
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
  if (theValue < 0 || theValue > 1)
    return aGEOMObject._retn();

  // Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
  HANDLE_NAMESPACE(GEOM_Object) aNewObject =
    GetOperations()->DivideEdge( anObject, theIndex, theValue, isByParameter );
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aNewObject);
}

//=============================================================================
/*!
 *  DivideEdgeByPoint
 */
//=============================================================================
GEOM::GEOM_Object_ptr
GEOM_IHealingOperations_i::DivideEdgeByPoint (GEOM::GEOM_Object_ptr theObject,
                                              CORBA::Short          theIndex,
                                              const GEOM::ListOfGO& thePoints)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Get the points
  std::list< HANDLE_NAMESPACE(GEOM_Object) > aPoints;
  if (! GetListOfObjectsImpl( thePoints, aPoints ))
    return aGEOMObject._retn();

  // Perform
  HANDLE_NAMESPACE(GEOM_Object) aNewObject =
    GetOperations()->DivideEdgeByPoint( anObject, theIndex, aPoints );
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aNewObject);
}

//=============================================================================
/*!
 *  FuseCollinearEdgesWithinWire
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::FuseCollinearEdgesWithinWire
                                          (GEOM::GEOM_Object_ptr theWire,
                                           const GEOM::ListOfGO& theVertices)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  HANDLE_NAMESPACE(GEOM_Object) aWire = GetObjectImpl(theWire);
  if (aWire.IsNull()) return aGEOMObject._retn();

  int ind, aLen;
  std::list<HANDLE_NAMESPACE(GEOM_Object)> aVerts;
  //Get the shapes
  aLen = theVertices.length();
  for (ind = 0; ind < aLen; ind++) {
    HANDLE_NAMESPACE(GEOM_Object) aSh = GetObjectImpl(theVertices[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aVerts.push_back(aSh);
  }

  //Perform operation
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->FuseCollinearEdgesWithinWire(aWire, aVerts);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetFreeBoundary
 */
//=============================================================================
CORBA::Boolean
GEOM_IHealingOperations_i::GetFreeBoundary ( const GEOM::ListOfGO & theObjects,
                                             GEOM::ListOfGO_out     theClosedWires,
                                             GEOM::ListOfGO_out     theOpenWires )
{
  theClosedWires = new GEOM::ListOfGO;
  theOpenWires = new GEOM::ListOfGO;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Get the objects
  Handle(TColStd_HSequenceOfTransient) anObjects = new TColStd_HSequenceOfTransient();
  for ( size_t i = 0; i < theObjects.length(); ++i )
  {
    HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObjects[i]);
    if (anObject.IsNull())
      return false;
    anObjects->Append( anObject );
  }

  Handle(TColStd_HSequenceOfTransient) aClosed = new TColStd_HSequenceOfTransient();
  Handle(TColStd_HSequenceOfTransient) anOpen  = new TColStd_HSequenceOfTransient();
  bool res = GetOperations()->GetFreeBoundary( anObjects, aClosed, anOpen );

  if ( !GetOperations()->IsDone() || !res )
    return false;

  int i, n = aClosed->Length();
  theClosedWires->length( n );
  for ( i = 1; i <= n; i++ )
    (*theClosedWires)[i-1] = GetObject(HANDLE_NAMESPACE(GEOM_Object)::DownCast(aClosed->Value(i)));

  n = anOpen->Length();
  theOpenWires->length( n );
  for ( i = 1, n = anOpen->Length(); i <= n; i++ )
    (*theOpenWires)[i-1] = GetObject(HANDLE_NAMESPACE(GEOM_Object)::DownCast(anOpen->Value(i)));

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
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
//  HANDLE_NAMESPACE(GEOM_Object) aNewObject =
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

  // Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
  HANDLE_NAMESPACE(GEOM_Object) aNewObject =
    GetOperations()->ChangeOrientationCopy( anObject );
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aNewObject);
}

//=============================================================================
/*!
 *  LimitTolerance
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IHealingOperations_i::LimitTolerance (GEOM::GEOM_Object_ptr theObject,
                                                                 CORBA::Double theTolerance)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull())
    return aGEOMObject._retn();

  // Perform
  HANDLE_NAMESPACE(GEOM_Object) aNewObject =
    GetOperations()->LimitTolerance(anObject, theTolerance);
  if (!GetOperations()->IsDone() || aNewObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aNewObject);
}

//================================================================================
/*!
 * \brief Return information on what has been done by the last called healing method
 */
//================================================================================

GEOM::ModifStatistics* GEOM_IHealingOperations_i::GetStatistics()
{
  const ShHealOper_ModifStats& stats = * GetOperations()->GetStatistics();
  const std::set< ShHealOper_ModifStats::Datum >& modifs = stats.GetData();
  std::set< ShHealOper_ModifStats::Datum >::const_iterator modif = modifs.begin();

  GEOM::ModifStatistics_var statsVar = new GEOM::ModifStatistics();
  statsVar->length( modifs.size() );
  for ( int i = 0; modif != modifs.end(); ++modif, ++i )
  {
    statsVar[ i ].name = modif->myModif.c_str();
    statsVar[ i ].count = modif->myCount;

    // Cut off "Unknown message invoked with the keyword " at the beginning
    const char* toRm = "Unknown message invoked with the keyword ";
    const size_t lenToRm = strlen( toRm );
    if ( modif->myModif.size() > lenToRm &&
         modif->myModif.compare( 0, lenToRm, toRm ) == 0 )
      statsVar[ i ].name = modif->myModif.substr( lenToRm ).c_str();
  }

  return statsVar._retn();
}
