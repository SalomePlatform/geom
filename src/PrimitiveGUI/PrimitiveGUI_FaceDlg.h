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
// File   : BasicGUI_FaceDlg.h
// Author : Dmitry Matveithev, OCN.
//

#ifndef BASICGUI_FACEDLG_H
#define BASICGUI_FACEDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_2Spin;
class DlgRef_1Sel2Spin;

//=================================================================================
// class    : PrimitiveGUI_FaceDlg
// purpose  :
//=================================================================================
class PrimitiveGUI_FaceDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  PrimitiveGUI_FaceDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
  ~PrimitiveGUI_FaceDlg();
  
protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  virtual void                       addSubshapesToStudy();
    
private:
  void                               Init();
  void                               enterEvent( QEvent* );
  
private:
  GEOM::GEOM_Object_var              myPoint1;   
  GEOM::GEOM_Object_var              myPoint2;
  GEOM::GEOM_Object_var              myVector;
  GEOM::GEOM_Object_var              myFace;
  
  DlgRef_2Spin*                      GroupDimensions;
  DlgRef_1Sel2Spin*                  GroupPlane;
    
private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();

  void                               ActivateThisDialog();
  void                               DeactivateActiveDialog();
  void                               ConstructorsClicked( int );    
    
  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ValueChangedInSpinBox( double );
  void                               SetDoubleSpinBoxStep( double );
};

#endif // BASICGUI_FACEDLG_H
