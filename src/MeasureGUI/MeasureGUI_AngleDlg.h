//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//
//
//  File   : MeasureGUI_AngleDlg.h
//  Author : Julia DOROVSKIKH
//  Module : GEOM

#ifndef DIALOGBOX_ANGLE_H
#define DIALOGBOX_ANGLE_H

#include "MeasureGUI_Skeleton.h"

class MeasureGUI_2Sel1LineEdit_QTD;

#if defined WNT && defined MEASUREGUI_EXPORTS
#define MEASUREGUI_EXPORT __declspec( dllexport )
#else
#define MEASUREGUI_EXPORT
#endif

//=================================================================================
// class    : MeasureGUI_AngleDlg
// purpose  :
//=================================================================================
class MEASUREGUI_EXPORT MeasureGUI_AngleDlg : public MeasureGUI_Skeleton
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
