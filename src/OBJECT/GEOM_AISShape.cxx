using namespace std;
//  File      : GEOM_AISShape.cxx
//  Created   : Wed Feb 20 17:24:59 2002
//  Author    : Nicolas REJNERI
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE 2002
//  $Header$

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
      myDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(Graphic3d_NOM_BRASS);
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
