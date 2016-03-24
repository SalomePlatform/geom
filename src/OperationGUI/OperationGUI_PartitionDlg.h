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
//  File   : OperationGUI_PartitionDlg.h
//  Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#ifndef OPERATIONGUI_PARTITIONDLG_H
#define OPERATIONGUI_PARTITIONDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_2Sel1List2Check;
class QCheckBox;

//=================================================================================
// class    : OperationGUI_PartitionDlg
// purpose  :
//=================================================================================
class OperationGUI_PartitionDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  OperationGUI_PartitionDlg( GeometryGUI*, QWidget* = 0 );
  ~OperationGUI_PartitionDlg();

  void                               SetListMaterials( GEOM::ListOfLong );
  GEOM::ListOfLong                   GetListMaterials();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  virtual void                       restoreSubShapes( SALOMEDS::Study_ptr, SALOMEDS::SObject_ptr );
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent( QEvent* );
  int                                GetLimit() const;

private:
  GEOM::ListOfGO                     myListShapes;
  GEOM::ListOfLong                   myListMaterials;
  GEOM::ListOfGO                     myListTools;
  GEOM::ListOfGO                     myListRemoveInside;
  GEOM::ListOfGO                     myListKeepInside;

  DlgRef_2Sel1List2Check*            GroupPoints;
  QCheckBox                         *mySelfInte;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ComboTextChanged();
  void                               ConstructorsClicked( int );
};

#endif // OPERATIONGUI_PARTITIONDLG_H
