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
//  File   : MeasureGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "MeasureGUI.h"
#include "QAD_Desktop.h"

#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include "MeasureGUI_PropertiesDlg.h"    // Method PROPERTIES
#include "MeasureGUI_CenterMassDlg.h"    // Method CENTER MASS
#include "MeasureGUI_InertiaDlg.h"       // Method INERTIA
#include "MeasureGUI_BndBoxDlg.h"        // Method BNDBOX
#include "MeasureGUI_DistanceDlg.h"      // Method DISTANCE
#include "MeasureGUI_MaxToleranceDlg.h"  // Method MAXTOLERANCE
#include "MeasureGUI_WhatisDlg.h"        // Method WHATIS
#include "MeasureGUI_CheckShapeDlg.h"    // Method CHECKSHAPE
#include "MeasureGUI_CheckCompoundOfBlocksDlg.h" // Method CHECKCOMPOUND
#include "MeasureGUI_PointDlg.h"         // Method POINTCOORDINATES

MeasureGUI* MeasureGUI::myGUIObject = 0;

//=======================================================================
// function : GetMeasureGUI()
// purpose  : Get the only MeasureGUI object [ static ]
//=======================================================================
MeasureGUI* MeasureGUI::GetMeasureGUI()
{
  if ( myGUIObject == 0 ) {
    // init MeasureGUI only once
    myGUIObject = new MeasureGUI();
  }
  return myGUIObject;
}

//=======================================================================
// function : MeasureGUI()
// purpose  : Constructor
//=======================================================================
MeasureGUI::MeasureGUI() : GEOMGUI()
{
}


//=======================================================================
// function : ~MeasureGUI()
// purpose  : Destructor
//=======================================================================
MeasureGUI::~MeasureGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool MeasureGUI::OnGUIEvent( int theCommandID, QAD_Desktop* parent )
{
  MeasureGUI* myMeasureGUI = GetMeasureGUI();
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(
    QAD_Application::getDesktop()->getActiveStudy()->getSelection() );

  switch ( theCommandID )
  {
    case 701   : new MeasureGUI_PropertiesDlg  ( parent, Sel ); break;  // LENGTH, AREA AND VOLUME
    case 702   : new MeasureGUI_CenterMassDlg  ( parent, Sel ); break;  // CENTER MASS
    case 703   : new MeasureGUI_InertiaDlg     ( parent, Sel ); break;  // INERTIA
    case 7041  : new MeasureGUI_BndBoxDlg      ( parent, Sel ); break;  // BOUNDING BOX
    case 7042  : new MeasureGUI_DistanceDlg    ( parent, Sel ); break;  // MIN DISTANCE
    case 705   : new MeasureGUI_MaxToleranceDlg( parent, Sel ); break;  // MAXTOLERANCE
    case 706   : new MeasureGUI_WhatisDlg      ( parent, Sel ); break;  // WHATIS
    case 707   : new MeasureGUI_CheckShapeDlg  ( parent, Sel ); break;  // CHECKSHAPE
    case 7072  : new MeasureGUI_CheckCompoundOfBlocksDlg  ( parent, Sel ); break;  // CHECKCOMPOUND
    case 708   : new MeasureGUI_PointDlg       ( parent, Sel ); break;  // POINT COORDINATES
    
    default: parent->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) ); break;
  }
  return true;
}


//=====================================================================================
// function : MakeCDGAndDisplay()
// purpose  :
//=====================================================================================
/*void MeasureGUI::MakeCDGAndDisplay(GEOM::GEOM_Shape_ptr Shape)
{
  QAD_Application::getDesktop()->putInfo("MakeCDGAndDisplay method from MeasureGUI should be reimplemented ...");
  
  try {
    GEOM::GEOM_Shape_var result = myGeom->MakeCDG(Shape);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return;
    }
    result->NameType(tr("GEOM_POINT"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }  
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
    }
  
  return;
}
*/


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  GEOMGUI* GetLibGUI()
  {
    return MeasureGUI::GetMeasureGUI();
  }
}
