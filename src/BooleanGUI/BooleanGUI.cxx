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
#include "BooleanGUI.h"
#include "BooleanGUI_Dialog.h"

#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"
#include <SUIT_Desktop.h>

//=======================================================================
// function : BooleanGUI()
// purpose  : Constructor
//=======================================================================
BooleanGUI::BooleanGUI( GeometryGUI* parent ) 
  : GEOMGUI( parent )
{
}

//=======================================================================
// function : ~BooleanGUI()
// purpose  : Destructor
//=======================================================================
BooleanGUI::~BooleanGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool BooleanGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  getGeometryGUI()->EmitSignalDeactivateDialog();

  int anOperation = 0;  
  switch ( theCommandID ) {
  case GEOMOp::OpFuse:
    anOperation = FUSE;    break;
  case GEOMOp::OpCommon:
    anOperation = COMMON;  break;
  case GEOMOp::OpCut:
    anOperation = CUT;     break;
  case GEOMOp::OpSection:
    anOperation = SECTION; break;
  default:
    return false;
  }
  
  QDialog* aDlg = new BooleanGUI_Dialog( anOperation, getGeometryGUI(), parent );
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
    return new BooleanGUI( parent );
  }
}
