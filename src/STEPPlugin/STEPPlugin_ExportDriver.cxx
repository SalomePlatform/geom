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
#include "STEPPlugin_ExportDriver.hxx"
#include "STEPPlugin_IExport.hxx"
#include "STEPPlugin_IOperations.hxx"

// KERNEL includes
#include <utilities.h>
#include <Basics_Utils.hxx>

// GEOM includes
#include "GEOM_Function.hxx"

// OOCT includes
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_Writer.hxx>
#include <Interface_Static.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& STEPPlugin_ExportDriver::GetID()
{
  static Standard_GUID aGUID("c47b9a61-c6a4-4335-8b13-6badb16b036f");
  return aGUID;
}

//=======================================================================
//function : STEPPlugin_ExportDriver
//purpose  :
//=======================================================================
STEPPlugin_ExportDriver::STEPPlugin_ExportDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer STEPPlugin_ExportDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction( Label() );

  STEPPlugin_IExport aData (aFunction);

  // retrieve the being exported shape
  TopoDS_Shape aShape;
  Handle(GEOM_Function) aRefFunction = aData.GetOriginal();
  if( aRefFunction.IsNull() ) return 0;
  aShape = aRefFunction->GetValue();
  if( aShape.IsNull() ) return 0;
  // set the result of function to be used by next operations
  aFunction->SetValue( aShape );

  TCollection_AsciiString aFileName = aData.GetFileName();
  Standard_Integer        anUnit    = aData.GetUnit();
  TCollection_AsciiString aWriteUnit;

  switch (anUnit) {
  case STEPPlugin_IOperations::LengthUnit_Inch:
    aWriteUnit = "INCH";
    break;
  case STEPPlugin_IOperations::LengthUnit_Millimeter:
    aWriteUnit = "MM";
    break;
  case STEPPlugin_IOperations::LengthUnit_Foot:
    aWriteUnit = "FT";
    break;
  case STEPPlugin_IOperations::LengthUnit_Mile:
    aWriteUnit = "MI";
    break;
  case STEPPlugin_IOperations::LengthUnit_Meter:
    aWriteUnit = "M";
    break;
  case STEPPlugin_IOperations::LengthUnit_Kilometer:
    aWriteUnit = "KM";
    break;
  case STEPPlugin_IOperations::LengthUnit_Milliinch:
    aWriteUnit = "MIL";
    break;
  case STEPPlugin_IOperations::LengthUnit_Micrometer:
    aWriteUnit = "UM";
    break;
  case STEPPlugin_IOperations::LengthUnit_Centimeter:
    aWriteUnit = "CM";
    break;
  case STEPPlugin_IOperations::LengthUnit_Microinch:
    aWriteUnit = "UIN";
    break;
  default:
    return 0;
  }

  MESSAGE("Export STEP into file " << aFileName.ToCString());

  try
  {
    // Set "C" numeric locale to save numbers correctly
    Kernel_Utils::Localizer loc;

    IFSelect_ReturnStatus status ;
    //VRV: OCC 4.0 migration
    STEPControl_Writer aWriter;
    Interface_Static::SetCVal("xstep.cascade.unit","M");
    Interface_Static::SetCVal("write.step.unit", aWriteUnit.ToCString());
    Interface_Static::SetIVal("write.step.nonmanifold", 1);
    status = aWriter.Transfer( aShape, STEPControl_AsIs );
    //VRV: OCC 4.0 migration
    if( status == IFSelect_RetDone )
      status = aWriter.Write( aFileName.ToCString() );

    // Return previous locale
    if( status == IFSelect_RetDone )
      return 1;
  }
  catch (Standard_Failure)
  {
      //THROW_SALOME_CORBA_EXCEPTION("Exception catched in STEPExport", SALOME::BAD_PARAM);
  }
  return 0;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================
bool STEPPlugin_ExportDriver::
GetCreationInformation( std::string&             theOperationName,
                        std::vector<GEOM_Param>& theParams )
{
  return false;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT( STEPPlugin_ExportDriver,GEOM_BaseDriver );
