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
// File   : BlocksGUI_BlockDlg.h
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//
#ifndef BLOCKSGUI_BLOCKDLG_H
#define BLOCKSGUI_BLOCKDLG_H

#include "GEOMBase_Skeleton.h"
#include "GEOM_GenericObjPtr.h"

class DlgRef_2Sel;
class DlgRef_6Sel;

//=================================================================================
// class    : BlocksGUI_BlockDlg
// purpose  :
//=================================================================================
class BlocksGUI_BlockDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  BlocksGUI_BlockDlg (GeometryGUI*, QWidget*);
  ~BlocksGUI_BlockDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid (QString&);
  virtual bool                       execute (ObjectList&);
  virtual void                       addSubshapesToStudy();
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent (QEvent*);

private:
  int                                myConstructorId;

  GEOM::GeomObjPtr                   myFace1, myFace2;
  GEOM::GeomObjPtr                   myFace3, myFace4, myFace5, myFace6;

  // to initialize the first selection field with a selected object on the dialog creation
  bool                               myInitial;

  DlgRef_2Sel*                       Group2F;
  DlgRef_6Sel*                       Group6F;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               ConstructorsClicked (int);

  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
};

#endif // BLOCKSGUI_BLOCKDLG_H
