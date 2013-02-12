// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  GEOM OBJECT : interactive object for Geometry entities visualization
//  File   : GEOM_AISShape.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM

/*!
  \class GEOM_AISShape GEOM_AISShape.hxx
  \brief ....
*/

#include "GEOM_AISShape.ixx"
#include "SALOME_InteractiveObject.hxx"
#include "GEOM_AISVector.hxx"

// Open CASCADE Includes
#include <AIS_Drawer.hxx>
#include <AIS_InteractiveContext.hxx>

#include <BRep_Tool.hxx>

#include <GCPnts_AbscissaPoint.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>

#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_IsoAspect.hxx>

#include <SelectBasics_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <StdSelect_DisplayMode.hxx>
#include <StdPrs_WFDeflectionShape.hxx>

#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <TColStd_SequenceOfInteger.hxx>

#include <V3d_View.hxx>

#include <SalomeApp_Tools.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

GEOM_AISShape::TopLevelDispMode GEOM_AISShape::myTopLevelDm = GEOM_AISShape::TopKeepCurrent;
Quantity_Color GEOM_AISShape::myTopLevelColor;


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
    Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast(anAllMap( i ));
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
                             const Standard_CString aName)
  : SALOME_AISShape(shape), myName(aName), myDisplayVectors(false)
{
  myShadingColor = Quantity_Color( Quantity_NOC_GOLDENROD );
  myPrevDisplayMode = 0;
  storeBoundaryColors();


  myEdgesInShadingColor = Quantity_Color( Quantity_NOC_GOLDENROD );

  myUIsoNumber = -1;
  myVIsoNumber = -1;

  myTopLevel = Standard_False;
  Graphic3d_MaterialAspect aMatAspect;
  if ( !HasMaterial() ) {
  	aMatAspect.SetAmbient( 0.5 );
  	aMatAspect.SetDiffuse( 0.5 );
  	aMatAspect.SetEmissive( 0.5 );
  	aMatAspect.SetShininess(0.5 );
  	aMatAspect.SetSpecular( 0.5 );
	
  	myDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(aMatAspect);
  	myDrawer->ShadingAspect()->Aspect()->SetBackMaterial(aMatAspect);
  }
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
  myName = aName;

  Handle(SALOME_InteractiveObject) IO = getIO();
  if ( !IO.IsNull() )
    IO->setName(aName);
}

Standard_CString GEOM_AISShape::getName(){
  return myName.ToCString();
}

void GEOM_AISShape::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                            const Handle(Prs3d_Presentation)& aPrs,
                            const Standard_Integer aMode)
{  
  if (IsInfinite()) aPrs->SetInfiniteState(Standard_True); //pas de prise en compte lors du FITALL

  Handle(AIS_InteractiveContext) anIC = GetContext();
 
  //   StdSelect_DisplayMode d = (StdSelect_DisplayMode) aMode;
  bool isTopLev = isTopLevel() && switchTopLevel();
  switch (aMode) {
    case 0://StdSelect_DM_Wireframe: 
    {
      restoreIsoNumbers();
      // Restore wireframe edges colors
      restoreBoundaryColors();
      if(isTopLev) {
	      SetColor(topLevelColor());
	      Handle(Prs3d_LineAspect) anAspect = Attributes()->WireAspect();
	      anAspect->SetColor( topLevelColor() );
	      Attributes()->SetWireAspect( anAspect );
      }
     StdPrs_WFDeflectionShape::Add(aPrs,myshape,myDrawer);      
      break;
    }
    case 1://StdSelect_DM_Shading:
    {
      restoreIsoNumbers();
      shadingMode(aPresentationManager, aPrs, aMode);
      // Store wireframe edges colors
      storeBoundaryColors();
      break;
    }
    case 3: //StdSelect_DM_HLR:
    {
      if(!isTopLev)
	      AIS_TexturedShape::Compute(aPresentationManager, aPrs, aMode);
      else 
	      shadingMode(aPresentationManager, aPrs, AIS_Shaded);
      break;
    }
  }

  if ( aMode == ShadingWithEdges ) {
    // Temporary store number of iso lines in order to recover its later 
    // when display mode is achnged to 'Wirefame' or 'Shading'.
    // Iso lines are not displayed in 'Shading with edges' mode.
    storeIsoNumbers();

    // Reset number of iso lines to 0
    resetIsoNumbers();

    //Shaded faces
    shadingMode(aPresentationManager, aPrs, AIS_Shaded);

    // Store wireframe edges colors
    storeBoundaryColors();

    // Coloring edges
    Handle(Prs3d_LineAspect) anAspect = myDrawer->UnFreeBoundaryAspect();
    anAspect->SetColor( myEdgesInShadingColor );
    myDrawer->SetUnFreeBoundaryAspect( anAspect );
    
    anAspect = myDrawer->FreeBoundaryAspect();
    anAspect->SetColor( myEdgesInShadingColor );
    myDrawer->SetFreeBoundaryAspect( anAspect );

    // Add edges to presentation
    StdPrs_WFDeflectionShape::Add(aPrs,myshape,myDrawer);
  }

  if (isShowVectors())
  {
    const bool isVector = IsKind(STANDARD_TYPE(GEOM_AISVector));
    TopExp_Explorer Exp ( myshape, TopAbs_EDGE );
    for ( ; Exp.More(); Exp.Next() ) {
      TopoDS_Vertex aV1, aV2;
      TopoDS_Edge anEdgeE = TopoDS::Edge(Exp.Current());
      if ( !isVector )
        // draw curve direction (issue 0021087)
        anEdgeE.Orientation( TopAbs_FORWARD );

      if ( anEdgeE.IsNull() ) continue;

      TopExp::Vertices(anEdgeE, aV1, aV2);
      gp_Pnt aP1 = BRep_Tool::Pnt(aV1);
      gp_Pnt aP2 = BRep_Tool::Pnt(aV2);

      double fp,lp;
      gp_Vec aDirVec;
      Handle(Geom_Curve) C = BRep_Tool::Curve(anEdgeE,fp,lp);

      if ( C.IsNull() ) continue;

      if ( anEdgeE.Orientation() == TopAbs_FORWARD )
        C->D1(lp, aP2, aDirVec);
      else {
        C->D1(fp, aP1, aDirVec);
        aP2 = aP1;
      }

      GeomAdaptor_Curve aAdC;
      aAdC.Load(C, fp, lp);
      Standard_Real aDist = GCPnts_AbscissaPoint::Length(aAdC, fp, lp); 
     
      if (aDist > gp::Resolution()) {
        gp_Dir aDir;
        if ( anEdgeE.Orientation() == TopAbs_FORWARD )
          aDir = aDirVec;
        else
          aDir = -aDirVec;

        Prs3d_Arrow::Draw(aPrs, aP2, aDir, M_PI/180.*5., aDist/10.);
      }
    }
  }
  //  aPrs->ReCompute(); // for hidden line recomputation if necessary...
}

void GEOM_AISShape::SetShadingColor(const Quantity_Color &aCol)
{
  myShadingColor = aCol;
}

void GEOM_AISShape::SetEdgesInShadingColor(const Quantity_Color &aCol)
{
  myEdgesInShadingColor = aCol;
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

void GEOM_AISShape::SetDisplayVectors(bool isDisplayed)
{
  myDisplayVectors = isDisplayed;
}

void GEOM_AISShape::shadingMode(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
				const Handle(Prs3d_Presentation)& aPrs,
				const Standard_Integer aMode)
{
  myDrawer->ShadingAspect()->Aspect()->SetDistinguishOn();

  Graphic3d_MaterialAspect aMatAspect(Graphic3d_NOM_PLASTIC);
  aMatAspect.SetTransparency(Transparency());
  Graphic3d_MaterialAspect currentFrontMaterial = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
  Graphic3d_MaterialAspect currentBackMaterial  = myDrawer->ShadingAspect()->Aspect()->BackMaterial();
  myDrawer->ShadingAspect()->Aspect()->SetFrontMaterial( isTopLevel() ? aMatAspect : currentFrontMaterial );
  myDrawer->ShadingAspect()->Aspect()->SetBackMaterial ( isTopLevel() ? aMatAspect : currentBackMaterial  );

  if( isTopLevel() && switchTopLevel() )
    myDrawer->ShadingAspect()->SetColor( topLevelColor() );
  else { 
    if(myDrawer->ShadingAspect()->Aspect()->FrontMaterial().MaterialType( Graphic3d_MATERIAL_ASPECT ))
      myDrawer->ShadingAspect()->SetColor(myShadingColor);
    else
      myDrawer->ShadingAspect()->SetColor(myDrawer->ShadingAspect()->Aspect()->FrontMaterial().AmbientColor());
  }

  // PAL12113: AIS_Shape::Compute() works correctly with shapes containing no faces
  //StdPrs_ShadedShape::Add(aPrs,myshape,myDrawer);
  AIS_Shape::Compute(aPresentationManager, aPrs, aMode);
}

void GEOM_AISShape::storeIsoNumbers()
{
  myUIsoNumber = myDrawer->UIsoAspect()->Number();
  myVIsoNumber = myDrawer->VIsoAspect()->Number();
}

void GEOM_AISShape::restoreIsoNumbers()
{
  if ( myUIsoNumber > 0 ) {
    // Restore number of U iso lines
    Handle(Prs3d_IsoAspect) anAspect = myDrawer->UIsoAspect();
    anAspect->SetNumber( myUIsoNumber );
    myDrawer->SetUIsoAspect( anAspect );
  }
  
  if ( myVIsoNumber > 0 ) {
    // Restore number of V iso lines
    Handle(Prs3d_IsoAspect) anAspect = myDrawer->VIsoAspect();
    anAspect->SetNumber( myVIsoNumber );
    myDrawer->SetVIsoAspect( anAspect );
  }
}

void GEOM_AISShape::resetIsoNumbers()
{
  Handle(Prs3d_IsoAspect) anAspect = myDrawer->UIsoAspect();
  anAspect->SetNumber( 0 );
  myDrawer->SetUIsoAspect( anAspect );
  
  anAspect = myDrawer->VIsoAspect();
  anAspect->SetNumber( 0 );
  myDrawer->SetVIsoAspect( anAspect );
}

void GEOM_AISShape::storeBoundaryColors()
{
  Aspect_TypeOfLine aLT;
  Standard_Real aW;

  myDrawer->FreeBoundaryAspect()->Aspect()->Values( myFreeBoundaryColor, aLT, aW);
  myDrawer->UnFreeBoundaryAspect()->Aspect()->Values( myUnFreeBoundaryColor, aLT, aW);
}
 
void GEOM_AISShape::restoreBoundaryColors()
{
  Handle(Prs3d_LineAspect) anAspect = myDrawer->FreeBoundaryAspect();
  anAspect->SetColor( myFreeBoundaryColor );
  myDrawer->SetFreeBoundaryAspect( anAspect );

  anAspect = myDrawer->UnFreeBoundaryAspect();
  anAspect->SetColor( myUnFreeBoundaryColor );
  myDrawer->SetUnFreeBoundaryAspect( anAspect );
}


Standard_Boolean GEOM_AISShape::isTopLevel() {
  return myTopLevel;
}

void GEOM_AISShape::setTopLevel(Standard_Boolean f) {
  if(f) {
    if(f != myTopLevel)
      myPrevDisplayMode = DisplayMode();
    Standard_Integer dm;
    switch(topLevelDisplayMode()) {
      case TopKeepCurrent : dm = myPrevDisplayMode; break;
      case TopWireFrame : dm = AIS_WireFrame; break;     
      case TopShadingWithEdges : dm = ShadingWithEdges; break;
      default : dm = AIS_Shaded; break;
    }
    SetDisplayMode(dm);
  } else {
    if(f != myTopLevel)
      SetDisplayMode(myPrevDisplayMode);
  }
  myTopLevel = f;
}

void GEOM_AISShape::setPrevDisplayMode(const Standard_Integer mode) {
  myPrevDisplayMode = mode;
}

Quantity_Color GEOM_AISShape::topLevelColor() {
  return myTopLevelColor;
}

void GEOM_AISShape::setTopLevelColor(const Quantity_Color c) {
  myTopLevelColor = c;
}

GEOM_AISShape::TopLevelDispMode GEOM_AISShape::topLevelDisplayMode() {
  return myTopLevelDm;
}
void GEOM_AISShape::setTopLevelDisplayMode(const GEOM_AISShape::TopLevelDispMode dm) {
  myTopLevelDm = dm;
}

Standard_Boolean GEOM_AISShape::switchTopLevel() {
	return myTopLevelDm != TopShowAdditionalWActor;
}

Standard_Boolean GEOM_AISShape::toActivate() {
	return Standard_True;
}
