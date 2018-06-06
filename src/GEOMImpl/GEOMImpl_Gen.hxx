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

#ifndef _GEOMImpl_GEN_HXX_
#define _GEOMImpl_GEN_HXX_

#include <GEOM_GEOMImpl.hxx>

#include <map>

#include "GEOMImpl_IBasicOperations.hxx"
#include "GEOMImpl_ITransformOperations.hxx"
#include "GEOMImpl_I3DPrimOperations.hxx"
#include "GEOMImpl_IShapesOperations.hxx"
#include "GEOMImpl_IBlocksOperations.hxx"
#include "GEOMImpl_IBooleanOperations.hxx"
#include "GEOMImpl_IHealingOperations.hxx"
#include "GEOMImpl_ICurvesOperations.hxx"
#include "GEOMImpl_ILocalOperations.hxx"
#include "GEOMImpl_IInsertOperations.hxx"
#include "GEOMImpl_IMeasureOperations.hxx"
#include "GEOMImpl_IGroupOperations.hxx"
#include "GEOMImpl_IFieldOperations.hxx"
#include "GEOM_Engine.hxx"

class GEOMIMPL_EXPORT GEOMImpl_Gen : public GEOM_Engine
{
 public:
  GEOMImpl_Gen();
  ~GEOMImpl_Gen();

  GEOMImpl_IBasicOperations* GetIBasicOperations();

  GEOMImpl_ITransformOperations* GetITransformOperations();

  GEOMImpl_I3DPrimOperations* GetI3DPrimOperations();

  GEOMImpl_IShapesOperations* GetIShapesOperations();

  GEOMImpl_IBlocksOperations* GetIBlocksOperations();

  GEOMImpl_IMeasureOperations* GetIMeasureOperations();

  GEOMImpl_IBooleanOperations* GetIBooleanOperations();

  GEOMImpl_ICurvesOperations* GetICurvesOperations();

  GEOMImpl_ILocalOperations* GetILocalOperations();

  GEOMImpl_IInsertOperations* GetIInsertOperations();

  GEOMImpl_IHealingOperations* GetIHealingOperations();

  GEOMImpl_IGroupOperations* GetIGroupOperations();

  GEOMImpl_IFieldOperations* GetIFieldOperations();

 private:

  GEOMImpl_IBasicOperations*     _BasicOperations;
  GEOMImpl_ITransformOperations* _TransformOperations;
  GEOMImpl_I3DPrimOperations*    _3DPrimOperations;
  GEOMImpl_IShapesOperations*    _ShapesOperations;
  GEOMImpl_IBlocksOperations*    _BlocksOperations;
  GEOMImpl_IBooleanOperations*   _BooleanOperations;
  GEOMImpl_IHealingOperations*   _HealingOperations;
  GEOMImpl_ICurvesOperations*    _CurvesOperations;
  GEOMImpl_ILocalOperations*     _LocalOperations;
  GEOMImpl_IInsertOperations*    _InsertOperations;
  GEOMImpl_IMeasureOperations*   _MeasureOperations;
  GEOMImpl_IGroupOperations*     _GroupOperations;
  GEOMImpl_IFieldOperations*     _FieldOperations;
};

#endif
