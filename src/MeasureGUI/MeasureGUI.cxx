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
// File   : MeasureGUI.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.

#include "MeasureGUI.h"

#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"
#include <GEOMGUI_TextTreeWdg.h>

#include <GEOMGUI_DimensionProperty.h>
#include <GEOMGUI_AnnotationAttrs.h>
#include <GEOMGUI_AnnotationMgr.h>

#include <LightApp_SelectionMgr.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_Desktop.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>

#include "MeasureGUI_PropertiesDlg.h"             // Method PROPERTIES
#include "MeasureGUI_CenterMassDlg.h"             // Method CENTER MASS
#include "MeasureGUI_NormaleDlg.h"                // Method NORMALE
#include "MeasureGUI_InertiaDlg.h"                // Method INERTIA
#include "MeasureGUI_BndBoxDlg.h"                 // Method BNDBOX
#include "MeasureGUI_DistanceDlg.h"               // Method DISTANCE
#include "MeasureGUI_AngleDlg.h"                  // Method ANGLE
#include "MeasureGUI_AnnotationDlg.h"             // Method ANNOTATION
#include "MeasureGUI_MaxToleranceDlg.h"           // Method MAXTOLERANCE
#include "MeasureGUI_WhatisDlg.h"                 // Method WHATIS
#include "MeasureGUI_CheckShapeDlg.h"             // Method CHECKSHAPE
#include "MeasureGUI_CheckCompoundOfBlocksDlg.h"  // Method CHECKCOMPOUND
#include "MeasureGUI_GetNonBlocksDlg.h"           // Method GET NON BLOCKS
#include "MeasureGUI_CheckSelfIntersectionsDlg.h" // Method CHECK SELF INTERSCTIONS
#include "MeasureGUI_FastCheckIntersectionsDlg.h" // Method FAST CHECK INTERSCTIONS
#include "MeasureGUI_PointDlg.h"                  // Method POINTCOORDINATES
#include "MeasureGUI_ManageDimensionsDlg.h"       // Method MANAGEDIMENSIONS
#ifndef DISABLE_PLOT2DVIEWER
#include "MeasureGUI_ShapeStatisticsDlg.h"        // Method SHAPE STATISTICS
#endif

#include <QApplication>

//=======================================================================
// function : MeasureGUI()
// purpose  : Constructor
//=======================================================================
MeasureGUI::MeasureGUI( GeometryGUI* parent ) : GEOMGUI( parent )
{
  myManageDimensionDlg = 0;
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
  case GEOMOp::OpAnnotation:
    dlg = new MeasureGUI_AnnotationDlg( getGeometryGUI(), true, parent );
    break; // ANNOTATION
  case GEOMOp::OpEditAnnotation:
    dlg = new MeasureGUI_AnnotationDlg( getGeometryGUI(), false, parent );
    break; // ANNOTATION
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
  case GEOMOp::OpFastCheckInters:
    dlg = new MeasureGUI_FastCheckIntersectionsDlg( getGeometryGUI(), parent );
    break; // FAST CHECK INTERSCTIONS
#ifndef DISABLE_PLOT2DVIEWER
  case GEOMOp::OpShapeStatistics:
    dlg = new MeasureGUI_ShapeStatisticsDlg( parent );
    break; // SHAPE STATISTICS
#endif
  case GEOMOp::OpPointCoordinates:
    dlg = new MeasureGUI_PointDlg( getGeometryGUI(), parent );
    break; // POINT COORDINATES
  case GEOMOp::OpManageDimensions:
    if( !myManageDimensionDlg ) {
      dlg = new MeasureGUI_ManageDimensionsDlg( getGeometryGUI(), parent );
      myManageDimensionDlg = dlg;
      connect( dlg, SIGNAL( finished(int) ), this, SLOT( onFinished(int) ) );
    }
    myManageDimensionDlg->activateWindow();
    break; // MANAGE DIMENSIONS
  case GEOMOp::OpShowAllDimensions:
    ChangeDimensionsVisibility( true );
    break; // SHOW ALL DIMENSIONS
  case GEOMOp::OpHideAllDimensions:
    ChangeDimensionsVisibility( false );
    break; // HIDE ALL DIMENSIONS
  case GEOMOp::OpShowAllAnnotations:
    ChangeAnnotationsVisibility( true );
    break; // SHOW ALL ANNOTATIONS
  case GEOMOp::OpHideAllAnnotations:
    ChangeAnnotationsVisibility( false );
    break; // HIDE ALL ANNOTATIONS
  case GEOMOp::OpDeleteAnnotation:
    DeleteAnnotation();
    break; // DELETE ANNOTATIOn
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

//=======================================================================
// function : ChangeDimensionsVisibility
// purpose  : 
//=======================================================================
void MeasureGUI::ChangeDimensionsVisibility( const bool theIsVisible )
{
  SalomeApp_Application* anApp = getGeometryGUI()->getApp();
  if ( !anApp )
    return;

  SalomeApp_Study* anActiveStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !anActiveStudy )
    return;

  Handle(SALOME_InteractiveObject) anIObject = getSingleSelectedIO();
  if ( anIObject.IsNull()
   || !anIObject->hasEntry() )
    return;

  SUIT_OverrideCursor wc;

  GEOMGUI_DimensionProperty aDimensions( anActiveStudy, anIObject->getEntry() );

  for ( int anIt = 0; anIt < aDimensions.GetNumber(); ++anIt ) {
    aDimensions.SetVisible( anIt, theIsVisible );
  }

  aDimensions.SaveToAttribute( anActiveStudy, anIObject->getEntry() );

  GEOM_Displayer( anActiveStudy ).Redisplay( anIObject, true );
}

//=======================================================================
// function : ChangeAnnotationsVisibility
// purpose  : 
//=======================================================================
void MeasureGUI::ChangeAnnotationsVisibility( const bool theIsVisible )
{
  SalomeApp_Application* anApp = getGeometryGUI()->getApp();
  if ( !anApp )
    return;

  SalomeApp_Study* anActiveStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !anActiveStudy )
    return;

  QString anEntry;
  Handle(SALOME_InteractiveObject) anIObject = getSingleSelectedIO();
  if ( !anIObject.IsNull() && anIObject->hasEntry() ) {
    anEntry = anIObject->getEntry();
  }
  if ( anEntry.isEmpty() ) {
    anEntry = getGeometryGUI()->GetTextTreeWdg()->getSingleSelectedObject();
  }

  if ( !anEntry.isEmpty() ) {
    _PTR(SObject) aSObj = anActiveStudy->studyDS()->FindObjectID( anEntry.toStdString() );

    const Handle(GEOMGUI_AnnotationAttrs)
      aShapeAnnotations = GEOMGUI_AnnotationAttrs::FindAttributes( aSObj );

    if ( aShapeAnnotations.IsNull() ) {
      return;
    }

    const int aCount = aShapeAnnotations->GetNbAnnotation();

    if ( aCount > 0 ) {

      SUIT_OverrideCursor wc;

      for ( int anI = 0; anI < aCount; ++anI ) {

        if ( !theIsVisible ) {
          getGeometryGUI()->GetAnnotationMgr()->Erase( anEntry, anI );
        }
        else {
          getGeometryGUI()->GetAnnotationMgr()->Display( anEntry , anI );
        }
      }

      getGeometryGUI()->emitAnnotationsUpdated( anEntry );
    }
  }
  else {
    if ( theIsVisible ) {
      getGeometryGUI()->GetAnnotationMgr()->DisplayAllAnnotations();
    }
    else {
      getGeometryGUI()->GetAnnotationMgr()->EraseAllAnnotations();
    }
    getGeometryGUI()->GetTextTreeWdg()->updateVisibility();
  }
}

//=======================================================================
// function : DeleteAnnotation
// purpose  : 
//=======================================================================
void MeasureGUI::DeleteAnnotation()
{
  SalomeApp_Application* anApp = getGeometryGUI()->getApp();
  if ( !anApp )
    return;

  SalomeApp_Study* anActiveStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !anActiveStudy )
    return;

  Handle(SALOME_InteractiveObject) anIObject = getSingleSelectedIO();
  if ( anIObject.IsNull()
   || !anIObject->hasEntry() )
    return;

  const QString aEntry = anIObject->getEntry();

  QString aObjEntry;

  int aIndex = 0;

  if ( getGeometryGUI()->GetAnnotationMgr()->getIndexFromEntry( aEntry, aObjEntry, aIndex ) )
  {
    _PTR(SObject) aSObj = anActiveStudy->studyDS()->FindObjectID( aObjEntry.toStdString() );

    const Handle(GEOMGUI_AnnotationAttrs)
      aShapeAnnotations = GEOMGUI_AnnotationAttrs::FindAttributes( aSObj );

    if ( aShapeAnnotations.IsNull() ) {
      return;
    }

    aShapeAnnotations->Remove( aIndex );

    getGeometryGUI()->GetAnnotationMgr()->EraseRemovedAnnotation( aObjEntry, aIndex );

    getGeometryGUI()->emitAnnotationsUpdated( aObjEntry );
  }
}

//=======================================================================
// function : getSingleSelectedIO
// purpose  : returns selected interactive object for single selection.
//=======================================================================
Handle(SALOME_InteractiveObject) MeasureGUI::getSingleSelectedIO()
{
  SalomeApp_Application* anApp = getGeometryGUI()->getApp();
  if ( !anApp ) {
    return Handle(SALOME_InteractiveObject)();
  }

  LightApp_SelectionMgr* aSelMgr = anApp->selectionMgr();
  if ( !aSelMgr ) {
    return Handle(SALOME_InteractiveObject)();
  }

  SALOME_ListIO aListIO;
  aSelMgr->selectedObjects( aListIO );
  if ( aListIO.Extent() != 1 ) {
    return Handle(SALOME_InteractiveObject)();
  }

  return aListIO.First();
}

//=======================================================================
// function : onFinished
// purpose  : called then "Manage Dimension" dialog is closed.
//=======================================================================
void MeasureGUI::onFinished(int /*theResult*/) {
  if(sender() == myManageDimensionDlg) {
    myManageDimensionDlg = 0;
  }
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
