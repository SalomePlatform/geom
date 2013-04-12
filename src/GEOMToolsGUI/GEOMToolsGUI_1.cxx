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

//  GEOM GEOMGUI : GUI for Geometry component
//  File   : GEOMToolsGUI_1.cxx
//  Author : Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)

#include <PyConsole_Console.h>

#include "GEOMToolsGUI.h"
#include "GEOMToolsGUI_TransparencyDlg.h"
#include "GEOMToolsGUI_NbIsosDlg.h"
#include "GEOMToolsGUI_DeflectionDlg.h"
#include "GEOMToolsGUI_MarkerDlg.h"
#include "GEOMToolsGUI_PublishDlg.h"
#include "GEOMToolsGUI_MaterialPropertiesDlg.h"
#include "GEOMToolsGUI_LineWidthDlg.h"
#include <Material_Model.h>

#include <GEOM_VTKPropertyMaterial.hxx>

#include <GeometryGUI.h>
#include <GeometryGUI_Operations.h>
#include <GEOM_Constants.h>
#include <GEOM_Displayer.h>

#include <GEOMBase.h>
#include <GEOM_Actor.h>

#include <Basics_OCCTVersion.hxx>

#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>

#include <SOCC_Prs.h>

#include <SVTK_Prs.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <SVTK_View.h>

#include <OCCViewer_ViewModel.h>

#include <SUIT_ViewManager.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Tools.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Module.h>

#include <LightApp_SelectionMgr.h>
#include <LightApp_NameDlg.h>

#include <GEOMImpl_Types.hxx>

#include "utilities.h"

// OCCT Includes
#include <AIS_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>


#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
#include <TColStd_HArray1OfByte.hxx>
#else
#include <Graphic3d_HArray1OfBytes.hxx>
#endif

// QT Includes
#include <QAction>
#include <QColorDialog>
#include <QInputDialog>
#include <QFileDialog>
#include <QList>

#include <QGridLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QPushButton>
#include <QKeyEvent>

// VTK includes
#include <vtkRenderer.h>

class QtxDialog;
// If the next macro is defined, autocolor feature works for all sub-shapes;
// if it is undefined, autocolor feature works for groups only
#define GENERAL_AUTOCOLOR
// Below macro, when uncommented, switches on simplified (more performant) algorithm
// of auto-color picking up
#define SIMPLE_AUTOCOLOR

void GEOMToolsGUI::OnCheckGeometry()
{
  SalomeApp_Application* app =
    dynamic_cast< SalomeApp_Application* >(SUIT_Session::session()->activeApplication());
  PyConsole_Console* pyConsole = app->pythonConsole();

  if (pyConsole)
    pyConsole->exec("from GEOM_usinggeom import *");
}

void GEOMToolsGUI::OnAutoColor()
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  SalomeApp_Study* appStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !appStudy ) return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() ) return;

  Handle(SALOME_InteractiveObject) anIObject = selected.First();

  _PTR(Study) aStudy = appStudy->studyDS();
  _PTR(SObject) aMainSObject(aStudy->FindObjectID(anIObject->getEntry()));
  GEOM::GEOM_Object_var aMainObject = GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aMainSObject));
  if (CORBA::is_nil(aMainObject)) return;

  aMainObject->SetAutoColor( true );

  QList<SALOMEDS::Color> aReservedColors;

  GEOM_Displayer displayer ( appStudy );

  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  int aMgrId = dynamic_cast< SUIT_ViewModel* >( window )->getViewManager()->getGlobalId();

  for( _PTR(ChildIterator) it( aStudy->NewChildIterator( aMainSObject ) ); it->More(); it->Next() )
  {
    _PTR(SObject) aChildSObject( it->Value() );
    GEOM::GEOM_Object_var aChildObject = GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aChildSObject));
    if( CORBA::is_nil( aChildObject ) )
      continue;

#ifndef GENERAL_AUTOCOLOR // auto-color for groups only
    if( aChildObject->GetType() != GEOM_GROUP )
      continue;
#endif                    // GENERAL_AUTOCOLOR

#ifdef SIMPLE_AUTOCOLOR   // simplified algorithm for auto-colors
    SALOMEDS::Color aColor = GEOM_Displayer::getPredefinedUniqueColor();
#else                     // old algorithm  for auto-colors
    SALOMEDS::Color aColor = GEOM_Displayer::getUniqueColor( aReservedColors );
    aReservedColors.append( aColor );
#endif                    // SIMPLE_AUTOCOLOR
    aChildObject->SetColor( aColor );
  
    QColor c( (int)( aColor.R * 255.0 ), (int)( aColor.G * 255.0 ), (int)( aColor.B * 255.0 ) );

    SUIT_OverrideCursor();
    
    appStudy->setObjectProperty( aMgrId, aChildObject->GetEntry(), GEOM::propertyName( GEOM::Color ), c );
    Handle( SALOME_InteractiveObject ) io = new SALOME_InteractiveObject( aChildObject->GetStudyEntry(), "GEOM", "" );
    if ( window->isVisible( io ) ) displayer.Redisplay( io, false );
  }
  displayer.UpdateViewer();

  // update actions
  appStudy->Modified();
  app->updateActions(); //SRN: To update a Save button in the toolbar
}

void GEOMToolsGUI::OnDisableAutoColor()
{
  SALOME_ListIO selected;
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if( !app )
    return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if( !aSelMgr || !appStudy )
    return;

  aSelMgr->selectedObjects( selected );
  if( selected.IsEmpty() )
    return;

  Handle(SALOME_InteractiveObject) anIObject = selected.First();

  _PTR(Study) aStudy = appStudy->studyDS();
  _PTR(SObject) aMainSObject( aStudy->FindObjectID( anIObject->getEntry() ) );
  GEOM::GEOM_Object_var aMainObject =  GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aMainSObject));
  if( CORBA::is_nil( aMainObject ) )
    return;

  aMainObject->SetAutoColor( false );
}

void GEOMToolsGUI::OnColor()
{
  // get active application
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  // get current study
  SalomeApp_Study* appStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !appStudy ) return;

  // get selection manager
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  // get selection
  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() ) return;

  GEOM_Displayer displayer( appStudy );

  // get active view
  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  // get view id
  int aMgrId = dynamic_cast< SUIT_ViewModel* >( window )->getViewManager()->getGlobalId();

  QColor color;
  QVariant v = appStudy->getObjectProperty( aMgrId, selected.First()->getEntry(), GEOM::propertyName( GEOM::Color ), color );

  // show Choose Color dialog box
  color = QColorDialog::getColor( v.value<QColor>(), app->desktop() );
  if ( !color.isValid() ) return;

  // iterate through list of objects and assign new color
  SUIT_OverrideCursor();
  for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    appStudy->setObjectProperty( aMgrId, io->getEntry(), GEOM::propertyName( GEOM::Color ), color );
    if ( window->isVisible( io ) ) displayer.Redisplay( io, false );
  }
  displayer.UpdateViewer();
  GeometryGUI::Modified();

  // update actions
  app->updateActions(); //SRN: To update a Save button in the toolbar
}

void GEOMToolsGUI::OnTexture()
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  SalomeApp_Study* appStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !appStudy ) return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO selected;  
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() ) return;

  GEOM_Displayer displayer( appStudy );
  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  int aMgrId = dynamic_cast< SUIT_ViewModel* >( window )->getViewManager()->getGlobalId();

  QString aTexture = QFileDialog::getOpenFileName( dynamic_cast< SUIT_ViewWindow* >( window ),tr("GEOM_SELECT_IMAGE"),QString(), tr("OCC_TEXTURE_FILES"));
  if( !aTexture.isEmpty() ) {
    displayer.SetTexture( aTexture.toStdString() );
    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      Handle( SALOME_InteractiveObject ) io = It.Value();
      appStudy->setObjectProperty( aMgrId, io->getEntry(), GEOM::propertyName( GEOM::DisplayMode ), 3 );
      if ( window->isVisible( io ) ) displayer.Redisplay( io, false );
    }
  }
  displayer.UpdateViewer();
  GeometryGUI::Modified();

  // update actions
  app->updateActions(); //SRN: To update a Save button in the toolbar
}

void GEOMToolsGUI::OnTransparency()
{
  GEOMToolsGUI_TransparencyDlg dlg( SUIT_Session::session()->activeApplication()->desktop() );
  dlg.exec();
}

void GEOMToolsGUI::OnChangeTransparency( bool increase )
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  SalomeApp_Study* appStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if( !appStudy) return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() ) return;

  GEOM_Displayer displayer( appStudy );
  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  int aMgrId = dynamic_cast< SUIT_ViewModel* >( window )->getViewManager()->getGlobalId();

  // Delta
  float delta = 0.1; // VSR: 23/11/2010 (transparency value <= 0.05 is ignored)
  if ( !increase )
    delta *= -1;

  QVariant v = appStudy->getObjectProperty( aMgrId, selected.First()->getEntry(), GEOM::propertyName( GEOM::Transparency ), QVariant() );
  float transp  = v.isValid() ? v.toFloat() : 1;

  // Compute new transparency value
  transp = transp + delta;
  if ( transp < 0 )
    transp = 0;
  else if ( transp > 1 )
    transp = 1;

  for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    appStudy->setObjectProperty( aMgrId ,io->getEntry(), GEOM::propertyName( GEOM::Transparency ), transp );
    if ( window->isVisible( io ) ) displayer.Redisplay( io, false );
  }
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}

void GEOMToolsGUI::OnNbIsos( ActionType actionType )
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  SalomeApp_Study* appStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !appStudy ) return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() ) return;

  GEOM_Displayer displayer( appStudy );
  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;
 
  int aMgrId = dynamic_cast< SUIT_ViewModel* >( window )->getViewManager()->getGlobalId();

  QVariant v = appStudy->getObjectProperty( aMgrId, selected.First()->getEntry(), GEOM::propertyName( GEOM::NbIsos ) , QVariant() );
  QString aStr = v.toString();
  QStringList aStrList = aStr.split( GEOM::subSectionSeparator() );
  int UIso = SUIT_Session::session()->resourceMgr()->integerValue( "Geometry", "iso_number_u", 1 );
  int VIso = SUIT_Session::session()->resourceMgr()->integerValue( "Geometry", "iso_number_v", 1 );
  if ( aStrList.count() == 2 ) {
    if ( !aStrList[0].isEmpty() ) UIso = aStrList[0].toInt();
    if ( !aStrList[1].isEmpty() ) VIso = aStrList[1].toInt();
  }

  int newNbUIso = -1;
  int newNbVIso = -1;

  if ( actionType == SHOWDLG ) {
    GEOMToolsGUI_NbIsosDlg * NbIsosDlg =
      new GEOMToolsGUI_NbIsosDlg( SUIT_Session::session()->activeApplication()->desktop() );

    NbIsosDlg->setU( UIso );
    NbIsosDlg->setV( VIso );

    if ( NbIsosDlg->exec() ) {
      SUIT_OverrideCursor();     
      newNbUIso = NbIsosDlg->getU();
      newNbVIso = NbIsosDlg->getV();
    } else //Cancel case
      return;
  }
  else if ( actionType == INCR || actionType == DECR ) {
    int delta = 1;
    if (actionType == DECR)
      delta = -1;

    newNbUIso = UIso + delta;
    newNbVIso = VIso + delta;

    if ( newNbUIso < 0 || newNbVIso < 0 || newNbUIso > 99 || newNbVIso > 99 )
      return;
  }

  for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    QString anIsos = QString( "%1%2%3" ).arg( newNbUIso ).arg( GEOM::subSectionSeparator() ).arg( newNbVIso );
    appStudy->setObjectProperty( aMgrId ,io->getEntry(), GEOM::propertyName( GEOM::NbIsos ), anIsos );
    if ( window->isVisible( io ) ) displayer.Redisplay( io, false );
  }
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}

void GEOMToolsGUI::OnDeflection()
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  SalomeApp_Study* appStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !appStudy ) return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() ) return;

  GEOM_Displayer displayer( appStudy );
  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  int aMgrId = dynamic_cast< SUIT_ViewModel* >( window )->getViewManager()->getGlobalId();

  QVariant v = appStudy->getObjectProperty( aMgrId, selected.First()->getEntry(), GEOM::propertyName( GEOM::Deflection ), QVariant() );
  double aDC =  v.isValid() ? v.toDouble() : SUIT_Session::session()->resourceMgr()->doubleValue( "Geometry", "deflection_coef", 0.001 );

  GEOMToolsGUI_DeflectionDlg * DeflectionDlg = new GEOMToolsGUI_DeflectionDlg
    ( SUIT_Session::session()->activeApplication()->desktop() );
  DeflectionDlg->setTheDC( aDC );
  if ( DeflectionDlg->exec() ) {
    SUIT_OverrideCursor();
    aDC = DeflectionDlg->getTheDC();

    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      Handle( SALOME_InteractiveObject ) io = It.Value();
      appStudy->setObjectProperty( aMgrId, io->getEntry(), GEOM::propertyName( GEOM::Deflection ), aDC );
      if ( window->isVisible( io ) ) displayer.Redisplay( io, false );
    }
  }
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}

void GEOMToolsGUI::OnSelectOnly(int mode)
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( app ) {
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
    GEOM_Displayer aDisp (appStudy);
    aDisp.GlobalSelection(mode);
    getGeometryGUI()->setLocalSelectionMode(mode);
  }
}

void GEOMToolsGUI::OnDiscloseConcealChildren( bool show )
{
  SALOME_ListIO selected;
  SalomeApp_Application* app =
    dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  
  SalomeApp_Module* mod = app ? dynamic_cast<SalomeApp_Module*>(app->activeModule()) : 0;
  
  GEOM_Displayer* disp  = mod ? dynamic_cast<GEOM_Displayer*>(mod->displayer()) : 0;

  if ( app && disp ) {
    LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
    if ( aSelMgr && appStudy ) {
      aSelMgr->selectedObjects( selected );
      if ( !selected.IsEmpty() ) {
        _PTR(Study) aStudy = appStudy->studyDS();
        _PTR(StudyBuilder) B = aStudy->NewBuilder();

        bool aLocked = ( _PTR(AttributeStudyProperties)( aStudy->GetProperties() ) )->IsLocked();
        if ( aLocked ) {
          SUIT_MessageBox::warning( app->desktop(),
                                    QObject::tr( "WRN_WARNING" ),
                                    QObject::tr( "WRN_STUDY_LOCKED" ) );
          return;
        }

        for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
          Handle(SALOME_InteractiveObject) IObject = It.Value();

          _PTR(SObject) obj ( aStudy->FindObjectID( IObject->getEntry() ) );
          _PTR(GenericAttribute) anAttr;
          if ( obj ) {
            _PTR(AttributeExpandable) aExp = B->FindOrCreateAttribute( obj, "AttributeExpandable" );
            aExp->SetExpandable( show );
            if(!show)
              disp->EraseWithChildren(IObject,true);
          } // if ( obj )
        } // iterator
      }
    }
    app->updateObjectBrowser( false );
    app->updateActions();
  }
}

void GEOMToolsGUI::OnPointMarker()
{
  GEOMToolsGUI_MarkerDlg dlg( SUIT_Session::session()->activeApplication()->desktop() );
  dlg.exec();
}

void GEOMToolsGUI::OnMaterialProperties()
{
 GEOMToolsGUI_MaterialPropertiesDlg* dlg = new GEOMToolsGUI_MaterialPropertiesDlg( SUIT_Session::session()->activeApplication()->desktop(), true, false, QtxDialog::OK | QtxDialog::Close | QtxDialog::Apply | QtxDialog::Help );
 dlg->show();
}

void GEOMToolsGUI::OnMaterialsLibrary()
{
	GEOMToolsGUI_MaterialPropertiesDlg dlg( SUIT_Session::session()->activeApplication()->desktop(), false, true, QtxDialog::Standard );
	dlg.setWindowTitle( tr( "MATERIAL_LIBRARY_TLT" ) );
  dlg.exec();
}

void GEOMToolsGUI::OnUnpublishObject() {
  SALOME_ListIO selected;
  SalomeApp_Application* app =
    dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );

  SalomeApp_Module* mod = app ? dynamic_cast<SalomeApp_Module*>(app->activeModule()) : 0;

  GEOM_Displayer* disp  = mod ? dynamic_cast<GEOM_Displayer*>(mod->displayer()) : 0;
  
  if ( app && disp ) {
    LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
    if ( aSelMgr && appStudy ) {
      aSelMgr->selectedObjects( selected );
      if ( !selected.IsEmpty() ) {
        _PTR(Study) aStudy = appStudy->studyDS();
        _PTR(StudyBuilder) B = aStudy->NewBuilder();

        bool aLocked = ( _PTR(AttributeStudyProperties)( aStudy->GetProperties() ) )->IsLocked();
        if ( aLocked ) {
          SUIT_MessageBox::warning( app->desktop(),
                                    QObject::tr( "WRN_WARNING" ),
                                    QObject::tr( "WRN_STUDY_LOCKED" ) );
          return;
        }

        for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
          Handle(SALOME_InteractiveObject) IObject = It.Value();

          _PTR(SObject) obj ( aStudy->FindObjectID( IObject->getEntry() ) );
          _PTR(GenericAttribute) anAttr;
          if ( obj ) {
            _PTR(AttributeDrawable) aDrw = B->FindOrCreateAttribute( obj, "AttributeDrawable" );
            aDrw->SetDrawable( false );
            disp->EraseWithChildren(IObject);
          } // if ( obj )
        } // iterator
        aSelMgr->clearSelected();
      }
    }
    app->updateObjectBrowser( false );
    app->updateActions();
  }
 
}

void GEOMToolsGUI::OnPublishObject() {
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if(!app)
    return;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if(!appStudy)
    return;
  
  _PTR(Study) aStudy = appStudy->studyDS();
  
  if(!aStudy)
    return;

  //Check lock of the study
  bool aLocked = ( _PTR(AttributeStudyProperties)( aStudy->GetProperties() ) )->IsLocked();
  if ( aLocked ) {
    SUIT_MessageBox::warning( app->desktop(),
                              QObject::tr( "WRN_WARNING" ),
                              QObject::tr( "WRN_STUDY_LOCKED" ) );
    return;
  } 
  
  GEOMToolsGUI_PublishDlg * publishDlg =
    new GEOMToolsGUI_PublishDlg( SUIT_Session::session()->activeApplication()->desktop() );
  publishDlg->exec();
}


void GEOMToolsGUI::OnEdgeWidth()
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  SalomeApp_Study* appStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !appStudy ) return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() ) return;

  GEOM_Displayer displayer( appStudy );

  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  int aMgrId = dynamic_cast< SUIT_ViewModel* >( window )->getViewManager()->getGlobalId();

  GEOMToolsGUI_LineWidthDlg* Dlg =
    new GEOMToolsGUI_LineWidthDlg( SUIT_Session::session()->activeApplication()->desktop(), "EDGE_WIDTH_TLT" );

  QVariant v = appStudy->getObjectProperty( aMgrId, selected.First()->getEntry(), GEOM::propertyName( GEOM::LineWidth ), QVariant() );
  int aWidth = v.isValid() ? v.toInt() : SUIT_Session::session()->resourceMgr()->integerValue( "Geometry", "edge_width", 1 );

  Dlg->setTheLW( aWidth );
  if ( Dlg->exec() ) {
    SUIT_OverrideCursor();
    aWidth = Dlg->getTheLW();
  } else
    return; //Cancel case
  for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    appStudy->setObjectProperty( aMgrId, io->getEntry(), GEOM::propertyName( GEOM::LineWidth ), aWidth );
    if ( window->isVisible( io ) ) displayer.Redisplay( io, false );
    }
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}


void GEOMToolsGUI::OnIsosWidth() {
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  SalomeApp_Study* appStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !appStudy ) return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() ) return;

  GEOM_Displayer displayer( appStudy );

  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  int aMgrId = dynamic_cast< SUIT_ViewModel* >( window )->getViewManager()->getGlobalId();

  GEOMToolsGUI_LineWidthDlg* Dlg =
    new GEOMToolsGUI_LineWidthDlg( SUIT_Session::session()->activeApplication()->desktop(), "ISOS_WIDTH_TLT" );

  QVariant v = appStudy->getObjectProperty( aMgrId, selected.First()->getEntry(), GEOM::propertyName( GEOM::IsosWidth ), QVariant() );
  int aWidth = v.isValid() ? v.toInt() : SUIT_Session::session()->resourceMgr()->integerValue( "Geometry", "isolines_width", 1 );

  Dlg->setTheLW( aWidth );
  if ( Dlg->exec() ) {
    SUIT_OverrideCursor();
    aWidth = Dlg->getTheLW();
  } else
    return; //Cancel case
  for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    appStudy->setObjectProperty( aMgrId, io->getEntry(), GEOM::propertyName( GEOM::IsosWidth ), aWidth );
    if ( window->isVisible( io ) ) displayer.Redisplay( io, false );
  }
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}

void GEOMToolsGUI::OnBringToFront() {
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  SalomeApp_Study* appStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy());  
  if (!appStudy) return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() ) return;

  GEOM_Displayer displayer( appStudy );

  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  int aMgrId = dynamic_cast< SUIT_ViewModel* >( window )->getViewManager()->getGlobalId();

  QAction* a = getGeometryGUI()->getAction( GEOMOp::OpBringToFront );
  bool checked = a->isChecked();

  for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    appStudy->setObjectProperty( aMgrId, io->getEntry(), GEOM::propertyName( GEOM::TopLevel ), checked );
    if ( window->isVisible( io ) ) displayer.Redisplay( io, false );
  }
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}

void GEOMToolsGUI::OnClsBringToFront() {
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  SalomeApp_Study* appStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !appStudy ) return;

  GEOM_Displayer displayer( appStudy );

  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  int aMgrId = dynamic_cast< SUIT_ViewModel* >( window )->getViewManager()->getGlobalId();

  SALOME_ListIO anIOlst;
  window->GetVisible( anIOlst );

  for ( SALOME_ListIteratorOfListIO It( anIOlst ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    appStudy->setObjectProperty( aMgrId, QString( io->getEntry() ), GEOM::propertyName( GEOM::TopLevel ), Standard_False );
    if ( window->isVisible( io ) ) displayer.Redisplay( io, false );
  }
  displayer.Redisplay( anIOlst );
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}
  
void GEOMToolsGUI::OnSetMaterial( const QVariant& theParam )
{
  if ( !theParam.canConvert<QString>() ) return;
  
  QString theName = theParam.toString();

  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;
  
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !study ) return;
  
  LightApp_SelectionMgr* selMgr = app->selectionMgr();

  SALOME_ListIO selected;
  selMgr->selectedObjects( selected );

  if ( selected.IsEmpty() ) return;
  
  GEOM_Displayer displayer( study );

  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;
  
  int mgrId = dynamic_cast<SUIT_ViewModel*>( window )->getViewManager()->getGlobalId();

  Material_Model aModel;
  aModel.fromResources( theName );
  QString prop = aModel.toProperties();

  for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
    Handle(SALOME_InteractiveObject) io = It.Value();
    study->setObjectProperty( mgrId, io->getEntry(), GEOM::propertyName( GEOM::Material ), prop );
    if ( window->isVisible( io ) ) displayer.Redisplay( io, false );
  }
  displayer.UpdateViewer();
}
