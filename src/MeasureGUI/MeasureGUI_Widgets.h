// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : MeasureGUI_Widgets.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef MEASUREGUI_WIDGETS_H
#define MEASUREGUI_WIDGETS_H

//////////////////////////////////////////
// MeasureGUI_1Sel12LineEdit
//////////////////////////////////////////

#include "ui_MeasureGUI_1Sel12LineEdit_QTD.h"

class MeasureGUI_1Sel12LineEdit : public QWidget, 
                                  public Ui::MeasureGUI_1Sel12LineEdit_QTD
{
  Q_OBJECT

public:
  MeasureGUI_1Sel12LineEdit( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_1Sel12LineEdit();
};

//////////////////////////////////////////
// MeasureGUI_1Sel1TextView1Check
//////////////////////////////////////////

#include "ui_MeasureGUI_1Sel1TextView1Check_QTD.h"

class MeasureGUI_1Sel1TextView1Check : public QWidget, 
                                       public Ui::MeasureGUI_1Sel1TextView1Check_QTD
{
  Q_OBJECT

public:
  MeasureGUI_1Sel1TextView1Check( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_1Sel1TextView1Check();
};

//////////////////////////////////////////
// MeasureGUI_1Sel1TextView2ListBox
//////////////////////////////////////////

#include "ui_MeasureGUI_1Sel1TextView2ListBox_QTD.h"

class MeasureGUI_1Sel1TextView2ListBox : public QWidget, 
                                         public Ui::MeasureGUI_1Sel1TextView2ListBox_QTD
{
  Q_OBJECT

public:
  MeasureGUI_1Sel1TextView2ListBox( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_1Sel1TextView2ListBox();
};

//////////////////////////////////////////
// MeasureGUI_1Sel1Check1TextView2ListBox
//////////////////////////////////////////

#include "ui_MeasureGUI_1Sel1Check1TextView2ListBox_QTD.h"

class MeasureGUI_1Sel1Check1TextView2ListBox : public QWidget,
                                               public Ui::MeasureGUI_1Sel1Check1TextView2ListBox_QTD
{
  Q_OBJECT

public:
  MeasureGUI_1Sel1Check1TextView2ListBox( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_1Sel1Check1TextView2ListBox();
};

//////////////////////////////////////////
// MeasureGUI_1Sel1TextView
//////////////////////////////////////////

#include "ui_MeasureGUI_1Sel1TextView_QTD.h"

class MeasureGUI_1Sel1TextView : public QWidget, 
                                 public Ui::MeasureGUI_1Sel1TextView_QTD
{
  Q_OBJECT

public:
  MeasureGUI_1Sel1TextView( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_1Sel1TextView();
};

//////////////////////////////////////////
// MeasureGUI_1Sel3LineEdit
//////////////////////////////////////////

#include "ui_MeasureGUI_1Sel3LineEdit_QTD.h"

class MeasureGUI_1Sel3LineEdit : public QWidget, 
                                 public Ui::MeasureGUI_1Sel3LineEdit_QTD
{
  Q_OBJECT

public:
  MeasureGUI_1Sel3LineEdit( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_1Sel3LineEdit();
};

//////////////////////////////////////////
// MeasureGUI_1Sel6LineEdit
//////////////////////////////////////////

#include "ui_MeasureGUI_1Sel6LineEdit_QTD.h"

class MeasureGUI_1Sel6LineEdit : public QWidget, 
                                 public Ui::MeasureGUI_1Sel6LineEdit_QTD
{
  Q_OBJECT

public:
  MeasureGUI_1Sel6LineEdit( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_1Sel6LineEdit();
};

//////////////////////////////////////////
// MeasureGUI_1Sel_Frame
//////////////////////////////////////////

#include "ui_MeasureGUI_1Sel_Frame_QTD.h"

class MeasureGUI_1Sel_Frame : public QWidget, 
                              public Ui::MeasureGUI_1Sel_Frame_QTD
{
  Q_OBJECT

public:
  MeasureGUI_1Sel_Frame( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_1Sel_Frame();
};

//////////////////////////////////////////
// MeasureGUI_1TreeWidget_4Buttons
//////////////////////////////////////////

#include "ui_MeasureGUI_1TreeWidget_4Button_QTD.h"

class MeasureGUI_1TreeWidget_4Button : public QWidget,
                                       public Ui::MeasureGUI_1TreeWidget_4Button_QTD
{
  Q_OBJECT

public:
  MeasureGUI_1TreeWidget_4Button( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_1TreeWidget_4Button();
};

//////////////////////////////////////////
// MeasureGUI_2Sel1LineEdit
//////////////////////////////////////////

#include "ui_MeasureGUI_2Sel1LineEdit_QTD.h"

class MeasureGUI_2Sel1LineEdit : public QWidget, 
                                 public Ui::MeasureGUI_2Sel1LineEdit_QTD
{
  Q_OBJECT

public:
  MeasureGUI_2Sel1LineEdit( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_2Sel1LineEdit();
};

//////////////////////////////////////////
// MeasureGUI_2Sel_Frame
//////////////////////////////////////////

#include "ui_MeasureGUI_2Sel_Frame_QTD.h"

class MeasureGUI_2Sel_Frame : public QWidget, 
                              public Ui::MeasureGUI_2Sel_Frame_QTD
{
  Q_OBJECT

public:
  MeasureGUI_2Sel_Frame( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_2Sel_Frame();
};

//////////////////////////////////////////
// MeasureGUI_3Sel_Frame
//////////////////////////////////////////

#include "ui_MeasureGUI_3Sel_Frame_QTD.h"

class MeasureGUI_3Sel_Frame : public QWidget, 
                              public Ui::MeasureGUI_3Sel_Frame_QTD
{
  Q_OBJECT

public:
  MeasureGUI_3Sel_Frame( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_3Sel_Frame();
};

//////////////////////////////////////////
// MeasureGUI_SkeletonBox
//////////////////////////////////////////

#include "ui_MeasureGUI_SkeletonBox_QTD.h"

class MeasureGUI_SkeletonBox : public QWidget, 
                               public Ui::MeasureGUI_SkeletonBox_QTD
{
  Q_OBJECT

public:
  MeasureGUI_SkeletonBox( QWidget* = 0, Qt::WindowFlags = 0 );
  ~MeasureGUI_SkeletonBox();
};

#endif // MEASUREGUI_WIDGETS_H
