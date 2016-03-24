// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : MeasureGUI_AngleDlg.h
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S.
//
#ifndef MEASUREGUI_ANGLEDLG_H
#define MEASUREGUI_ANGLEDLG_H

#include "MeasureGUI_Skeleton.h"

class MeasureGUI_2Sel1LineEdit;

//=================================================================================
// class    : MeasureGUI_AngleDlg
// purpose  :
//=================================================================================
class MeasureGUI_AngleDlg : public MeasureGUI_Skeleton
{ 
  Q_OBJECT

public:
  MeasureGUI_AngleDlg( GeometryGUI*, QWidget* );
  ~MeasureGUI_AngleDlg();

protected:
  // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
  virtual void                  processObject();
  virtual SALOME_Prs*           buildPrs();
  virtual void                  SelectionIntoArgument();
  virtual void                  LineEditReturnPressed();
  virtual void                  SetEditCurrentArgument();
  virtual bool                  isValid( QString& );

private:
  void                          Init();
  bool                          getParameters( double& );
  void                          activateSelection();

private:
  QLineEdit*                    myEditCurrentArgument;
  QLineEdit*                    mySelEdit2;
  QPushButton*                  mySelBtn2;
  MeasureGUI_2Sel1LineEdit*     myGrp;

  GEOM::GeomObjPtr              myObj2;
};

#endif // MEASUREGUI_ANGLEDLG_H
