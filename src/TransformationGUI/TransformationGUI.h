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
//  File   : TransformationGUI.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#ifndef TRANSFORMATIONGUI_H
#define TRANSFORMATIONGUI_H

#include "GEOMBase.h"
#include <gp_Vec.hxx>

//=================================================================================
// class    : TransformationGUI
// purpose  :
//=================================================================================
class TransformationGUI : public QObject
{
  Q_OBJECT /* for QT compatibility */

public :
  TransformationGUI();
  ~TransformationGUI();

  static bool OnGUIEvent(int theCommandID, QAD_Desktop* parent);

  void MakeTranslationAndDisplay(GEOM::GEOM_Shape_ptr Shape, const gp_Vec V);
  void MakeRotationAndDisplay(GEOM::GEOM_Shape_ptr Shape, const gp_Pnt loc,
			      const gp_Dir dir, const Standard_Real angle);
  void MakeMirrorAndDisplay(GEOM::GEOM_Shape_ptr Shape1, GEOM::GEOM_Shape_ptr Shape2);
  void MakeScaleAndDisplay(GEOM::GEOM_Shape_ptr Shape, const gp_Pnt centralPoint,
			   const Standard_Real factor);
  void MakeMultiTranslation1DAndDisplay(GEOM::GEOM_Shape_ptr Shape,
					const gp_Dir Dir, const double Step, const short NbTimes);
  void MakeMultiTranslation2DAndDisplay(GEOM::GEOM_Shape_ptr Shape,
					const gp_Dir Dir1, const double Step1, const short NbTimes1,
					const gp_Dir Dir2, const double Step2, const short NbTimes2);
  void MakeMultiRotation1DAndDisplay(GEOM::GEOM_Shape_ptr Shape,
				     const gp_Dir Dir, const gp_Pnt Loc, const short NbTimes);
  void MakeMultiRotation2DAndDisplay(GEOM::GEOM_Shape_ptr Shape,
				     const gp_Dir Dir, const gp_Pnt Loc, const double Ang,
				     const short NbTimes1, const double Step, const short NbTimes2);

  GEOMBase* myGeomBase;
  GEOMContext* myGeomGUI;
  GEOM::GEOM_Gen_var myGeom;   /* Current Geom Component */

};

#endif
