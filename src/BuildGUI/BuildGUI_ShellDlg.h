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
//
// File   : BuildGUI_ShellDlg.h
// Author : Damien COQUERET, Open CASCADE S.A.S.
//

#ifndef BUILDGUI_SHELLDLG_H
#define BUILDGUI_SHELLDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_1Sel;

//=================================================================================
// class    : BuildGUI_ShellDlg
// purpose  :
//=================================================================================
class BuildGUI_ShellDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  BuildGUI_ShellDlg( GeometryGUI*, QWidget* = 0 );
  ~BuildGUI_ShellDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );    

private:
  void                               Init();
  void                               enterEvent( QEvent* );

private:
  GEOM::ListOfGO                     myFacesAndShells;
  bool                               myOkFacesAndShells;/* to check when arguments is defined */
  
  DlgRef_1Sel*                       GroupShell;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               SetEditCurrentArgument();
  void                               SelectionIntoArgument();
  void                               ActivateThisDialog();
};

#endif // BUILDGUI_SHELLDLG_H
