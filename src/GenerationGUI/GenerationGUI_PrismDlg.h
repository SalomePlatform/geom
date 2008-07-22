// GEOM GEOMGUI : GUI for Geometry component
//
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : GenerationGUI_PrismDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//

#ifndef GENERATIONGUI_PRISMDLG_H
#define GENERATIONGUI_PRISMDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_2Sel1Spin2Check;
class DlgRef_3Sel1Check;
class DlgRef_1Sel3Spin1Check;

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

private:
  void                               Init();
  void                               enterEvent( QEvent* );
  double                             getHeight() const;
 
private:
  GEOM::GEOM_Object_var              myBase; /* Base shape */
  GEOM::GEOM_Object_var              myVec;  /* Vector, defining the direction */
  GEOM::GEOM_Object_var              myPoint1, myPoint2;   /* Points for extrusion */ 
  
  bool                               myOkBase;
  bool                               myOkVec;
  bool                               myOkPnt1;
  bool                               myOkPnt2;
  bool                               myBothway;
  bool                               myBothway2;
  bool                               myBothway3;
  
  DlgRef_2Sel1Spin2Check*            GroupPoints;
  DlgRef_3Sel1Check*                 GroupPoints2; // for second layout for extrusion using 2 points
  DlgRef_1Sel3Spin1Check*            GroupPoints3;
  
private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ConstructorsClicked( int );
  void                               ValueChangedInSpinBox();
  void                               SetDoubleSpinBoxStep( double );
  void                               onReverse();
  void                               onBothway();
};

#endif // GENERATIONGUI_PRISMDLG_H
