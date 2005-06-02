//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  CEA
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org or email : webmaster.salome@opencascade.org
//
//
//
//  File   : BooleanGUI.cxx
//  Author : Julia DOOVSKIKH
//  Module : GEOM
//  $Header$

#include "BlocksGUI.h"

#include "BlocksGUI_QuadFaceDlg.h"
#include "BlocksGUI_BlockDlg.h"
#include "BlocksGUI_TrsfDlg.h"
//#include "BlocksGUI_CheckMultiBlockDlg.h"
#include "BlocksGUI_ExplodeDlg.h"
#include "BlocksGUI_PropagateDlg.h"

#include "GeometryGUI.h"

#include "SUIT_Desktop.h"
#include "SUIT_MessageBox.h"
#include "SUIT_Session.h"

BlocksGUI* BlocksGUI::myGUIObject = 0;

//=======================================================================
// function : GetBlocksGUI()
// purpose  : Get the only BlocksGUI object [ static ]
//=======================================================================
BlocksGUI* BlocksGUI::GetBlocksGUI( GeometryGUI* parent )
{
  if ( myGUIObject == 0 )
    myGUIObject = new BlocksGUI( parent );

  return myGUIObject;
}

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

  QDialog* aDlg = NULL;

  switch (theCommandID)
  {
    case 9999:
      aDlg = new BlocksGUI_BlockDlg (parent);
      break;

    case 9998:
      aDlg = new BlocksGUI_TrsfDlg (parent);
      break;

    case 9997:
      aDlg = new BlocksGUI_QuadFaceDlg (parent);
      break;

    case 9996:
//      aDlg = new BlocksGUI_CheckMultiBlockDlg (parent, Sel);
      SUIT_MessageBox::warn1 (parent,
			      QObject::tr("WRN_WARNING"), 
			      QObject::tr("WRN_NOT_IMPLEMENTED"),
			      QObject::tr("BUT_OK"));
      break;
      
    case 9995:
      aDlg = new BlocksGUI_ExplodeDlg (parent);
      break;
    case 99991:
      aDlg = new BlocksGUI_PropagateDlg (parent, "");
      break;

    default:
      SUIT_Session::session()->activeApplication()->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
      break;
  }

  if (aDlg != NULL)
    aDlg->show();

  return true;
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return BlocksGUI::GetBlocksGUI( parent );
  }
}
