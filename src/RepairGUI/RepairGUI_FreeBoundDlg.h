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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : RepairGUI_FreeBoundDlg.h
//  Author : Sergey LITONIN
//  Module : GEOM

#ifndef DIALOGBOX_WHATIS_H
#define DIALOGBOX_WHATIS_H

#include <qdialog.h>
#include "GEOMBase_Helper.h"

class QLineEdit;
class QLabel;
class GEOM_Displayer;
class GeometryGUI;

//=================================================================================
// class    : RepairGUI_FreeBoundDlg
// purpose  : Dialog for displaying free boundaries of selected face, shell or solid
//=================================================================================
class RepairGUI_FreeBoundDlg : public QDialog,
                               public GEOMBase_Helper
{ 
    Q_OBJECT

public:
    RepairGUI_FreeBoundDlg(GeometryGUI*, QWidget*, const char* name = 0);
    ~RepairGUI_FreeBoundDlg();

private slots:
  void                                  onClose();
  void                                  onHelp(); 
  void                                  onDeactivate();
  void                                  onActivate();
  void                                  onSelectionDone();
                                                                                  
private:
  void                                  Init();
  void                                  enterEvent( QEvent* e );
  void                                  closeEvent( QCloseEvent* e );
  void                                  activateSelection();
  
  virtual GEOM::GEOM_IOperations_ptr    createOperation();
  virtual bool                          execute( ObjectList& objects );
  virtual bool                          isValid( QString& );

private:
  QLineEdit*                            myEdit;
  QLabel*                               myClosedLbl;
  QLabel*                               myOpenLbl;
  GEOM::GEOM_Object_var                 myObj;
  GeometryGUI*                          myGeomGUI;
  int                                   myNbClosed;
  int                                   myNbOpen;
  QString                               myHelpFileName;
};

#endif // DIALOGBOX_WHATIS_H
