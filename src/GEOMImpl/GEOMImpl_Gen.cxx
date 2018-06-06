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

#ifdef WIN32
#pragma warning( disable:4786 )
#endif

#include <Standard_Stream.hxx>

#include <GEOMImpl_Gen.hxx>

#include "utilities.h"
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>

#include <TFunction_Driver.hxx>
#include <TFunction_DriverTable.hxx>

#include <GEOMImpl_PointDriver.hxx>
#include <GEOMImpl_VectorDriver.hxx>
#include <GEOMImpl_LineDriver.hxx>
#include <GEOMImpl_PlaneDriver.hxx>
#include <GEOMImpl_MarkerDriver.hxx>
#include <GEOMImpl_ArcDriver.hxx>
#include <GEOMImpl_CircleDriver.hxx>
#include <GEOMImpl_EllipseDriver.hxx>
#include <GEOMImpl_PolylineDriver.hxx>
#include <GEOMImpl_SplineDriver.hxx>
#include <GEOMImpl_SketcherDriver.hxx>
#include <GEOMImpl_3DSketcherDriver.hxx>
#include <GEOMImpl_BoxDriver.hxx>
#include <GEOMImpl_FaceDriver.hxx>
#include <GEOMImpl_DiskDriver.hxx>
#include <GEOMImpl_ConeDriver.hxx>
#include <GEOMImpl_CylinderDriver.hxx>
#include <GEOMImpl_PrismDriver.hxx>
#include <GEOMImpl_PipeDriver.hxx>
#include <GEOMImpl_PipePathDriver.hxx>
#include <GEOMImpl_ThruSectionsDriver.hxx>
#include <GEOMImpl_RevolutionDriver.hxx>
#include <GEOMImpl_ShapeDriver.hxx>
#include <GEOMImpl_BlockDriver.hxx>
#include <GEOMImpl_SphereDriver.hxx>
#include <GEOMImpl_TorusDriver.hxx>
#include <GEOMImpl_BooleanDriver.hxx>
#include <GEOMImpl_ChamferDriver.hxx>
#include <GEOMImpl_FilletDriver.hxx>
#include <GEOMImpl_Fillet1dDriver.hxx>
#include <GEOMImpl_Fillet2dDriver.hxx>
#include <GEOMImpl_TranslateDriver.hxx>
#include <GEOMImpl_RotateDriver.hxx>
#include <GEOMImpl_MirrorDriver.hxx>
#include <GEOMImpl_ProjectionDriver.hxx>
#include <GEOMImpl_OffsetDriver.hxx>
#include <GEOMImpl_ScaleDriver.hxx>
#include <GEOMImpl_PositionDriver.hxx>
#include <GEOMImpl_PartitionDriver.hxx>
#include <GEOMImpl_CopyDriver.hxx>
#include <GEOMImpl_ExportDriver.hxx>
#include <GEOMImpl_ImportDriver.hxx>
#include <GEOMImpl_ArchimedeDriver.hxx>
#include <GEOMImpl_HealingDriver.hxx>
#include <GEOMImpl_FillingDriver.hxx>
#include <GEOMImpl_GlueDriver.hxx>
#include <GEOMImpl_MeasureDriver.hxx>
#include <GEOMImpl_FieldDriver.hxx>

//=============================================================================
/*!
 *  default constructor:
 */
//=============================================================================

GEOMImpl_Gen::GEOMImpl_Gen()
{
   MESSAGE("GEOMImpl_Gen::GEOMImpl_Gen");

   // Basic elements
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_PointDriver::GetID(), new GEOMImpl_PointDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_VectorDriver::GetID(), new GEOMImpl_VectorDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_LineDriver::GetID(), new GEOMImpl_LineDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_PlaneDriver::GetID(), new GEOMImpl_PlaneDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_MarkerDriver::GetID(), new GEOMImpl_MarkerDriver());

   // Curves
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_ArcDriver::GetID(), new GEOMImpl_ArcDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_CircleDriver::GetID(), new GEOMImpl_CircleDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_EllipseDriver::GetID(), new GEOMImpl_EllipseDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_PolylineDriver::GetID(), new GEOMImpl_PolylineDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_SplineDriver::GetID(), new GEOMImpl_SplineDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_SketcherDriver::GetID(), new GEOMImpl_SketcherDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_3DSketcherDriver::GetID(), new GEOMImpl_3DSketcherDriver());

   // 3D Primitives
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_BoxDriver::GetID(), new GEOMImpl_BoxDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_FaceDriver::GetID(), new GEOMImpl_FaceDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_DiskDriver::GetID(), new GEOMImpl_DiskDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_ConeDriver::GetID(), new GEOMImpl_ConeDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_CylinderDriver::GetID(), new GEOMImpl_CylinderDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_PrismDriver::GetID(), new GEOMImpl_PrismDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_PipeDriver::GetID(), new GEOMImpl_PipeDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_PipePathDriver::GetID(), new GEOMImpl_PipePathDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_ThruSectionsDriver::GetID(), new GEOMImpl_ThruSectionsDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_RevolutionDriver::GetID(), new GEOMImpl_RevolutionDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_SphereDriver::GetID(), new GEOMImpl_SphereDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_TorusDriver::GetID(), new GEOMImpl_TorusDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_FillingDriver::GetID(), new GEOMImpl_FillingDriver());

   // Shapes Operations
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_ShapeDriver::GetID(), new GEOMImpl_ShapeDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_GlueDriver::GetID(), new GEOMImpl_GlueDriver());

   // Blocks Operations
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_BlockDriver::GetID(), new GEOMImpl_BlockDriver());

   // Boolean Operations, Partition
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_BooleanDriver::GetID(), new GEOMImpl_BooleanDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_PartitionDriver::GetID(), new GEOMImpl_PartitionDriver());

   // Local Operations
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_ChamferDriver::GetID(), new GEOMImpl_ChamferDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_FilletDriver::GetID(), new GEOMImpl_FilletDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_Fillet1dDriver::GetID(), new GEOMImpl_Fillet1dDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_Fillet2dDriver::GetID(), new GEOMImpl_Fillet2dDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_ArchimedeDriver::GetID(), new GEOMImpl_ArchimedeDriver());

   // Geometrical Transformations, Offset, Scale
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_TranslateDriver::GetID(), new GEOMImpl_TranslateDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_RotateDriver::GetID(), new GEOMImpl_RotateDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_MirrorDriver::GetID(), new GEOMImpl_MirrorDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_ProjectionDriver::GetID(), new GEOMImpl_ProjectionDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_OffsetDriver::GetID(), new GEOMImpl_OffsetDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_ScaleDriver::GetID(), new GEOMImpl_ScaleDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_PositionDriver::GetID(), new GEOMImpl_PositionDriver());

   // Insert Operations (Copy, Import/Export)
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_CopyDriver::GetID(), new GEOMImpl_CopyDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_ImportDriver::GetID(), new GEOMImpl_ImportDriver());
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_ExportDriver::GetID(), new GEOMImpl_ExportDriver());

   // Shape Healing
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_HealingDriver::GetID(), new GEOMImpl_HealingDriver());

   // Measurements
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_MeasureDriver::GetID(), new GEOMImpl_MeasureDriver());

   // Field
   TFunction_DriverTable::Get()->AddDriver(GEOMImpl_FieldDriver::GetID(), new GEOMImpl_FieldDriver());

   /*@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@*/

   SetEngine(this);

   _BasicOperations = new GEOMImpl_IBasicOperations( this );
   _TransformOperations = new GEOMImpl_ITransformOperations( this );
   _3DPrimOperations = new GEOMImpl_I3DPrimOperations( this );
   _ShapesOperations = new GEOMImpl_IShapesOperations( this );
   _BlocksOperations = new GEOMImpl_IBlocksOperations( this );
   _BooleanOperations = new GEOMImpl_IBooleanOperations( this );
   _HealingOperations = new GEOMImpl_IHealingOperations( this );
   _CurvesOperations = new GEOMImpl_ICurvesOperations( this );
   _LocalOperations = new GEOMImpl_ILocalOperations( this );
   _InsertOperations = new GEOMImpl_IInsertOperations( this );
   _MeasureOperations = new GEOMImpl_IMeasureOperations( this );
   _GroupOperations = new GEOMImpl_IGroupOperations( this );
   _FieldOperations = new GEOMImpl_IFieldOperations( this );
}

//=============================================================================
/*!
 *
 */
//=============================================================================

GEOMImpl_Gen::~GEOMImpl_Gen()
{
  MESSAGE("GEOMImpl_Gen::~GEOMImpl_Gen");

  delete _BasicOperations;
  delete _TransformOperations;
  delete _3DPrimOperations;
  delete _ShapesOperations;
  delete _BlocksOperations;
  delete _BooleanOperations;
  delete _HealingOperations;
  delete _CurvesOperations;
  delete _LocalOperations;
  delete _InsertOperations;
  delete _MeasureOperations;
  delete _GroupOperations;
  delete _FieldOperations;
}

//=============================================================================
/*!
 * GetIBasicOperations
 */
//=============================================================================
GEOMImpl_IBasicOperations* GEOMImpl_Gen::GetIBasicOperations()
{
  return _BasicOperations;
}

//=============================================================================
/*!
 * GetITransformOperations
 */
//=============================================================================
GEOMImpl_ITransformOperations* GEOMImpl_Gen::GetITransformOperations()
{
  return _TransformOperations;
}

//=============================================================================
/*!
 * GetIBooleanOperations
 */
//=============================================================================
GEOMImpl_IBooleanOperations* GEOMImpl_Gen::GetIBooleanOperations()
{
  return _BooleanOperations;
}

//=============================================================================
/*!
 * GetIHealingOperations
 */
//=============================================================================
GEOMImpl_IHealingOperations* GEOMImpl_Gen::GetIHealingOperations()
{
  return _HealingOperations;
}

//=============================================================================
/*!
 * GetI3DPrimOperations
 */
//=============================================================================
GEOMImpl_I3DPrimOperations* GEOMImpl_Gen::GetI3DPrimOperations()
{
  return _3DPrimOperations;
}

//=============================================================================
/*!
 * GetIShapesOperations
 */
//=============================================================================
GEOMImpl_IShapesOperations* GEOMImpl_Gen::GetIShapesOperations()
{
  return _ShapesOperations;
}

//=============================================================================
/*!
 * GetIBlocksOperations
 */
//=============================================================================
GEOMImpl_IBlocksOperations* GEOMImpl_Gen::GetIBlocksOperations()
{
  return _BlocksOperations;
}

//=============================================================================
/*!
 * GetICurvesOperations
 */
//=============================================================================
GEOMImpl_ICurvesOperations* GEOMImpl_Gen::GetICurvesOperations()
{
  return _CurvesOperations;
}

//=============================================================================
/*!
 * GetILocalOperations
 */
//=============================================================================
GEOMImpl_ILocalOperations* GEOMImpl_Gen::GetILocalOperations()
{
  return _LocalOperations;
}

//=============================================================================
/*!
 * GetIInsertOperations
 */
//=============================================================================
GEOMImpl_IInsertOperations* GEOMImpl_Gen::GetIInsertOperations()
{
  return _InsertOperations;
}

//=============================================================================
/*!
 * GetIMeasureOperations
 */
//=============================================================================
GEOMImpl_IMeasureOperations* GEOMImpl_Gen::GetIMeasureOperations()
{
  return _MeasureOperations;
}

//=============================================================================
/*!
 * GetIGroupOperations
 */
//=============================================================================
GEOMImpl_IGroupOperations* GEOMImpl_Gen::GetIGroupOperations()
{
  return _GroupOperations;
}

//=============================================================================
/*!
 * GetIFieldOperations
 */
//=============================================================================
GEOMImpl_IFieldOperations* GEOMImpl_Gen::GetIFieldOperations()
{
  return _FieldOperations;
}
