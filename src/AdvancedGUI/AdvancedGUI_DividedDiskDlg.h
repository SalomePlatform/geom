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

#ifndef ADVANCEDGUI_DIVIDEDDISKDLG_H
#define ADVANCEDGUI_DIVIDEDDISKDLG_H

#include <GEOMBase_Skeleton.h>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(AdvancedGEOM)

class DlgRef_1Spin;
class DlgRef_3Radio;
class DlgRef_2Sel1Spin;

//=================================================================================
// class    : AdvancedGUI_DividedDiskDlg
// purpose  :
//=================================================================================
class AdvancedGUI_DividedDiskDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  AdvancedGUI_DividedDiskDlg( GeometryGUI*, QWidget* = 0 );
  ~AdvancedGUI_DividedDiskDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent( QEvent* );

private:
  GEOM::GeomObjPtr                   myPoint, myDir;
  DlgRef_1Spin*                      GroupParams;
  DlgRef_3Radio*                     GroupOrientation;
  DlgRef_3Radio*                     GroupPattern;
  DlgRef_2Sel1Spin*                  GroupPntVecR;
  int                                myOrientation;
  GEOM::pattern                      myPattern;
  
private slots:
  void                               ConstructorsClicked ( int );
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               SetEditCurrentArgument();
  void                               SelectionIntoArgument();
  void                               ActivateThisDialog();
  void                               ValueChangedInSpinBox();
  void                               RadioButtonClicked();
  void                               SetDoubleSpinBoxStep( double );
};

#endif // ADVANCEDGUI_DIVIDEDDISKDLG_H
