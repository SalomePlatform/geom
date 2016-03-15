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
// File   : OperationGUI_FilletDlg.h
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#ifndef OPERATIONGUI_FILLETDLG_H
#define OPERATIONGUI_FILLETDLG_H

#include <GEOMBase_Skeleton.h>

#include <TColStd_IndexedMapOfInteger.hxx>

class DlgRef_1Sel1Spin;
class DlgRef_2Sel3Spin2Rb;

//=================================================================================
// class    : OperationGUI_FilletDlg
// purpose  :
//=================================================================================
class OperationGUI_FilletDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT
      
public:
  OperationGUI_FilletDlg( GeometryGUI*, QWidget* );
  ~OperationGUI_FilletDlg();
  
protected:
  // redefined from GEOMBase_Helper
  virtual                             GEOM::GEOM_IOperations_ptr createOperation();
  virtual                             bool isValid( QString& );
  virtual                             bool execute( ObjectList& );
  virtual QList<GEOM::GeomObjPtr>     getSourceObjects();

private slots:
  void                                ClickOnOk();
  bool                                ClickOnApply();
  void                                ActivateThisDialog();
  void                                LineEditReturnPressed();
  void                                SelectionIntoArgument();
  void                                SetEditCurrentArgument();
  void                                ValueChangedInSpinBox( double );
  void                                ConstructorsClicked( int );
  void                                RadioButtonClicked();
  
private:
  void                                Init();
  void                                enterEvent( QEvent* );
  void                                activateSelection();
  void                                enableWidgets();
  double                              getRadius() const;

private:
  int                                 myConstructorId;
  
  GEOM::GEOM_Object_var               myShape;
  TColStd_IndexedMapOfInteger         myEdges;
  TColStd_IndexedMapOfInteger         myFaces;

  // to initialize the first selection field with a selected object on the dialog creation
  bool                               myInitial;
  
  DlgRef_1Sel1Spin*                   Group1;
  DlgRef_2Sel3Spin2Rb*                Group2;
  DlgRef_2Sel3Spin2Rb*                Group3;
};

#endif // OPERATIONGUI_FILLETDLG_H
