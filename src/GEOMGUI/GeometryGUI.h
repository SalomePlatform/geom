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
// File   : GeometryGUI.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef GEOMETRYGUI_H
#define GEOMETRYGUI_H

#include "GEOM_GEOMGUI.hxx"

#include <Basics_OCCTVersion.hxx>

#include <SalomeApp_Module.h>

#include <GEOM_Client.hxx>
#include <SALOME_InteractiveObject.hxx>
#include <SALOMEDSClient.hxx>

#include "GEOMGUI.h"
#include "GEOMPluginGUI.h"

// QT Includes
#include <QMap>
#include <QPair>

// OCCT Includes
#include <gp_Ax3.hxx>

#include <TColStd_HArray1OfByte.hxx>

// IDL headers
#include "SALOMEconfig.h"
#include CORBA_CLIENT_HEADER(SALOMEDS)

#include <SALOME_ListIO.hxx>

class QDialog;
class QMenu;
class QAction;
class GEOMGUI_OCCSelector;
class LightApp_VTKSelector;
class LightApp_Selection;
class SUIT_ViewManager;
class SalomeApp_Study;
class GEOMGUI_CreationInfoWdg;
class GEOMGUI_TextTreeWdg;

//=================================================================================
// class    : GeometryGUI
// purpose  :
//=================================================================================
class  GEOMGUI_EXPORT GeometryGUI : public SalomeApp_Module
{
  Q_OBJECT;

public:
  // Constructor
  GeometryGUI();

  // Destructor
  ~GeometryGUI();

  virtual LightApp_Displayer* displayer();
  virtual void                initialize( CAM_Application* );
  virtual QString             engineIOR() const;

  static Handle(TColStd_HArray1OfByte) getTexture (SalomeApp_Study*, int, int&, int&);

  static bool                 InitGeomGen();

  static  GEOM::GEOM_Gen_var  GetGeomGen();

  static CORBA::Object_var    ClientSObjectToObject (_PTR(SObject) theSObject);
  static SALOMEDS::Study_var  ClientStudyToStudy (_PTR(Study) theStudy);

  static void                 Modified( bool = true );

  GEOM_Client&                GetShapeReader()    { static SHAPE_READER(myShapeReader);return myShapeReader; }

  // Get active dialog box
  QDialog*                    GetActiveDialogBox(){ return myActiveDialogBox; }
  // Set active dialog box
  void                        SetActiveDialogBox( QDialog* aDlg );

  // Non modal dialog boxes management
  void                        EmitSignalDeactivateDialog();
  void                        EmitSignalCloseAllDialogs();
  void                        EmitSignalDefaultStepValueChanged( double newVal );

  // Process action
  void                        OnGUIEvent( int id, const QVariant& theParam =  QVariant( QVariant::Invalid ) );
  virtual bool                activateOperation( int actionId );
  virtual bool                activateOperation( const QString& actionId );
  virtual bool                activateOperation( const QString& actionId, const QString& plugin );

  // The Working Plane management
  void                        SetWorkingPlane( gp_Ax3 wp ) { myWorkingPlane = wp;   }
  gp_Ax3                      GetWorkingPlane()            { return myWorkingPlane; }
  void                        ActiveWorkingPlane();

  virtual bool                renameObject( const QString&, const QString& );
  virtual bool                renameAllowed( const QString& ) const;

  virtual void                windows( QMap<int, int>& ) const;
  virtual void                viewManagers( QStringList& ) const;

  virtual void                contextMenuPopup( const QString&, QMenu*, QString& );
  virtual void                createPreferences();
  virtual void                preferencesChanged( const QString&, const QString& );
  int                         getLocalSelectionMode() const;
  void                        setLocalSelectionMode(const int mode);

  virtual void storeVisualParameters  (int savePoint);
  virtual void restoreVisualParameters(int savePoint);

  QAction*                    getAction(const int id);

  virtual void                message( const QString& msg);
  static void                 ClearShapeBuffer( GEOM::GEOM_Object_ptr );
  static GEOM::GEOM_Object_ptr
                              GetObjectFromIOR( const QString& IOR );

  static QString              GetIORFromObject( GEOM::GEOM_Object_ptr object );

  virtual bool                isDraggable( const SUIT_DataObject* what ) const;
  virtual bool                isDropAccepted( const SUIT_DataObject* where ) const;
  virtual void                dropObjects( const DataObjectList& what, 
                                           SUIT_DataObject* where,
                                           const int row, Qt::DropAction action );

  void                        emitDimensionsUpdated( QString entry );

public slots:
  virtual bool                deactivateModule( SUIT_Study* );
  virtual bool                activateModule( SUIT_Study* );
  virtual void                OnKeyPress  ( SUIT_ViewWindow*, QKeyEvent*   );
  virtual void                OnMousePress( SUIT_ViewWindow*, QMouseEvent* );
  virtual void                OnMouseMove ( SUIT_ViewWindow*, QMouseEvent* );
  virtual void                OnMouseRelease ( SUIT_ViewWindow*, QMouseEvent* );

protected slots:
  virtual void                onViewManagerAdded( SUIT_ViewManager* );
  virtual void                onViewManagerRemoved( SUIT_ViewManager* );

private slots:
  void                        OnGUIEvent();
  void                        onWindowActivated( SUIT_ViewWindow* );
  void                        onViewAboutToShow();
  void                        OnSetMaterial( const QString& );
  void                        updateMaterials();
  void                        updateCreationInfo();
  void                        onAutoBringToFront();
  void                        updateFieldColorScale();

signals :
  void                        SignalDeactivateActiveDialog();
  void                        SignalCloseAllDialogs();
  void                        SignalDefaultStepValueChanged( double newVal );
  void                        SignalDependencyTreeParamChanged( const QString&, const QString& );
  void                        SignalDependencyTreeRenameObject( const QString& );
  void                        DimensionsUpdated( const QString& );

protected:
  virtual LightApp_Selection* createSelection() const;

private:
  GEOMGUI*                    getLibrary( const QString& libraryName );
  GEOMPluginGUI*              getPluginLibrary( const QString& libraryName );
  void                        createGeomAction( const int id, const QString& po_id,
                                                const QString& icon_id = QString(""),
                                                const int key = 0, const bool toggle = false,
                                                const QString& shortcutAction = QString() );
  void                        createPopupItem( const int, const QString& clients, const QString& types,
                                               const bool isSingle = false, const int isVisible = -1,
                                               const bool isExpandAll = false, const bool isOCC = false,
                                               const int parentId = -1 );
  void                        addPluginActions();

  void                        createOriginAndBaseVectors();

public:
  static GEOM::GEOM_Gen_var   myComponentGeom;   // GEOM engine!!!

private:  

  typedef QMap<long, Handle(TColStd_HArray1OfByte)> TextureMap;

  typedef QMap<long, TextureMap> StudyTextureMap;
  typedef QMap<QString, GEOMGUI*> GUIMap;

  typedef QPair<QString, QString> PluginAction;

  GUIMap                      myGUIMap;          // GUI libraries map
  QDialog*                    myActiveDialogBox; // active dialog box
  gp_Ax3                      myWorkingPlane;
  //QMap<int,QString>           myRules;           // popup rules
  static StudyTextureMap      myTextureMap;      // texture map

  QMap<int, PluginAction>      myPluginActions; // plugin actions
  QMap<QString, QString>       myPluginLibs;    // plugin name to plugin client library

  QList<GEOMGUI_OCCSelector*>  myOCCSelectors;
  QList<LightApp_VTKSelector*> myVTKSelectors;

  LightApp_Displayer*         myDisplayer;
  int                         myLocalSelectionMode; //Select Only

  GEOMGUI_CreationInfoWdg*    myCreationInfoWdg;
  
  GEOMGUI_TextTreeWdg*       myTextTreeWdg;
  
  SALOME_ListIO               myTopLevelIOList;               

  friend class DisplayGUI;
};

#endif
