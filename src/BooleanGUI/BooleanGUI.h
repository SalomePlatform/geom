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
//  File   : BooleanGUI.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#ifndef BOOLEANGUI_H
#define BOOLEANGUI_H

#include "GEOMBase.h"

//=================================================================================
// class    : BooleanGUI
// purpose  :
//=================================================================================
class BooleanGUI : public QObject
{
  Q_OBJECT /* for QT compatibility */

public :
  BooleanGUI();
  ~BooleanGUI();

  static bool OnGUIEvent(int theCommandID, QAD_Desktop* parent);

  void MakeBooleanAndDisplay(GEOM::GEOM_Shape_ptr Shape1,
			     GEOM::GEOM_Shape_ptr Shape2,
			     const short operation);

  GEOMBase* myGeomBase;
  GEOMContext* myGeomGUI;
  GEOM::GEOM_Gen_var myGeom;   /* Current Geom Component */

};

#endif
