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
// File   : GenerationGUI.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.

#include "GenerationGUI.h"

#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"

#include <SUIT_Desktop.h>
#include <SalomeApp_Application.h>

#include "GenerationGUI_PrismDlg.h"     // Method PRISM
#include "GenerationGUI_RevolDlg.h"     // Method REVOL
#include "GenerationGUI_FillingDlg.h"   // Method FILLING
#include "GenerationGUI_PipeDlg.h"      // Method PIPE
#include "GenerationGUI_PipePathDlg.h"  // Method RESTORE PATH
#include "GenerationGUI_ThicknessDlg.h" // Method THICKNESS

//=======================================================================
// function : GenerationGUI()
// purpose  : Constructor
//=======================================================================
GenerationGUI::GenerationGUI( GeometryGUI* parent )
  : GEOMGUI( parent )
{
}

//=======================================================================
// function : ~GenerationGUI()
// purpose  : Destructor
//=======================================================================
GenerationGUI::~GenerationGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool GenerationGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();

  QDialog* aDlg = NULL;

  switch ( theCommandID ) {
  case GEOMOp::OpPrism:      aDlg = new GenerationGUI_PrismDlg    ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpRevolution: aDlg = new GenerationGUI_RevolDlg    ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpFilling:    aDlg = new GenerationGUI_FillingDlg  ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpPipe:       aDlg = new GenerationGUI_PipeDlg     ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpPipePath:   aDlg = new GenerationGUI_PipePathDlg ( getGeometryGUI(), parent ); break;
  case GEOMOp::OpThickness:  aDlg = new GenerationGUI_ThicknessDlg( getGeometryGUI(), parent ); break;
    
  default: app->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) ); break;
  }

  if (aDlg != NULL )
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
    return new GenerationGUI( parent );
  }
}
