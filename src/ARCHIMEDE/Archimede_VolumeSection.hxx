//  File      : Archimede_VolumeSection.hxx
//  Created   : Fri Feb 22 09:28:13 CET 2002
//  Author    : PULV 

//  Modified  : Fri Feb 22 09:28:13 CET 2002
//  Author    : Nicolas REJNERI
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE 2002
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
