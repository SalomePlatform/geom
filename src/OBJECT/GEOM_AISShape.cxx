// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

/*!
  \class GEOM_AISShape GEOM_AISShape.hxx
  \brief ....
*/

#include "GEOM_AISShape.hxx"
#include "GEOM_AISVector.hxx"

#include <GEOMUtils.hxx>

#include <Basics_OCCTVersion.hxx>

// Open CASCADE Includes
#include <AIS_InteractiveContext.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_DisplayMode.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <V3d_View.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#if OCC_VERSION_LARGE > 0x06070200
#include <Prs3d_VertexDrawMode.hxx>
#endif

#if OCC_VERSION_MAJOR < 7
  #include <StdPrs_WFDeflectionShape.hxx>
#else
  #include <StdPrs_WFShape.hxx>
#endif

#include <SalomeApp_Tools.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

OCCT_IMPLEMENT_STANDARD_RTTIEXT(GEOM_AISShape, SALOME_AISShape)

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
#if OCC_VERSION_LARGE > 0x06080100
      const Handle(SelectMgr_SensitiveEntity) aHSenEntity = sel->Sensitive();
      if( aHSenEntity.IsNull() )
        continue;

      Handle(SelectBasics_SensitiveEntity) entity = aHSenEntity->BaseSensitive();
#else
      Handle(SelectBasics_SensitiveEntity) entity = sel->Sensitive();
#endif
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
  : SALOME_AISShape(shape),
    myName(aName),
    myDisplayVectors(false),
    myDisplayVertices(false),
    myFieldDataType(GEOM::FDT_Double),
    myFieldDimension(0),
    myFieldStepRangeMin(0),
    myFieldStepRangeMax(0)
{
  //rnv: Commented to avoid bug with local selection 
  //SetHilightMode( CustomHighlight ); // override setting the mode to 0 inside AIS_Shape constructor

  myShadingColor = Quantity_Color( Quantity_NOC_GOLDENROD );
  myPrevDisplayMode = 0;

  myEdgesInShadingColor = Quantity_Color( Quantity_NOC_GOLDENROD );

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

GEOM_AISShape::~GEOM_AISShape()
{
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
  // AKL: use old behavior to avoid keeping object's wireframe
  //      if to change shape properties (for example: 'Clear Top Level State','Color', 'Isos') 
  //      calling popup menu over(!) the shape in OCC viewer.
  anIC->SetToHilightSelected( false );

  bool anIsField = !myFieldStepData.isEmpty();
  bool anIsColorField = anIsField && myFieldDataType != GEOM::FDT_String;
  bool anIsTextField = anIsField && myFieldDataType == GEOM::FDT_String;

#if OCC_VERSION_LARGE > 0x06070200
  if (isShowVertices())
    myDrawer->SetVertexDrawMode(Prs3d_VDM_All);
#endif

  //   StdSelect_DisplayMode d = (StdSelect_DisplayMode) aMode;
  bool isTopLev = isTopLevel() && switchTopLevel();
  switch (aMode) {
    case Wireframe:
    case CustomHighlight:
    {
      if(isTopLev) {
              SetColor(topLevelColor());
              Handle(Prs3d_LineAspect) anAspect = Attributes()->WireAspect();
              anAspect->SetColor( topLevelColor() );
              Attributes()->SetWireAspect( anAspect );
      }
      if( !isTopLev && anIsColorField && myFieldDimension == 1 )
        drawField( aPrs, false, aMode == CustomHighlight );
      else
#if OCC_VERSION_MAJOR < 7
        StdPrs_WFDeflectionShape::Add(aPrs,myshape,myDrawer);      
#else
        StdPrs_WFShape::Add(aPrs,myshape,myDrawer);
#endif
      break;
    }
    case Shading:
    {
      shadingMode(aPresentationManager, aPrs, aMode);
      break;
    }
    case ShadingWithEdges:
    {
      myDrawer->SetFaceBoundaryDraw( Standard_True );
      shadingMode(aPresentationManager, aPrs, Shading);
      if( anIsColorField && myFieldDimension == 1 ) {
        myDrawer->SetFaceBoundaryDraw( Standard_False );
        drawField( aPrs );
      }
      break;
    }
    case TexturedShape:
    {
#ifdef USE_TEXTURED_SHAPE
        AIS_TexturedShape::Compute(aPresentationManager, aPrs, aMode);
#else
        AIS_Shape::Compute(aPresentationManager, aPrs, aMode);
#endif
    }
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

  // draw color field on vertices
  if( anIsColorField && myFieldDimension == 0 && aMode != CustomHighlight )
    drawField( aPrs );

  // draw text field
  if( anIsTextField )
    drawField( aPrs, true );

  if( isShowName() )
    drawName( aPrs );

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

void GEOM_AISShape::SetLabelColor(const Quantity_Color &aCol) {
  myLabelColor = aCol;
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

void GEOM_AISShape::SetDisplayVertices(bool isDisplayed)
{
  myDisplayVertices = isDisplayed;
}

void GEOM_AISShape::SetDisplayName(bool isDisplayed)
{
  myDisplayName = isDisplayed;
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

  bool anIsColorField = !myFieldStepData.isEmpty() && myFieldDataType != GEOM::FDT_String;
  if( anIsColorField && ( myFieldDimension == 2 || myFieldDimension == 3 || myFieldDimension == -1 ) )
  {
    drawField( aPrs );
  }
  else
  {
    // PAL12113: AIS_Shape::Compute() works correctly with shapes containing no faces
    //StdPrs_ShadedShape::Add(aPrs,myshape,myDrawer);
#ifdef USE_TEXTURED_SHAPE
    AIS_TexturedShape::Compute(aPresentationManager, aPrs, aMode);
#else
    AIS_Shape::Compute(aPresentationManager, aPrs, aMode);
#endif
  }
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
      case TopWireFrame :        dm = Wireframe;         break;
      case TopShading :          dm = Shading;           break;
      case TopShadingWithEdges : dm = ShadingWithEdges;  break;
      default :                  dm = myPrevDisplayMode; break;
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
  return ( myTopLevel && myTopLevelDm == TopShowAdditionalWActor ) ? false : true;
}

void GEOM_AISShape::setFieldStepInfo( const GEOM::field_data_type theFieldDataType,
                                      const int theFieldDimension,
                                      const QList<QVariant>& theFieldStepData,
                                      const TCollection_AsciiString& theFieldStepName,
                                      const double theFieldStepRangeMin,
                                      const double theFieldStepRangeMax )
{
  myFieldDataType = theFieldDataType;
  myFieldDimension = theFieldDimension;
  myFieldStepData = theFieldStepData;
  myFieldStepName = theFieldStepName;
  myFieldStepRangeMin = theFieldStepRangeMin;
  myFieldStepRangeMax = theFieldStepRangeMax;
}

void GEOM_AISShape::getFieldStepInfo( GEOM::field_data_type& theFieldDataType,
                                      int& theFieldDimension,
                                      QList<QVariant>& theFieldStepData,
                                      TCollection_AsciiString& theFieldStepName,
                                      double& theFieldStepRangeMin,
                                      double& theFieldStepRangeMax ) const
{
  theFieldDataType = myFieldDataType;
  theFieldDimension = myFieldDimension;
  theFieldStepData = myFieldStepData;
  theFieldStepName = myFieldStepName;
  theFieldStepRangeMin = myFieldStepRangeMin;
  theFieldStepRangeMax = myFieldStepRangeMax;
}

void GEOM_AISShape::drawField( const Handle(Prs3d_Presentation)& thePrs,
                               const bool theIsString,
                               const bool theIsHighlight )
{
  if( myFieldStepData.isEmpty() )
    return;

  QListIterator<QVariant> aFieldStepDataIter( myFieldStepData );

  TopAbs_ShapeEnum aShapeType = TopAbs_SHAPE;
  switch( myFieldDimension )
  {
    case 0: aShapeType = TopAbs_VERTEX; break;
    case 1: aShapeType = TopAbs_EDGE; break;
    case 2: aShapeType = TopAbs_FACE; break;
    case 3: aShapeType = TopAbs_SOLID; break;
    case -1: aShapeType = TopAbs_VERTEX; break;
  }

  TopTools_IndexedMapOfShape aShapeMap;
  TopExp::MapShapes( myshape, aShapeMap );

  TColStd_IndexedMapOfInteger anIndexMap;

  TopExp_Explorer anExp;
  for( anExp.Init( myshape, aShapeType ); anExp.More(); anExp.Next() )
  {
    TopoDS_Shape aSubShape = anExp.Current();
    if( !aSubShape.IsNull() )
    {
      Standard_Integer aSubShapeIndex = aShapeMap.FindIndex( aSubShape );
      if( anIndexMap.Contains( aSubShapeIndex ) )
        continue;

      anIndexMap.Add( aSubShapeIndex );

      Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup( thePrs );

      QColor aQColor;
      QString aString;
      if( aFieldStepDataIter.hasNext() )
      {
        const QVariant& aVariant = aFieldStepDataIter.next();
        if( theIsString )
          aString = aVariant.toString();
        else
          aQColor = aVariant.value<QColor>();
      }
      else
        break;

      if( theIsString )
      {
        gp_Pnt aCenter;
        if( computeMassCenter( aSubShape, aCenter ) )
        {
          Graphic3d_Vertex aVertex( aCenter.X(), aCenter.Y(), aCenter.Z() );

          Handle(Graphic3d_AspectText3d) anAspectText3d = new Graphic3d_AspectText3d();
          anAspectText3d->SetStyle( Aspect_TOST_ANNOTATION );
          anAspectText3d->SetColor( myLabelColor );
          aGroup->SetPrimitivesAspect( anAspectText3d );

          aGroup->Text( aString.toLatin1().constData(), aVertex, 14 );
        }
      }
      else
      {
        Quantity_Color aColor( aQColor.redF(), aQColor.greenF(), aQColor.blueF(), Quantity_TOC_RGB );
        if( myFieldDimension == 0 )
        {
          TopoDS_Vertex aVertexShape = TopoDS::Vertex( aSubShape );
          if( !aVertexShape.IsNull() )
          {
            gp_Pnt aPnt = BRep_Tool::Pnt( aVertexShape );

            Handle(Graphic3d_AspectMarker3d) anAspectMarker3d = new Graphic3d_AspectMarker3d();
            anAspectMarker3d->SetColor( aColor );
            anAspectMarker3d->SetType( Aspect_TOM_POINT );
            anAspectMarker3d->SetScale( 10.0 );
            aGroup->SetPrimitivesAspect( anAspectMarker3d );

            Handle(Graphic3d_ArrayOfPoints) anArray = new Graphic3d_ArrayOfPoints( 1 );
            anArray->AddVertex( aPnt.X(), aPnt.Y(), aPnt.Z() );

            aGroup->AddPrimitiveArray( anArray );
          }
        }
        else if( myFieldDimension == 1 )
        {
          myDrawer->WireAspect()->SetColor( aColor );
          if( theIsHighlight )
            myDrawer->WireAspect()->SetWidth( myOwnWidth );
          else
            myDrawer->WireAspect()->SetWidth( myOwnWidth + 4 );
#if OCC_VERSION_MAJOR < 7
          StdPrs_WFDeflectionShape::Add( thePrs, aSubShape, myDrawer );
#else
          StdPrs_WFShape::Add( thePrs, aSubShape, myDrawer );
#endif
        }
        else if( myFieldDimension == 2 ||
                 myFieldDimension == 3 ||
                 myFieldDimension == -1 )
        {
          myDrawer->ShadingAspect()->SetColor( aColor );
          StdPrs_ShadedShape::Add( thePrs, aSubShape, myDrawer );
        }
      }
    }
  }
}

void GEOM_AISShape::drawName( const Handle(Prs3d_Presentation)& thePrs )
{
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup( thePrs );

  gp_Ax3 anAx3 = GEOMUtils::GetPosition(myshape);
  gp_Pnt aCenter = anAx3.Location();

  Graphic3d_Vertex aVertex( aCenter.X(), aCenter.Y(), aCenter.Z() );

  Handle(Graphic3d_AspectText3d) anAspectText3d = new Graphic3d_AspectText3d();
  anAspectText3d->SetStyle( Aspect_TOST_ANNOTATION );
  anAspectText3d->SetColor( myLabelColor );
  aGroup->SetPrimitivesAspect( anAspectText3d );

  const char* aName = getIO()->getName();
  aGroup->Text( TCollection_ExtendedString( aName ), aVertex, 16 );
}

Standard_Boolean GEOM_AISShape::computeMassCenter( const TopoDS_Shape& theShape,
                                                   gp_Pnt&             theCenter )
{
  theCenter.SetCoord( 0,0,0 );
  Standard_Integer aNbPoints = 0;

  if ( theShape.ShapeType() == TopAbs_EDGE )
  {
    double f,l;
    Handle(Geom_Curve) curve = BRep_Tool::Curve( TopoDS::Edge( theShape ), f, l );
    if ( !curve.IsNull() )
    {
      theCenter = curve->Value( 0.5 * ( f + l ));
      aNbPoints = 1;
    }
  }
  else if ( theShape.ShapeType() == TopAbs_FACE )
  {
    BRepAdaptor_Surface surface( TopoDS::Face( theShape ));
    theCenter = surface.Value( 0.5 * ( surface.FirstUParameter() + surface.LastUParameter() ),
                               0.5 * ( surface.FirstVParameter() + surface.LastVParameter() ));
    aNbPoints = 1;
  }

  if ( aNbPoints == 0 )
  {
    TopExp_Explorer anExp;
    for( anExp.Init( theShape, TopAbs_VERTEX ); anExp.More(); anExp.Next() )
    {
      TopoDS_Vertex aVertex = TopoDS::Vertex( anExp.Current() );
      if( !aVertex.IsNull() )
      {
        gp_Pnt aPnt = BRep_Tool::Pnt( aVertex );
        theCenter.ChangeCoord() += aPnt.XYZ();
        aNbPoints++;
      }
    }
  }

  if ( aNbPoints > 0 )
    theCenter.ChangeCoord() /= (Standard_Real) aNbPoints;

  return aNbPoints;
}
