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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : BasicGUI.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#ifndef BASICGUI_H
#define BASICGUI_H

#include "GEOMBase.h"
#include <V3d_View.hxx>

//=================================================================================
// class    : BasicGUI
// purpose  :
//=================================================================================
class BasicGUI : public QObject
{
  Q_OBJECT /* for QT compatibility */

public :
  BasicGUI();
  ~BasicGUI();

  static bool OnGUIEvent(int theCommandID, QAD_Desktop* parent);
  static bool OnMousePress(QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame);

  gp_Pnt ConvertClickToPoint(Standard_Real x, Standard_Real y, Handle(V3d_View) aView);

  void MakePointAndDisplay(const double x, const double y, const double z);
  void MakeLineAndDisplay(const gp_Pnt InitPoint, const gp_Pnt LastPoint);
  void MakeCircleAndDisplay(const gp_Pnt CenterPoint, const gp_Dir dir, const Standard_Real Radius);
  void MakeEllipseAndDisplay(const gp_Pnt CenterPoint, const gp_Dir dir,
			     const Standard_Real Major_Radius, const Standard_Real Minor_Radius);
  void MakeArcAndDisplay(gp_Pnt InitPoint, gp_Pnt CirclePoint, gp_Pnt EndPoint);
  void MakeVectorAndDisplay(const gp_Pnt P1, const gp_Pnt P2);
  void MakePlaneAndDisplay(const gp_Pnt P1, const Standard_Real dx, const Standard_Real dy,
			   const Standard_Real dz, const Standard_Real TrimSize);
  void MakeWorkingPlane(const gp_Pnt P, const gp_Dir D);

  GEOMBase* myGeomBase;
  GEOMContext* myGeomGUI;
  GEOM::GEOM_Gen_var myGeom;   /* Current Geom Component */

};

#endif
