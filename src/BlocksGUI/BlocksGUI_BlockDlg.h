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
//  See http://www.salome-platform.org or email : webmaster.salome@opencascade.org
//
//
//
//  File   : BlocksGUI_BlockDlg.h
//  Author : Julia DOROVSKIKH
//  Module : GEOM

#ifndef DIALOGBOX_BLOCK_H
#define DIALOGBOX_BLOCK_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel_QTD.h"
#include "DlgRef_6Sel_QTD.h"


//=================================================================================
// class    : BlocksGUI_BlockDlg
// purpose  :
//=================================================================================
class BlocksGUI_BlockDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  BlocksGUI_BlockDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
		      bool modal = FALSE);
  ~BlocksGUI_BlockDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid (QString&);
  virtual bool                       execute (ObjectList& objects);

private:
  void Init();
  void enterEvent (QEvent* e);

private:
  int                   myConstructorId;

  GEOM::GEOM_Object_var myFace1, myFace2;
  GEOM::GEOM_Object_var myFace3, myFace4, myFace5, myFace6;

  DlgRef_2Sel_QTD*      Group2F;
  DlgRef_6Sel_QTD*      Group6F;

private slots:
  void ClickOnOk();
  bool ClickOnApply();
  void ActivateThisDialog();
  void ConstructorsClicked (int constructorId);

  void SelectionIntoArgument();
  void SetEditCurrentArgument();
};

#endif // DIALOGBOX_BLOCK_H
