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
// File   : PrimitiveGUI_TorusDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#ifndef PRIMITIVEGUI_TORUSDLG_H
#define PRIMITIVEGUI_TORUSDLG_H

#include "GEOMBase_Skeleton.h"
#include "GEOM_GenericObjPtr.h"

class DlgRef_2Sel2Spin;
class DlgRef_2Spin;

//=================================================================================
// class    : PrimitiveGUI_TorusDlg
// purpose  :
//=================================================================================
class PrimitiveGUI_TorusDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  PrimitiveGUI_TorusDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
  ~PrimitiveGUI_TorusDlg();
  
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
  double                             getRadius1() const;
  double                             getRadius2() const;
  
private:
  GEOM::GeomObjPtr                   myPoint, myDir;

  DlgRef_2Sel2Spin*                  GroupPoints;
  DlgRef_2Spin*                      GroupDimensions;
  
private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ConstructorsClicked( int );
  void                               ValueChangedInSpinBox();
  void                               SetDoubleSpinBoxStep( double );
};

#endif // PRIMITIVEGUI_TORUSDLG_H
