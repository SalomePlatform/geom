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

#include "SALOMEGUI.h"

#include "GEOMGUI.h"
#include "GEOM_Client.hxx"
#include "SALOME_InteractiveObject.hxx"
#include <qmap.h>

#include "gp_Ax3.hxx"

typedef QMap<QString, GEOMGUI*> GUIMap;

class QDialog;
class QPopupMenu;

//=================================================================================
// class    : GeometryGUI
// purpose  :
//=================================================================================
class GeometryGUI : public SALOMEGUI
{
  Q_OBJECT;

protected:
  // Constructor
  GeometryGUI(); // hide constructor to avoid direct creation
  // get or load GUI library by name
  GEOMGUI* getLibrary( const QString& libraryName );

public:
  // Destructor
  ~GeometryGUI();

  // Get the only GeometryGUI object
  static GeometryGUI*         GetGeomGUI();

  static CORBA::ORB_var       GetORB();

  GEOM::GEOM_Gen_ptr          GetGeomGen()        { return myComponentGeom; }
  GEOM_Client&                GetShapeReader()    { return myShapeReader; }
  Standard_CString&           GetFatherior()      { return myFatherior; }
  void                        SetState( const int state ) { myState = state; }
  int                         GetState() const    { return myState; }
  
  // Get active dialog box
  QDialog*                    GetActiveDialogBox(){ return myActiveDialogBox; }
  // Set active dialog box
  void                        SetActiveDialogBox( QDialog* aDlg );

  // Non modal dialog boxes management
  void                        EmitSignalDeactivateDialog();
  void                        EmitSignalCloseAllDialogs();
  void                        EmitSignalDefaultStepValueChanged( double newVal );

  // The following methods are called from IAPP
  virtual bool OnGUIEvent(int theCommandID, QAD_Desktop* parent);
  virtual bool OnMousePress(QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame);
  virtual bool OnMouseMove(QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame);
  virtual bool OnKeyPress(QKeyEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame);
  virtual bool SetSettings(QAD_Desktop* parent);
  virtual bool ActiveStudyChanged(QAD_Desktop* parent);
  virtual void Deactivate();
  virtual void BuildPresentation( const Handle(SALOME_InteractiveObject)&,
                                  QAD_ViewFrame*  = 0 );
  virtual void SupportedViewType (int* buffer, int bufferSize);
  virtual void DefinePopup(QString & theContext, QString & theParent, QString & theObject);
  virtual bool CustomPopup(QAD_Desktop* parent, QPopupMenu* popup, const QString& theContext,
			   const QString& theParent, const QString& theObject);

  // The Working Plane management
  void SetWorkingPlane(gp_Ax3 WorkingPlane) { myWorkingPlane = WorkingPlane; };
  gp_Ax3 GetWorkingPlane() { return myWorkingPlane; };
  void ActiveWorkingPlane();

signals :
  void SignalDeactivateActiveDialog();
  void SignalCloseAllDialogs();
  void SignalDefaultStepValueChanged( double newVal );

private:
  static GeometryGUI* myContext;         // the only GeometryGUI object

  GUIMap              myGUIMap;          // GUI libraries map
  QDialog*            myActiveDialogBox; // active dialog box
  GEOM_Client         myShapeReader;     // geom shape reader
  Standard_CString    myFatherior;
  GEOM::GEOM_Gen_var  myComponentGeom;   // GEOM engine
  int                 myState;           // identify a method
  gp_Ax3              myWorkingPlane;
};

#endif

