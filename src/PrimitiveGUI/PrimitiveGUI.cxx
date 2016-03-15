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

// GEOM GEOMGUI : GUI for Geometry component
// File   : PrimitiveGUI.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "PrimitiveGUI.h"

#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"

#include <SUIT_Desktop.h>
#include <SalomeApp_Application.h>

#include "PrimitiveGUI_BoxDlg.h"      // Method BOX
#include "PrimitiveGUI_CylinderDlg.h" // Method CYLINDER
#include "PrimitiveGUI_SphereDlg.h"   // Method SPHERE
#include "PrimitiveGUI_TorusDlg.h"    // Method TORUS
#include "PrimitiveGUI_ConeDlg.h"     // Method CONE
#include "PrimitiveGUI_FaceDlg.h"     // Method FACE
#include "PrimitiveGUI_DiskDlg.h"     // Method DISK

//=======================================================================
// function : PrimitiveGUI()
// purpose  : Constructor
//=======================================================================
PrimitiveGUI::PrimitiveGUI( GeometryGUI* parent ) : GEOMGUI( parent )
{
}

//=======================================================================
// function : ~PrimitiveGUI
// purpose  : Destructor
//=======================================================================
PrimitiveGUI::~PrimitiveGUI()
{
}

//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool PrimitiveGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();

  QDialog* aDlg = NULL;

  switch ( theCommandID ) {
  case GEOMOp::OpBox:       // BOX
    aDlg = new PrimitiveGUI_BoxDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpCylinder:  // CYLINDER
    aDlg = new PrimitiveGUI_CylinderDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpSphere:    // SPHERE
    aDlg = new PrimitiveGUI_SphereDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpTorus:     // TORUS
    aDlg = new PrimitiveGUI_TorusDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpCone:      // CONE
    aDlg = new PrimitiveGUI_ConeDlg( getGeometryGUI(), parent);
    break;
  case GEOMOp::OpRectangle: // FACE
    aDlg = new PrimitiveGUI_FaceDlg( getGeometryGUI(), parent);
    break;
  case GEOMOp::OpDisk:      // DISK
    aDlg = new PrimitiveGUI_DiskDlg( getGeometryGUI(), parent);
    break;
  default:
    app->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) );
    break;
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
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new PrimitiveGUI( parent );
  }
}
