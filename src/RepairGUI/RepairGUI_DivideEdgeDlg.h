// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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
// File   : RepairGUI_DivideEdgeDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#ifndef REPAIRGUI_DIVIDEEDGEDLG_H
#define REPAIRGUI_DIVIDEEDGEDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_2SelExt;
class SalomeApp_DoubleSpinBox;
class QButtonGroup;

//=================================================================================
// class    : RepairGUI_DivideEdgeDlg
// purpose  :
//=================================================================================
class RepairGUI_DivideEdgeDlg : public GEOMBase_Skeleton
{
  Q_OBJECT
      
public:
  RepairGUI_DivideEdgeDlg( GeometryGUI*, QWidget* = 0, bool = false );
  ~RepairGUI_DivideEdgeDlg();
  
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
  void                               initSelection();
  
  bool                               getIsByParameter() const;
  int                                myIndex;

private:
  GEOM::GEOM_Object_var              myObject;
  QList<GEOM::GeomObjPtr>            myPoints;
  bool                               myProjectionOK;

  DlgRef_2SelExt*                    GroupPoints;
  QButtonGroup*                      myIsParameterGr;
  SalomeApp_DoubleSpinBox*           myValEdt;
  QLabel*                            myValLbl;

protected slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();

  void                               displayPreview();
  void                               ValueChangedInSpinBox();
  void                               ActivateThisDialog();
  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ConstructorsClicked( int );
};

#endif // REPAIRGUI_DIVIDEEDGEDLG_H
