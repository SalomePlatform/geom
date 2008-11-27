//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  GEOM GEOMGUI : GUI for Geometry component
//  File   : MeasureGUI_AngleDlg.h
//  Author : Julia DOROVSKIKH
//  Module : GEOM
//
#ifndef DIALOGBOX_ANGLE_H
#define DIALOGBOX_ANGLE_H

#include "GEOM_MeasureGUI.hxx"

#include "MeasureGUI_Skeleton.h"

class MeasureGUI_2Sel1LineEdit_QTD;

//=================================================================================
// class    : MeasureGUI_AngleDlg
// purpose  :
//=================================================================================
class GEOM_MEASUREGUI_EXPORT MeasureGUI_AngleDlg : public MeasureGUI_Skeleton
{ 
  Q_OBJECT

public:
  MeasureGUI_AngleDlg (GeometryGUI* GUI, QWidget* parent);
  ~MeasureGUI_AngleDlg();

protected:
  // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
  virtual void                  processObject();
  virtual SALOME_Prs*           buildPrs();
  virtual void                  SelectionIntoArgument();
  virtual void                  LineEditReturnPressed();
  virtual void                  SetEditCurrentArgument();
  virtual bool                  isValid (QString& msg);

private:
  void                          Init();
  bool                          getParameters (double& theAngle);

private:
  QLineEdit*                    myEditCurrentArgument;
  QLineEdit*                    mySelEdit2;
  QPushButton*                  mySelBtn2;
  MeasureGUI_2Sel1LineEdit_QTD* myGrp;

  GEOM::GEOM_Object_var         myObj2;
};

#endif // DIALOGBOX_ANGLE_H
