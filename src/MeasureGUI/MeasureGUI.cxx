// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : MeasureGUI.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.

#include "MeasureGUI.h"

#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"
#include <SUIT_Desktop.h>
#include <SalomeApp_Application.h>

#include "MeasureGUI_PropertiesDlg.h"    // Method PROPERTIES
#include "MeasureGUI_CenterMassDlg.h"    // Method CENTER MASS
#include "MeasureGUI_NormaleDlg.h"       // Method NORMALE
#include "MeasureGUI_InertiaDlg.h"       // Method INERTIA
#include "MeasureGUI_BndBoxDlg.h"        // Method BNDBOX
#include "MeasureGUI_DistanceDlg.h"      // Method DISTANCE
#include "MeasureGUI_AngleDlg.h"         // Method ANGLE
#include "MeasureGUI_MaxToleranceDlg.h"  // Method MAXTOLERANCE
#include "MeasureGUI_WhatisDlg.h"        // Method WHATIS
#include "MeasureGUI_CheckShapeDlg.h"    // Method CHECKSHAPE
#include "MeasureGUI_CheckCompoundOfBlocksDlg.h" // Method CHECKCOMPOUND
#include "MeasureGUI_GetNonBlocksDlg.h"  // Method GET NON BLOCKS
#include "MeasureGUI_CheckSelfIntersectionsDlg.h" // Method CHECK SELF INTERSCTIONS
#include "MeasureGUI_PointDlg.h"         // Method POINTCOORDINATES

#include <QApplication>

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
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();

  QDialog* dlg = 0;
  switch ( theCommandID ) {
  case GEOMOp::OpProperties:
    dlg = new MeasureGUI_PropertiesDlg( getGeometryGUI(), parent );
    break; // LENGTH, AREA AND VOLUME
  case GEOMOp::OpCenterMass:
    dlg = new MeasureGUI_CenterMassDlg( getGeometryGUI(), parent );
    break; // CENTER MASS7
  case GEOMOp::OpInertia:
    dlg = new MeasureGUI_InertiaDlg( getGeometryGUI(), parent );
    break; // INERTIA
  case GEOMOp::OpNormale : 
    dlg = new MeasureGUI_NormaleDlg( getGeometryGUI(), parent );
    break; // NORMALE
  case GEOMOp::OpBoundingBox:
    dlg = new MeasureGUI_BndBoxDlg( getGeometryGUI(), parent );
    break; // BOUNDING BOX
  case GEOMOp::OpMinDistance:
    dlg = new MeasureGUI_DistanceDlg( getGeometryGUI(), parent );
    break; // MIN DISTANCE
  case GEOMOp::OpAngle: 
    dlg = new MeasureGUI_AngleDlg( getGeometryGUI(), parent );
    break; // ANGLE
  case GEOMOp::OpTolerance: 
    dlg = new MeasureGUI_MaxToleranceDlg( getGeometryGUI(), parent );
    break; // MAXTOLERANCE
  case GEOMOp::OpWhatIs:
    dlg = new MeasureGUI_WhatisDlg( getGeometryGUI(), parent );
    break; // WHATIS
  case GEOMOp::OpCheckShape:
    dlg = new MeasureGUI_CheckShapeDlg( getGeometryGUI(), parent );
    break; // CHECKSHAPE
  case GEOMOp::OpCheckCompound:
    dlg = new MeasureGUI_CheckCompoundOfBlocksDlg( getGeometryGUI(), parent );
    break; // CHECKCOMPOUND
  case GEOMOp::OpGetNonBlocks:
    dlg = new MeasureGUI_GetNonBlocksDlg(getGeometryGUI(), parent);
    break; // GET NON BLOCKS
  case GEOMOp::OpCheckSelfInters:
    dlg = new MeasureGUI_CheckSelfIntersectionsDlg( getGeometryGUI(), parent );
    break; // CHECK SELF INTERSCTIONS
  case GEOMOp::OpPointCoordinates:
    dlg = new MeasureGUI_PointDlg( getGeometryGUI(), parent );
    break; // POINT COORDINATES
  default: 
    app->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) ); 
    break;
  }
  if ( dlg ) {
    dlg->updateGeometry();
    dlg->resize( dlg->minimumSizeHint() );
    dlg->show();
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
    return new MeasureGUI( parent );
  }
}
