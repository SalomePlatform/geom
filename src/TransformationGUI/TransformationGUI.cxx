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
// File   : TransformationGUI.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.

#include "TransformationGUI.h"

#include <GEOMBase.h>
#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"

#include <SUIT_Desktop.h>
#include <SUIT_ViewModel.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SALOME_ListIO.hxx>

#include "TransformationGUI_ExtensionDlg.h"          // Method EXTENSION
#include "TransformationGUI_MultiTranslationDlg.h"   // Method MULTI TRANSLATION
#include "TransformationGUI_MultiRotationDlg.h"      // Method MULTI ROTATION
#include "TransformationGUI_TranslationDlg.h"        // Method TRANSLATION
#include "TransformationGUI_RotationDlg.h"           // Method ROTATION
#include "TransformationGUI_MirrorDlg.h"             // Method MIRROR
#include "TransformationGUI_ScaleDlg.h"              // Method SCALE
#include "TransformationGUI_OffsetDlg.h"             // Method OFFSET
#include "TransformationGUI_ProjectionDlg.h"         // Method PROJECTION
#include "TransformationGUI_PositionDlg.h"           // Method POSITION
#include "TransformationGUI_ProjectionOnCylDlg.h"    // Method PROJECTION ON CYLINDER

//=======================================================================
// function : TransformationGUI()
// purpose  : Constructor
//=======================================================================
TransformationGUI::TransformationGUI( GeometryGUI* parent )
  : GEOMGUI( parent )
{
}

//=======================================================================
// function : ~TransformationGUI()
// purpose  : Destructor
//=======================================================================
TransformationGUI::~TransformationGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool TransformationGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();
  QDialog* aDlg = NULL;

  switch ( theCommandID ) {
  case GEOMOp::OpTranslate:      // TRANSLATION
    aDlg = new TransformationGUI_TranslationDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpRotate:         // ROTATION
    aDlg = new TransformationGUI_RotationDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpChangeLoc:      // POSITION
    aDlg = new TransformationGUI_PositionDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpMirror:         // MIRROR
    aDlg = new TransformationGUI_MirrorDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpScale:          // SCALE
    aDlg = new TransformationGUI_ScaleDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpOffset:         // OFFSET
    aDlg = new TransformationGUI_OffsetDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpProjection:     // PROJECTION
    aDlg = new TransformationGUI_ProjectionDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpProjOnCyl:      // PROJECTION ON CYLINDER
    aDlg = new TransformationGUI_ProjectionOnCylDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpMultiTranslate: // MULTI TRANSLATION
    aDlg = new TransformationGUI_MultiTranslationDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpMultiRotate:    // MULTI ROTATION
    aDlg = new TransformationGUI_MultiRotationDlg( getGeometryGUI(), parent );
    break;
  case GEOMOp::OpReimport:       // RELOAD IMPORTED SHAPE
    {
      SalomeApp_Study* anAppStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());

      GEOM::GEOM_ITransformOperations_var anOp =
        GeometryGUI::GetGeomGen()->GetITransformOperations(anAppStudy->id());
      if (anOp->_is_nil()) return false;

      GEOM_Displayer aDisp (anAppStudy);
      SUIT_Desktop* desk = app->desktop();
      QList<SUIT_ViewWindow*> wnds = desk->windows();

      LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
      SALOME_ListIO aSelList;
      aSelMgr->selectedObjects(aSelList);

      SALOME_ListIteratorOfListIO aSelIt (aSelList);
      for (; aSelIt.More(); aSelIt.Next()) {
        Handle(SALOME_InteractiveObject) io = aSelIt.Value();
        GEOM::GEOM_Object_var aGeomObj = GEOMBase::ConvertIOinGEOMObject( io );
        if ( !CORBA::is_nil( aGeomObj ) ) {
          anOp->RecomputeObject(aGeomObj);

          SUIT_ViewWindow* wnd;
          QListIterator<SUIT_ViewWindow*> it (wnds);
          while (it.hasNext() && (wnd = it.next()))
          {
            if (SUIT_ViewManager* vman = wnd->getViewManager()) {
              if (SUIT_ViewModel* vmodel = vman->getViewModel()) {
                if (SALOME_View* view = dynamic_cast<SALOME_View*>(vmodel)) {
                  if (view->isVisible(io)) {
                    aDisp.Erase(io, true, false, view);
                    getGeometryGUI()->GetShapeReader().RemoveShapeFromBuffer( getGeometryGUI()->getApp()->orb()->object_to_string( aGeomObj.in() ) );
                    aDisp.Display(io, true, view);
                  }
                  // ? Redisplay subshapes ?
                }
              }
            }
          }
        }
      } // for (; aSelIt.More(); aSelIt.Next())
    }
    break;
  case GEOMOp::OpExtension:     // EXTENSION
    aDlg = new TransformationGUI_ExtensionDlg( getGeometryGUI(), parent );
    break;
  default:
    app->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) );
    break;
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
#ifdef WIN32
  __declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new TransformationGUI( parent );
  }
}
