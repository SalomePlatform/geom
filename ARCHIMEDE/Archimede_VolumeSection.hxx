//  GEOM ARCHIMEDE : algorithm implementation
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
//  File   : Archimede_VolumeSection.hxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#ifndef ARCHIMEDE_VOLUMESECTION_HXX
#define ARCHIMEDE_VOLUMESECTION_HXX

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <TopoDS_Shape.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

class VolumeSection{

public:
  // Constructeur effectuant le maillage de peau de la shape
  VolumeSection(TopoDS_Shape , Standard_Real); 

  //Methode qui affecte à un point,les coordonnées de centre de la boite englobante de la shape 
  void CenterOfGravity();

  // Methode qui calcule le volume sous un plan Z = h
  Standard_Real CalculateVolume(Standard_Real); 

  // Methode qui resout l'equation V(h)=constante
  Standard_Real Archimede(Standard_Real , Standard_Real); 

  // Methode permettant de "setter" un plan afin de l'utiliser à l'interieur de la classe
  void SetPlane(Handle (Geom_Plane));

  // Methode permettant de récupérer la shape modifiée à l'extérieur de la classe
  TopoDS_Shape GetShape();

  // Methode effectuant la rotation du plan et de la shape
  void MakeRotation(gp_Dir);

  // Methode effectuant la rotation inverse du plan et de la shape
  Handle (Geom_RectangularTrimmedSurface) InvMakeRotation(gp_Dir,Handle(Geom_RectangularTrimmedSurface));

  // Methode permettant de découper le plan selon une projection de la Shape
  Handle (Geom_RectangularTrimmedSurface) TrimSurf();

  // Methode permmettant de deplacer le plan jusqu'a la position donnée par Archimède
  Handle (Geom_RectangularTrimmedSurface) AjustePlan(Handle(Geom_RectangularTrimmedSurface),Standard_Real,gp_Pnt);

  void getZ( double& min, double& max);
  
private:
  
  TopoDS_Shape  myShape;
  Standard_Real Tolerance;
  gp_Pnt        InitPoint;
  Standard_Real Zmin,Zmax,Ymin,Ymax,Xmin,Xmax;
  Handle(Geom_Plane) myPlane;
  
  Standard_Real ElementaryVolume(gp_Pnt,gp_Pnt,gp_Pnt);
  gp_Pnt        Intersection(gp_Pnt,gp_Pnt,Standard_Real);
  
};
#endif
