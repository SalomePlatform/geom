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
// File   : MeasureGUI_WhatisDlg.h
// Author : Nicolas REJNERI, Open CASCADE S.A.S.
//
#ifndef MEASUREGUI_WHATISDLG_H
#define MEASUREGUI_WHATISDLG_H

#include "MeasureGUI_Skeleton.h"

class QMessageBox;

//=================================================================================
// class    : MeasureGUI_WhatisDlg
// purpose  :
//=================================================================================
class MeasureGUI_WhatisDlg : public MeasureGUI_Skeleton
{ 
  Q_OBJECT

  class Whatis;
      
public:
  MeasureGUI_WhatisDlg( GeometryGUI*, QWidget* );
  ~MeasureGUI_WhatisDlg();

protected:
  // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
  virtual void                        processObject();
  virtual void                        activateSelection();
  virtual void                        SelectionIntoArgument();
  
protected slots:
  virtual void                        ClickOnProperties();

private:
  void                                Init();
  bool                                getParameters( QString& );
  QString                             getKindOfShape( QString& );
  
private:
  Whatis*                             myGrp;
  QMessageBox*                        myParamsDlg;
};

#endif // MEASUREGUI_WHATISDLG_H
