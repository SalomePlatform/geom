// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include "Material_Model.h"

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
      if ( io->IsKind( STANDARD_TYPE(GEOM_AISShape) ) ) {
	Handle(GEOM_AISShape) aGAISShape = Handle(GEOM_AISShape)::DownCast( io );
        aGAISShape->SetShadingColor( aQuanColor );
	aGAISShape->storeBoundaryColors();
      }

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

void GEOMToolsGUI::SetColor( const QString& entry, const QColor& color, bool updateViewer )
{
  if ( entry.isEmpty() || !color.isValid() ) return;

  // get active application
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  // get current study
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !appStudy ) return;

  // get active view
  SUIT_ViewWindow* window = app->desktop()->activeWindow();
  if ( !window ) return;
  
  bool isOCC = ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );
  bool isVTK = ( window && window->getViewManager()->getType() == SVTK_Viewer::Type() );

  // get view id
  int mgrId = window->getViewManager()->getGlobalId();

  Handle(SALOME_InteractiveObject) IO =
    new SALOME_InteractiveObject( entry.toLatin1().data(), "GEOM", "");

  if ( isVTK ) {
    SVTK_ViewWindow* vtkVW = dynamic_cast<SVTK_ViewWindow*>( window );
    if ( !vtkVW ) return;
    SVTK_View* aView = vtkVW->getView();

    GEOMToolsGUI::setVtkColor( appStudy, mgrId, aView, IO, color );
    if ( updateViewer ) aView->Repaint();
  }
  else if ( isOCC ) {
    OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*> ( window->getViewManager()->getViewModel() );
    Handle (AIS_InteractiveContext) ic = vm->getAISContext();
    GEOMToolsGUI::setOccColor( appStudy, mgrId, ic, IO, color ); 
    if ( updateViewer ) ic->UpdateCurrentViewer();
  }

  // mark study as modified
  GeometryGUI::Modified();
  
  // update actions
  app->updateActions(); //SRN: To update a Save button in the toolbar
}

void GEOMToolsGUI::OnColor()
{
  // get active application
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  // get current study
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !appStudy ) return;

  // get selection manager
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  // get selection
  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() ) return;
  
  // get active view
  SUIT_ViewWindow* window = app->desktop()->activeWindow();
  bool isOCC = ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );
  bool isVTK = ( window && window->getViewManager()->getType() == SVTK_Viewer::Type() );

  // get view id
  int mgrId = window->getViewManager()->getGlobalId();

  if ( isVTK ) {
    SVTK_ViewWindow* vtkVW = dynamic_cast<SVTK_ViewWindow*>( window );
    if ( !vtkVW ) return;

    // get initial color (use first object from selection)
    SVTK_View* aView = vtkVW->getView();
    QColor color = aView->GetColor( selected.First()  );
    QVariant v = appStudy->getObjectProperty( mgrId, selected.First()->getEntry(), COLOR_PROP, color );

    // show Choose Color dialog box
    color = QColorDialog::getColor( v.value<QColor>(), app->desktop() );
    if ( !color.isValid() ) return;

    // iterate through list of objects and assign new color
    SUIT_OverrideCursor();
    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      GEOMToolsGUI::setVtkColor( appStudy, mgrId, aView, It.Value(), color ); 
    }
    aView->Repaint();
  }
  else if ( isOCC ) {
    // find AIS interactive object (for first item in selection)
    Handle(AIS_InteractiveObject) io = GEOMBase::GetAIS( selected.First() );
    if ( io.IsNull() ) return;

    // get initial color (use first object from selection)
    Quantity_Color aColor;
    io->Color( aColor ); 
    QColor color = QColor((int)( aColor.Red()   * 255.0 ),
			  (int)( aColor.Green() * 255.0 ),
			  (int)( aColor.Blue()  * 255.0 ));
    QVariant v = appStudy->getObjectProperty( mgrId, selected.First()->getEntry(), COLOR_PROP, color );

    // show Choose Color dialog box
    color = QColorDialog::getColor( v.value<QColor>(), app->desktop() );
    if ( !color.isValid() ) return;

    // iterate through list of objects and assign new color
    SUIT_OverrideCursor();
    OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*> ( window->getViewManager()->getViewModel() );
    Handle (AIS_InteractiveContext) ic = vm->getAISContext();
    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      GEOMToolsGUI::setOccColor( appStudy, mgrId, ic, It.Value(), color ); 
    }
    ic->UpdateCurrentViewer();
  }

  // mark study as modified
  GeometryGUI::Modified();
  
  // update actions
  app->updateActions(); //SRN: To update a Save button in the toolbar
}


void GEOMToolsGUI::setVtkColor( SalomeApp_Study* study,                     // study
				int mgrId,                                  // view window id
				SVTK_View* view,                            // VTK view
				const Handle(SALOME_InteractiveObject)& IO, // interactive object
				const QColor& color )                       // color 
{
  // get material property
  QString matProp;
  matProp = study->getObjectProperty( mgrId, IO->getEntry(), MATERIAL_PROP, matProp ).toString();
  Material_Model material;
  material.fromProperties( matProp );

  // change color only for shapes with not physical type of material
  if ( !material.isPhysical() ) {
    view->SetColor( IO, color );
    study->setObjectProperty( mgrId, IO->getEntry(), COLOR_PROP, color );
  }

  // store color to GEOM_Object
  _PTR(Study) aStudy = study->studyDS();
  _PTR(SObject) aSObject( aStudy->FindObjectID( IO->getEntry() ) );
  if ( !aSObject ) return;

  GEOM::GEOM_Object_var anObject =
    GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aSObject));

  if ( CORBA::is_nil( anObject ) ) return;

  SALOMEDS::Color aSColor;
  aSColor.R = (double)color.red() / 255.0;
  aSColor.G = (double)color.green() / 255.0;
  aSColor.B = (double)color.blue() / 255.0;
  anObject->SetColor( aSColor );
  anObject->SetAutoColor( false );
}

void GEOMToolsGUI::setOccColor( SalomeApp_Study* study,                     // study
				int mgrId,                                  // view window id
				const Handle(AIS_InteractiveContext)& ic,   // OCC interactive context
				const Handle(SALOME_InteractiveObject)& IO, // interactive object
				const QColor& color )                       // color 
{
  // get AIS object
  Handle(AIS_InteractiveObject) io = GEOMBase::GetAIS( IO, true );
  if ( io.IsNull() ) return;

  // get material property
  QString matProp;
  matProp = study->getObjectProperty( mgrId, IO->getEntry(), MATERIAL_PROP, matProp ).toString();
  Material_Model material;
  material.fromProperties( matProp );

  Quantity_Color aColor = Quantity_Color( color.red() / 255., color.green() / 255., color.blue() / 255., Quantity_TOC_RGB );
	      
  // change color only for shapes with not physical type of material
  if ( !material.isPhysical() ) {
    if ( io->IsKind( STANDARD_TYPE(AIS_Shape) ) ) {
      TopoDS_Shape theShape = Handle(AIS_Shape)::DownCast( io )->Shape();
      bool onlyVertex = (theShape.ShapeType() == TopAbs_VERTEX || GEOM_Displayer::isCompoundOfVertices( theShape ));
      if ( onlyVertex ) {
	// set color for a point
	Handle(AIS_Drawer) aCurDrawer = io->Attributes();
	Handle(Prs3d_PointAspect) aCurPointAspect =  aCurDrawer->PointAspect();
	Quantity_Color aCurColor;
	Standard_Real aCurScale;
	Aspect_TypeOfMarker aCurTypeOfMarker;
	aCurPointAspect->Aspect()->Values( aCurColor, aCurTypeOfMarker, aCurScale );
	if ( aCurTypeOfMarker != Aspect_TOM_USERDEFINED ) {
	  aCurDrawer->SetPointAspect( new Prs3d_PointAspect( aCurTypeOfMarker, aColor, aCurScale ) );
	}
	else {
	  Standard_Integer aWidth, aHeight;
	  aCurPointAspect->GetTextureSize( aWidth, aHeight );
#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
	  Handle(TColStd_HArray1OfByte) aTexture = aCurPointAspect->GetTexture();
#else
	  Handle(Graphic3d_HArray1OfBytes) aTexture = aCurPointAspect->GetTexture();
#endif
	  aCurDrawer->SetPointAspect( new Prs3d_PointAspect( aColor, 1, aWidth, aHeight, aTexture ) );
	}
	ic->SetLocalAttributes( io, aCurDrawer, Standard_False );
      }
    }
    
    io->SetColor( aColor );
    if ( io->IsKind( STANDARD_TYPE(GEOM_AISShape) ) ) {
      Handle(GEOM_AISShape) aGAISShape = Handle(GEOM_AISShape)::DownCast( io );
      aGAISShape->SetShadingColor( aColor );
      aGAISShape->storeBoundaryColors();
    }

    io->Redisplay( Standard_True );

    study->setObjectProperty( mgrId, IO->getEntry(), COLOR_PROP, color );
  }    

  // store color to GEOM_Object
  _PTR(Study) aStudy = study->studyDS();
  _PTR(SObject) aSObject( aStudy->FindObjectID( IO->getEntry() ) );
  GEOM::GEOM_Object_var anObject =
    GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aSObject));
  SALOMEDS::Color aSColor;
  aSColor.R = (double)color.red() / 255.0;
  aSColor.G = (double)color.green() / 255.0;
  aSColor.B = (double)color.blue() / 255.0;
  anObject->SetColor( aSColor );
  anObject->SetAutoColor( false );		 
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
        if ( isOCC ) {
          QString aTexture = QFileDialog::getOpenFileName(window,tr( "GEOM_SELECT_IMAGE"),QString(), tr("OCC_TEXTURE_FILES"));
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
              } // if ( !io.IsNull() )
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
      CurObject->restoreIsoNumbers();
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
        int aMgrId = window->getViewManager()->getGlobalId();
        CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());

        Handle(AIS_Drawer) CurDrawer = CurObject->Attributes();

	QVariant v = aStudy->getObjectProperty( aMgrId, CurObject->getIO()->getEntry(), EDGE_WIDTH_PROP , QVariant() );

	int width = v.isValid() ? v.toInt() : 1;

        CurDrawer->SetUIsoAspect( new Prs3d_IsoAspect(Quantity_NOC_GRAY75, Aspect_TOL_SOLID, width , newNbUIso) );
        CurDrawer->SetVIsoAspect( new Prs3d_IsoAspect(Quantity_NOC_GRAY75, Aspect_TOL_SOLID, width , newNbVIso) );

	CurObject->storeIsoNumbers();
        
        ic->SetLocalAttributes(CurObject, CurDrawer);
        ic->Redisplay(CurObject);

        QString anIsos("%1%2%3");anIsos = anIsos.arg(newNbUIso);anIsos = anIsos.arg(DIGIT_SEPARATOR);anIsos = anIsos.arg(newNbVIso);
        aStudy->setObjectProperty(aMgrId ,CurObject->getIO()->getEntry(), ISOS_PROP, anIsos);
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
    if (GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(anAct)) {
      anActor->RestoreIsoNumbers();
      anActor->GetNbIsos(UIso,VIso);
    }
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
	anActor->StoreIsoNumbers();

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
  GEOMToolsGUI_MaterialPropertiesDlg dlg( SUIT_Session::session()->activeApplication()->desktop() );
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
  SUIT_ViewWindow* window = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );


  bool isOCC = (window && window->getViewManager()->getType() == OCCViewer_Viewer::Type());
  bool isVTK = (window && window->getViewManager()->getType() == SVTK_Viewer::Type());
  int mgrId = window->getViewManager()->getGlobalId();

  if (isOCC) { // if is OCCViewer
    OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*>(window->getViewManager()->getViewModel());
    Handle (AIS_InteractiveContext) ic = vm->getAISContext();
    SALOME_View* view = GEOM_Displayer::GetActiveView();
    ic->InitCurrent();
    if (ic->MoreCurrent()) {
      Handle(GEOM_AISShape) CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());

      int aWidth = (int)CurObject->Width();

      GEOMToolsGUI_LineWidthDlg * Dlg = new GEOMToolsGUI_LineWidthDlg
        (SUIT_Session::session()->activeApplication()->desktop(),"EDGE_WIDTH_TLT");
      Dlg->setTheLW(aWidth);
      int aNewWidth = 0;
      if (Dlg->exec()) {
		aNewWidth = Dlg->getTheLW();
		bool ok = (aNewWidth != aWidth && aNewWidth != 0 );
		if (ok) {
		  for(; ic->MoreCurrent(); ic->NextCurrent()) {
			CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());
				SOCC_Prs* aPrs =  dynamic_cast<SOCC_Prs*>(view->CreatePrs(CurObject->getIO()->getEntry()));
				AIS_ListOfInteractive anAISObjects;
				aPrs->GetObjects( anAISObjects );
				AIS_ListIteratorOfListOfInteractive aIter( anAISObjects );
				for ( ; aIter.More(); aIter.Next() ) {
				  Handle(SALOME_AISShape) cur =  Handle(SALOME_AISShape)::DownCast(aIter.Value());
				  if ( !cur.IsNull() ) {
	 				cur->SetWidth(aNewWidth);
					ic->Redisplay(cur);
				  }
				}
			appStudy->setObjectProperty(mgrId, CurObject->getIO()->getEntry(), EDGE_WIDTH_PROP, aNewWidth);
		  }
		}
	  }
    }
    else {
      return;
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

    int aWidth = 1;

    vtkActor* anAct = aCollection->GetNextActor();
    if (GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(anAct))
      aWidth = anActor->GetWidth();
    else
      return;

    GEOMToolsGUI_LineWidthDlg * Dlg = new GEOMToolsGUI_LineWidthDlg
      (SUIT_Session::session()->activeApplication()->desktop(),"EDGE_WIDTH_TLT");
    
    Dlg->setTheLW(aWidth);
    if (Dlg->exec()) {
      SUIT_OverrideCursor();
      aWidth = Dlg->getTheLW();
      while (anAct != NULL) {
        if (GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(anAct)) {
          // There are no casting to needed actor.
          anActor->SetWidth(aWidth);
          appStudy->setObjectProperty(mgrId, anActor->getIO()->getEntry(), EDGE_WIDTH_PROP, aWidth);
        }
        anAct = aCollection->GetNextActor();
      }
    }
    GeometryGUI::Modified();
  } // end vtkviewer
}


void GEOMToolsGUI::OnIsosWidth() {
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
      CurObject->restoreIsoNumbers();
      Handle(AIS_Drawer)    CurDrawer = CurObject->Attributes();

      Handle(Graphic3d_AspectLine3d) asp = CurDrawer->UIsoAspect()->Aspect();
      
      Quantity_Color C;
      Aspect_TypeOfLine T;
      Standard_Real W;
      asp->Values(C,T,W);

      int aWidth = (int)W;

      GEOMToolsGUI_LineWidthDlg * Dlg =
	new GEOMToolsGUI_LineWidthDlg( SUIT_Session::session()->activeApplication()->desktop(),"ISOS_WIDTH_TLT" );

      Dlg->setTheLW( aWidth );

      if ( Dlg->exec() ) {
	SUIT_OverrideCursor();          
	aWidth = Dlg->getTheLW();
      } else //Cancel case
	return;
      
      for(; ic->MoreCurrent(); ic->NextCurrent()) {
	int aMgrId = window->getViewManager()->getGlobalId();
	CurObject = Handle(GEOM_AISShape)::DownCast(ic->Current());
	
	Handle(AIS_Drawer) CurDrawer = CurObject->Attributes();
	
	CurObject->Attributes()->UIsoAspect()->SetWidth(aWidth);
	CurObject->Attributes()->VIsoAspect()->SetWidth(aWidth);
	
	ic->Redisplay(CurObject);
	
	aStudy->setObjectProperty(aMgrId ,CurObject->getIO()->getEntry(), ISOS_WIDTH_PROP, aWidth);
      }
      GeometryGUI::Modified();
    }
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

    int aWidth = 1;

    vtkActor* anAct = aCollection->GetNextActor();
    if (GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(anAct)) {
      aWidth = anActor->GetIsosWidth();
    }
    else
      return;
    
    GEOMToolsGUI_LineWidthDlg* Dlg =
      new GEOMToolsGUI_LineWidthDlg( SUIT_Session::session()->activeApplication()->desktop(), "ISOS_WIDTH_TLT" );
    
    Dlg->setTheLW( aWidth );
    if (Dlg->exec() ) {
      SUIT_OverrideCursor();
      aWidth = Dlg->getTheLW();
    } else 
      return; //Cancel case 
    
    while( anAct!= NULL ) {
      if(GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(anAct)) {
        // There are no casting to needed actor.
	anActor->SetIsosWidth(aWidth);
	int aMgrId = window->getViewManager()->getGlobalId();
        aStudy->setObjectProperty(aMgrId ,anActor->getIO()->getEntry(), ISOS_WIDTH_PROP, aWidth);
      }
      anAct = aCollection->GetNextActor();
    }
    
    view->Repaint();
    GeometryGUI::Modified();
  } // end vtkviewer  
}

void GEOMToolsGUI::OnBringToFront() {
 SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app )
    return;

  SalomeApp_Module* mod = dynamic_cast<SalomeApp_Module*>(app->activeModule());
  if(!mod)
    return;
  
  GEOM_Displayer* disp  = dynamic_cast<GEOM_Displayer*>(mod->displayer());
  
  if(!disp)
    return;
  
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr )
    return;

  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() )
    return;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());  
  if(!appStudy)
    return;

  SUIT_ViewWindow* window = app->desktop()->activeWindow();

  OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*>( window->getViewManager()->getViewModel() );
  if ( !vm )
    return;
  
  bool isOCC = ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );

  QAction* a = getGeometryGUI()->getAction( GEOMOp::OpBringToFront );
  bool checked = a->isChecked();
  
  if ( isOCC ) {
    GEOMBase* gb = new GEOMBase();
    Handle(GEOM_AISShape) aisShape;
   
    Handle(AIS_InteractiveContext) ic = vm->getAISContext();
    SALOME_ListIO anIOlst;
    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      aisShape = gb->ConvertIOinGEOMAISShape( It.Value(), true );
      if ( !aisShape.IsNull() ) {
	aisShape->setTopLevel(checked);
	int aMgrId = window->getViewManager()->getGlobalId();
	appStudy->setObjectProperty( aMgrId, aisShape->getIO()->getEntry(), TOP_LEVEL_PROP, checked );
	anIOlst.Append(aisShape->getIO());
      }
    } // for...
    disp->Redisplay(anIOlst);
    GeometryGUI::Modified();
  } // if ( isOCC )
}

void GEOMToolsGUI::OnClsBringToFront() {
  SalomeApp_Application* app =
    dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if(!app)
    return;

  SalomeApp_Module* mod = dynamic_cast<SalomeApp_Module*>(app->activeModule());
  if(!mod)
    return;
  
  GEOM_Displayer* disp  = dynamic_cast<GEOM_Displayer*>(mod->displayer());
  
  if(!disp)
    return;


  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());  
  if(!appStudy)
    return;
  
  SUIT_ViewWindow* window = app->desktop()->activeWindow();  
  
  bool isOCC = ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );
  
  if(isOCC){ // if is OCCViewer
    
    OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*>( window->getViewManager()->getViewModel() );
    Handle (AIS_InteractiveContext) ic = vm->getAISContext();
    
    SALOME_ListIO anIOlst;
    AIS_ListOfInteractive aList;
    ic->DisplayedObjects( aList );
    for ( AIS_ListIteratorOfListOfInteractive it( aList ); it.More(); it.Next() ) {
      
      Handle(GEOM_AISShape) CurObject = Handle(GEOM_AISShape)::DownCast(it.Value());
      
      if(CurObject.IsNull())
	continue;
      
      CurObject->setTopLevel(Standard_False);
      
      int aMgrId = window->getViewManager()->getGlobalId();
      appStudy->setObjectProperty( aMgrId, QString(CurObject->getIO()->getEntry()), TOP_LEVEL_PROP, Standard_False );
      anIOlst.Append(CurObject->getIO());
    }
    disp->Redisplay(anIOlst);
    GeometryGUI::Modified();
  }
}
  
