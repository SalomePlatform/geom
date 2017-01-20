// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GEOMGUI_AnnotationMgr.h"

#include <GEOMGUI_AnnotationAttrs.h>
#include <GEOM_Annotation.hxx>
#include <GEOM_Client.hxx>
#include <GEOM_Constants.h>
#include <GEOM_Displayer.h>
#include <GEOMGUI_TextTreeWdg.h>

#include <GeometryGUI.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SALOME_Prs.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>

#include <SOCC_Prs.h>
#include <SOCC_ViewWindow.h>

#include <Quantity_Color.hxx>
#include <TCollection_AsciiString.hxx>

#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <gp_Ax3.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <QFont>
#include <QColor>


GEOMGUI_AnnotationMgr::GEOMGUI_AnnotationMgr( SalomeApp_Application* theApplication )
: myApplication( theApplication )
{
}

QString GEOMGUI_AnnotationMgr::GetEntrySeparator()
{
	return "_annotation:";
}

//================================================================
// Function : CreatePresentation
// Purpose  :
//================================================================
SALOME_Prs* GEOMGUI_AnnotationMgr::CreatePresentation( const GEOMGUI_AnnotationAttrs::Properties& theProperty,
                                                       GEOM::GEOM_Object_ptr theObject,
                                                       SOCC_Viewer* theView,
                                                       const QString& theEntry )
{
  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !aView ) {
    return NULL;
  }

  Handle ( GEOM_Annotation ) aPresentation = new GEOM_Annotation();
  if ( !theEntry.isEmpty() ) {
    // owner should be set to provide selection mechanizm
    Handle( SALOME_InteractiveObject ) anIO = new SALOME_InteractiveObject();
    anIO->setEntry( theEntry.toLatin1().constData() );
    aPresentation->SetOwner( anIO );
  }

  aPresentation->SetIsScreenFixed( theProperty.IsScreenFixed );

  TopoDS_Shape aShape = GEOM_Client::get_client().GetShape( GeometryGUI::GetGeomGen(), theObject );
  if ( !aShape.IsNull() ) {

    gp_Ax3 aShapeLCS = gp_Ax3().Transformed( aShape.Location().Transformation() );

    GEOMGUI_AnnotationAttrs::SetupPresentation( aPresentation, theProperty, aShapeLCS );

    if ( theProperty.ShapeType == TopAbs_SHAPE ) {

      aPresentation->SetHilightShape( aShape );
    }
    else if ( theProperty.ShapeIndex > 0 ) {

      TopTools_IndexedMapOfShape aSubShapeMap;
      TopExp::MapShapes( aShape, static_cast<TopAbs_ShapeEnum>( theProperty.ShapeType ), aSubShapeMap );
      if ( theProperty.ShapeIndex <= aSubShapeMap.Extent() ) {

        aPresentation->SetHilightShape( aSubShapeMap( theProperty.ShapeIndex ) );
      }
    }
  }

  setDisplayProperties( aPresentation, aView, getEntry( theObject ).c_str() );

  // add Prs to preview
  SUIT_ViewWindow* vw = getApplication()->desktop()->activeWindow();
  SOCC_Prs* aPrs =
      dynamic_cast<SOCC_Prs*>( ( aView )->CreatePrs( 0 ) );

  if ( aPrs )
    aPrs->AddObject( aPresentation );

  return aPrs;
}

bool GEOMGUI_AnnotationMgr::IsDisplayed( const QString& theEntry, const int theIndex, SOCC_Viewer* theView ) const
{
  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !aView || !myVisualized.contains( aView ) )
    return false;

  EntryToAnnotations anEntryToAnnotation = myVisualized[aView];
  if ( !anEntryToAnnotation.contains( theEntry ) )
    return false;

  AnnotationToPrs anAnnotationToPrs = anEntryToAnnotation[theEntry];
  if ( !anAnnotationToPrs.contains( theIndex ) )
    return false;

  return true;
}

//=======================================================================
// function : GEOMGUI_AnnotationMgr::Display
// purpose  : Displays annotation shape presentation in view. It creates an annotation presentation
// and stores it in an internal container
//=======================================================================
void GEOMGUI_AnnotationMgr::Display( const QString& theEntry, const int theIndex, SOCC_Viewer* theView,
                                     const bool isStoreViewState, const bool isUpdateViewer )
{
  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !aView )
    return;

  if ( IsDisplayed( theEntry, theIndex, aView ) )
    return;

  GEOMGUI_AnnotationAttrs::Properties aProperty;
  GEOM::GEOM_Object_ptr anObject;
  getObject( theEntry, theIndex, anObject, aProperty );

  // display presentation in the viewer
  QString anEntry = QString("%1%2%3").arg(theEntry).arg(GetEntrySeparator()).arg(theIndex);
  SALOME_Prs* aPrs = CreatePresentation( aProperty, anObject, aView, anEntry );
  ((SALOME_View*)aView)->Display( getDisplayer(), aPrs );
  if ( isUpdateViewer )
    getDisplayer()->UpdateViewer();

  EntryToAnnotations anEntryToMap;
  if ( myVisualized.contains( aView ) )
    anEntryToMap = myVisualized[aView];

  // store displayed parameters to an internal container
  AnnotationToPrs anAnnotationToPrsMap;
  if ( anEntryToMap.contains( theEntry ) )
    anAnnotationToPrsMap = anEntryToMap[theEntry];
  anAnnotationToPrsMap[theIndex] = aPrs;
  anEntryToMap[theEntry] = anAnnotationToPrsMap;
  myVisualized[aView] = anEntryToMap;

  if ( isStoreViewState ) {
    // change persistent for the entry: set visible state in true for indices which presentations are shown
    storeVisibleState( theEntry, theView );
    storeFixedPosition( theEntry, theView );
  }
}

void GEOMGUI_AnnotationMgr::Redisplay( const QString& theEntry, const int theIndex,
                                       const GEOMGUI_AnnotationAttrs::Properties& theProperty )
{
  SUIT_Session* ses = SUIT_Session::session();
  SUIT_Application* app = ses->activeApplication();
  if ( app )
  {
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
          SOCC_Viewer* aView = dynamic_cast<SOCC_Viewer*>(vmodel);
          if ( aView )
            Redisplay( theEntry, theIndex, theProperty, aView );
        }
      }
    }
  }
}

void GEOMGUI_AnnotationMgr::Redisplay( const QString& theEntry, const int theIndex,
                                       const GEOMGUI_AnnotationAttrs::Properties& theProperty,
                                       SOCC_Viewer* theView )
{
  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !aView )
    return;

  if ( !myVisualized.contains( aView ) )
    return;

  EntryToAnnotations anEntryToAnnotation = myVisualized[aView];
  if ( !anEntryToAnnotation.contains( theEntry ) )
    return;

  AnnotationToPrs anAnnotationToPrs = anEntryToAnnotation[theEntry];
  if ( !anAnnotationToPrs.contains( theIndex ) )
    return;

  GEOMGUI_AnnotationAttrs::Properties aProperty;
  GEOM::GEOM_Object_ptr anObject;
  getObject( theEntry, theIndex, anObject, aProperty );
  TopoDS_Shape aShape = GEOM_Client::get_client().GetShape( GeometryGUI::GetGeomGen(), anObject );
  gp_Ax3 aShapeLCS = gp_Ax3().Transformed( aShape.Location().Transformation() );

  // erase presentation from the viewer
  SALOME_Prs* aPrs = anAnnotationToPrs[theIndex];
  SOCC_Prs* anOCCPrs = dynamic_cast<SOCC_Prs*>( aPrs );
  if ( anOCCPrs ) {
    AIS_ListOfInteractive anIOs;
    anOCCPrs->GetObjects( anIOs );
    AIS_ListIteratorOfListOfInteractive anIter( anIOs );

    for ( ; anIter.More(); anIter.Next() ) {
      Handle(AIS_InteractiveObject) aPrs = anIter.Value();
      Handle(GEOM_Annotation) aPresentation = Handle(GEOM_Annotation)::DownCast( aPrs );

      GEOMGUI_AnnotationAttrs::SetupPresentation( aPresentation, theProperty, aShapeLCS );
      aView->getAISContext()->Redisplay( aPresentation );
    }
  }
}

void GEOMGUI_AnnotationMgr::Erase( const QString& theEntry, const int theIndex, SOCC_Viewer* theView,
                                   const bool isUpdateViewer )
{
  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !aView )
    return;

  if ( !myVisualized.contains( aView ) )
    return;

  EntryToAnnotations anEntryToAnnotation = myVisualized[aView];
  if ( !anEntryToAnnotation.contains( theEntry ) )
    return;

  AnnotationToPrs anAnnotationToPrs = anEntryToAnnotation[theEntry];
  if ( !anAnnotationToPrs.contains( theIndex ) )
    return;

  // erase presentation from the viewer
  SALOME_Prs* aPrs = anAnnotationToPrs[theIndex];
  ((SALOME_View*)aView)->Erase( getDisplayer(), aPrs );
  if ( isUpdateViewer )
    getDisplayer()->UpdateViewer();

  // remove displayed parameters from an internal container
  anAnnotationToPrs.remove( theIndex );
  anEntryToAnnotation[theEntry] = anAnnotationToPrs;
  if (anAnnotationToPrs.isEmpty()) {
    anEntryToAnnotation.remove( theEntry );
  }
  else {
    anEntryToAnnotation[theEntry] = anAnnotationToPrs;
  }
  myVisualized[aView] = anEntryToAnnotation;

  // change persistent for the entry: set visible state in true for indices which presentations are shown
  storeVisibleState( theEntry, theView );
}

void GEOMGUI_AnnotationMgr::DisplayVisibleAnnotations( const QString& theEntry, SOCC_Viewer* theView,
                                                       const bool isUpdateViewer )
{
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( getApplication()->activeStudy() );
  _PTR(SObject) aSObj = aStudy->studyDS()->FindObjectID( theEntry.toStdString() );
  const Handle(GEOMGUI_AnnotationAttrs) aShapeAnnotations = GEOMGUI_AnnotationAttrs::FindAttributes( aSObj );
  if ( !aShapeAnnotations.IsNull() ) {
    const int aCount = aShapeAnnotations->GetNbAnnotation();
    std::vector<bool> isVisible( aCount );
    for ( int anIndex = 0; anIndex < aCount; ++anIndex )
    {
      isVisible[anIndex] = aShapeAnnotations->GetIsVisible( anIndex );
    }
    for ( int anIndex = 0; anIndex < aCount; ++anIndex )
    {
      if ( isVisible[anIndex] )
        Display( theEntry, anIndex, theView, true, isUpdateViewer );
    }
  }
}

void GEOMGUI_AnnotationMgr::EraseVisibleAnnotations( const QString& theEntry, SOCC_Viewer* theView,
                                                     const bool isUpdateViewer )
{
  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !myVisualized.contains( aView ) )
    return;

  EntryToAnnotations anEntryToAnnotation = myVisualized[aView];
  if ( !anEntryToAnnotation.contains( theEntry ) )
    return;
  AnnotationToPrs anAnnotationToPrs = anEntryToAnnotation[theEntry];

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( getApplication()->activeStudy() );
  _PTR(SObject) aSObj = aStudy->studyDS()->FindObjectID( theEntry.toStdString() );
  const Handle(GEOMGUI_AnnotationAttrs) aShapeAnnotations = GEOMGUI_AnnotationAttrs::FindAttributes( aSObj );

  const int aCount = aShapeAnnotations->GetNbAnnotation();
  for ( int anIndex = 0; anIndex < aCount; ++anIndex )
  {
    if ( !anAnnotationToPrs.contains( anIndex ) )
      continue;

    // erase presentation from the viewer
    SALOME_Prs* aPrs = anAnnotationToPrs[anIndex];
    ((SALOME_View*)aView)->Erase( getDisplayer(), aPrs );
  }
  if ( isUpdateViewer )
    getDisplayer()->UpdateViewer();
  anEntryToAnnotation.remove( theEntry );
  myVisualized[aView] = anEntryToAnnotation;
}

//=======================================================================
// function : GEOMGUI_AnnotationMgr::EraseRemovedAnnotation
// purpose  : Method to update internal maps after removing an
//  annotation from the object.
//=======================================================================
void GEOMGUI_AnnotationMgr::EraseRemovedAnnotation( const QString& theEntry, const int theIndex )
{
  QMap<SOCC_Viewer*, EntryToAnnotations>::iterator aEntryMapIt = myVisualized.begin();
  for ( ; aEntryMapIt != myVisualized.end(); ++aEntryMapIt ) {
    SOCC_Viewer* aView = aEntryMapIt.key();
    EntryToAnnotations& anEntryToAnnotation = aEntryMapIt.value();
    if ( !anEntryToAnnotation.contains( theEntry ) )
      continue;

    AnnotationToPrs aUpdatedToPrs;
    AnnotationToPrs& anAnnotationToPrs = anEntryToAnnotation[theEntry];
    AnnotationToPrs::iterator anAnnotationIt = anAnnotationToPrs.begin();
    for ( ; anAnnotationIt != anAnnotationToPrs.end(); ++anAnnotationIt ) {

      const int aIndex = anAnnotationIt.key();
      SALOME_Prs* aPrs = anAnnotationIt.value();
      if ( aIndex > theIndex ) {
        aUpdatedToPrs[aIndex - 1] = aPrs;
      }
      else if ( aIndex != theIndex ) {
        aUpdatedToPrs[aIndex] = aPrs;
      }
      else {
        ((SALOME_View*)aView)->Erase( getDisplayer(), aPrs );
      }
    }

    anAnnotationToPrs = aUpdatedToPrs;
  }
  getDisplayer()->UpdateViewer();
}

//=======================================================================
// function : GEOMGUI_AnnotationMgr::UpdateVisibleAnnotations
// purpose  : 
//=======================================================================
void GEOMGUI_AnnotationMgr::UpdateVisibleAnnotations( const QString& theEntry, SOCC_Viewer* theView )
{
  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !myVisualized.contains( aView ) )
    return;

  EntryToAnnotations& anEntryToAnnotation = myVisualized[aView];
  if ( !anEntryToAnnotation.contains( theEntry ) )
    return;

  AnnotationToPrs& anAnnotationToPrs = anEntryToAnnotation[theEntry];
  AnnotationToPrs::iterator anIt = anAnnotationToPrs.begin();
  for (; anIt != anAnnotationToPrs.end(); ++anIt ) {
    SOCC_Prs* aPrs =
      dynamic_cast<SOCC_Prs*> (anIt.value());

    GEOMGUI_AnnotationAttrs::Properties aProperty;
    GEOM::GEOM_Object_ptr anObject;
    getObject( theEntry, anIt.key(), anObject, aProperty );
    TopoDS_Shape aShape = GEOM_Client::get_client().GetShape( GeometryGUI::GetGeomGen(), anObject );
    gp_Ax3 aShapeLCS = gp_Ax3().Transformed( aShape.Location().Transformation() );

    AIS_ListOfInteractive aIObjects;
    aPrs->GetObjects( aIObjects );
    AIS_ListOfInteractive::Iterator aIOIt( aIObjects );
    for ( ; aIOIt.More(); aIOIt.Next() ) {

      Handle(GEOM_Annotation) aPresentation =
        Handle(GEOM_Annotation)::DownCast( aIOIt.Value() );

      if ( aPresentation.IsNull() )
        continue;

      if ( !aShape.IsNull() ) {

        gp_Ax3 aShapeLCS = gp_Ax3().Transformed( aShape.Location().Transformation() );
        GEOMGUI_AnnotationAttrs::SetupPresentation( aPresentation, aProperty, aShapeLCS );
        if ( aProperty.ShapeType == TopAbs_SHAPE ) {
          aPresentation->SetHilightShape( aShape );
        }
        else if ( aProperty.ShapeIndex > 0 ) {
          TopTools_IndexedMapOfShape aSubShapeMap;
          TopExp::MapShapes( aShape, static_cast<TopAbs_ShapeEnum>( aProperty.ShapeType ), aSubShapeMap );
          if ( aProperty.ShapeIndex <= aSubShapeMap.Extent() ) {
            aPresentation->SetHilightShape( aSubShapeMap( aProperty.ShapeIndex ) );
          }
        }
      }

      setDisplayProperties( aPresentation, aView, theEntry );

      aView->getAISContext()->Redisplay( aPresentation );
    }
  }
  getDisplayer()->UpdateViewer();
}

void GEOMGUI_AnnotationMgr::DisplayAllAnnotations( SOCC_Viewer* theView )
{
  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !myVisualized.contains( aView ) )
    return;

  GeometryGUI* aModule = dynamic_cast<GeometryGUI*>( getApplication()->activeModule() );
  GEOMGUI_TextTreeWdg* aTextWidget = aModule->GetTextTreeWdg();
  QList<QString> anEntries = aTextWidget->getAllEntries( GEOMGUI_TextTreeWdg::AnnotationShape );

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( getApplication()->activeStudy() );
  for ( int i = 0, aCount = anEntries.size(); i < aCount; i++ ) {
     QString anEntry = anEntries[i];

    _PTR(SObject) aSObj = aStudy->studyDS()->FindObjectID( anEntry.toStdString() );
    if ( !aSObj )
      continue;

    const Handle(GEOMGUI_AnnotationAttrs) aShapeAnnotations = GEOMGUI_AnnotationAttrs::FindAttributes( aSObj );
    if ( aShapeAnnotations.IsNull() )
      continue;

    int anAnnotationsCount = aShapeAnnotations->GetNbAnnotation();
    for ( int anIndex = 0; anIndex < anAnnotationsCount; ++anIndex )
    {
      Display( anEntry, anIndex, aView, false, false );
    }
    getDisplayer()->UpdateViewer();
    storeVisibleState( anEntry, aView );
    storeFixedPosition( anEntry, aView );
  }
}

void GEOMGUI_AnnotationMgr::EraseAllAnnotations( SOCC_Viewer* theView )
{
  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !myVisualized.contains( aView ) )
    return;

  GeometryGUI* aModule = dynamic_cast<GeometryGUI*>( getApplication()->activeModule() );
  GEOMGUI_TextTreeWdg* aTextWidget = aModule->GetTextTreeWdg();
  QList<QString> anEntries = aTextWidget->getAllEntries( GEOMGUI_TextTreeWdg::AnnotationShape );

  for ( int i = 0, aCount = anEntries.size(); i < aCount; i++ ) {
    QString anEntry = anEntries[i];
    EraseVisibleAnnotations( anEntry, aView, false );
    storeVisibleState( anEntry, aView );
  }
  getDisplayer()->UpdateViewer();
}

void GEOMGUI_AnnotationMgr::SetPreviewStyle( const QString& theEntry, const int theIndex, const bool theIsPreview )
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  const QColor aFontColor = aResMgr->colorValue( "Geometry", "shape_annotation_font_color", QColor( 255, 255, 255 ) );
  const QColor aLineColor = aResMgr->colorValue( "Geometry", "shape_annotation_line_color", QColor( 255, 255, 255 ) );

  const Quantity_Color aOcctFontColor( aFontColor.redF(), aFontColor.greenF(), aFontColor.blueF(), Quantity_TOC_RGB );
  const Quantity_Color aOcctLineColor( aLineColor.redF(), aLineColor.greenF(), aLineColor.blueF(), Quantity_TOC_RGB );

  QMap<SOCC_Viewer*, EntryToAnnotations>::Iterator aViewIt = myVisualized.begin();
  for (; aViewIt != myVisualized.end(); ++aViewIt ) {

    Handle(GEOM_Annotation) aPresentation = getAISPresentation ( theEntry, theIndex, aViewIt.key() );
    if ( aPresentation.IsNull() ) {
      continue;
    }

    if ( theIsPreview ) {
      aPresentation->SetTextColor( Quantity_NOC_VIOLET );
      aPresentation->SetLineColor( Quantity_NOC_VIOLET );
      aPresentation->SetDepthCulling( Standard_False );
    }
    else {
      aPresentation->SetTextColor( aOcctFontColor );
      aPresentation->SetLineColor( aOcctLineColor );
      aPresentation->SetDepthCulling( Standard_True );
    }
  }
  getDisplayer()->UpdateViewer();
}

Handle(SALOME_InteractiveObject) GEOMGUI_AnnotationMgr::FindInteractiveObject( const QString& theEntry,
                                                                               const int theIndex,
                                                                               SOCC_Viewer* theView ) const
{
  Handle(SALOME_InteractiveObject) anIO;

  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !myVisualized.contains( aView ) )
    return anIO;

  EntryToAnnotations anEntryToAnnotation = myVisualized[aView];
  if ( !anEntryToAnnotation.contains( theEntry ) )
    return anIO;

  AnnotationToPrs anAnnotationToPrs = anEntryToAnnotation[theEntry];
  if ( !anAnnotationToPrs.contains(theIndex) )
    return anIO;

  SALOME_Prs* aPrs = anAnnotationToPrs[theIndex];
  SOCC_Prs* anOCCPrs = dynamic_cast<SOCC_Prs*>( aPrs );
  if ( !anOCCPrs )
    return anIO;

  AIS_ListOfInteractive anIOs;
  anOCCPrs->GetObjects( anIOs );
  AIS_ListIteratorOfListOfInteractive anIter( anIOs );
  for ( ; anIter.More() && anIO.IsNull(); anIter.Next() ) {
    Handle(AIS_InteractiveObject) aPrs = anIter.Value();
    if ( aPrs->GetOwner() )
      anIO = Handle(SALOME_InteractiveObject)::DownCast( aPrs->GetOwner() );
  }
  return anIO;
}

int GEOMGUI_AnnotationMgr::FindAnnotationIndex( Handle(SALOME_InteractiveObject) theIO,
                                                SOCC_Viewer* theView )
{
  int anIndex = -1;

  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !myVisualized.contains( aView ) )
    return anIndex;

  QString anEntry = theIO->getEntry();

  EntryToAnnotations anEntryToAnnotation = myVisualized[aView];
  if ( !anEntryToAnnotation.contains( anEntry ) )
    return anIndex;

  AnnotationToPrs anAnnotationToPrs = anEntryToAnnotation[anEntry];
  //typedef QMap<int, SALOME_Prs*> AnnotationToPrs;
  AnnotationToPrs::const_iterator anIt = anAnnotationToPrs.begin(),
                                  aLast = anAnnotationToPrs.end();
  for (; anIt != aLast && anIndex < 0; anIt++) {
    SALOME_Prs* aPrs = anIt.value();
    SOCC_Prs* anOCCPrs = dynamic_cast<SOCC_Prs*>( aPrs );
    if ( !anOCCPrs )
      continue;

    AIS_ListOfInteractive anIOs;
    anOCCPrs->GetObjects( anIOs );
    AIS_ListIteratorOfListOfInteractive anIter( anIOs );
    for ( ; anIter.More() && anIndex < 0; anIter.Next() ) {
      Handle(AIS_InteractiveObject) aPrs = anIter.Value();
      if ( aPrs->GetOwner() ) {
        Handle(SALOME_InteractiveObject) aPrsOwner = Handle(SALOME_InteractiveObject)::DownCast(aPrs->GetOwner());
        if ( aPrsOwner == theIO )
          anIndex = anIt.key();
      }
    }
  }

  return anIndex;
}

//=======================================================================
// function : GEOMGUI_AnnotationMgr::RemoveView
// purpose  : 
//=======================================================================
void GEOMGUI_AnnotationMgr::RemoveView( SOCC_Viewer* theView )
{
  if ( !theView && myVisualized.contains( theView ) )
    myVisualized.remove( theView );
}

QString GEOMGUI_AnnotationMgr::getDisplayedIndicesInfo( const QString& theEntry, SOCC_Viewer* theView ) const
{
  QString aDisplayedIndices;

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( getApplication()->activeStudy() );
  _PTR(SObject) aSObj = aStudy->studyDS()->FindObjectID( theEntry.toStdString() );
  if ( !aSObj )
  {
    return aDisplayedIndices;
  }
  const Handle(GEOMGUI_AnnotationAttrs) aShapeAnnotations = GEOMGUI_AnnotationAttrs::FindAttributes( aSObj );
  if ( !aShapeAnnotations.IsNull() )
  {
    const int aCount = aShapeAnnotations->GetNbAnnotation();
    QStringList anIndices;
    for ( int anIndex = 0; anIndex < aCount; ++anIndex )
    {
      if (IsDisplayed( theEntry, anIndex, theView ) )
        anIndices.append( QString::number(anIndex) );
    }
    aDisplayedIndices = anIndices.join(";");
  }
  return aDisplayedIndices;
}

void GEOMGUI_AnnotationMgr::setDisplayedIndicesInfo( const QString& theEntry, SOCC_Viewer* theView,
                                                     const QString theIndicesInfo )
{
  if ( theIndicesInfo.isEmpty() )
    return;

  QStringList anIndices = theIndicesInfo.split( ";" );
  for ( int i = 0, aCount = anIndices.size(); i < aCount; i++ ) {
    Display( theEntry, anIndices[i].toInt(), theView );
  }
}

GEOM_Displayer* GEOMGUI_AnnotationMgr::getDisplayer() const
{
  LightApp_Module* aModule = dynamic_cast<LightApp_Module*>( getApplication()->activeModule() );
  return dynamic_cast<GEOM_Displayer*>( aModule->displayer() );
}

SOCC_Viewer* GEOMGUI_AnnotationMgr::viewOrActiveView( SOCC_Viewer* theView ) const
{
  SOCC_Viewer* aView = theView;
  if ( !aView ) {
    SalomeApp_Application* anApp = getApplication();
    SUIT_ViewWindow* anActiveWindow = anApp->desktop()->activeWindow();
    if ( anActiveWindow ) {
      aView = dynamic_cast<SOCC_Viewer*>( anActiveWindow->getViewManager()->getViewModel() );
    }
  }
  return aView;
}

QString GEOMGUI_AnnotationMgr::makeAnnotationEntry( const QString& theEntry, const int theIndex )
{
  return QString("%1%2%3").arg(theEntry).arg(GetEntrySeparator()).arg(theIndex);
}

bool GEOMGUI_AnnotationMgr::getIndexFromEntry( const QString& theEntry, QString& theObjEntry, int& theIndex )
{
  QStringList aSplit = theEntry.split( GetEntrySeparator() );
  if ( aSplit.size() < 2 )
    return false;

  bool isOk = true;
  theObjEntry = aSplit.at( 0 );
  theIndex = aSplit.at( 1 ).toInt( &isOk );
  return isOk;
}

void GEOMGUI_AnnotationMgr::getObject( const QString& theEntry, const int theIndex,
                                       GEOM::GEOM_Object_ptr& theObject,
                                       GEOMGUI_AnnotationAttrs::Properties& theProperty )
{
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( getApplication()->activeStudy() );
  _PTR(SObject) aSObj = aStudy->studyDS()->FindObjectID( theEntry.toStdString() );
  const Handle(GEOMGUI_AnnotationAttrs) aShapeAnnotations = GEOMGUI_AnnotationAttrs::FindAttributes( aSObj );
  if ( !aShapeAnnotations.IsNull() ) {
    aShapeAnnotations->GetProperties( theIndex, theProperty );

    theObject = GEOM::GEOM_Object::_narrow( GeometryGUI::ClientSObjectToObject(aSObj) );
  }
}

void GEOMGUI_AnnotationMgr::storeFixedPosition( const QString& theEntry, SOCC_Viewer* theView )
{
  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !aView || !myVisualized.contains( aView ) )
    return;

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( getApplication()->activeStudy() );
  _PTR(SObject) aSObj = aStudy->studyDS()->FindObjectID( theEntry.toStdString() );
  const Handle(GEOMGUI_AnnotationAttrs) aShapeAnnotations = GEOMGUI_AnnotationAttrs::FindAttributes( aSObj );
  if ( aShapeAnnotations.IsNull() )
    return;


  EntryToAnnotations anEntryToAnnotation = myVisualized[aView];
  AnnotationToPrs anAnnotationToPrs;
  if ( anEntryToAnnotation.contains( theEntry ) )
    anAnnotationToPrs = anEntryToAnnotation[theEntry];

  AnnotationToPrs::iterator anIt = anAnnotationToPrs.begin();
  for (; anIt != anAnnotationToPrs.end(); ++anIt ) {
    int anIndex = anIt.key();
    bool isFixedAnnotation = aShapeAnnotations->GetIsScreenFixed( anIndex );
    if ( !isFixedAnnotation )
      continue;

    SOCC_Prs* aPrs = dynamic_cast<SOCC_Prs*> (anIt.value());
    Handle(GEOM_Annotation) anAnnotationPresentation;

    AIS_ListOfInteractive aIObjects;
    aPrs->GetObjects( aIObjects );
    AIS_ListOfInteractive::Iterator aIOIt( aIObjects );
    for ( ; aIOIt.More(); aIOIt.Next() ) {
      anAnnotationPresentation = Handle(GEOM_Annotation)::DownCast( aIOIt.Value() );
      if ( !anAnnotationPresentation.IsNull() )
        break;
    }
    if ( !anAnnotationPresentation.IsNull() )
      aShapeAnnotations->SetPosition( anIndex, anAnnotationPresentation->GetPosition() );
  }
}

void GEOMGUI_AnnotationMgr::storeVisibleState( const QString& theEntry, SOCC_Viewer* theView )
{
  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( !aView || !myVisualized.contains( aView ) )
    return;

  EntryToAnnotations anEntryToAnnotation = myVisualized[aView];
  AnnotationToPrs anAnnotationToPrs;
  if ( anEntryToAnnotation.contains( theEntry ) )
    anAnnotationToPrs = anEntryToAnnotation[theEntry];


  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( getApplication()->activeStudy() );
  _PTR(SObject) aSObj = aStudy->studyDS()->FindObjectID( theEntry.toStdString() );
  const Handle(GEOMGUI_AnnotationAttrs) aShapeAnnotations = GEOMGUI_AnnotationAttrs::FindAttributes( aSObj );
  if ( !aShapeAnnotations.IsNull() ) {
    const int aCount = aShapeAnnotations->GetNbAnnotation();
    for ( int anIndex = 0; anIndex < aCount; ++anIndex ) {
      bool aVisible = anAnnotationToPrs.contains( anIndex );
      aShapeAnnotations->SetIsVisible( anIndex, aVisible );
    }
  }
}

//=======================================================================
// function : GEOMGUI_AnnotationMgr::getEntry
// purpose  : 
//=======================================================================
std::string GEOMGUI_AnnotationMgr::getEntry( const GEOM::GEOM_Object_ptr theObject )
{
  SUIT_Session* session = SUIT_Session::session();
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( session->activeApplication() );
  if ( app )
  {
    CORBA::String_var IOR = app->orb()->object_to_string( theObject );
    if ( strcmp(IOR.in(), "") != 0 )
    {
      SalomeApp_Study* study = ( SalomeApp_Study* )app->activeStudy();
      _PTR(SObject) SO ( study->studyDS()->FindObjectIOR( std::string(IOR) ) );
      if ( SO )
        return SO->GetID();
    }
  }
  return std::string();
}

//=======================================================================
// function : GEOMGUI_AnnotationMgr::getName
// purpose  : 
//=======================================================================
std::string GEOMGUI_AnnotationMgr::getName( const GEOM::GEOM_Object_ptr theObject )
{
  SUIT_Session* session = SUIT_Session::session();
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( session->activeApplication() );
  if ( app )
  {
    CORBA::String_var IOR = app->orb()->object_to_string( theObject );
    if ( strcmp(IOR.in(), "") != 0 )
    {
      SalomeApp_Study* study = ( SalomeApp_Study* )app->activeStudy();
      _PTR(SObject) aSObj ( study->studyDS()->FindObjectIOR( std::string(IOR) ) );

      _PTR(GenericAttribute) anAttr;

      if ( aSObj && aSObj->FindAttribute( anAttr, "AttributeName") )
      {
        _PTR(AttributeName) aNameAttr( anAttr );
        return aNameAttr->Value();
      }
    }
  }
  return std::string();
}

//=======================================================================
// function : GEOMGUI_AnnotationMgr::setDisplayProperties
// purpose  : 
//=======================================================================
void GEOMGUI_AnnotationMgr::setDisplayProperties( const Handle(GEOM_Annotation)& thePrs,
                                                  SOCC_Viewer* theView,
                                                  const QString& theEntry )
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  const QFont  aFont      = aResMgr->fontValue( "Geometry", "shape_annotation_font", QFont( "Y14.5M-2009", 24 ) );
  const QColor aFontColor = aResMgr->colorValue( "Geometry", "shape_annotation_font_color", QColor( 0, 0, 127 ) );
  const QColor aLineColor = aResMgr->colorValue( "Geometry", "shape_annotation_line_color", QColor( 0, 0, 127 ) );
  const double aLineWidth = aResMgr->doubleValue( "Geometry", "shape_annotation_line_width", 1.0 );
  const int aLineStyle    = aResMgr->integerValue( "Geometry", "shape_annotation_line_style", 0 );
  const bool isAutoHide   = aResMgr->booleanValue( "Geometry", "shape_annotation_autohide", false );

  const Quantity_Color aOcctFontColor( aFontColor.redF(), aFontColor.greenF(), aFontColor.blueF(), Quantity_TOC_RGB );
  const Quantity_Color aOcctLineColor( aLineColor.redF(), aLineColor.greenF(), aLineColor.blueF(), Quantity_TOC_RGB );
  const Standard_Real aFontHeight = aFont.pixelSize() != -1 ? aFont.pixelSize() : aFont.pointSize();

  thePrs->SetFont( TCollection_AsciiString( aFont.family().toLatin1().data() ) );
  thePrs->SetTextHeight( aFontHeight );
  thePrs->SetTextColor( Quantity_Color( aFontColor.redF(), aFontColor.greenF(), aFontColor.blueF(), Quantity_TOC_RGB ) );
  thePrs->SetLineColor( Quantity_Color( aLineColor.redF(), aLineColor.greenF(), aLineColor.blueF(), Quantity_TOC_RGB ) );
  thePrs->SetLineWidth( aLineWidth );
  thePrs->SetLineStyle( static_cast<Aspect_TypeOfLine>( aLineStyle ) );
  thePrs->SetAutoHide( isAutoHide ? Standard_True : Standard_False );
  thePrs->SetDepthCulling( Standard_True );

  SOCC_Viewer* aView = viewOrActiveView( theView );
  if ( aView && !theEntry.isEmpty() ) {

    SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( getApplication()->activeStudy() );
    int aMgrId = dynamic_cast< SUIT_ViewModel* >( aView )->getViewManager()->getGlobalId();
    QVariant aVal = aStudy->getObjectProperty( aMgrId, theEntry, GEOM::propertyName( GEOM::TopLevel ), QVariant() );
    bool isBringToFront = aVal.isValid() ? aVal.toBool() : false;
    if( isBringToFront ) {
      thePrs->SetZLayer( Graphic3d_ZLayerId_Topmost );
    }
    else {
      thePrs->SetDefaultZLayer();
    }
  }
  else {
    thePrs->SetDefaultZLayer();
  }
}

//=======================================================================
// function : GEOMGUI_AnnotationMgr::getAISPresentation
// purpose  : 
//=======================================================================
Handle(GEOM_Annotation) GEOMGUI_AnnotationMgr::getAISPresentation ( const QString& theEntry,
                                                                    const int theIndex,
                                                                    SOCC_Viewer* theView )
{
  if ( !myVisualized.contains( theView ) ) {
    return Handle(GEOM_Annotation)();
  }

  EntryToAnnotations& aEntryToAnnotation = myVisualized[theView];
  if ( !aEntryToAnnotation.contains( theEntry ) ) {
    return Handle(GEOM_Annotation)();
  }

  AnnotationToPrs& aAnnotationToPrs = aEntryToAnnotation[theEntry];
  if ( !aAnnotationToPrs.contains( theIndex ) ) {
    return Handle(GEOM_Annotation)();
  }

  SALOME_Prs* aPrs = aAnnotationToPrs[theIndex];

  // set or unset preview style for the presentation
  AIS_ListOfInteractive aIObjects;
  ((SOCC_Prs*)aPrs)->GetObjects( aIObjects );
  AIS_ListOfInteractive::Iterator aIOIt( aIObjects );
  for ( ; aIOIt.More(); aIOIt.Next() ) {
    return Handle(GEOM_Annotation)::DownCast( aIOIt.Value() );
  }

  return Handle(GEOM_Annotation)();
}
