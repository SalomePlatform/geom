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

#include "GEOM_ILocalOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_ILocalOperations_i::GEOM_ILocalOperations_i (PortableServer::POA_ptr thePOA,
                                                  GEOM::GEOM_Gen_ptr theEngine,
                                                  ::GEOMImpl_ILocalOperations* theImpl)
     :GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_ILocalOperations_i::GEOM_ILocalOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_ILocalOperations_i::~GEOM_ILocalOperations_i()
{
  MESSAGE("GEOM_ILocalOperations_i::~GEOM_ILocalOperations_i");
}


//=============================================================================
/*!
 *  MakeFilletAll
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeFilletAll
                      (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Create the Fillet
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeFilletAll(aShapeRef, theR);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFilletEdges
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeFilletEdges
                      (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
                       const GEOM::ListOfLong& theEdges)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Get the reference edges
  int ind = 0;
  int aLen = theEdges.length();
  std::list<int> anEdges;
  for (; ind < aLen; ind++) {
    anEdges.push_back(theEdges[ind]);
  }

  //Create the Fillet
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeFilletEdges(aShapeRef, theR, anEdges);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFilletEdges R1 R2
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeFilletEdgesR1R2
                     (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR1,
                      CORBA::Double theR2, const GEOM::ListOfLong& theEdges)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Get the reference edges
  int ind = 0;
  int aLen = theEdges.length();
  std::list<int> anEdges;
  for (; ind < aLen; ind++) {
    anEdges.push_back(theEdges[ind]);
  }

  //Create the Fillet
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeFilletEdgesR1R2(aShapeRef, theR1, theR2, anEdges);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFilletFaces
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeFilletFaces
                      (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
                       const GEOM::ListOfLong& theFaces)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Get the reference faces
  int ind = 0;
  int aLen = theFaces.length();
  std::list<int> aFaces;
  for (; ind < aLen; ind++) {
    aFaces.push_back(theFaces[ind]);
  }

  //Create the Fillet
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeFilletFaces(aShapeRef, theR, aFaces);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFilletFaces R1 R2
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeFilletFacesR1R2
                      (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR1,
                       CORBA::Double theR2, const GEOM::ListOfLong& theFaces)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Get the reference faces
  int ind = 0;
  int aLen = theFaces.length();
  std::list<int> aFaces;
  for (; ind < aLen; ind++) {
    aFaces.push_back(theFaces[ind]);
  }

  //Create the Fillet
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeFilletFacesR1R2(aShapeRef, theR1, theR2, aFaces);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFillet2D
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeFillet2D
                      (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
                       const GEOM::ListOfLong& theVertexes)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Get the reference vertex
  int ind = 0;
  int aLen = theVertexes.length();
  std::list<int> aVertexes;
  for (; ind < aLen; ind++) {
    aVertexes.push_back(theVertexes[ind]);
  }

  //Create the Fillet
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeFillet2D(aShapeRef, theR, aVertexes);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFillet1D
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeFillet1D
                      (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
                       const GEOM::ListOfLong& theVertexes,
                       CORBA::Boolean doIgnoreSecantVertices)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape (wire)
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Get the reference vertex
  int ind = 0;
  int aLen = theVertexes.length();
  std::list<int> aVertexes;
  for (; ind < aLen; ind++) {
    aVertexes.push_back(theVertexes[ind]);
  }

  //Create the Fillet
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeFillet1D(aShapeRef, theR, aVertexes, doIgnoreSecantVertices);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeChamferAll
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeChamferAll
                      (GEOM::GEOM_Object_ptr theShape, CORBA::Double theD)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Create the Chamfer
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeChamferAll(aShapeRef, theD);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeChamferEdge
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeChamferEdge
                      (GEOM::GEOM_Object_ptr theShape,
                       CORBA::Double theD1, CORBA::Double theD2,
                       CORBA::Long theFace1, CORBA::Long theFace2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Create the Chamfer
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeChamferEdge(aShapeRef, theD1, theD2, theFace1, theFace2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}
//=============================================================================
/*!
 *  MakeChamferEdgeAD
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeChamferEdgeAD
                      (GEOM::GEOM_Object_ptr theShape,
                       CORBA::Double theD, CORBA::Double theAngle,
                       CORBA::Long theFace1, CORBA::Long theFace2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Create the Chamfer
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeChamferEdgeAD(aShapeRef, theD, theAngle, theFace1, theFace2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeChamferFaces
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeChamferFaces
                      (GEOM::GEOM_Object_ptr theShape,
                       CORBA::Double theD1, CORBA::Double theD2,
                       const GEOM::ListOfLong& theFaces)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Get the reference faces
  int ind = 0;
  int aLen = theFaces.length();
  std::list<int> aFaces;
  for (; ind < aLen; ind++) {
    aFaces.push_back(theFaces[ind]);
  }

  //Create the Chamfer
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeChamferFaces(aShapeRef, theD1, theD2, aFaces);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}
//=============================================================================
/*!
 *  MakeChamferFacesAD
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeChamferFacesAD
                      (GEOM::GEOM_Object_ptr theShape,
                       CORBA::Double theD, CORBA::Double theAngle,
                       const GEOM::ListOfLong& theFaces)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Get the reference faces
  int ind = 0;
  int aLen = theFaces.length();
  std::list<int> aFaces;
  for (; ind < aLen; ind++) {
    aFaces.push_back(theFaces[ind]);
  }

  //Create the Chamfer
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeChamferFacesAD(aShapeRef, theD, theAngle, aFaces);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeChamferEdges
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeChamferEdges
                      (GEOM::GEOM_Object_ptr theShape,
                       CORBA::Double theD1, CORBA::Double theD2,
                       const GEOM::ListOfLong& theEdges)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Get the reference edges
  int ind = 0;
  int aLen = theEdges.length();
  std::list<int> aEdges;
  for (; ind < aLen; ind++) {
    aEdges.push_back(theEdges[ind]);
  }

  //Create the Chamfer
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeChamferEdges(aShapeRef, theD1, theD2, aEdges);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeChamferEdgesAD
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeChamferEdgesAD
                      (GEOM::GEOM_Object_ptr theShape,
                       CORBA::Double theD, CORBA::Double theAngle,
                       const GEOM::ListOfLong& theEdges)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Get the reference edges
  int ind = 0;
  int aLen = theEdges.length();
  std::list<int> aEdges;
  for (; ind < aLen; ind++) {
    aEdges.push_back(theEdges[ind]);
  }

  //Create the Chamfer
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeChamferEdgesAD(aShapeRef, theD, theAngle, aEdges);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeArchimede
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ILocalOperations_i::MakeArchimede (GEOM::GEOM_Object_ptr theShape,
                                                              CORBA::Double theWeight,
                                                              CORBA::Double theWaterDensity,
                                                              CORBA::Double theMeshingDeflection)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Create the Archimede
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeArchimede
    (aShapeRef, theWeight, theWaterDensity, theMeshingDeflection);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetSubShapeIndex
 */
//=============================================================================
CORBA::Long GEOM_ILocalOperations_i::GetSubShapeIndex
  (GEOM::GEOM_Object_ptr theShape, GEOM::GEOM_Object_ptr theSubShape)
{
  //Get the reference shapes
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  HANDLE_NAMESPACE(GEOM_Object) aSubShapeRef = GetObjectImpl(theSubShape);
  if (aShapeRef.IsNull() || aSubShapeRef.IsNull()) return -1;

  //Get the unique ID of <theSubShape> inside <theShape>
  CORBA::Long anID = GetOperations()->GetSubShapeIndex(aShapeRef, aSubShapeRef);
  if (!GetOperations()->IsDone())
    return -1;

  return anID;
}
