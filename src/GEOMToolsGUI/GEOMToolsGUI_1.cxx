//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// GEOM GEOMGUI : GUI for Geometry component
// File   : GEOMToolsGUI_1.cxx
// Author : Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)
//
#include <PyConsole_Console.h>

#include "GEOMToolsGUI.h"
#include "GEOMToolsGUI_TransparencyDlg.h"
#include "GEOMToolsGUI_NbIsosDlg.h"
#include "GEOMToolsGUI_MarkerDlg.h"

#include <GeometryGUI.h>
#include <GEOM_Displayer.h>

#include <GEOMBase.h>
#include <GEOM_Actor.h>

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
#include <Graphic3d_HArray1OfBytes.hxx>

// QT Includes
#include <QColorDialog>
#include <QList>

// VTK includes
#include <vtkRenderer.h>

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

void GEOMToolsGUI::OnRename()
{
  SALOME_ListIO selected;
  SalomeApp_Application* app =
    dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( app ) {
    LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
    if ( aSelMgr && appStudy ) {
      aSelMgr->selectedObjects( selected );
      if ( !selected.IsEmpty() ) {
	_PTR(Study) aStudy = appStudy->studyDS();

	bool aLocked = (_PTR(AttributeStudyProperties)(aStudy->GetProperties()))->IsLocked();
	if ( aLocked ) {
	  SUIT_MessageBox::warning ( app->desktop(),
				     QObject::tr("WRN_WARNING"),
				     QObject::tr("WRN_STUDY_LOCKED") );
	  return;
	}

        bool isAny = false; // is there any appropriate object selected
	for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
	  Handle(SALOME_InteractiveObject) IObject = It.Value();

	  _PTR(SObject) obj ( aStudy->FindObjectID(IObject->getEntry()) );
	  _PTR(GenericAttribute) anAttr;
	  if ( obj ) {
	    if ( obj->FindAttribute(anAttr, "AttributeName") ) {
	      _PTR(AttributeName) aName (anAttr);

              GEOM::GEOM_Object_var anObj =
                GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(obj));
              if (!CORBA::is_nil(anObj)) {
                isAny = true;
                QString newName = LightApp_NameDlg::getName( app->desktop(), aName->Value().c_str() );
                if (!newName.isEmpty()) {
                  aName->SetValue( newName.toLatin1().data() ); // rename the SObject
                  IObject->setName( newName.toLatin1().data() );// rename the InteractiveObject
                  anObj->SetName( newName.toLatin1().data() );  // Rename the corresponding GEOM_Object
                  (dynamic_cast<SalomeApp_Module*>(app->activeModule()))->updateObjBrowser( false );
                }
              } // if ( anObj )
	    } // if ( name attribute )
	  } // if ( obj )
	} // iterator

        if (!isAny) {
	  SUIT_MessageBox::warning( app->desktop(),
				    QObject::tr("WRN_WARNING"),
				    QObject::tr("GEOM_WRN_NO_APPROPRIATE_SELECTION") );
	  return;
        }
      }
    }
  }
  
  app->updateActions(); //SRN: To update a Save button in the toolbar
}

void GEOMToolsGUI::OnCheckGeometry()
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  PyConsole_Console* pyConsole = app->pythonConsole();

  if(pyConsole)
    pyConsole->exec("from GEOM_usinggeom import *");
}

void GEOMToolsGUI::OnAutoColor()
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
  GEOM::GEOM_Object_var aMainObject = GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aMainSObject));
  if( CORBA::is_nil( aMainObject ) )
    return;

  aMainObject->SetAutoColor( true );

  QList<SALOMEDS::Color> aReservedColors;

  GEOM_Displayer aDisp (appStudy);

  SALOME_View* vf = aDisp.GetActiveView();

  SUIT_ViewWindow* window = app->desktop()->activeWindow();
  bool isOCC = ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );
  bool isVTK = ( window && window->getViewManager()->getType() == SVTK_Viewer::Type() );

  for( _PTR(ChildIterator) it( aStudy->NewChildIterator( aMainSObject ) ); it->More(); it->Next() )
  {
    _PTR(SObject) aChildSObject( it->Value() );
    GEOM::GEOM_Object_var aChildObject = GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aChildSObject));
    if( CORBA::is_nil( aChildObject ) )
      continue;

    if( aChildObject->GetType() != GEOM_GROUP )
      continue;

    SALOMEDS::Color aColor = GEOM_Displayer::getUniqueColor( aReservedColors );
    aChildObject->SetColor( aColor );
    aReservedColors.append( aColor );

    QColor c( (int)( aColor.R * 255.0 ), (int)( aColor.G * 255.0 ), (int)( aColor.B * 255.0 ) );

    SALOME_Prs* aPrs = vf->CreatePrs( aChildSObject->GetID().c_str() );

    if ( isVTK )
    {
      SVTK_ViewWindow* vtkVW = dynamic_cast<SVTK_ViewWindow*>( window );
      if ( !vtkVW )
	return;
      SVTK_View* aView = vtkVW->getView();
      SUIT_OverrideCursor();
      for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() )
	aView->SetColor( It.Value(), c );
    }
    else if ( isOCC )
    {
      OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*>( window->getViewManager()->getViewModel() );
      Handle(AIS_InteractiveContext) ic = vm->getAISContext();

      SOCC_Prs* anOCCPrs = dynamic_cast<SOCC_Prs*>( aPrs );
      if( !anOCCPrs )
	continue;

      AIS_ListOfInteractive aList;
      anOCCPrs->GetObjects( aList );
      if( !aList.Extent() )
	continue;

      Handle(AIS_InteractiveObject) io = aList.First();
      if( io.IsNull() )
	continue;

      Quantity_Color aQuanColor( c.red() / 255., c.green() / 255., c.blue() / 255., Quantity_TOC_RGB );

      // Set color for a point
      Handle(AIS_Drawer) aCurDrawer = io->Attributes();
      Handle(Prs3d_PointAspect) aCurPointAspect = aCurDrawer->PointAspect();
      Quantity_Color aCurColor;
      Standard_Real aCurScale;
      Aspect_TypeOfMarker aCurTypeOfMarker;
      aCurPointAspect->Aspect()->Values( aCurColor, aCurTypeOfMarker, aCurScale );
      if ( aCurTypeOfMarker != Aspect_TOM_USERDEFINED ) {
	aCurDrawer->SetPointAspect( new Prs3d_PointAspect( aCurTypeOfMarker, aQuanColor, aCurScale) );
      }
      else {
	Standard_Integer aWidth, aHeight;
	aCurPointAspect->GetTextureSize( aWidth, aHeight );
	Handle(Graphic3d_HArray1OfBytes) aTexture = aCurPointAspect->GetTexture();
	aCurDrawer->SetPointAspect( new Prs3d_PointAspect( aQuanColor, 1, aWidth, aHeight, aTexture ) );
      }
      ic->SetLocalAttributes( io, aCurDrawer );

      io->SetColor( aQuanColor );
      if ( io->IsKind( STANDARD_TYPE(GEOM_AISShape) ) )
	Handle(GEOM_AISShape)::DownCast( io )->SetShadingColor( aQuanColor );

      io->Redisplay( Standard_True );
    }
  }

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
  SALOME_ListIO selected;
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( app && appStudy ) {
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
	  QColor c = QColorDialog::getColor( initcolor, app->desktop() );
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
		  if ( aCurTypeOfMarker != Aspect_TOM_USERDEFINED ) {
		    aCurDrawer->SetPointAspect( new Prs3d_PointAspect( aCurTypeOfMarker, aColor, aCurScale) );
		  }
		  else {
		    Standard_Integer aWidth, aHeight;
		    aCurPointAspect->GetTextureSize( aWidth, aHeight );
		    Handle(Graphic3d_HArray1OfBytes) aTexture = aCurPointAspect->GetTexture();
		    aCurDrawer->SetPointAspect( new Prs3d_PointAspect( aColor, 1, aWidth, aHeight, aTexture ) );
		  }
		  ic->SetLocalAttributes(io, aCurDrawer);
		  
		  io->SetColor( aColor );
		  if ( io->IsKind( STANDARD_TYPE(GEOM_AISShape) ) )
		    Handle(GEOM_AISShape)::DownCast( io )->SetShadingColor( aColor );

		  io->Redisplay( Standard_True );

		  // store color to GEOM_Object
		  _PTR(Study) aStudy = appStudy->studyDS();
		  _PTR(SObject) aSObject( aStudy->FindObjectID( It.Value()->getEntry() ) );
		  GEOM::GEOM_Object_var anObject =
		    GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aSObject));

		  SALOMEDS::Color aSColor;
		  aSColor.R = (double)c.red() / 255.0;
		  aSColor.G = (double)c.green() / 255.0;
		  aSColor.B = (double)c.blue() / 255.0;
		  anObject->SetColor( aSColor );
		  anObject->SetAutoColor( false );
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
  bool isVTK = ( window && window->getViewManager()->getType() == SVTK_Viewer::Type() );

  if(isOCC){ // if is OCCViewer

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
  else if(isVTK){ // if is VTKViewer
    //
    // Warning. It's works incorrect. must be recheked.
    //
    SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
    if ( !app )
      return;
    LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
    if ( !aSelMgr )
      return;
    SALOME_ListIO selected;
    aSelMgr->selectedObjects( selected );
    if ( selected.IsEmpty() )
      return;
    
    SVTK_ViewWindow* vtkVW = dynamic_cast<SVTK_ViewWindow*>( window );
    if ( !vtkVW )
      return;

    SALOME_View* view = GEOM_Displayer::GetActiveView();

    vtkActorCollection* aCollection = vtkActorCollection::New();
    
    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      Handle(SALOME_InteractiveObject) anIObject = It.Value();
      SALOME_Prs* aPrs = view->CreatePrs( anIObject->getEntry() ); 
      SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( aPrs );
      if ( vtkPrs ) {
	vtkActorCollection* anActors = vtkPrs->GetObjects();
	anActors->InitTraversal();
	vtkActor* anAct = anActors->GetNextActor();
	aCollection->AddItem(anAct);
      }
    }
  
    if(aCollection)
      aCollection->InitTraversal();
    else
      return;
   
    int UIso = 0;
    int VIso = 0;
    
    vtkActor* anAct = aCollection->GetNextActor();
    if (GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(anAct))
      anActor->GetNbIsos(UIso,VIso);
    else
      return;
    
    GEOMToolsGUI_NbIsosDlg* NbIsosDlg =
      new GEOMToolsGUI_NbIsosDlg( SUIT_Session::session()->activeApplication()->desktop() );

    NbIsosDlg->setU( UIso );
    NbIsosDlg->setV( VIso );

    if ( NbIsosDlg->exec() ) {
      SUIT_OverrideCursor();
      
      while( anAct!=NULL ) {
	if(GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(anAct)){
	  // There are no casting to needed actor.
	  UIso = NbIsosDlg->getU();
	  VIso = NbIsosDlg->getV();
	  int aIsos[2]={UIso,VIso};
	  anActor->SetNbIsos(aIsos);
	}
	anAct = aCollection->GetNextActor();
      }
    }
  } // end vtkviewer
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

void GEOMToolsGUI::OnShowHideChildren( bool show )
{
  SALOME_ListIO selected;
  SalomeApp_Application* app =
    dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( app ) {
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
