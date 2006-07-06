//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2004  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : GEOMToolsGUI_1.cxx
//  Author : Sergey ANIKIN
//  Module : GEOM
//  $Header$

#include <PythonConsole_PyConsole.h>

#include "GEOMToolsGUI.h"

#include "GeometryGUI.h"
#include "GEOMToolsGUI_TransparencyDlg.h"
#include "GEOMToolsGUI_NbIsosDlg.h"        // Method ISOS adjustement

#include "GEOM_Actor.h"
#include "GEOMBase.h"

#include "SALOME_ListIO.hxx"
#include "SALOME_ListIteratorOfListIO.hxx"

#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <SVTK_View.h>

#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewWindow.h>

#include <SUIT_ViewManager.h>
#include <SUIT_Application.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_MessageBox.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Module.h>

#include <LightApp_SelectionMgr.h>
#include <LightApp_NameDlg.h>

#include "SALOMEDSClient.hxx"

#include "utilities.h"

// OCCT Includes
#include <AIS_Drawer.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Graphic3d_AspectMarker3d.hxx>

// VTK Includes
#include <vtkBMPReader.h>
#include <vtkTexture.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTransformTextureCoords.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>

// QT Includes
#include <qfileinfo.h>
#include <qcolordialog.h>
#include <qspinbox.h>
#include <qapplication.h>

using namespace std;


void GEOMToolsGUI::OnSettingsColor()
{
  SUIT_Session* sess = SUIT_Session::session();
  SUIT_ResourceMgr* resMgr = sess->resourceMgr();
  SUIT_Desktop* desk = sess->activeApplication()->desktop();

  QColor anInitColor = resMgr->colorValue( "Geometry", "SettingsShadingColor", QColor( "yellow" ) );

  QColor aDialogColor = QColorDialog::getColor(anInitColor, desk );
  if( aDialogColor.isValid() )
  {
    QString type = desk->activeWindow()->getViewManager()->getType();
    if( type != OCCViewer_Viewer::Type() && type != SVTK_Viewer::Type() )
      MESSAGE("Settings Color is not supported for current Viewer");

    resMgr->setValue( "Geometry", "SettingsShadingColor", aDialogColor );
  }
}

void GEOMToolsGUI::OnSettingsIsos()
{
/*
  SUIT_Session* sess = SUIT_Session::session();
  SUIT_ResourceMgr* resMgr = sess->resourceMgr();
  SUIT_Desktop* desk = sess->activeApplication()->desktop();

  SUIT_ViewManager* vman = desk->activeWindow()->getViewManager();
  QString type = vman->getType();

  if ( type != OCCViewer_Viewer::Type() )
    return;

  OCCViewer_Viewer* vm = (OCCViewer_Viewer*)vman->getViewModel();
  Handle (AIS_InteractiveContext) ic = vm->getAISContext();

  int IsoU = resMgr->integerValue( "Geometry:SettingsIsoU", 1 );
  int IsoV = resMgr->integerValue( "Geometry:SettingsIsoV", 1 );

  ic->DefaultDrawer()->UIsoAspect()->SetNumber( IsoU );
  ic->DefaultDrawer()->VIsoAspect()->SetNumber( IsoV );

  GEOMBase_NbIsosDlg* NbIsosDlg = new GEOMBase_NbIsosDlg(desk, tr("GEOM_MEN_ISOS"), TRUE);

  NbIsosDlg->SpinBoxU->setValue(IsoU);
  NbIsosDlg->SpinBoxV->setValue(IsoV);

  if(NbIsosDlg->exec()) {
    IsoU = NbIsosDlg->SpinBoxU->text().toInt();
    IsoV = NbIsosDlg->SpinBoxV->text().toInt();

    ic->DefaultDrawer()->UIsoAspect()->SetNumber(UIso);
    ic->DefaultDrawer()->VIsoAspect()->SetNumber(VIso);
    resMgr->setValue("Geometry:SettingsIsoU", isoU);
    resMgr->setValue("Geometry:SettingsIsoV", isoV);
  }
*/
}

void GEOMToolsGUI::OnSettingsStep()
{
  SUIT_Session* sess = SUIT_Session::session();
  SUIT_ResourceMgr* resMgr = sess->resourceMgr();

  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100. );

  Standard_Boolean res = false;
  double dd = GEOMBase::Parameter( res, QString("%1").arg(step), tr("GEOM_MEN_STEP_LABEL"), tr("GEOM_STEP_TITLE"), 0.001, 10000.0, 3);
  if(res) {
    resMgr->setValue( "Geometry", "SettingsGeomStep", dd );

    /* Emit signal to GeometryGUI_SpinBoxes */
    getGeometryGUI()->EmitSignalDefaultStepValueChanged( dd );
  }
  else
    sess->activeApplication()->putInfo(tr("GEOM_PRP_ABORT"));
}

void GEOMToolsGUI::OnRename()
{
  SALOME_ListIO selected;
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( app ) {
    LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
    if ( aSelMgr && appStudy ) {
      aSelMgr->selectedObjects( selected );
      if ( !selected.IsEmpty() ) {
	_PTR(Study) aStudy = appStudy->studyDS();

	bool aLocked = (_PTR(AttributeStudyProperties)(aStudy->GetProperties()))->IsLocked();
	if ( aLocked ) {
	  SUIT_MessageBox::warn1 ( app->desktop(),
				   QObject::tr("WRN_WARNING"),
				   QObject::tr("WRN_STUDY_LOCKED"),
				   QObject::tr("BUT_OK") );
	  return;
	}

	for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
	  Handle(SALOME_InteractiveObject) IObject = It.Value();

	  _PTR(SObject) obj ( aStudy->FindObjectID(IObject->getEntry()) );
	  _PTR(GenericAttribute) anAttr;
	  if ( obj ) {
	    if( obj->FindAttribute(anAttr, "AttributeName") ) {
	      _PTR(AttributeName) aName (anAttr);

	      QString newName = LightApp_NameDlg::getName( app->desktop(), aName->Value().c_str() );
	      if ( !newName.isEmpty() ) {
		aName->SetValue( newName.latin1() ); // rename the SObject
		IObject->setName( newName.latin1() );// rename the InteractiveObject
		(dynamic_cast<SalomeApp_Module*>(app->activeModule()))->updateObjBrowser( false );
	      }
	    } // if ( name attribute )
	  } // if ( obj )
	} // iterator
      }
    }
  }
  
  app->updateActions(); //SRN: To update a Save button in the toolbar
}

void GEOMToolsGUI::OnCheckGeometry()
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  PythonConsole* pyConsole = app->pythonConsole();

  if(pyConsole)
    pyConsole->exec("from GEOM_usinggeom import *");
}

void GEOMToolsGUI::OnColor()
{
  SALOME_ListIO selected;
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( app ) {
    LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
    if ( aSelMgr ) {
      aSelMgr->selectedObjects( selected );
      if ( !selected.IsEmpty() ) {
	SUIT_ViewWindow* window = app->desktop()->activeWindow();
	bool isOCC = ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );
	bool isVTK = ( window && window->getViewManager()->getType() == SVTK_Viewer::Type() );
	if ( isVTK ) {
	  SVTK_ViewWindow* vtkVW = dynamic_cast<SVTK_ViewWindow*>( window );
	  if ( !vtkVW )
	    return;
	  SVTK_View* aView = vtkVW->getView();
	  QColor initcolor = aView->GetColor( selected.First()  );
	  QColor c = QColorDialog::getColor( QColor(), app->desktop() );
	  if ( c.isValid() ) {
	    SUIT_OverrideCursor();
	    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
	      aView->SetColor( It.Value(), c );
	    }
	  }
	} // if ( isVTK )
	else if ( isOCC ) {
	  Handle(AIS_InteractiveObject) io = GEOMBase::GetAIS( selected.First() );
	  if ( !io.IsNull() ) {
	    Quantity_Color aColor;
	    io->Color( aColor );
	    QColor initcolor( (int)( aColor.Red() * 255.0 ), (int)( aColor.Green() * 255.0 ), (int)( aColor.Blue() * 255.0 ) );
	    QColor c =  QColorDialog::getColor( initcolor, app->desktop() );
	    if ( c.isValid() ) {
	      SUIT_OverrideCursor();
	      aColor = Quantity_Color( c.red() / 255., c.green() / 255., c.blue() / 255., Quantity_TOC_RGB );
	      for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
		io = GEOMBase::GetAIS( It.Value(), true );
		if ( !io.IsNull() ) {
		  // Set color for a point
		  OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*>( window->getViewManager()->getViewModel() );
		  Handle (AIS_InteractiveContext) ic = vm->getAISContext();
		  Handle(AIS_Drawer) aCurDrawer = io->Attributes();
		  Handle(Prs3d_PointAspect) aCurPointAspect =  aCurDrawer->PointAspect();
		  Quantity_Color aCurColor;
		  Standard_Real aCurScale;
		  Aspect_TypeOfMarker aCurTypeOfMarker;
		  aCurPointAspect->Aspect()->Values( aCurColor, aCurTypeOfMarker, aCurScale );
		  aCurDrawer->SetPointAspect( new Prs3d_PointAspect( aCurTypeOfMarker, aColor, aCurScale) );
		  ic->SetLocalAttributes(io, aCurDrawer);
		  
		  io->SetColor( aColor );
		  if ( io->IsKind( STANDARD_TYPE(GEOM_AISShape) ) )
		    Handle(GEOM_AISShape)::DownCast( io )->SetShadingColor( aColor );

		  io->Redisplay( Standard_True );
		}
	      }
	    } // if c.isValid()
	  } // first IO is not null
	} // if ( isOCC )
      } // if ( selection not empty )
    }
  }
  
  app->updateActions(); //SRN: To update a Save button in the toolbar
}

void GEOMToolsGUI::OnTransparency()
{
  GEOMToolsGUI_TransparencyDlg dlg( SUIT_Session::session()->activeApplication()->desktop() );
  dlg.exec();
}

void GEOMToolsGUI::OnNbIsos()
{
  SUIT_ViewWindow* window = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  bool isOCC = ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );

  if ( !isOCC )
    return;

  OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*>( window->getViewManager()->getViewModel() );
  Handle (AIS_InteractiveContext) ic = vm->getAISContext();

  ic->InitCurrent();
  if ( ic->MoreCurrent() ) {
    Handle(GEOM_AISShape) CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());
    Handle(AIS_Drawer)    CurDrawer = CurObject->Attributes();

    int UIso = CurDrawer->UIsoAspect()->Number();
    int VIso = CurDrawer->VIsoAspect()->Number();

    GEOMToolsGUI_NbIsosDlg * NbIsosDlg =
      new GEOMToolsGUI_NbIsosDlg( SUIT_Session::session()->activeApplication()->desktop() );

    NbIsosDlg->setU( UIso );
    NbIsosDlg->setV( VIso );

    if ( NbIsosDlg->exec() ) {
      SUIT_OverrideCursor();
      for(; ic->MoreCurrent(); ic->NextCurrent()) {
        CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());
	Handle(AIS_Drawer) CurDrawer = CurObject->Attributes();

	int nbUIso = NbIsosDlg->getU();
	int nbVIso = NbIsosDlg->getV();

	CurDrawer->SetUIsoAspect( new Prs3d_IsoAspect(Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 0.5 , nbUIso) );
	CurDrawer->SetVIsoAspect( new Prs3d_IsoAspect(Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 0.5 , nbVIso) );

	ic->SetLocalAttributes(CurObject, CurDrawer);
	ic->Redisplay(CurObject);
      }
    }
  }
}

void GEOMToolsGUI::OnOpen()
{
/*
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  _PTR(Study) aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();

  SALOME_ListIteratorOfListIO It(Sel->StoredIObjects());
  Handle(SALOME_InteractiveObject) anIObject;
  for(;It.More();It.Next()) {
    anIObject = It.Value();
    _PTR(SObject) obj ( aStudy->FindObjectID(anIObject->getEntry()) );
    _PTR(AttributePersistentRef) aPersist;
    _PTR(AttributeIOR) anIOR;
    if(obj) {
      // this SObject may be GEOM module root SObject
      _PTR(ChildIterator) anIter ( aStudy->NewChildIterator() );
      _PTR(GenericAttribute) anAttr;
      bool useSubItems = false;
      while (anIter->More() && !useSubItems) {
	_PTR(SObject) subobj ( anIter->Value() );
	if (subobj->FindAttribute(anAttr, "AttributePersistentRef")) {
	  useSubItems = true;
	  obj =  subobj;
	}
	else
	  anIter->Next();
      }
      obj->FindAttribute(anAttr, "AttributePersistentRef");

      while(useSubItems?anIter->More():!anAttr->_is_nil()) {
	if(!obj->FindAttribute(anAttr, "AttributeIOR") &&
	   obj->FindAttribute(anAttr, "AttributePersistentRef")) {
	  _PTR(SComponent) FComp ( obj->GetFatherComponent() );
	  if (FComp) {
	    if (FComp->FindAttribute(anAttr, "AttributeName")) {
	      _PTR(AttributeName) aName ( anAttr );
	      QString compName = QAD_Application::getDesktop()->getComponentName(aName->Value().c_str());
	      //		    parent->loadComponentData(parent->getComponentName(aName->Value()));
	      Engines::Component_var comp ;
	      if ( compName.compare("SUPERV") == 0 ) {
		comp = QAD_Application::getDesktop()->getEngine( "SuperVisionContainer", compName) ;
	      }
	      else {
		comp = QAD_Application::getDesktop()->getEngine( "FactoryServer", compName);
		if ( comp->_is_nil() )
		  comp = QAD_Application::getDesktop()->getEngine( "FactoryServerPy", compName);
	      }

	      if (!CORBA::is_nil(comp)) {
		SALOMEDS::Driver_var   driver = SALOMEDS::Driver::_narrow(comp);
		if (!CORBA::is_nil(driver)) {
		  SALOMEDS::StudyBuilder_var  B = dynamic_cast<SALOMEDS_Study*>(aStudy.get())->GetStudy()->NewBuilder();
		  if (!CORBA::is_nil(B)) {
		    B->LoadWith(FComp,driver);
		  } else {
		    return;
		  }
		}
		else {
		  MESSAGE("loadComponentData(): Driver is null");
		  return;
		}
	      }
	      else {
		MESSAGE("loadComponentData(): Engine is null");
		return;
	      }
		// 		// load
		// 		Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer","GEOM");
		// 		if (!CORBA::is_nil(comp)) {
		// 		  SALOMEDS::Driver_var driver = SALOMEDS::Driver::_narrow(comp);
		// 		  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();
		// 		  SALOMEDS::SComponent_var SC = aStudy->FindComponent("GEOM");
		// 		  if (!CORBA::is_nil(SC))
		// 		    aStudyBuilder->LoadWith(SC,driver);
	    }
	  }
	  else {
	    MESSAGE("Component is null");
	  }
	}
	if(useSubItems) {
	  anIter->Next();
	  obj.reset( anIter->Value() );
	}
	else
	  anAttr = NULL;
      }
    }
  }
*/
}
