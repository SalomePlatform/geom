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
//  File   : RepairGUI.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#ifndef REPAIRGUI_H
#define REPAIRGUI_H

#include "GEOMBase.h"

//=================================================================================
// class    : RepairGUI
// purpose  :
//=================================================================================
class RepairGUI : public QObject
{
  Q_OBJECT /* for QT compatibility */

public :
  RepairGUI();
  ~RepairGUI();

  static bool OnGUIEvent(int theCommandID, QAD_Desktop* parent);

  void MakeSewingAndDisplay(GEOM::GEOM_Gen::ListOfIOR& listShapesIOR, 
			    const Standard_Real precision);
  void MakeOrientationChangeAndDisplay(GEOM::GEOM_Shape_ptr Shape);
  bool OnSuppressHole(const char* ShapeTopoIOR,
		      const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdFace,
		      const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdWire,
		      const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdEndFace);
  bool OnSuppressHolesInFaceOrShell(const char* ShapeTopoIOR,
				    const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdWires);
  bool OnSuppressFaces(const TopoDS_Shape& ShapeTopo,
		       const char* ShapeTopoIOR,
		       const Standard_Integer& aLocalContextId,
		       bool& myUseLocalContext);

  GEOMBase* myGeomBase;
  GEOMContext* myGeomGUI;
  GEOM::GEOM_Gen_var myGeom;   /* Current Geom Component */

};

#endif
