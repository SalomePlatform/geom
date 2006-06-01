//  GEOMCONTEXT
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : GEOMContext.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#ifndef GEOMCONTEXT_H
#define GEOMCONTEXT_H

#include "QAD_Desktop.h"
#include "GEOM_Client.hxx"
#include <OSD_SharedLibrary.hxx>
#include <qapplication.h>

//=================================================================================
// class    : GEOMContext
// purpose  :
//=================================================================================
class GEOMContext : public QObject
{
  Q_OBJECT /* for QT compatibility */

public :
  GEOMContext();
  ~GEOMContext();

private :
  QDialog* myActiveDialogBox; /* Unique active dialog box */
  GEOM_Client myShapeReader;
  Standard_CString myFatherior;

public :
  int myNbGeom; /* Unique name for a geom entity */

  static GEOMContext* GetOrCreateGeomGUI(QAD_Desktop* desktop);
  static GEOMContext* GetGeomGUI();

  OSD_SharedLibrary myGUILibrary;
  GEOM::GEOM_Gen_var myComponentGeom;
  int myState; /* Identify a method */

  /* Returns the active DialogBox */
  QDialog* GetActiveDialogBox(){return myActiveDialogBox ;};
  int& GetNbGeom(){return myNbGeom;};
  GEOM_Client& GetShapeReader(){return myShapeReader;};
  Standard_CString& GetFatherior(){return myFatherior;};

  bool LoadLibrary(QString GUILibrary);

  /* Sets 'myActiveDialogBox' a pointer to the active Dialog Box  */
  void SetActiveDialogBox(QDialog* aDlg);

  /* Non modal dialog boxes magement */
  void EmitSignalDeactivateDialog();
  void EmitSignalCloseAllDialogs();
  void EmitSignalDefaultStepValueChanged(double newVal);

signals :
  void SignalDeactivateActiveDialog();
  void SignalCloseAllDialogs();
  void SignalDefaultStepValueChanged(double newVal);

};

#endif

