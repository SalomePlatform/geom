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

#ifndef _GEOM_IGroupOperations_i_HeaderFile
#define _GEOM_IGroupOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IGroupOperations.hxx"

class GEOM_I_EXPORT GEOM_IGroupOperations_i :
    public virtual POA_GEOM::GEOM_IGroupOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_IGroupOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
                           ::GEOMImpl_IGroupOperations* theImpl);
  ~GEOM_IGroupOperations_i();

  GEOM::GEOM_Object_ptr CreateGroup (GEOM::GEOM_Object_ptr theMainShape, CORBA::Long theShapeType);

  void AddObject (GEOM::GEOM_Object_ptr theGroup, CORBA::Long theSubShapeId);

  void RemoveObject (GEOM::GEOM_Object_ptr theGroup, CORBA::Long theSubShapeId);

  void UnionList (GEOM::GEOM_Object_ptr theGroup, const GEOM::ListOfGO& theSubShapes);

  void DifferenceList (GEOM::GEOM_Object_ptr theGroup, const GEOM::ListOfGO& theSubShapes);

  void UnionIDs (GEOM::GEOM_Object_ptr theGroup, const GEOM::ListOfLong& theSubShapes);

  void DifferenceIDs (GEOM::GEOM_Object_ptr theGroup, const GEOM::ListOfLong& theSubShapes);

  GEOM::GEOM_Object_ptr UnionGroups (GEOM::GEOM_Object_ptr theGroup1, GEOM::GEOM_Object_ptr theGroup2);
  GEOM::GEOM_Object_ptr IntersectGroups (GEOM::GEOM_Object_ptr theGroup1, GEOM::GEOM_Object_ptr theGroup2);
  GEOM::GEOM_Object_ptr CutGroups (GEOM::GEOM_Object_ptr theGroup1, GEOM::GEOM_Object_ptr theGroup2);

  GEOM::GEOM_Object_ptr UnionListOfGroups (const GEOM::ListOfGO& theGList);
  GEOM::GEOM_Object_ptr IntersectListOfGroups (const GEOM::ListOfGO& theGList);
  GEOM::GEOM_Object_ptr CutListOfGroups (const GEOM::ListOfGO& theGList1, const GEOM::ListOfGO& theGList2);

  CORBA::Long GetType (GEOM::GEOM_Object_ptr theGroup);

  GEOM::GEOM_Object_ptr GetMainShape (GEOM::GEOM_Object_ptr theGroup);

  GEOM::ListOfLong* GetObjects (GEOM::GEOM_Object_ptr theGroup);

  ::GEOMImpl_IGroupOperations* GetOperations()
  { return (::GEOMImpl_IGroupOperations*)GetImpl(); }
};

#endif
