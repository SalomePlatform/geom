// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
#include "GeometryGUI_Operations.h"

#include <SUIT_Desktop.h>
#include <SalomeApp_Application.h>

#include "AdvancedGUI_PipeTShapeDlg.h"
#include "AdvancedGUI_DividedDiskDlg.h"
#include "AdvancedGUI_DividedCylinderDlg.h"
//@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@//

#include <QDialog>

//=======================================================================
// function : AdvancedGUI()
// purpose  : Constructor
//=======================================================================
AdvancedGUI::AdvancedGUI( GeometryGUI* parent ) : GEOMGUI( parent )
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
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();

  QDialog* aDlg = NULL;

  switch ( theCommandID ) {
  case GEOMOp::OpPipeTShape:
    aDlg = new AdvancedGUI_PipeTShapeDlg( getGeometryGUI(), parent );
    break;
//   case GEOMOp::OpPipeTShapeGroups:
//     aDlg = new AdvancedGUI_PipeTShapeGroupsDlg( getGeometryGUI(), parent );
//     break;
  case GEOMOp::OpDividedDisk:
    aDlg = new AdvancedGUI_DividedDiskDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpDividedCylinder:
    aDlg = new AdvancedGUI_DividedCylinderDlg( getGeometryGUI(), parent );
    break;
  //@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@//
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
    return new AdvancedGUI( parent );
  }
}
