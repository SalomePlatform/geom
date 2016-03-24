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
// File   : BooleanGUI.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "BuildGUI.h"

#include <SUIT_Desktop.h>
#include <SalomeApp_Application.h>

#include "BuildGUI_EdgeDlg.h"       // Method EDGE
#include "BuildGUI_WireDlg.h"       // Method WIRE
#include "BuildGUI_FaceDlg.h"       // Method FACE
#include "BuildGUI_ShellDlg.h"      // Method SHELL
#include "BuildGUI_SolidDlg.h"      // Method SOLID
#include "BuildGUI_CompoundDlg.h"   // Method COMPOUND

#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"

//=======================================================================
// function : BuildGUI()
// purpose  : Constructor
//=======================================================================
BuildGUI::BuildGUI( GeometryGUI* parent )
  : GEOMGUI( parent )
{
}

//=======================================================================
// function : ~BuildGUI()
// purpose  : Destructor
//=======================================================================
BuildGUI::~BuildGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool BuildGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  getGeometryGUI()->EmitSignalDeactivateDialog();

  QDialog* aDlg = 0;

  switch ( theCommandID ) {
  case GEOMOp::OpEdge:     aDlg = new BuildGUI_EdgeDlg    ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpWire:     aDlg = new BuildGUI_WireDlg    ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpFace:     aDlg = new BuildGUI_FaceDlg    ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpShell:    aDlg = new BuildGUI_ShellDlg   ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpSolid:    aDlg = new BuildGUI_SolidDlg   ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpCompound: aDlg = new BuildGUI_CompoundDlg( getGeometryGUI(), parent ); break;
  default: 
    getGeometryGUI()->getApp()->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) ); 
    break;
  }
  
  if ( aDlg ) {
    aDlg->updateGeometry();
    aDlg->resize( aDlg->minimumSizeHint() );
    aDlg->show();
  }

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
    return new BuildGUI( parent );
  }
}
