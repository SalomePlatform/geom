// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
// File   : EntityGUI_Widgets.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef ENTITYGUI_WIDGETS_H
#define ENTITYGUI_WIDGETS_H

//////////////////////////////////////////
// EntityGUI_1Sel
//////////////////////////////////////////

#include "ui_EntityGUI_1Sel_QTD.h"

class EntityGUI_1Sel : public QWidget, 
                       public Ui::EntityGUI_1Sel_QTD
{
  Q_OBJECT

public:
  EntityGUI_1Sel( QWidget* = 0, Qt::WindowFlags = 0 );
  ~EntityGUI_1Sel();
};

//////////////////////////////////////////
// EntityGUI_1Spin
//////////////////////////////////////////

#include "ui_EntityGUI_1Spin_QTD.h"

class EntityGUI_1Spin : public QWidget, 
                        public Ui::EntityGUI_1Spin_QTD
{
  Q_OBJECT

public:
  EntityGUI_1Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~EntityGUI_1Spin();
};

//////////////////////////////////////////
// EntityGUI_2Spin
//////////////////////////////////////////

#include "ui_EntityGUI_2Spin_QTD.h"

class EntityGUI_2Spin : public QWidget, 
                        public Ui::EntityGUI_2Spin_QTD
{
  Q_OBJECT

public:
  EntityGUI_2Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~EntityGUI_2Spin();
};

//////////////////////////////////////////
// EntityGUI_3Spin
//////////////////////////////////////////

#include "ui_EntityGUI_3Spin_QTD.h"

class EntityGUI_3Spin : public QWidget, 
                        public Ui::EntityGUI_3Spin_QTD
{
  Q_OBJECT

public:
  EntityGUI_3Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~EntityGUI_3Spin();
};

//////////////////////////////////////////
// EntityGUI_4Spin
//////////////////////////////////////////

#include "ui_EntityGUI_4Spin_QTD.h"

class EntityGUI_4Spin : public QWidget, 
                        public Ui::EntityGUI_4Spin_QTD
{
  Q_OBJECT

public:
  EntityGUI_4Spin( QWidget* = 0, Qt::WindowFlags = 0 );
  ~EntityGUI_4Spin();
};

//////////////////////////////////////////
// EntityGUI_Dir1
//////////////////////////////////////////

#include "ui_EntityGUI_Dir1_QTD.h"

class EntityGUI_Dir1 : public QWidget, 
                       public Ui::EntityGUI_Dir1_QTD
{
  Q_OBJECT

public:
  EntityGUI_Dir1( QWidget* = 0, Qt::WindowFlags = 0 );
  ~EntityGUI_Dir1();

  QButtonGroup* ButtonGroup;
};

//////////////////////////////////////////
// EntityGUI_Dir2
//////////////////////////////////////////

#include "ui_EntityGUI_Dir2_QTD.h"

class EntityGUI_Dir2 : public QWidget, 
                       public Ui::EntityGUI_Dir2_QTD
{
  Q_OBJECT

public:
  EntityGUI_Dir2( QWidget* = 0, Qt::WindowFlags = 0 );
  ~EntityGUI_Dir2();

  QButtonGroup* ButtonGroup;
};

//////////////////////////////////////////
// EntityGUI_Point
//////////////////////////////////////////

#include "ui_EntityGUI_Point_QTD.h"

class EntityGUI_Point : public QWidget, 
                        public Ui::EntityGUI_Point_QTD
{
  Q_OBJECT

public:
  EntityGUI_Point( QWidget* = 0, Qt::WindowFlags = 0 );
  ~EntityGUI_Point();

  QButtonGroup* ButtonGroup;
};

//////////////////////////////////////////
// EntityGUI_Skeleton
//////////////////////////////////////////

#include "ui_EntityGUI_Skeleton_QTD.h"

class QButtonGroup;

class EntityGUI_Skeleton : public QWidget, 
                           public Ui::EntityGUI_Skeleton_QTD
{
  Q_OBJECT

public:
  EntityGUI_Skeleton( QWidget* = 0, Qt::WindowFlags = 0 );
  ~EntityGUI_Skeleton();

  QButtonGroup* ButtonGroup;
};

#endif // ENTITYGUI_WIDGETS_H