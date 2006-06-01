//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  CEA
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
//  File   : BlocksGUI_ExplodeDlg.h
//  Author : Julia DOROVSKIKH
//  Module : GEOM

#ifndef DIALOGBOX_BLOCK_EXPLODE_H
#define DIALOGBOX_BLOCK_EXPLODE_H

#include "GEOMBase_Skeleton.h"

class DlgRef_SpinBox;
class QTextEdit;
class QCheckBox;

//=================================================================================
// class    : BlocksGUI_ExplodeDlg
// purpose  :
//=================================================================================
class BlocksGUI_ExplodeDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  BlocksGUI_ExplodeDlg (GeometryGUI*, QWidget* parent, bool modal = FALSE);
  ~BlocksGUI_ExplodeDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid (QString& msg);
  virtual bool                       execute (ObjectList& objects);
  virtual GEOM::GEOM_Object_ptr      getFather(GEOM::GEOM_Object_ptr theObj);

  virtual const char*                getNewObjectName() const; 

private:
  void Init();
  void enterEvent (QEvent* e);

  void activateSelection();

  void updateButtonState();
  bool isAllSubShapes() const;
  int  shapeType() const;

  void clearTemporary();

private:
  int                        myConstructorId;

  CORBA::Long                myNbBlocks;
  GEOM::GEOM_Object_var      myObject;

  ObjectList                 myTmpObjs;

  QFrame*                    myGrp1;

  QPushButton*               mySelBtn;
  QLineEdit*                 mySelName;

  DlgRef_SpinBox*            mySpinBoxMin;
  DlgRef_SpinBox*            mySpinBoxMax;

  QTextEdit*                 myBlocksNb;

  QCheckBox*                 myCheckBtn;

private slots:
  void ClickOnOk();
  bool ClickOnApply();
  void ActivateThisDialog();
  void ConstructorsClicked (int constructorId);

  void SelectionIntoArgument();
  void SetEditCurrentArgument();

  void SubShapeToggled();

  void ValueChangedInSpinBox (double newValue);
};

#endif // DIALOGBOX_BLOCK_EXPLODE_H
