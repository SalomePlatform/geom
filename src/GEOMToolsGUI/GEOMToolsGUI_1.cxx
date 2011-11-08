// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <GeometryGUI.h>
#include <GeometryGUI_Operations.h>
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

#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
#include <TColStd_HArray1OfByte.hxx>
#else
#include <Graphic3d_HArray1OfBytes.hxx>
#endif

// QT Includes
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
  SALOME_ListIO selected;
  SalomeApp_Application* app =
    dynamic_cast< SalomeApp_Application* >(SUIT_Session::session()->activeApplication());
  if (!app)
    return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  if (!aSelMgr || !appStudy)
    return;

  aSelMgr->selectedObjects(selected);
  if (selected.IsEmpty())
    return;

  Handle(SALOME_InteractiveObject) anIObject = selected.First();

  _PTR(Study) aStudy = appStudy->studyDS();
  _PTR(SObject) aMainSObject(aStudy->FindObjectID(anIObject->getEntry()));
  GEOM::GEOM_Object_var aMainObject = GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aMainSObject));
  if (CORBA::is_nil(aMainObject))
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

#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
        Handle(TColStd_HArray1OfByte) aTexture = aCurPointAspect->GetTexture();
#else
        Handle(Graphic3d_HArray1OfBytes) aTexture = aCurPointAspect->GetTexture();
#endif

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
        int mgrId = window->getViewManager()->getGlobalId();
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
              appStudy->setObjectProperty(mgrId,It.Value()->getEntry(),COLOR_PROP, c);
            }
            GeometryGUI::Modified();
          }
        } // if ( isVTK )
        else if ( isOCC ) {
          Handle(AIS_InteractiveObject) io = GEOMBase::GetAIS( selected.First() );
          if ( !io.IsNull() ) {
            Quantity_Color aColor;
            io->Color( aColor );
            QColor initcolor ((int)( aColor.Red() * 255.0 ),
                              (int)( aColor.Green() * 255.0 ),
                              (int)( aColor.Blue() * 255.0 ));
            QColor c =  QColorDialog::getColor( initcolor, app->desktop() );
            if ( c.isValid() ) {
              SUIT_OverrideCursor();
              aColor = Quantity_Color( c.red() / 255., c.green() / 255., c.blue() / 255., Quantity_TOC_RGB );
              OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*> ( window->getViewManager()->getViewModel() );
              Handle (AIS_InteractiveContext) ic = vm->getAISContext();
              for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
                io = GEOMBase::GetAIS( It.Value(), true );
                if ( !io.IsNull() ) {
                  
                  if ( io->IsKind( STANDARD_TYPE(AIS_Shape) ) ) {
                    TopoDS_Shape theShape = Handle(AIS_Shape)::DownCast( io )->Shape();
                    bool onlyVertex = (theShape.ShapeType() == TopAbs_VERTEX || GEOM_Displayer::isCompoundOfVertices( theShape ));
                    if (onlyVertex) {
                      // Set color for a point

                      Handle(AIS_Drawer) aCurDrawer = io->Attributes();
                      Handle(Prs3d_PointAspect) aCurPointAspect =  aCurDrawer->PointAspect();
                      Quantity_Color aCurColor;
                      Standard_Real aCurScale;
                      Aspect_TypeOfMarker aCurTypeOfMarker;
                      aCurPointAspect->Aspect()->Values( aCurColor, aCurTypeOfMarker, aCurScale );
                      if ( aCurTypeOfMarker != Aspect_TOM_USERDEFINED ) {
                        aCurDrawer->SetPointAspect(new Prs3d_PointAspect(aCurTypeOfMarker, aColor, aCurScale));
                      }
                      else {
                        Standard_Integer aWidth, aHeight;
                        aCurPointAspect->GetTextureSize( aWidth, aHeight );

#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
                        Handle(TColStd_HArray1OfByte) aTexture = aCurPointAspect->GetTexture();
#else
                        Handle(Graphic3d_HArray1OfBytes) aTexture = aCurPointAspect->GetTexture();
#endif

                        aCurDrawer->SetPointAspect(new Prs3d_PointAspect(aColor, 1, aWidth, aHeight, aTexture));
                      }
                      ic->SetLocalAttributes(io, aCurDrawer, Standard_False);
                    }
                  }

                  io->SetColor( aColor );
                  if ( io->IsKind( STANDARD_TYPE(GEOM_AISShape) ) )
                    Handle(GEOM_AISShape)::DownCast( io )->SetShadingColor( aColor );

                  appStudy->setObjectProperty(mgrId,It.Value()->getEntry(), COLOR_PROP, c);

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
              } // for
              ic->UpdateCurrentViewer();
              GeometryGUI::Modified();
            } // if c.isValid()
          } // first IO is not null
        } // if ( isOCC )
      } // if ( selection not empty )
    }
  }

  app->updateActions(); //SRN: To update a Save button in the toolbar
}

void GEOMToolsGUI::OnTexture()
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
        int mgrId = window->getViewManager()->getGlobalId();
        if ( isOCC ) {
          QString aTexture = QFileDialog::getOpenFileName(window,tr( "SELECT_IMAGE"),QString("/home"), tr("OCC_IMAGE_FILES"));
          if( !aTexture.isEmpty() )
          {
            SUIT_OverrideCursor();
            OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*> ( window->getViewManager()->getViewModel() );
            Handle (AIS_InteractiveContext) ic = vm->getAISContext();
            Handle(AIS_InteractiveObject) io ;
            for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
              io = GEOMBase::GetAIS( It.Value(), true );
              if ( !io.IsNull() ) {   
                if ( io->IsKind( STANDARD_TYPE(GEOM_AISShape) ) )
                  Handle(GEOM_AISShape)::DownCast( io )->SetTextureFileName(TCollection_AsciiString(aTexture.toStdString().c_str())); 
                
              io->Redisplay( Standard_True );
              }
            } // for
            ic->UpdateCurrentViewer();
            GeometryGUI::Modified();
            GeometryGUI* myGeomGUI = getGeometryGUI();
            myGeomGUI->OnGUIEvent(GEOMOp::OpTexture);
          } // if ( !selFile.isEmpty() )
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

void GEOMToolsGUI::OnChangeTransparency( bool increase )
{
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

  Handle(SALOME_InteractiveObject) FirstIOS =  selected.First();
  if ( FirstIOS.IsNull() )
    return;

  // Delta
  float delta = 0.1; // VSR: 23/11/2010 (transparency value <= 0.05 is ignored)
  if ( !increase )
    delta *= -1;
        
  SUIT_ViewWindow* window = app->desktop()->activeWindow();
  bool isOCC = ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );
  bool isVTK = ( window && window->getViewManager()->getType() == SVTK_Viewer::Type() );

  if ( isVTK ) {
    SVTK_ViewWindow* vtkVW = dynamic_cast<SVTK_ViewWindow*>( window );
    if ( !vtkVW )
      return;
    SVTK_View* aView = vtkVW->getView();
   
    float transp = aView->GetTransparency(FirstIOS);
    
    // Compute new transparency value
    transp = transp + delta;
    if ( transp < 0 )
      transp = 0;
    else if ( transp > 1 )
      transp = 1;

    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      aView->SetTransparency( It.Value(), transp );
    }
    aView->Repaint();
    GeometryGUI::Modified();
  } // if ( isVTK )
        
  else if ( isOCC ) {
    GEOMBase* gb = new GEOMBase();
    Handle(GEOM_AISShape) aisShape;
   
    aisShape = gb->ConvertIOinGEOMAISShape( FirstIOS, true );
    if( aisShape.IsNull() )
      return;
    float transp = aisShape->Transparency();

    // Compute new transparency value
    transp = transp + delta;
    if ( transp < 0 )
      transp = 0;
    else if ( transp > 1 )
      transp = 1;

    OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*>( window->getViewManager()->getViewModel() );
    if ( !vm )
      return;
    Handle(AIS_InteractiveContext) ic = vm->getAISContext();
    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      aisShape = gb->ConvertIOinGEOMAISShape( It.Value(), true );
      if ( !aisShape.IsNull() ) {
        ic->SetTransparency( aisShape, transp, false );
        ic->Redisplay( aisShape, Standard_False, Standard_True );
      }
    } // for...
    ic->UpdateCurrentViewer();
    GeometryGUI::Modified();
  } // if ( isOCC )
}

void GEOMToolsGUI::OnNbIsos( ActionType actionType )
{
  SalomeApp_Application* app =
    dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  SUIT_ViewWindow* window = app->desktop()->activeWindow();

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

      for(; ic->MoreCurrent(); ic->NextCurrent()) {
        CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());

        Handle(AIS_Drawer) CurDrawer = CurObject->Attributes();
        
        CurDrawer->SetUIsoAspect( new Prs3d_IsoAspect(Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 0.5 , newNbUIso) );
        CurDrawer->SetVIsoAspect( new Prs3d_IsoAspect(Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 0.5 , newNbVIso) );
        
        ic->SetLocalAttributes(CurObject, CurDrawer);
        ic->Redisplay(CurObject);

        QString anIsos("%1%2%3");anIsos = anIsos.arg(newNbUIso);anIsos = anIsos.arg(DIGIT_SEPARATOR);anIsos = anIsos.arg(newNbVIso);
        int aMgrId = window->getViewManager()->getGlobalId();
        aStudy->setObjectProperty(aMgrId ,CurObject->getIO()->getEntry(), "Isos", anIsos);
      }
    }
    GeometryGUI::Modified();
  }
  else if(isVTK){ // if is VTKViewer
    //
    // Warning. It's works incorrect. must be recheked.
    //
    SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >
      ( SUIT_Session::session()->activeApplication() );
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
    
    int newNbUIso = -1;
    int newNbVIso = -1;

    if ( actionType == SHOWDLG ) {
      GEOMToolsGUI_NbIsosDlg* NbIsosDlg =
        new GEOMToolsGUI_NbIsosDlg( SUIT_Session::session()->activeApplication()->desktop() );

      NbIsosDlg->setU( UIso );
      NbIsosDlg->setV( VIso );

      if ( NbIsosDlg->exec() ) {
        SUIT_OverrideCursor();

        newNbUIso = NbIsosDlg->getU();
        newNbVIso = NbIsosDlg->getV();
      } else 
        return; //Cancel case 
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
    
    while( anAct!=NULL ) {
      if(GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(anAct)){
        // There are no casting to needed actor.
        int aIsos[2]={newNbUIso,newNbVIso};
        anActor->SetNbIsos(aIsos);

        QString anIsos("%1%2%3");anIsos = anIsos.arg(newNbUIso);anIsos = anIsos.arg(DIGIT_SEPARATOR);anIsos = anIsos.arg(newNbVIso);
        int aMgrId = window->getViewManager()->getGlobalId();
        aStudy->setObjectProperty(aMgrId ,anActor->getIO()->getEntry(), ISOS_PROP, anIsos);
      }
      anAct = aCollection->GetNextActor();
    }
    view->Repaint();
    GeometryGUI::Modified();
  } // end vtkviewer
}

void GEOMToolsGUI::OnDeflection()
{
  SUIT_ViewWindow* window = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );


  bool isOCC = (window && window->getViewManager()->getType() == OCCViewer_Viewer::Type());
  bool isVTK = (window && window->getViewManager()->getType() == SVTK_Viewer::Type());
  int mgrId = window->getViewManager()->getGlobalId();

  if (isOCC) { // if is OCCViewer
    OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*>(window->getViewManager()->getViewModel());
    Handle (AIS_InteractiveContext) ic = vm->getAISContext();

    ic->InitCurrent();
    if (ic->MoreCurrent()) {
      Handle(GEOM_AISShape) CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());

      Standard_Real aDC, aPrevDC;
      Standard_Boolean isOwnDC = CurObject->OwnDeviationCoefficient(aDC, aPrevDC);
      if (!isOwnDC)
        aDC = ic->DeviationCoefficient();

      GEOMToolsGUI_DeflectionDlg * DeflectionDlg = new GEOMToolsGUI_DeflectionDlg
        (SUIT_Session::session()->activeApplication()->desktop());
      DeflectionDlg->setTheDC(aDC);
      double aNewDC = 0.0;
      bool ok = false;
      while (!ok) {
        if (DeflectionDlg->exec()) {
          SUIT_OverrideCursor();
          aNewDC = DeflectionDlg->getTheDC();
          ok = (1e-07 <= aNewDC && aNewDC <= 1.0); // spinbox can return zero
          if (ok) {
            for (; ic->MoreCurrent(); ic->NextCurrent()) {
              CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());
              ic->SetDeviationCoefficient(CurObject, aNewDC, Standard_True);
              ic->Redisplay(CurObject);
              appStudy->setObjectProperty(mgrId,CurObject->getIO()->getEntry(), DEFLECTION_COEFF_PROP, aNewDC);
            }
          }
        }
        else {
          ok = true;
        }
      }
    }
    GeometryGUI::Modified();
  }
  else if (isVTK) { // if is VTKViewer
    SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>
      (SUIT_Session::session()->activeApplication());
    if (!app)
      return;

    LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
    if (!aSelMgr)
      return;

    SALOME_ListIO selected;
    aSelMgr->selectedObjects(selected);
    if (selected.IsEmpty())
      return;

    SVTK_ViewWindow* vtkVW = dynamic_cast<SVTK_ViewWindow*>(window);
    if (!vtkVW)
      return;

    SALOME_View* view = GEOM_Displayer::GetActiveView();

    vtkActorCollection* aCollection = vtkActorCollection::New();

    for (SALOME_ListIteratorOfListIO It (selected); It.More(); It.Next()) {
      Handle(SALOME_InteractiveObject) anIObject = It.Value();
      SALOME_Prs* aPrs = view->CreatePrs(anIObject->getEntry());
      SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>(aPrs);
      if (vtkPrs) {
        vtkActorCollection* anActors = vtkPrs->GetObjects();
        anActors->InitTraversal();
        vtkActor* anAct = anActors->GetNextActor();
        aCollection->AddItem(anAct);
      }
    }

    if (aCollection)
      aCollection->InitTraversal();
    else
      return;

    double aDC = 0.;

    vtkActor* anAct = aCollection->GetNextActor();
    if (GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(anAct))
      aDC = anActor->GetDeflection();
    else
      return;

    GEOMToolsGUI_DeflectionDlg* DeflectionDlg = new GEOMToolsGUI_DeflectionDlg
      (SUIT_Session::session()->activeApplication()->desktop());
    DeflectionDlg->setTheDC(aDC);
    if (DeflectionDlg->exec()) {
      SUIT_OverrideCursor();
      aDC = DeflectionDlg->getTheDC();
      while (anAct != NULL) {
        if (GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(anAct)) {
          // There are no casting to needed actor.
          anActor->SetDeflection(aDC);
          appStudy->setObjectProperty(mgrId, anActor->getIO()->getEntry(), DEFLECTION_COEFF_PROP, aDC);
        }
        anAct = aCollection->GetNextActor();
      }
    }
    GeometryGUI::Modified();
  } // end vtkviewer
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
