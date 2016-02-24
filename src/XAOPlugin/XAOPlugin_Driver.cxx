// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include "XAOPlugin_Driver.hxx"
#include "XAOPlugin_IImportExport.hxx"

// GEOM includes
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include <XAO_Xao.hxx>
#include <XAO_Geometry.hxx>
#include <XAO_Group.hxx>
#include <XAO_XaoUtils.hxx>

// OCC includes
#include <Standard_Stream.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& XAOPlugin_Driver::GetID()
{
    static Standard_GUID aGUID("FF1BBB71-5D14-4df2-980B-3A668264EA16");
    return aGUID;
}

//=======================================================================
//function : XAOPlugin_Driver
//purpose  :
//=======================================================================
XAOPlugin_Driver::XAOPlugin_Driver()
{
}

//=======================================================================
//function : ~XAOPlugin_Driver
//purpose  :
//=======================================================================
XAOPlugin_Driver::~XAOPlugin_Driver()
{
}

//function : Execute
//purpose  :
//=======================================================================
Standard_Integer XAOPlugin_Driver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  XAOPlugin_IImportExport iexao(function);
  TCollection_AsciiString xao   = iexao.GetData();

  TopoDS_Shape shape;

  Standard_Integer functionType = function->GetType();
  if (functionType == EXPORT_SHAPE)
  {
  }
  else if (functionType == IMPORT_SHAPE)
  {
  }
  else
  {
    // other construction modes here
  }

  if (shape.IsNull()) return 0;
  function->SetValue(shape);

#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif

  return 1;
}

//=======================================================================
//function : GetCreationInformation
//purpose  : Returns a name of creation operation and names and values of
//           creation parameters
//=======================================================================

bool XAOPlugin_Driver::GetCreationInformation(std::string&             theOperationName,
                                              std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return false;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  theOperationName = "ImportXAO";
  AddParam( theParams, "File name", function->GetString( GetFileNameTag() ));
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT(XAOPlugin_Driver, GEOM_BaseDriver);
 
