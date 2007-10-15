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
// File   : BasicGUI_PointDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//

#ifndef BASICGUI_POINTDLG_H
#define BASICGUI_POINTDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_1Sel1Spin;
class DlgRef_3Spin;
class DlgRef_1Sel3Spin;

class QLineEdit;
class QGroupBox;

class gp_Pnt;

//=================================================================================
// class    : BasicGUI_PointDlg
// purpose  :
//=================================================================================
class BasicGUI_PointDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  BasicGUI_PointDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );

  ~BasicGUI_PointDlg();

  bool                               acceptMouseEvent() const { return ( getConstructorId() == 0 );  }
  void                               OnPointSelected( const gp_Pnt& ); // called by BasicGUI::OnMousePress()

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  
  virtual void                       closeEvent( QCloseEvent* );

private:
  void                               Init();
  void                               enterEvent( QEvent* );
  double                             getParameter() const;

private:
  GEOM::GEOM_Object_var              myEdge;
  GEOM::GEOM_Object_var              myRefPoint; 
  
  DlgRef_3Spin*                      GroupXYZ;
  DlgRef_1Sel3Spin*                  GroupRefPoint;
  DlgRef_1Sel1Spin*                  GroupOnCurve;
  
  QGroupBox*                         myCoordGrp;
  QLineEdit*                         myX;
  QLineEdit*                         myY;
  QLineEdit*                         myZ;
  
private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               DeactivateActiveDialog();
  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ConstructorsClicked( int );
  void                               ValueChangedInSpinBox( double );
};

#endif // BASICGUI_POINTDLG_H
