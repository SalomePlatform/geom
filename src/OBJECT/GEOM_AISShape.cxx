//  GEOM OBJECT : interactive object for Geometry entities visualization
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
//  File   : GEOM_AISShape.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

using namespace std;
/*!
  \class GEOM_AISShape GEOM_AISShape.hxx
  \brief ....
*/

#include "GEOM_AISShape.ixx"
#include "SALOME_InteractiveObject.hxx"

#include "utilities.h"

// Open CASCADE Includes
#include <AIS_Drawer.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <StdSelect_DisplayMode.hxx>
#include <StdPrs_WFShape.hxx>
#include <StdPrs_ShadedShape.hxx>

#include <Graphic3d_AspectFillArea3d.hxx>

GEOM_AISShape::GEOM_AISShape(const TopoDS_Shape& shape,
			     const Standard_CString aName): SALOME_AISShape(shape)
{
  myIO = NULL;
  myName = new char [strlen(aName)+1];
  strcpy( myName, aName);

  myShadingColor = Quantity_Color( Quantity_NOC_GOLDENROD );
}

void GEOM_AISShape::setIO(const Handle(SALOME_InteractiveObject)& io){
  myIO = io;
}

Handle(SALOME_InteractiveObject) GEOM_AISShape::getIO(){
  return myIO;
}

Standard_Boolean GEOM_AISShape::hasIO(){
  return !( myIO == NULL ) ;
}

void GEOM_AISShape::setName(const Standard_CString aName)
{
  myName = new char [strlen(aName)+1];
  strcpy( myName, aName);

  if ( hasIO() )
    myIO->setName(aName);
}

Standard_CString GEOM_AISShape::getName(){
  return myName;
}

void GEOM_AISShape::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
			    const Handle(Prs3d_Presentation)& aPrs,
			    const Standard_Integer aMode)
{  
  if (IsInfinite()) aPrs->SetInfiniteState(Standard_True); //pas de prise en compte lors du FITALL
  
  StdSelect_DisplayMode d = (StdSelect_DisplayMode) aMode;

  switch (d) {
  case StdSelect_DM_Wireframe: 
    {
      StdPrs_WFShape::Add(aPrs,myshape,myDrawer);
      break;
    }
  case StdSelect_DM_Shading:
    {
      myDrawer->ShadingAspect()->Aspect()->SetDistinguishOn();
      
      Graphic3d_MaterialAspect aMatAspect;
      aMatAspect.SetAmbient( 1 );
      aMatAspect.SetDiffuse( 0 );
      aMatAspect.SetEmissive( 0 );
      aMatAspect.SetShininess(1 );
      aMatAspect.SetSpecular( 0 );
      
      myDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(aMatAspect);
      myDrawer->ShadingAspect()->Aspect()->SetBackMaterial(Graphic3d_NOM_JADE);
      
      Graphic3d_MaterialAspect FMat = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
      Graphic3d_MaterialAspect BMat = myDrawer->ShadingAspect()->Aspect()->BackMaterial();
      FMat.SetTransparency(myTransparency); BMat.SetTransparency(myTransparency);
      myDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(FMat);
      myDrawer->ShadingAspect()->Aspect()->SetBackMaterial(BMat);

      //Handle(Graphic3d_AspectFillArea3d) a4bis = myDrawer->ShadingAspect()->Aspect();
      //       P->SetPrimitivesAspect(a4bis);
      //        G->SetGroupPrimitivesAspect(a4bis);
      //a4bis->SetInteriorColor(myShadingColor);
      myDrawer->ShadingAspect()->SetColor(myShadingColor);

      StdPrs_ShadedShape::Add(aPrs,myshape,myDrawer);
      break;
    }
  }
  //  aPrs->ReCompute(); // for hidden line recomputation if necessary...
}

void GEOM_AISShape::SetTransparency(const Standard_Real aValue)
{
  if(aValue<0.0 || aValue>1.0) return;
  
  if(aValue<=0.05) 
    {
      UnsetTransparency();
      return;
    }

  Graphic3d_MaterialAspect FMat = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
  Graphic3d_MaterialAspect BMat = myDrawer->ShadingAspect()->Aspect()->BackMaterial();
  FMat.SetTransparency(aValue); BMat.SetTransparency(aValue);
  myDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(FMat);
  myDrawer->ShadingAspect()->Aspect()->SetBackMaterial(BMat);
  myTransparency = aValue;
}

void GEOM_AISShape::SetShadingColor(const Quantity_Color &aCol)
{
  myShadingColor = aCol;
}
