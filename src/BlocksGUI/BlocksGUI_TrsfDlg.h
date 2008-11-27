//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  GEOM GEOMGUI : GUI for Geometry component
//  File   : BlocksGUI_TrsfDlg.h
//  Author : Julia DOROVSKIKH
//  Module : GEOM
//
#ifndef DIALOGBOX_BLOCK_MULTITRSF_H
#define DIALOGBOX_BLOCK_MULTITRSF_H

#include "GEOM_BlocksGUI.hxx"

#include "GEOMBase_Skeleton.h"

class DlgRef_SpinBox;

//=================================================================================
// class    : BlocksGUI_TrsfDlg
// purpose  :
//=================================================================================
class GEOM_BLOCKSGUI_EXPORT BlocksGUI_TrsfDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

  enum { MainObj1, Face1, Face2, MainObj2, Face1U, Face2U, Face1V, Face2V };
  enum { SpinBox1, SpinBox2U, SpinBox2V };

public:
  BlocksGUI_TrsfDlg (GeometryGUI*, QWidget* parent, bool modal = FALSE);
  ~BlocksGUI_TrsfDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid (QString& msg);
  virtual bool                       execute (ObjectList& objects);

private:
  void Init();
  void enterEvent (QEvent* e);

  void createSelWg (const QString&, QPixmap&, QWidget*, const int);
  void activateSelection();
  void enableWidgets();

private:
  int                        myConstructorId;

  GEOM::GEOM_Object_var      myShape;
  QMap<int, int>             myFaces;

  QFrame*                    myGrp1;
  QFrame*                    myGrp2;

  QMap<int, QPushButton*>    mySelBtn;
  QMap<int, QLineEdit*>      mySelName;
  QMap<int, DlgRef_SpinBox*> mySpinBox;

private slots:
  void ClickOnOk();
  bool ClickOnApply();
  void ActivateThisDialog();
  void ConstructorsClicked (int constructorId);

  void SelectionIntoArgument();
  void SetEditCurrentArgument();

  void ValueChangedInSpinBox (double newValue);
};

#endif // DIALOGBOX_BLOCK_MULTITRSF_H
