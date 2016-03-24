// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : GEOMBase_DlgSkeleton.h
// Author : Roman NIKOLAEV, Open CASCADE S.A.S.

#ifndef GEOMBASE_DLGSKELETON_N
#define GEOMBASE_DLGSKELETON_N

#include <QWidget>
#include "GEOM_GEOMBase.hxx"

class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class QRadioButton;
class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;

class GEOMBASE_EXPORT GEOMBase_DlgSkeleton : public QWidget
{
  Q_OBJECT
 public:
  GEOMBase_DlgSkeleton( QWidget* = 0, Qt::WindowFlags = 0 );
  ~GEOMBase_DlgSkeleton();
 private: 
  void init();
 public:
  QGridLayout *gridLayout;
  QGroupBox *GroupConstructors;
  QHBoxLayout *hboxLayout;
  QRadioButton *RadioButton1;
  QRadioButton *RadioButton2;
  QRadioButton *RadioButton3;
  QRadioButton *RadioButton4;
  QRadioButton *RadioButton5;
  QGroupBox *GroupBoxName;
  QHBoxLayout *hboxLayout1;
  QLabel *NameLabel;
  QLineEdit *ResultName;
  QWidget *GroupMedium;
  QGroupBox *GroupBoxPublish;
  QVBoxLayout *vboxLayout;
  QCheckBox *CheckBoxRestoreSS;
  QCheckBox *CheckBoxAddPrefix;
  QCheckBox *CheckBoxPreview;
  QGroupBox *GroupButtons;
  QHBoxLayout *hboxLayout2;
  QPushButton *buttonOk;
  QPushButton *buttonApply;
  QPushButton *buttonCancel;
  QPushButton *buttonHelp;
};

#endif //GEOMBASE_DLGSKELETON_N
