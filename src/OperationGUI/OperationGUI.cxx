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
//  File   : OperationGUI.cxx
//  Author : Damien COQUERET, Open CASCADE S.A.S.

#include "OperationGUI.h"

#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"

#include <SUIT_Desktop.h>
#include <SalomeApp_Application.h>

#include "OperationGUI_PartitionDlg.h"   // Method PARTITION
#include "OperationGUI_ArchimedeDlg.h"   // Method ARCHIMEDE
#include "OperationGUI_FilletDlg.h"      // Method FILLET
#include "OperationGUI_Fillet1d2dDlg.h"  // Method FILLET 2D and FILLET 1D
#include "OperationGUI_ChamferDlg.h"     // Method CHAMFER
#include "OperationGUI_GetShapesOnShapeDlg.h"
#include "OperationGUI_GetSharedShapesDlg.h"
#include "OperationGUI_ExtrudedFeatureDlg.h" // Methods EXTRUDED BOSS / CUT
#include "OperationGUI_TransferDataDlg.h"
#include "OperationGUI_ExtractionDlg.h"

//=======================================================================
// function : OperationGUI()
// purpose  : Constructor
//=======================================================================
OperationGUI::OperationGUI( GeometryGUI* parent ) :
  GEOMGUI( parent )
{
}

//=======================================================================
// function : ~OperationGUI()
// purpose  : Destructor
//=======================================================================
OperationGUI::~OperationGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool OperationGUI::OnGUIEvent (int theCommandID, SUIT_Desktop* parent)
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if (!app) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();

  switch (theCommandID) {
  case GEOMOp::OpPartition:     (new OperationGUI_PartitionDlg       (getGeometryGUI(), parent))->show(); break;
  case GEOMOp::OpArchimede:     (new OperationGUI_ArchimedeDlg       (getGeometryGUI(), parent))->show(); break;
  case GEOMOp::OpFillet3d:      (new OperationGUI_FilletDlg          (getGeometryGUI(), parent))->show(); break;
  case GEOMOp::OpChamfer:       (new OperationGUI_ChamferDlg         (getGeometryGUI(), parent))->show(); break;
  case GEOMOp::OpShapesOnShape: (new OperationGUI_GetShapesOnShapeDlg(getGeometryGUI(), parent))->show(); break;
  case GEOMOp::OpSharedShapes:  (new OperationGUI_GetSharedShapesDlg (getGeometryGUI(), parent))->show(); break;
  case GEOMOp::OpExtrudedBoss:  (new OperationGUI_ExtrudedFeatureDlg (BOSS, getGeometryGUI(), parent))->show(); break;
  case GEOMOp::OpExtrudedCut:   (new OperationGUI_ExtrudedFeatureDlg (CUT, getGeometryGUI(), parent))->show(); break;
  case GEOMOp::OpFillet1d:      (new OperationGUI_Fillet1d2dDlg      (getGeometryGUI(), parent, true))->show(); break;
  case GEOMOp::OpFillet2d:      (new OperationGUI_Fillet1d2dDlg      (getGeometryGUI(), parent, false))->show(); break;
  case GEOMOp::OpTransferData:  (new OperationGUI_TransferDataDlg    (getGeometryGUI(), parent))->show(); break;
  case GEOMOp::OpExtraction:    (new OperationGUI_ExtractionDlg      (getGeometryGUI(), parent))->show(); break;
  default:
    app->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
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
    return new OperationGUI( parent );
  }
}
