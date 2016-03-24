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

#ifndef _GEOM_IFieldOperations_i_HeaderFile
#define _GEOM_IFieldOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IFieldOperations.hxx"

class GEOM_I_EXPORT GEOM_IFieldOperations_i :
  public virtual POA_GEOM::GEOM_IFieldOperations,
  public virtual GEOM_IOperations_i
{
 public:
  GEOM_IFieldOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
                           ::GEOMImpl_IFieldOperations* theImpl);
  ~GEOM_IFieldOperations_i();

  /*!
   *  \brief Creates a field
   */
  GEOM::GEOM_Field_ptr CreateField (GEOM::GEOM_Object_ptr     theShape,
                                    const char*               theName,
                                    GEOM::field_data_type     theType,
                                    CORBA::Short              theDimension,
                                    const GEOM::string_array& theComponentNames);
  /*!
   *  \brief Returns number of fields on a shape
   */
  CORBA::Long CountFields( GEOM::GEOM_Object_ptr shape);

  /*!
   *  \brief Returns all fields on a shape
   */
  GEOM::ListOfFields* GetFields( GEOM::GEOM_Object_ptr shape );

  /*!
   *  \brief Returns a field on a shape by its name
   */
  GEOM::GEOM_Field_ptr GetField( GEOM::GEOM_Object_ptr shape,
                                 const char*           name);
  /*!
   *  \brief Returns number of sub-shapes of given dimension
   */
  ::CORBA::Long GetNbSubShapes(::GEOM::GEOM_Object_ptr shape,
                               ::CORBA::Short          fieldDim);

  ::GEOMImpl_IFieldOperations* GetOperations()
  { return (::GEOMImpl_IFieldOperations*)GetImpl(); }
};

#endif
