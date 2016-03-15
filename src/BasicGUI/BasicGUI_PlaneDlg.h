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
// File   : BasicGUI_PlaneDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#ifndef BASICGUI_PLANEDLG_H
#define BASICGUI_PLANEDLG_H

#include "GEOMBase_Skeleton.h"
#include "GEOM_GenericObjPtr.h"

class DlgRef_1Sel1Spin;
class DlgRef_2Sel1Spin;
class DlgRef_3Sel1Spin;
class DlgRef_3Radio1Sel1Spin;

//=================================================================================
// class    : BasicGUI_PlaneDlg
// purpose  :
//=================================================================================
class BasicGUI_PlaneDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  BasicGUI_PlaneDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
  ~BasicGUI_PlaneDlg();
  
protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  virtual void                       addSubshapesToStudy();
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent( QEvent* );
  double                             getSize() const;
  QString                            getSizeAsString() const; 
  int                                myOriginType;

private:
  GEOM::GeomObjPtr                   myPoint, myDir, myPoint1, myPoint2, myPoint3, myFace, myVec1, myVec2, myLCS;

  DlgRef_2Sel1Spin*                  GroupPntDir;
  DlgRef_3Sel1Spin*                  Group3Pnts;
  DlgRef_1Sel1Spin*                  GroupFace;
  DlgRef_2Sel1Spin*                  Group2Vec;
  DlgRef_3Radio1Sel1Spin*            GroupLCS;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  
  void                               ActivateThisDialog();
  void                               DeactivateActiveDialog();
  
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ConstructorsClicked( int );
  void                               ValueChangedInSpinBox( double );
  void                               SetDoubleSpinBoxStep( double );
  void                               GroupClicked();
};

#endif // BASICGUI_PLANEDLG_H
