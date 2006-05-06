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

/*!
  \class GEOM_AISShape GEOM_AISShape.hxx
  \brief ....
*/

#include "GEOM_AISShape.ixx"
#include "SALOME_InteractiveObject.hxx"

#include "utilities.h"

// Open CASCADE Includes
#include <AIS_Drawer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <StdSelect_DisplayMode.hxx>
#include <StdPrs_WFShape.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

using namespace std;

static void getEntityOwners( const Handle(AIS_InteractiveObject)& theObj,
			     const Handle(AIS_InteractiveContext)& theIC,
			     SelectMgr_IndexedMapOfOwner& theMap )
{
  if ( theObj.IsNull() || theIC.IsNull() )
    return;

  TColStd_ListOfInteger modes;
  theIC->ActivatedModes( theObj, modes );

  TColStd_ListIteratorOfListOfInteger itr( modes );
  for (; itr.More(); itr.Next() ) {
    int m = itr.Value();
    if ( !theObj->HasSelection( m ) )
      continue;

    Handle(SelectMgr_Selection) sel = theObj->Selection( m );

    for ( sel->Init(); sel->More(); sel->Next() ) {
      Handle(SelectBasics_SensitiveEntity) entity = sel->Sensitive();
      if ( entity.IsNull() )
	continue;

      Handle(SelectMgr_EntityOwner) owner =
	Handle(SelectMgr_EntityOwner)::DownCast(entity->OwnerId());
      if ( !owner.IsNull() )
	theMap.Add( owner );
    }
  }
}

static void indicesToOwners( const TColStd_IndexedMapOfInteger& aIndexMap,
 			     const TopoDS_Shape& aMainShape,
			     const SelectMgr_IndexedMapOfOwner& anAllMap, 
			     SelectMgr_IndexedMapOfOwner& aToHiliteMap )
{
  TopTools_IndexedMapOfShape aMapOfShapes;
  TopExp::MapShapes(aMainShape, aMapOfShapes);

  for  ( Standard_Integer i = 1, n = anAllMap.Extent(); i <= n; i++ ) {
    Handle(SelectMgr_EntityOwner) anOwner = anAllMap( i );
    if ( anOwner.IsNull() || !anOwner->HasShape() )
      continue;

    const TopoDS_Shape& aSubShape = anOwner->Shape();
    Standard_Integer aSubShapeId = aMapOfShapes.FindIndex( aSubShape );
    if ( !aSubShapeId || !aIndexMap.Contains( aSubShapeId ) )
      continue;
    
    if ( !aToHiliteMap.Contains( anOwner ) )
      aToHiliteMap.Add( anOwner );
  }
}

GEOM_AISShape::GEOM_AISShape(const TopoDS_Shape& shape,
			     const Standard_CString aName): SALOME_AISShape(shape)
{
  myName = new char [strlen(aName)+1];
  strcpy( myName, aName);

  myShadingColor = Quantity_Color( Quantity_NOC_GOLDENROD );
}

void GEOM_AISShape::setIO(const Handle(SALOME_InteractiveObject)& io){
  SetOwner( io );
}

Handle(SALOME_InteractiveObject) GEOM_AISShape::getIO(){
  Handle(SALOME_InteractiveObject) IO;
  if ( !GetOwner().IsNull() )
    IO = Handle(SALOME_InteractiveObject)::DownCast( GetOwner() );
  return IO;
}

Standard_Boolean GEOM_AISShape::hasIO(){
  return !getIO().IsNull();
}

void GEOM_AISShape::setName(const Standard_CString aName)
{
  myName = new char [strlen(aName)+1];
  strcpy( myName, aName);

  Handle(SALOME_InteractiveObject) IO = getIO();
  if ( !IO.IsNull() )
    IO->setName(aName);
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
      aMatAspect.SetAmbient( 0.5 );
      aMatAspect.SetDiffuse( 0.5 );
      aMatAspect.SetEmissive( 0.5 );
      aMatAspect.SetShininess(0.5 );
      aMatAspect.SetSpecular( 0.5 );
      
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

      // PAL12113: AIS_Shape::Compute() works correctly with shapes containing no faces
      //StdPrs_ShadedShape::Add(aPrs,myshape,myDrawer);
      AIS_Shape::Compute(aPresentationManager, aPrs, aMode);
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

void GEOM_AISShape::highlightSubShapes(const TColStd_IndexedMapOfInteger& aIndexMap, 
				       const Standard_Boolean aHighlight )
{
  Handle(AIS_InteractiveObject) anObj = this;
  Handle(AIS_InteractiveContext) anIC = GetContext();
  if ( anIC.IsNull() || !anIC->HasOpenedContext() ) 
    return;

  Standard_Boolean isAutoHilight = anIC->AutomaticHilight();
  anIC->SetAutomaticHilight( false );

  anIC->ClearSelected( false );

  if ( aHighlight ) {
    SelectMgr_IndexedMapOfOwner anAllMap, aToHiliteMap;

    // Get entity owners for all activated selection modes
    getEntityOwners( anObj, anIC, anAllMap );

    // Convert <aIndexMap> into the map of owners to highlight/unhighlight
    indicesToOwners( aIndexMap, Shape(), anAllMap, aToHiliteMap );


    for ( Standard_Integer i = 1, n = aToHiliteMap.Extent(); i <= n; i++ )
      anIC->AddOrRemoveSelected( aToHiliteMap( i ), false );
  }

  anIC->SetAutomaticHilight( isAutoHilight );
  anIC->HilightSelected( false );
}
