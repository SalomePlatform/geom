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
// File   : BlocksGUI_TrsfDlg.h
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//
#ifndef BLOCKSGUI_TRSFDLG_H
#define BLOCKSGUI_TRSFDLG_H

#include <GEOMBase_Skeleton.h>

#include <QMap>

class SalomeApp_IntSpinBox;
class QGroupBox;
class QPushButton;
class QLineEdit;

//=================================================================================
// class    : BlocksGUI_TrsfDlg
// purpose  :
//=================================================================================
class BlocksGUI_TrsfDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

  enum { MainObj1, Face1, Face2, MainObj2, Face1U, Face2U, Face1V, Face2V };
  enum { SpinBox1, SpinBox2U, SpinBox2V };

public:
  BlocksGUI_TrsfDlg (GeometryGUI*, QWidget*);
  ~BlocksGUI_TrsfDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid (QString&);
  virtual bool                       execute (ObjectList&);
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent(QEvent*);

  void                               createSelWg (const QString&, QPixmap&, QWidget*, const int);
  void                               createSpinWg (const QString&, QWidget*, const int);
  void                               activateSelection();
  void                               enableWidgets();

private:
  int                                myConstructorId;

  GEOM::GEOM_Object_var              myShape;
  QMap<int, int>                     myFaces;

  // to initialize the first selection field with a selected object on the dialog creation
  bool                               myInitial;

  QGroupBox*                         myGrp1;
  QGroupBox*                         myGrp2;

  QMap<int, QPushButton*>            mySelBtn;
  QMap<int, QLineEdit*>              mySelName;
  QMap<int, SalomeApp_IntSpinBox*>   mySpinBox;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               ConstructorsClicked (int);

  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();

  void                               ValueChangedInSpinBox (int);
};

#endif // BLOCKSGUI_TRSFDLG_H
