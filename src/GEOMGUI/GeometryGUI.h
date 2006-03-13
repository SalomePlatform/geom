//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org
//
//
//
//  File   : GeometryGUI.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef GEOMETRYGUI_H
#define GEOMETRYGUI_H

#include "SalomeApp_Module.h"

#include "GEOMGUI.h"
#include "GEOM_Client.hxx"
#include "SALOME_InteractiveObject.hxx"

#include "SALOMEDSClient.hxx"

// QT Includes
#include <qmap.h>

// OCCT Includes
#include <gp_Ax3.hxx>

// IDL headers
#include "SALOMEconfig.h"
#include CORBA_CLIENT_HEADER(SALOMEDS)

#ifdef WNT
#include <SALOME_WNT.hxx>
#else
#define SALOME_WNT_EXPORT
#endif

//#if defined WNT 
//#include <SALOME_WNT.hxx>
//#else
//#define SALOME_WNT_EXPORT
//#endif

#if defined WNT && defined WIN32 && defined SALOME_WNT_EXPORTS
#define GEOMGUI_WNT_EXPORT __declspec( dllexport )
#else
#define GEOMGUI_WNT_EXPORT
#endif

typedef QMap<QString, GEOMGUI*> GUIMap;

class QDialog;
class QPopupMenu;
class GEOMGUI_OCCSelector;
class LightApp_VTKSelector;
class LightApp_Selection;
class SUIT_ViewManager;

//=================================================================================
// class    : GeometryGUI
// purpose  :
//=================================================================================
class  GEOMGUI_WNT_EXPORT GeometryGUI : public SalomeApp_Module
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

  static bool                 InitGeomGen();   //BugID IPAL9186: SRN: To be called by Python scripts

  static  GEOM::GEOM_Gen_var  GetGeomGen();//        { return GeometryGUI::myComponentGeom; }

  static CORBA::Object_var    ClientSObjectToObject (_PTR(SObject) theSObject);
  static SALOMEDS::Study_var  ClientStudyToStudy (_PTR(Study) theStudy);

  GEOM_Client&                GetShapeReader()    { return myShapeReader; }
  Standard_CString&           GetFatherior()      { return myFatherior; }
  //void                        SetState( const int state ) { myState = state; }
  //int                         GetState() const    { return myState; }

  // Get active dialog box
  QDialog*                    GetActiveDialogBox(){ return myActiveDialogBox; }
  // Set active dialog box
  void                        SetActiveDialogBox( QDialog* aDlg );

  // Non modal dialog boxes management
  void                        EmitSignalDeactivateDialog();
  void                        EmitSignalCloseAllDialogs();
  void                        EmitSignalDefaultStepValueChanged( double newVal );

  void                        OnGUIEvent( int id );

//  virtual bool                SetSettings();
//  virtual void                SupportedViewType ( int* buffer, int bufferSize );
  virtual void                BuildPresentation( const Handle(SALOME_InteractiveObject)&, SUIT_ViewWindow* = 0 );

//  virtual void                DefinePopup( QString & theContext, QString & theParent, QString & theObject);
//  virtual bool                CustomPopup( QAD_Desktop* parent, QPopupMenu* popup, const QString& theContext,
//			                   const QString& theParent, const QString& theObject );

  // The Working Plane management
  void                        SetWorkingPlane( gp_Ax3 wp ) { myWorkingPlane = wp;   }
  gp_Ax3                      GetWorkingPlane()            { return myWorkingPlane; }
  void                        ActiveWorkingPlane();

  virtual void                windows( QMap<int, int>& ) const;
  virtual void                viewManagers( QStringList& ) const;

  virtual void                contextMenuPopup( const QString&, QPopupMenu*, QString& );
  virtual void                createPreferences();
  virtual void                preferencesChanged( const QString&, const QString& );


public slots:
  virtual bool                deactivateModule( SUIT_Study* );
  virtual bool                activateModule( SUIT_Study* );
  virtual void                OnKeyPress  ( SUIT_ViewWindow*, QKeyEvent*   );
  virtual void                OnMousePress( SUIT_ViewWindow*, QMouseEvent* );
  virtual void                OnMouseMove ( SUIT_ViewWindow*, QMouseEvent* );

protected slots:
  virtual void                onViewManagerAdded( SUIT_ViewManager* );
  virtual void                onViewManagerRemoved( SUIT_ViewManager* );

private slots:
  void                        OnGUIEvent();
  void                        onWindowActivated( SUIT_ViewWindow* );

signals :
  void                        SignalDeactivateActiveDialog();
  void                        SignalCloseAllDialogs();
  void                        SignalDefaultStepValueChanged( double newVal );

protected:
  virtual LightApp_Selection* createSelection() const;

private:
  GEOMGUI*                    getLibrary( const QString& libraryName );
  void                        createGeomAction( const int id, const QString& po_id,
						const QString& icon_id = QString(""),
						const int key = 0, const bool toggle = false );
  void                        createPopupItem( const int, const QString& clients, const QString& types,
					       const bool isSingle = false, const int isVisible = -1,
					       const bool isExpandAll = false, const bool isOCC = false,
					       const int parentId = -1 );

public:
  static GEOM::GEOM_Gen_var   myComponentGeom;   // GEOM engine!!!
private:  
  GUIMap                      myGUIMap;          // GUI libraries map
  QDialog*                    myActiveDialogBox; // active dialog box
  GEOM_Client                 myShapeReader;     // geom shape reader
  Standard_CString            myFatherior;
  int                         myState;           // identify a method
  gp_Ax3                      myWorkingPlane;
  QMap<int,QString>           myRules;           // popup rules

  QPtrList<GEOMGUI_OCCSelector>   myOCCSelectors;
  QPtrList<LightApp_VTKSelector> myVTKSelectors;

  LightApp_Displayer*         myDisplayer;

friend class DisplayGUI;
};

#endif
