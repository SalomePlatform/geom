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

#ifndef ADVANCEDGUI_DIVIDEDCYLINDERDLG_H
#define ADVANCEDGUI_DIVIDEDCYLINDERDLG_H

#include <GEOMBase_Skeleton.h>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(AdvancedGEOM)

class DlgRef_2Spin;
class DlgRef_3Radio;

//=================================================================================
// class    : AdvancedGUI_DividedCylinderDlg
// purpose  :
//=================================================================================
class AdvancedGUI_DividedCylinderDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  AdvancedGUI_DividedCylinderDlg( GeometryGUI*, QWidget* = 0 );
  ~AdvancedGUI_DividedCylinderDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );

private:
  void                               Init();
  void                               enterEvent( QEvent* );

private:
  DlgRef_2Spin*                      GroupParams;
  DlgRef_3Radio*                     GroupPattern;
  GEOM::pattern                      myPattern;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               ValueChangedInSpinBox();
  void                               SetDoubleSpinBoxStep( double );
  void                               RadioButtonClicked();
};

#endif // ADVANCEDGUI_DIVIDEDCYLINDERDLG_H
