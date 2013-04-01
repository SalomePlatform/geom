// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : RepairGUI_FreeFacesDlg.h
// Author : Vladimir KLYACHIN, Open CASCADE S.A.S. (vladimir.klyachin@opencascade.com)
//
#ifndef REPAIRGUI_FREEFACESDLG_H
#define REPAIRGUI_FREEFACESDLG_H

#include <QDialog>
#include <GEOMBase_Helper.h>

class GEOM_Displayer;
class QPushButton;
class QLineEdit;
class GeometryGUI;

//=================================================================================
// class    : RepairGUI_FreeFacesDlg
// purpose  :
//=================================================================================
class RepairGUI_FreeFacesDlg : public QDialog,
                               public GEOMBase_Helper
{
  Q_OBJECT

public:
  RepairGUI_FreeFacesDlg( GeometryGUI*, QWidget* = 0, bool = false );
  ~RepairGUI_FreeFacesDlg();
  
protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  
private:
  void                               Init();
  void                               enterEvent( QEvent* );
  void                               closeEvent( QCloseEvent* );
  void                               keyPressEvent( QKeyEvent* );
  void                               activateSelection();
  GEOM_Displayer*                    getDisplayer();

private slots:
  void                               onClose();
  void                               onHelp(); 
  void                               onDeactivate();
  void                               onActivate();
  void                               onSelectionDone();
  void                               onSetEditCurrentArgument();

private:
  GEOM_Displayer*                    myDisplayer;
  GEOM::GEOM_Object_var              myObj;
  QPushButton*                       mySelBtn;
  QLineEdit*                         myEdit;
  GeometryGUI*                       myGeomGUI;
  QString                            myHelpFileName;
};

#endif // REPAIRGUI_FREEFACESDLG_H
