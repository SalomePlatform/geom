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
// File   : MeasureGUI_DistanceDlg.h
// Author : Nicolas REJNERI, Open CASCADE S.A.S.

#ifndef MEASUREGUI_DISTANCEDLG_H
#define MEASUREGUI_DISTANCEDLG_H

#include <GEOMBase_Skeleton.h>

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

class gp_Pnt;

//=================================================================================
// class    : MeasureGUI_DistanceGroup
// purpose  :
//=================================================================================
class MeasureGUI_DistanceGroup : public QWidget
{
  Q_OBJECT

 public:
  MeasureGUI_DistanceGroup (QWidget *parent);
  ~MeasureGUI_DistanceGroup();

 public:
  QGridLayout *gridLayout;
  QGroupBox *GroupBox1;
  QGridLayout *gridLayout1;

  // 2Sel
  QLabel *TextLabel1;
  QLabel *TextLabel2;
  QPushButton *PushButton1;
  QPushButton *PushButton2;
  QLineEdit *LineEdit1;
  QLineEdit *LineEdit2;

  // 1Combo
  QLabel *TextLabel7;
  QComboBox *ComboBox1;

  // 4Text
  QLabel *TextLabel3;
  QLabel *TextLabel4;
  QLabel *TextLabel5;
  QLabel *TextLabel6;
  QLineEdit *LineEdit3;
  QLineEdit *LineEdit4;
  QLineEdit *LineEdit5;
  QLineEdit *LineEdit6;
};

//=================================================================================
// class    : MeasureGUI_DistanceDlg
// purpose  :
//=================================================================================
class MeasureGUI_DistanceDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  MeasureGUI_DistanceDlg (GeometryGUI*, QWidget*);
  ~MeasureGUI_DistanceDlg();

protected:
  // redefined from GEOMBase_Helper and GEOMBase_Skeleton
  virtual GEOM::GEOM_IOperations_ptr  createOperation();
  virtual bool                        isValid (QString&);
  virtual bool                        execute (ObjectList&);
  virtual void                        addSubshapesToStudy();
  virtual QString                     getNewObjectName (int CurrObj = -1) const; 
  virtual QList<GEOM::GeomObjPtr>     getSourceObjects();

  void                                redisplayPreview();
  virtual void                        processObject();
  virtual SALOME_Prs*                 buildPrs();

private:
  void                                Init();
  void                                enterEvent (QEvent*);
  void                                activateSelection();

private:
  GEOM::GeomObjPtr                    myObj1;
  GEOM::GeomObjPtr                    myObj2;

  MeasureGUI_DistanceGroup*           myGrp;
  GEOM::ListOfDouble_var              myDbls;

private slots:
  void                                ClickOnOk();
  bool                                ClickOnApply();
  void                                ActivateThisDialog();
  void                                SelectionIntoArgument();
  void                                SetEditCurrentArgument();
  void                                SolutionSelected (int i);
};

#endif // MEASUREGUI_DISTANCEDLG_H
