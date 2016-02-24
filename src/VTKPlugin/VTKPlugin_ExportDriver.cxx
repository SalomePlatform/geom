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
#include "VTKPlugin_ExportDriver.hxx"
#include "VTKPlugin_IExport.hxx"

// KERNEL includes
#include <utilities.h>
#include <Basics_Utils.hxx>

// GEOM includes
#include "GEOM_Function.hxx"
#include "OCC2VTK_Tools.h"
#include "GEOM_VertexSource.h"
#include "GEOM_EdgeSource.h"
#include "GEOM_WireframeFace.h"
#include "GEOM_ShadingFace.h"

// OOCT includes
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <Poly_Triangulation.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

// VTK includes
#include <vtkAppendPolyData.h>
#include <vtkPolyDataWriter.h>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& VTKPlugin_ExportDriver::GetID()
{
  static Standard_GUID aGUID("0966443c-6f3c-4ebe-ba46-c0833786d817");
  return aGUID;
}

//=======================================================================
//function : VTKPlugin_ExportDriver
//purpose  :
//=======================================================================
VTKPlugin_ExportDriver::VTKPlugin_ExportDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer VTKPlugin_ExportDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction( Label() );

  VTKPlugin_IExport aData (aFunction);

  // retrieve the being exported shape
  TopoDS_Shape aShape;
  Handle(GEOM_Function) aRefFunction = aData.GetOriginal();
  if( aRefFunction.IsNull() ) return 0;
  aShape = aRefFunction->GetValue();
  if( aShape.IsNull() ) return 0;
  // set the result of function to be used by next operations
  aFunction->SetValue( aShape );

  TCollection_AsciiString aFileName = aData.GetFileName();
  float aDeflection = float( aData.GetDeflection() );

  MESSAGE( "Export VTK into file " << aFileName );
  try
  {
    // Set "C" numeric locale to save numbers correctly
    Kernel_Utils::Localizer loc;

    vtkPolyData* pd = GEOM::GetVTKData( aShape, aDeflection );
    vtkPolyDataWriter* aWriter = vtkPolyDataWriter::New();    
    aWriter->SetInputData( pd );
    aWriter->SetFileName( aFileName.ToCString() );
    aWriter->Write();
    aWriter->Delete();
    pd->Delete(); //instanciated by the GEOM::GetData(...) method

    return 1;
  }
  catch( Standard_Failure )
  {
    //THROW_SALOME_CORBA_EXCEPTION("Exception catched in ExportVTK", SALOME::BAD_PARAM);
  }
  return 0;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================
bool VTKPlugin_ExportDriver::
GetCreationInformation( std::string&             theOperationName,
                        std::vector<GEOM_Param>& theParams )
{
  return false;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT( VTKPlugin_ExportDriver,GEOM_BaseDriver );
