// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef ADVANCEDGUI_SMOOTHINGSURFACEDLG_H
#define ADVANCEDGUI_SMOOTHINGSURFACEDLG_H

#include <GEOMBase_Skeleton.h>
#include "GEOM_GenericObjPtr.h"

#include <list>

class DlgRef_1Sel;
class SalomeApp_IntSpinBox;

//=================================================================================
// class    : AdvancedGUI_SmoothingSurfaceDlg
// purpose  :
//=================================================================================
class AdvancedGUI_SmoothingSurfaceDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  AdvancedGUI_SmoothingSurfaceDlg( GeometryGUI*, QWidget* = 0 );
  ~AdvancedGUI_SmoothingSurfaceDlg();

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
  int                                getNbPoints() const;

private:
  DlgRef_1Sel*                 GroupPoints;
  QList<GEOM::GeomObjPtr>            myPoints;
  SalomeApp_IntSpinBox              *myNbMaxSpin;
  SalomeApp_IntSpinBox              *myDegMaxSpin;
  SalomeApp_DoubleSpinBox           *myDMaxSpin;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  
  void                               ActivateThisDialog();
  //void                               DeactivateActiveDialog();
  
  //void                               CheckButtonToggled();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  //void                               ValueChangedInSpinBox();
  //void                               SetDoubleSpinBoxStep( double );
};

#endif // ADVANCEDGUI_SMOOTHINGSURFACEDLG_H
