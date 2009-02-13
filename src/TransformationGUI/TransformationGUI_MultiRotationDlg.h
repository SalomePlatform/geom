//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// GEOM GEOMGUI : GUI for Geometry component
// File   : TransformationGUI_MultiRotationDlg.h
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#ifndef TRANSFORMATIONGUI_MULTIROTATIONDLG_H
#define TRANSFORMATIONGUI_MULTIROTATIONDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_2Sel4Spin1Check;
class DlgRef_2Sel1Spin;

//=================================================================================
// class    : TransformationGUI_MultiRotationDlg
// purpose  :
//=================================================================================
class TransformationGUI_MultiRotationDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  TransformationGUI_MultiRotationDlg( GeometryGUI*, QWidget* = 0,
				      bool = false, Qt::WindowFlags = 0 );
  ~TransformationGUI_MultiRotationDlg();
  
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
  GEOM::GEOM_Object_var              myBase, myVector;
  int                                myNbTimes1;
  int                                myNbTimes2;
  Standard_Real                      myAng;
  Standard_Real                      myStep;

  // to initialize the first selection field with a selected object on the dialog creation
  bool                               myInitial;
  
  DlgRef_2Sel1Spin*                  GroupPoints;
  DlgRef_2Sel4Spin1Check*            GroupDimensions;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ReverseAngle();
  void                               ValueChangedInSpinBox( double );
  void                               TextValueChangedInSpinBox( const QString& );
  void                               ConstructorsClicked( int );
  void                               SetDoubleSpinBoxStep( double );
};

#endif // TRANSFORMATIONGUI_MULTIROTATIONDLG_H
