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
// File      : GEOM_wrap.hxx
// Created   : Mon Dec 24 14:39:49 2012
// Author    : Edward AGAPOV (eap)


#ifndef __GEOM_wrap_HXX__
#define __GEOM_wrap_HXX__

#include <SALOME_GenericObj_wrap.hxx>

namespace GEOM
{
  // Declare a type GEOM::GEOM_Object_wrap to replace GEOM::GEOM_Object_var,
  // to be used in the case if a new GEOM_Object is received, in which case
  // obj->UnRegister() must be called to avoid a memory leak.
  // The GEOM::GEOM_Object_wrap object calls UnRegister() at destruction.
  // A simple rule: use GEOM::GEOM_Object_wrap if an object is returned by a
  // GEOM operation (GEOM::GEOM_IBasicOperations etc).
  //
  class GEOM_BasicObject;
  class GEOM_Object;
  class GEOM_Field;
  class GEOM_FieldStep;
  typedef SALOME::GenericObj_wrap< GEOM_BasicObject > GEOM_BaseObject_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_Object      > GEOM_Object_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_Field       > GEOM_Field_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_FieldStep   > GEOM_FieldStep_wrap;
  //
  // wrappers of operations
  class GEOM_IBasicOperations;
  class GEOM_ITransformOperations;
  class GEOM_I3DPrimOperations;
  class GEOM_IShapesOperations;
  class GEOM_IBlocksOperations;
  class GEOM_IBooleanOperations;
  class GEOM_ICurvesOperations;
  class GEOM_ILocalOperations;
  class GEOM_IHealingOperations;
  class GEOM_IInsertOperations;
  class GEOM_IMeasureOperations;
  class GEOM_IGroupOperations;
  class GEOM_IFieldOperations;
  typedef SALOME::GenericObj_wrap< GEOM_IBasicOperations    > GEOM_IBasicOperations_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_ITransformOperations> GEOM_ITransformOperations_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_I3DPrimOperations   > GEOM_I3DPrimOperations_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_IShapesOperations   > GEOM_IShapesOperations_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_IBlocksOperations   > GEOM_IBlocksOperations_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_IBooleanOperations  > GEOM_IBooleanOperations_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_ICurvesOperations   > GEOM_ICurvesOperations_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_ILocalOperations    > GEOM_ILocalOperations_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_IHealingOperations  > GEOM_IHealingOperations_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_IInsertOperations   > GEOM_IInsertOperations_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_IMeasureOperations  > GEOM_IMeasureOperations_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_IGroupOperations    > GEOM_IGroupOperations_wrap;
  typedef SALOME::GenericObj_wrap< GEOM_IFieldOperations    > GEOM_IFieldOperations_wrap;
}

#endif
