//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GenerationGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

using namespace std;
#include "GenerationGUI.h"

#include "SALOMEGUI_QtCatchCorbaException.hxx"
#include "QAD_Desktop.h"

#include "GenerationGUI_PrismDlg.h"     // Method PRISM
#include "GenerationGUI_RevolDlg.h"     // Method REVOL
#include "GenerationGUI_FillingDlg.h"   // Method FILLING
#include "GenerationGUI_PipeDlg.h"      // Method PIPE

GenerationGUI* GenerationGUI::myGUIObject = 0;

//=======================================================================
// function : GetGenerationGUI()
// purpose  : Get the only GenerationGUI object [ static ]
//=======================================================================
GenerationGUI* GenerationGUI::GetGenerationGUI()
{
  if ( myGUIObject == 0 )
    myGUIObject = new GenerationGUI();

  return myGUIObject;
}

//=======================================================================
// function : GenerationGUI()
// purpose  : Constructor
//=======================================================================
GenerationGUI::GenerationGUI() : GEOMGUI()
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
bool GenerationGUI::OnGUIEvent( int theCommandID, QAD_Desktop* parent )
{
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  
  SALOME_Selection* Sel = SALOME_Selection::Selection(
    QAD_Application::getDesktop()->getActiveStudy()->getSelection() );
  
  QDialog* aDlg = NULL;

  switch ( theCommandID )
  {
    case 4031: aDlg = new GenerationGUI_PrismDlg   ( parent, "", Sel ); break;
    case 4032: aDlg = new GenerationGUI_RevolDlg   ( parent, "", Sel ); break;
    case 4033: aDlg = new GenerationGUI_FillingDlg ( parent, "", Sel ); break;
    case 4034: aDlg = new GenerationGUI_PipeDlg    ( parent, "", Sel ); break;
    
    default: parent->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) ); break;
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
  GEOMGUI* GetLibGUI()
  {
    return GenerationGUI::GetGenerationGUI();
  }
}
