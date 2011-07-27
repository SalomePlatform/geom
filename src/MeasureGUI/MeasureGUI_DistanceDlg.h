//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : MeasureGUI_DistanceDlg.h
// Author : Nicolas REJNERI, Open CASCADE S.A.S.
//
#ifndef MEASUREGUI_DISTANCEDLG_H
#define MEASUREGUI_DISTANCEDLG_H

#include "MeasureGUI_Skeleton.h"

class MeasureGUI_2Sel4LineEdit;
class gp_Pnt;

//=================================================================================
// class    : MeasureGUI_DistanceDlg
// purpose  :
//=================================================================================

class MeasureGUI_DistanceDlg : public MeasureGUI_Skeleton
{ 
  Q_OBJECT
    
public:
  MeasureGUI_DistanceDlg( GeometryGUI*, QWidget* );
  ~MeasureGUI_DistanceDlg();

protected:
  // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
  virtual void                        processObject();
  virtual SALOME_Prs*                 buildPrs();
  virtual void                        SelectionIntoArgument();
  virtual void                        LineEditReturnPressed();
  virtual void                        SetEditCurrentArgument();
  virtual bool                        isValid( QString& );

private:
  void                                Init();
  bool                                getParameters( double&,
                                                     gp_Pnt&,
                                                     gp_Pnt& );
private:
  QLineEdit*                          myEditCurrentArgument;
  QLineEdit*                          mySelEdit2;
  QPushButton*                        mySelBtn2;
  
  MeasureGUI_2Sel4LineEdit*           myGrp;
  GEOM::GEOM_Object_var               myObj2;
};

#endif // MEASUREGUI_DISTANCEDLG_H
