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
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//
#include "BlocksGUI.h"

#include "BlocksGUI_QuadFaceDlg.h"
#include "BlocksGUI_BlockDlg.h"
#include "BlocksGUI_TrsfDlg.h"
#include "BlocksGUI_ExplodeDlg.h"
#include "BlocksGUI_PropagateDlg.h"

#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>

//=======================================================================
// function : BlocksGUI()
// purpose  : Constructor
//=======================================================================
BlocksGUI::BlocksGUI( GeometryGUI* parent )
  : GEOMGUI( parent )
{
}

//=======================================================================
// function : ~BlocksGUI()
// purpose  : Destructor
//=======================================================================
BlocksGUI::~BlocksGUI()
{
}

//=======================================================================
// function : OnGUIEvent()
// purpose  :
//=======================================================================
bool BlocksGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  getGeometryGUI()->EmitSignalDeactivateDialog();

  QDialog* aDlg = 0;

  switch ( theCommandID ) {
  case GEOMOp::OpHexaSolid:      aDlg = new BlocksGUI_BlockDlg    ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpMultiTransform: aDlg = new BlocksGUI_TrsfDlg     ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpQuadFace:       aDlg = new BlocksGUI_QuadFaceDlg ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpExplodeBlock:   aDlg = new BlocksGUI_ExplodeDlg  ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpPropagate:      aDlg = new BlocksGUI_PropagateDlg( getGeometryGUI(), parent ); break;
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
    return new BlocksGUI( parent );
  }
}
