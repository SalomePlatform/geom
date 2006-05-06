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
#include "SUIT_Desktop.h"
#include "SUIT_Session.h"

#include "SalomeApp_Tools.h"

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
MeasureGUI* MeasureGUI::GetMeasureGUI( GeometryGUI* parent )
{
  if ( myGUIObject == 0 ) {
    // init MeasureGUI only once
    myGUIObject = new MeasureGUI( parent );
  }
  return myGUIObject;
}

//=======================================================================
// function : MeasureGUI()
// purpose  : Constructor
//=======================================================================
MeasureGUI::MeasureGUI( GeometryGUI* parent ) : GEOMGUI( parent )
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
bool MeasureGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  MeasureGUI* myMeasureGUI = GetMeasureGUI( getGeometryGUI() );
  getGeometryGUI()->EmitSignalDeactivateDialog();

  switch ( theCommandID )
  {
    case 701 : new MeasureGUI_PropertiesDlg  (getGeometryGUI(), parent); break; // LENGTH, AREA AND VOLUME
    case 702 : new MeasureGUI_CenterMassDlg  (getGeometryGUI(), parent); break; // CENTER MASS
    case 703 : new MeasureGUI_InertiaDlg     (getGeometryGUI(), parent); break; // INERTIA
    case 7041: new MeasureGUI_BndBoxDlg      (getGeometryGUI(), parent); break; // BOUNDING BOX
    case 7042: new MeasureGUI_DistanceDlg    (getGeometryGUI(), parent); break; // MIN DISTANCE
    case 705 : new MeasureGUI_MaxToleranceDlg(getGeometryGUI(), parent); break; // MAXTOLERANCE
    case 706 : new MeasureGUI_WhatisDlg      (getGeometryGUI(), parent); break; // WHATIS
    case 707 : new MeasureGUI_CheckShapeDlg  (getGeometryGUI(), parent); break; // CHECKSHAPE
    case 7072: new MeasureGUI_CheckCompoundOfBlocksDlg  (getGeometryGUI(), parent); break; // CHECKCOMPOUND
    case 708 : new MeasureGUI_PointDlg       (getGeometryGUI(), parent); break; // POINT COORDINATES

    default: 
      SUIT_Session::session()->activeApplication()->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) ); 
      break;
  }
  return true;
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
#ifdef WNT
	__declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return MeasureGUI::GetMeasureGUI( parent );
  }
}
