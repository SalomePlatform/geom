// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// File   : TransformationGUI.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//

#include "TransformationGUI.h"

#include <GEOMBase.h>
#include <GeometryGUI.h>

#include <SUIT_Desktop.h>
#include <SUIT_ViewModel.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>

#include "TransformationGUI_MultiTranslationDlg.h"   // Method MULTI TRANSLATION
#include "TransformationGUI_MultiRotationDlg.h"      // Method MULTI ROTATION
#include "TransformationGUI_TranslationDlg.h"        // Method TRANSLATION
#include "TransformationGUI_RotationDlg.h"           // Method ROTATION
#include "TransformationGUI_MirrorDlg.h"             // Method MIRROR
#include "TransformationGUI_ScaleDlg.h"              // Method SCALE
#include "TransformationGUI_OffsetDlg.h"             // Method OFFSET
#include "TransformationGUI_PositionDlg.h"           // Method POSITION

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
  case 5021: // TRANSLATION
    aDlg = new TransformationGUI_TranslationDlg( getGeometryGUI(), parent );
    break;
  case 5022: // ROTATION
    aDlg = new TransformationGUI_RotationDlg( getGeometryGUI(), parent );
    break;
  case 5023: // POSITION
    aDlg = new TransformationGUI_PositionDlg( getGeometryGUI(), parent );
    break;
  case 5024: // MIRROR
    aDlg = new TransformationGUI_MirrorDlg( getGeometryGUI(), parent );
    break;
  case 5025: // SCALE
    aDlg = new TransformationGUI_ScaleDlg( getGeometryGUI(), parent );
    break;
  case 5026: // OFFSET
    aDlg = new TransformationGUI_OffsetDlg( getGeometryGUI(), parent );
    break;
  case 5027: // MULTI TRANSLATION
    aDlg = new TransformationGUI_MultiTranslationDlg( getGeometryGUI(), parent );
    break;
  case 5028: // MULTI ROTATION
    aDlg = new TransformationGUI_MultiRotationDlg( getGeometryGUI(), parent );
    break;
  case 5029: // RELOAD IMPORTED SHAPE
    {
      LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
      SALOME_ListIO aSelList;
      aSelMgr->selectedObjects(aSelList);
      if (aSelList.Extent() == 1) {
        Standard_Boolean testResult = Standard_False;
        GEOM::GEOM_Object_var aGeomObj =
          GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);
        if (testResult) {
          SalomeApp_Study* anAppStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
          GEOM::GEOM_ITransformOperations_var anOp =
            GeometryGUI::GetGeomGen()->GetITransformOperations(anAppStudy->id());
          if (!anOp->_is_nil()) {
            anOp->RecomputeObject(aGeomObj);
            GEOM_Displayer aDisp (anAppStudy);
            //aDisp.Redisplay(aSelList.First());
            //aDisp.Display(aSelList.First());
            Handle(SALOME_InteractiveObject) theIO = aSelList.First();
            SUIT_Desktop* desk = app->desktop();
            QList<SUIT_ViewWindow*> wnds = desk->windows();
            SUIT_ViewWindow* wnd;
            QListIterator<SUIT_ViewWindow*> it( wnds );
            while ( it.hasNext() && (wnd = it.next()) )
            {
              SUIT_ViewManager* vman = wnd->getViewManager();
              if ( vman )
              {
                SUIT_ViewModel* vmodel = vman->getViewModel();
                if ( vmodel )
                {
                  SALOME_View* view = dynamic_cast<SALOME_View*>(vmodel);
                  if ( view )
                  {
                    //if (view->isVisible(theIO) || view == GetActiveView())
                    if (view->isVisible(theIO))
                    {
                      //SALOME_Prs* prs = view->CreatePrs( theIO->getEntry() );
                      //if ( prs ) {
                      //  prs->Update(&aDisp);
                      //  view->Repaint();
                      //}
                      aDisp.Erase(theIO, false, false, view);
                      aDisp.Display(theIO, true, view);
                    }
                  }
                }
              }
            }
            // ? Redisplay subshapes ?
          }
        }
      }
    }
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
#ifdef WNT
  __declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new TransformationGUI( parent );
  }
}
