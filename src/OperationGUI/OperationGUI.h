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
//  File   : OperationGUI.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#ifndef OPERATIONGUI_H
#define OPERATIONGUI_H

#include "GEOMBase.h"

//=================================================================================
// class    : OperationGUI
// purpose  :
//=================================================================================
class OperationGUI : public QObject
{
  Q_OBJECT /* for QT compatibility */

public :
  OperationGUI();
  ~OperationGUI();

  static bool OnGUIEvent(int theCommandID, QAD_Desktop* parent);

  void MakePartitionAndDisplay(const GEOM::GEOM_Gen::ListOfIOR& listShapesIOR,
			       const GEOM::GEOM_Gen::ListOfIOR& listToolsIOR,
			       const GEOM::GEOM_Gen::ListOfIOR& listKeepInsIOR,
			       const GEOM::GEOM_Gen::ListOfIOR& listRemoveInsIOR,
			       const GEOM::shape_type limit ) ;
  void Archimede(const Handle(SALOME_InteractiveObject)& IO, const double aWeight,
		 const double aWaterDensity, const double aMeshingDeflection);

  /* Method for Fillet */
  bool OnFilletGetAll(const TopoDS_Shape& ShapeTopo, const double Radius,
		      const int SubShapeType, const char* ShapeTopoIOR);
  bool OnFilletGetSelected(const TopoDS_Shape& ShapeTopo, const char* ShapeTopoIOR,
			   const double Radius, const int SubShapeType,
			   Standard_Integer& aLocalContextId, bool& myUseLocalContext);

  /* Methods for Chamfer */
  bool OnChamferGetAll(const TopoDS_Shape& ShapeTopo, const double D1, const double D2, 
		       const int SubShapeType, const char* ShapeTopoIOR);
  bool OnChamferGetSelected(const TopoDS_Shape& ShapeTopo,
			    const char* ShapeTopoIOR,
			    const double D1, const double D2, const int SubShapeType,
			    Standard_Integer& aLocalContextId,
			    bool& myUseLocalContext);

  GEOMBase* myGeomBase;
  GEOMContext* myGeomGUI;
  GEOM::GEOM_Gen_var myGeom;   /* Current Geom Component */

};

#endif
