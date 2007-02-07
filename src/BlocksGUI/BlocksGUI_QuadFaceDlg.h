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
//  File   : BlocksGUI_QuadFaceDlg.h
//  Author : Julia DOROVSKIKH
//  Module : GEOM

#ifndef DIALOGBOX_QUAD_FACE_H
#define DIALOGBOX_QUAD_FACE_H

#include "GEOM_BlocksGUI.hxx"

#include "GEOMBase_Skeleton.h"

//=================================================================================
// class    : BlocksGUI_QuadFaceDlg
// purpose  :
//=================================================================================
class GEOM_BLOCKSGUI_EXPORT BlocksGUI_QuadFaceDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

  enum { Vertex1, Vertex2, Vertex3, Vertex4,
	 Edge12, Edge22,
	 Edge14, Edge24, Edge34, Edge44 };

public:
  BlocksGUI_QuadFaceDlg (GeometryGUI*, QWidget* parent, bool modal = FALSE);
  ~BlocksGUI_QuadFaceDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual                       bool isValid (QString& msg);
  virtual                       bool execute (ObjectList& objects);

private:
  void Init();
  void enterEvent (QEvent* e);

  void createSelWg (const QString&, QPixmap&, QWidget*, const int);
  void activateSelection();

private:
  int                     myConstructorId;

  GEOM::GEOM_Object_var   myShape1;
  GEOM::GEOM_Object_var   myShape2;
  GEOM::GEOM_Object_var   myShape3;
  GEOM::GEOM_Object_var   myShape4;

  QFrame*                 myGrp1;
  QFrame*                 myGrp2;
  QFrame*                 myGrp3;

  QMap<int, QPushButton*> mySelBtn;
  QMap<int, QLineEdit*>   mySelName;

private slots:
  void ClickOnOk();
  bool ClickOnApply();
  void ActivateThisDialog();
  void ConstructorsClicked( int constructorId );

  void SelectionIntoArgument();
  void SetEditCurrentArgument();
};

#endif // DIALOGBOX_QUAD_FACE_H
