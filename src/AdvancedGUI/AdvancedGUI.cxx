// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  File   : AdvancedGUI.cxx
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "AdvancedGUI.h"

#include "GeometryGUI.h"
//#include "GeometryGUI_Operations.h"

#include <SUIT_Desktop.h>
#include <SalomeApp_Application.h>

#include "AdvancedGUI_PipeTShapeDlg.h"
#include "AdvancedGUI_DividedDiskDlg.h"
#include "AdvancedGUI_DividedCylinderDlg.h"
#include "AdvancedGUI_SmoothingSurfaceDlg.h"
//@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@//

#include <QDialog>

#include <iostream>

//=======================================================================
// function : AdvancedGUI()
// purpose  : Constructor
//=======================================================================
AdvancedGUI::AdvancedGUI( GeometryGUI* parent ) : GEOMPluginGUI( parent )
{
}

//=======================================================================
// function : ~AdvancedGUI
// purpose  : Destructor
//=======================================================================
AdvancedGUI::~AdvancedGUI()
{
}

//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool AdvancedGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  switch ( theCommandID ) {
  case 1:
    return OnGUIEvent("TShape_Basic", parent);
  case 2:
    return OnGUIEvent("DividedDisk", parent);
  case 3:
    return OnGUIEvent("DividedCylinder", parent);
  case 4:
    return OnGUIEvent("SmoothingSurface", parent);
  //@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@//
  default:
    return OnGUIEvent("", parent);
  }
  return false;
}

//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool AdvancedGUI::OnGUIEvent( const QString& theCommandID, SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();

  QDialog* aDlg = NULL;

  if (theCommandID == "TShape_Basic") {
    aDlg = new AdvancedGUI_PipeTShapeDlg( getGeometryGUI(), parent );
  }
  //else if (theCommandID == "TShape_Groups") {
  //  aDlg = new AdvancedGUI_PipeTShapeGroupsDlg( getGeometryGUI(), parent );
  //}
  else if (theCommandID == "DividedDisk") {
    aDlg = new AdvancedGUI_DividedDiskDlg( getGeometryGUI(), parent );
  }
  else if (theCommandID == "DividedCylinder") {
    aDlg = new AdvancedGUI_DividedCylinderDlg( getGeometryGUI(), parent );
  }
  else if (theCommandID == "SmoothingSurface") {
    aDlg = new AdvancedGUI_SmoothingSurfaceDlg( getGeometryGUI(), parent );
  }
  //@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@//
  else {
    app->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) );
  }

  if ( aDlg != NULL )
    aDlg->show();

  return true;
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
#ifdef WIN32
  __declspec( dllexport )
#endif
  GEOMPluginGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new AdvancedGUI( parent );
  }
}
