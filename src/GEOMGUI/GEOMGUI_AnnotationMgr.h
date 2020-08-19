// Copyright (C) 2007-2020  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef GEOMGUI_ANNOTATIONMGR_H
#define GEOMGUI_ANNOTATIONMGR_H

#include "GEOM_GEOMGUI.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

#include <QObject>
#include <GEOMGUI_AnnotationAttrs.h>

#include <SALOME_Prs.h>
#include <SALOME_InteractiveObject.hxx>

#include <SOCC_ViewModel.h>

class SalomeApp_Application;
class GEOM_Annotation;
class GEOM_Displayer;

/*!
 * \brief A help class to process visualization of annotation presentations.
 * It contains a map of presentations shown in each viewer.
 * and correct 2D position persistent properties of annotations of active viewer.
 * When the viewer is closed, information about the viewer is removed from the manager 
 */
class GEOMGUI_EXPORT GEOMGUI_AnnotationMgr : public QObject
{
  Q_OBJECT

public:
  GEOMGUI_AnnotationMgr( SalomeApp_Application* theApplication );
  ~GEOMGUI_AnnotationMgr() {}

  static QString GetEntrySeparator();

  SALOME_Prs* CreatePresentation( const GEOMGUI_AnnotationAttrs::Properties& theProperty,
                                  GEOM::GEOM_Object_ptr theObject,
                                  SOCC_Viewer* theView = 0,
                                  const QString& theEntry = QString() );

  bool IsDisplayed( const QString& theEntry, const int theIndex, SOCC_Viewer* theView = 0 ) const;
  void Display( const QString& theEntry, const int theIndex, SOCC_Viewer* theView = 0,
                const bool isStoreViewState = true, const bool isUpdateViewer = true );
  void Erase( const QString& theEntry, const int theIndex, SOCC_Viewer* theView = 0,
              const bool isUpdateViewer = true );
  void EraseRemovedAnnotation( const QString& theEntry, const int theIndex );
  void Redisplay( const QString& theEntry, const int theIndex,
                  const GEOMGUI_AnnotationAttrs::Properties& theProperties);
  void Redisplay( const QString& theEntry, const int theIndex,
                  const GEOMGUI_AnnotationAttrs::Properties& theProperties, SOCC_Viewer* theView );

  void DisplayVisibleAnnotations( const QString& theEntry, SOCC_Viewer* theView = 0,
                                  const bool isUpdateViewer = true );
  void EraseVisibleAnnotations( const QString& theEntry, SOCC_Viewer* theView = 0,
                                const bool isUpdateViewer = true );
  void UpdateVisibleAnnotations( const QString& theEntry, SOCC_Viewer* theView = 0 );

  void DisplayAllAnnotations( SOCC_Viewer* theView = 0 );
  void EraseAllAnnotations( SOCC_Viewer* theView = 0 );

  void SetPreviewStyle( const QString& theEntry, const int theIndex, const bool theIsPreview );

  void RemoveView( SOCC_Viewer* theView );

  int FindAnnotationIndex( Handle(SALOME_InteractiveObject) theIO,
                           SOCC_Viewer* theView = 0 );

  Handle(SALOME_InteractiveObject) FindInteractiveObject( const QString& theEntry, const int theIndex,
                                                          SOCC_Viewer* theView = 0 ) const;

  QString getDisplayedIndicesInfo( const QString& theEntry, SOCC_Viewer* theView ) const;

  void setDisplayedIndicesInfo( const QString& theEntry, SOCC_Viewer* theView, const QString theIndicesInfo );

  QString makeAnnotationEntry( const QString& theEntry, const int theIndex );

  void storeFixedPosition( const QString& theEntry, SOCC_Viewer* theView );

  bool getIndexFromEntry( const QString& theEntry, QString& theObjEntry, int& theIndex );

  bool isAnnotationEntry( const QString& theEntry ) { return theEntry.indexOf( GetEntrySeparator() ) != -1; }

protected:

  SalomeApp_Application* getApplication() const { return myApplication; }

  GEOM_Displayer* getDisplayer() const;

  SOCC_Viewer* viewOrActiveView( SOCC_Viewer* theView ) const;

  void getObject( const QString& theEntry, const int theIndex,
                  GEOM::GEOM_Object_ptr& anObject,
                  GEOMGUI_AnnotationAttrs::Properties& aProperty );

  void storeVisibleState( const QString& theEntry, SOCC_Viewer* theView );

  std::string getEntry( const GEOM::GEOM_Object_ptr theObject );

  std::string getName( const GEOM::GEOM_Object_ptr theObject );

  void setDisplayProperties( const Handle(GEOM_Annotation)& thePrs,
                             SOCC_Viewer* theView = 0,
                             const QString& theEntry = QString() );

  Handle(GEOM_Annotation) getAISPresentation ( const QString& theEntry,
                                               const int theIndex,
                                               SOCC_Viewer* theView );

private:
  SalomeApp_Application* myApplication;

  typedef QMap<int, SALOME_Prs*> AnnotationToPrs;
  typedef QMap<QString, AnnotationToPrs> EntryToAnnotations;
  QMap<SOCC_Viewer*, EntryToAnnotations> myVisualized;
};
#endif
