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
// File   : RepairGUI_FreeBoundDlg.h
// Author : Sergey LITONIN, Open CASCADE S.A.S. (sergey.litonin@opencascade.com)

#ifndef REPAIRGUI_FREEBOUNDDLG_H
#define REPAIRGUI_FREEBOUNDDLG_H

#include <QDialog>
#include <GEOMBase_Skeleton.h>

class QLineEdit;
class QLabel;
class GeometryGUI;

//=================================================================================
// class    : RepairGUI_FreeBoundDlg
// purpose  : Dialog for displaying free boundaries of selected face, shell or solid
//=================================================================================
class RepairGUI_FreeBoundDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  RepairGUI_FreeBoundDlg (GeometryGUI*, QWidget*);
  ~RepairGUI_FreeBoundDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr    createOperation();
  virtual bool                          isValid (QString&);
  virtual bool                          execute (ObjectList&);
  virtual GEOM::GEOM_Object_ptr         getFather (GEOM::GEOM_Object_ptr);
  virtual QString                       getNewObjectName (int CurrObj = -1) const; 
  virtual QList<GEOM::GeomObjPtr>       getSourceObjects();

private:
  void                                  Init();
  void                                  enterEvent (QEvent*);
  void                                  activateSelection();

private:
  QLineEdit*                            myEdit;
  QLabel*                               myClosedLbl;
  QLabel*                               myOpenLbl;

  GEOM::GEOM_Object_var                 myObj;
  int                                   myNbClosed;
  int                                   myNbOpen;
  int                                   myCurrObj;

private slots:
  void                                  ClickOnOk();
  bool                                  ClickOnApply();
  void                                  SelectionIntoArgument();
  void                                  ActivateThisDialog();
};

#endif // REPAIRGUI_FREEBOUNDDLG_H
