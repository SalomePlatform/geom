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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : MeasureGUI_PointDlg.h
//  Author : Sergey LITONIN
//  Module : GEOM

#ifndef MEASUREGUI_POINTDLG_H
#define MEASUREGUI_POINTDLG_H

#include "MeasureGUI_Skeleton.h"

class QLineEdit;

#if defined WNT && defined MEASUREGUI_EXPORTS
#define MEASUREGUI_EXPORT __declspec( dllexport )
#else
#define MEASUREGUI_EXPORT
#endif

//=================================================================================
// class    : MeasureGUI_PointDlg
// purpose  : Dialog for displaying point coordinates
//=================================================================================

class MEASUREGUI_EXPORT MeasureGUI_PointDlg : public MeasureGUI_Skeleton
{ 
  Q_OBJECT

public:
                                      MeasureGUI_PointDlg( GeometryGUI* GUI,
							   QWidget*     parent );
                                      ~MeasureGUI_PointDlg();

protected:

  // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
  virtual void                        activateSelection();
  virtual void                        SelectionIntoArgument();

private:

  void                                Init();
private:
  QLineEdit*                          myX;
  QLineEdit*                          myY;
  QLineEdit*                          myZ;

};

#endif // DIALOGBOX_PROPERTIES_H














