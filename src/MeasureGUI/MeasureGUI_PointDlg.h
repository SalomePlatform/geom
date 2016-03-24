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
// File   : MeasureGUI_PointDlg.h
// Author : Sergey LITONIN, Open CASCADE S.A.S. (sergey.litonin@opencascade.com)
//
#ifndef MEASUREGUI_POINTDLG_H
#define MEASUREGUI_POINTDLG_H

#include "MeasureGUI_Skeleton.h"

class MeasureGUI_1Sel3LineEdit;

//=================================================================================
// class    : MeasureGUI_PointDlg
// purpose  : Dialog for displaying point coordinates
//=================================================================================

class MeasureGUI_PointDlg : public MeasureGUI_Skeleton
{ 
  Q_OBJECT

public:
  MeasureGUI_PointDlg( GeometryGUI*, QWidget* );
  ~MeasureGUI_PointDlg();

protected:
  // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
  virtual void                        activateSelection();
  virtual void                        SelectionIntoArgument();
  
private:
  void                                Init();

private:
  MeasureGUI_1Sel3LineEdit*           myGrp;
};

#endif // MEASUREGUI_POINTDLG_H
