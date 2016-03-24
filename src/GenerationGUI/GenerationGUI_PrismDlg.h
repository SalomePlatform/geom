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
// File   : GenerationGUI_PrismDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#ifndef GENERATIONGUI_PRISMDLG_H
#define GENERATIONGUI_PRISMDLG_H

#include "GEOMBase_Skeleton.h"
#include "GEOM_GenericObjPtr.h"

class DlgRef_2Sel1Spin3Check1Spin;
class DlgRef_3Sel2Check3Spin;
class DlgRef_1Sel3Spin2Check1Spin;
class DlgRef_1Check1Spin1Check;

//=================================================================================
// class    : GenerationGUI_PrismDlg
// purpose  :
//=================================================================================
class GenerationGUI_PrismDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  GenerationGUI_PrismDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
  ~GenerationGUI_PrismDlg();
  
protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );    
  virtual void                       addSubshapesToStudy();
  virtual bool                       extractPrefix() const;
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent( QEvent* );
 
private:
  QList<GEOM::GeomObjPtr>            myBaseObjects;  /* Base shapes */
  GEOM::GeomObjPtr                   myVec;  /* Vector, defining the direction */
  GEOM::GeomObjPtr                   myPoint1, myPoint2;   /* Points for extrusion */ 
  
  DlgRef_2Sel1Spin3Check1Spin*       GroupVecH;
  DlgRef_3Sel2Check3Spin*            Group2Points;
  DlgRef_1Sel3Spin2Check1Spin*       GroupDXDYDZ;
  DlgRef_1Check1Spin1Check*          GroupThickening;
  
private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ConstructorsClicked( int );
  void                               ValueChangedInSpinBox();
  void                               SetDoubleSpinBoxStep( double );
  void                               onReverse();
  void                               onBothway();
  void                               onScalePrism(bool);
  void                               onAddThickening( bool );
  void                               onChangeDirection( bool );
};

#endif // GENERATIONGUI_PRISMDLG_H
