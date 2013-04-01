// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : MeasureGUI_CheckCompoundOfBlocksDlg.h
// Author : Vladimir KLYACHIN, Open CASCADE S.A.S. (vladimir.klyachin@opencascade.com)
//
#ifndef MEASUREGUI_CHECKCOMPOUNDOFBLOCKSDLG_H
#define MEASUREGUI_CHECKCOMPOUNDOFBLOCKSDLG_H

#include <GEOMBase_Skeleton.h>

class MeasureGUI_1Sel1TextView2ListBox;

//=================================================================================
// class    : MeasureGUI_CheckCompoundOfBlocksDlg
// purpose  :
//=================================================================================

class MeasureGUI_CheckCompoundOfBlocksDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  MeasureGUI_CheckCompoundOfBlocksDlg( GeometryGUI*, QWidget* );
  ~MeasureGUI_CheckCompoundOfBlocksDlg();
  
protected:
  // redefined from GEOMBase_Helper and GEOMBase_Skeleton
  virtual GEOM::GEOM_IOperations_ptr  createOperation();
  virtual bool                        isValid( QString& );
  virtual bool                        execute( ObjectList& );
  virtual void                        processObject();

private slots:
  void                                ClickOnOk();
  bool                                ClickOnApply();
  void                                ActivateThisDialog();
  void                                LineEditReturnPressed();
  void                                SelectionIntoArgument();
  void                                SetEditCurrentArgument();
  
  void                                onErrorsListSelectionChanged();
  void                                onSubShapesListSelectionChanged();
  
private:
  void                                Init();
  void                                enterEvent( QEvent* );
  bool                                getBCErrors( bool&,
                                                   GEOM::GEOM_IBlocksOperations::BCErrors& );
  void                                activateSelection();

private:
  GEOM::GEOM_Object_var               myObj;
  MeasureGUI_1Sel1TextView2ListBox*   myGrp;
};

#endif // MEASUREGUI_CHECKCOMPOUNDOFBLOCKSDLG_H
