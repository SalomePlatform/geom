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
// File   : OperationGUI_MaterialDlg.h
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//
#ifndef OPERATIONGUI_MATERIALDLG_H
#define OPERATIONGUI_MATERIALDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_1List1Spin1Btn;

//=================================================================================
// class    : OperationGUI_MaterialDlg
// purpose  :
//=================================================================================
class OperationGUI_MaterialDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  OperationGUI_MaterialDlg( GeometryGUI*, QWidget*, GEOM::ListOfGO,
                            bool = false );
  ~OperationGUI_MaterialDlg();
  
private:
  void                       Init();
  void                       enterEvent( QEvent* );
  
private:
  GEOM::ListOfGO             myListShapes;
  GEOM::ListOfLong           myListMaterials;
  
  DlgRef_1List1Spin1Btn*     GroupPoints;
  
private slots:
  void                       ClickOnOk();
  void                       ActivateThisDialog();
  void                       SelectionIntoArgument();
  void                       SetMaterial();
};

#endif // OPERATIONGUI_MATERIALDLG_H
