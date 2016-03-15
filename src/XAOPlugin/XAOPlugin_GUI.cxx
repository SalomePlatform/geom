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
#include "XAOPlugin_GUI.h"
#include "XAOPlugin_ExportDlg.h"
#include "XAOPlugin_ImportDlg.h"

// GUI includes
#include <SUIT_Desktop.h>
#include <SalomeApp_Application.h>

// GEOM includes
#include "GeometryGUI.h"

//=======================================================================
// function : XAOPlugin_GUI()
// purpose  : Constructor
//=======================================================================
XAOPlugin_GUI::XAOPlugin_GUI( GeometryGUI* parent ) : GEOMPluginGUI( parent )
{
}

//=======================================================================
// function : ~XAOPlugin_GUI
// purpose  : Destructor
//=======================================================================
XAOPlugin_GUI::~XAOPlugin_GUI()
{
}

//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool XAOPlugin_GUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  switch ( theCommandID ) {
  case 1:
    return OnGUIEvent("Export_XAO", parent);
  case 2:
    return OnGUIEvent("Import_XAO", parent);
  default:
    return OnGUIEvent("", parent);
  }
  return false;
}

//=======================================================================
// function : OnGUIEvent()
// purpose  :
//=======================================================================
bool XAOPlugin_GUI::OnGUIEvent( const QString& theCommandID, SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if (!app) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();

  QDialog* dialog = NULL;
  if( theCommandID == "Export_XAO" )
    dialog = new XAOPlugin_ExportDlg(getGeometryGUI(), parent);
  else if( theCommandID == "Import_XAO" )
    dialog = new XAOPlugin_ImportDlg(getGeometryGUI(), parent);
  else
    app->putInfo( tr("GEOM_PRP_COMMAND").arg( theCommandID ) );

  if (dialog != NULL)
    dialog->show();

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
  GEOMGUI* GetLibGUI(GeometryGUI* parent)
  {
    return new XAOPlugin_GUI(parent);
  }
}
