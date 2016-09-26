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

//  GEOM OBJECT : interactive object for Geometry entities visualization
//  File   : GEOM_Annotation.hxx
//  Module : GEOM
//
#include <GEOM_Annotation.hxx>

// OCCT includes
#include <AIS_InteractiveContext.hxx>
#include <Font_FTFont.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Camera.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Graphic3d_Vec4.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Text.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT( GEOM_Annotation, AIS_InteractiveObject )

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
GEOM_Annotation::GEOM_Annotation() : AIS_InteractiveObject()
{
  SetPosition( gp_Pnt( 0.0, 0.0, 0.0 ) );
  SetIsScreenFixed( Standard_False );
  SetAttachPoint( gp_Pnt( 0.0, 0.0, 0.0 ) );
  SetDisplayMode( 0 );
  SetZLayer( Graphic3d_ZLayerId_Default );
  SetAutoHide( Standard_True );
  SetHilightMode( HighlightAll );
  SetMutable( Standard_True );
  SetDepthCulling( Standard_True );

  Handle(Prs3d_TextAspect) aTextAspect = new Prs3d_TextAspect();
  aTextAspect->SetHeight( 20.0 );
  aTextAspect->SetColor( Quantity_Color( 1.0, 1.0, 1.0, Quantity_TOC_RGB ) );
  myDrawer->SetTextAspect( aTextAspect );

  Handle(Prs3d_LineAspect) aLineAspect =
    new Prs3d_LineAspect( Quantity_NOC_WHITE, Aspect_TOL_SOLID, 1.0 );
  myDrawer->SetLineAspect( aLineAspect );

  Handle(Prs3d_LineAspect) aHiddenLineAspect =
    new Prs3d_LineAspect( Quantity_NOC_WHITE, Aspect_TOL_DOT, 1.0 );
  myDrawer->SetHiddenLineAspect( aHiddenLineAspect );

  Handle(Prs3d_PointAspect) aPointAspect =
    new Prs3d_PointAspect( Aspect_TOM_POINT, Quantity_NOC_WHITE, 4.0 );
  myDrawer->SetPointAspect( aPointAspect );
}

// =======================================================================
// function : SetText
// purpose  :
// =======================================================================
void GEOM_Annotation::SetText( const TCollection_ExtendedString& theText )
{
  if (myText != theText)
  {
    SetToUpdate();
  }

  myText = theText;
}

// =======================================================================
// function : SetPosition
// purpose  :
// =======================================================================
void GEOM_Annotation::SetPosition( const gp_Pnt& thePosition )
{
  SetPosition( thePosition, Standard_True );
}

// =======================================================================
// function : SetPosition
// purpose  :
// =======================================================================
void GEOM_Annotation::SetPosition( const gp_Pnt& thePosition, const Standard_Boolean theUpdateSelection )
{
  myPosition = thePosition;

  if ( !myIsScreenFixed )
  {
    Handle(Graphic3d_TransformPers) aPersistence =
      new Graphic3d_TransformPers( Graphic3d_TMF_ZoomRotatePers, thePosition );

    AIS_InteractiveObject::SetTransformPersistence( aPersistence );
  }

  SetToUpdate();

  if( theUpdateSelection )
  {
    UpdateSelection();
  }
}

// =======================================================================
// function : SetIsScreenFixed
// purpose  :
// =======================================================================
void GEOM_Annotation::SetIsScreenFixed( const Standard_Boolean theIsFixed )
{
  myIsScreenFixed = theIsFixed;

  Handle(Graphic3d_TransformPers) aPersistence;

  if (!myIsScreenFixed)
  {
    aPersistence = new Graphic3d_TransformPers( Graphic3d_TMF_ZoomRotatePers, myPosition );
  }
  else
  {
    aPersistence = new Graphic3d_TransformPers( Graphic3d_TMF_2d, Aspect_TOTP_CENTER );
  }

  AIS_InteractiveObject::SetTransformPersistence( aPersistence );

  SetZLayer( myIsScreenFixed ? Graphic3d_ZLayerId_Topmost : Graphic3d_ZLayerId_Default );

  SetToUpdate();

  UpdateSelection();
}

// =======================================================================
// function : Set2dPosition
// purpose  :
// =======================================================================
void GEOM_Annotation::Set2dPosition( const Handle(V3d_View)& theView )
{
  if ( myIsScreenFixed )
  {
    return;
  }

  gp_Pnt aPosition2d = ConvertPosition2d( myPosition, myAttach, theView );

  SetIsScreenFixed( Standard_True );

  SetPosition( aPosition2d );
}

// =======================================================================
// function : Set3dPosition
// purpose  :
// =======================================================================
void GEOM_Annotation::Set3dPosition( const Handle(V3d_View)& theView )
{
  if ( !myIsScreenFixed )
  {
    return;
  }

  gp_Pnt aPosition3d = ConvertPosition3d( myPosition, myAttach, theView );

  SetIsScreenFixed( Standard_False );

  SetPosition( aPosition3d );
}

// =======================================================================
// function : SetAttachPoint
// purpose  :
// =======================================================================
void GEOM_Annotation::SetAttachPoint( const gp_Pnt& thePoint )
{
  myAttach = thePoint;
}

// =======================================================================
// function : SetHilightShape
// purpose  : Sets shape (annotated shape) that will be used for hilighting.
// =======================================================================
void GEOM_Annotation::SetHilightShape( const TopoDS_Shape& theShape )
{
  if ( myShape.IsEqual( theShape ) )
  {
    return;
  }

  myShape = theShape;
  SetToUpdate();
  UpdateSelection();
}

// =======================================================================
// function : SetColor
// purpose  :
// =======================================================================
void GEOM_Annotation::SetColor( const Quantity_Color& theColor )
{
  SetTextColor( theColor );
  SetLineColor( theColor );
}

// =======================================================================
// function : SetTextColor
// purpose  :
// =======================================================================
void GEOM_Annotation::SetTextColor( const Quantity_Color& theColor )
{
  myDrawer->TextAspect()->SetColor( theColor );

  SetToUpdate();
}

// =======================================================================
// function : SetLineColor
// purpose  :
// =======================================================================
void GEOM_Annotation::SetLineColor( const Quantity_Color& theColor )
{
  myDrawer->LineAspect()->SetColor( theColor );
  myDrawer->HiddenLineAspect()->SetColor( theColor );
  myDrawer->PointAspect()->SetColor( theColor );

  SetToUpdate();
}

// =======================================================================
// function : SetLineWidth
// purpose  :
// =======================================================================
void GEOM_Annotation::SetLineWidth( const Standard_Real theLineWidth )
{
  if ( GetLineWidth() != theLineWidth )
  {
    myDrawer->LineAspect()->SetWidth( theLineWidth );
    myDrawer->HiddenLineAspect()->SetWidth( theLineWidth );

    SetToUpdate();
  }
}

// =======================================================================
// function : SetLineStyle
// purpose  :
// =======================================================================
void GEOM_Annotation::SetLineStyle( const Aspect_TypeOfLine theStyle )
{
  if ( GetLineStyle() != theStyle )
  {
    myDrawer->LineAspect()->SetTypeOfLine( theStyle );

    SetToUpdate();
  }
}

// =======================================================================
// function : SetHiddenLineStyle
// purpose  :
// =======================================================================
void GEOM_Annotation::SetHiddenLineStyle( const Aspect_TypeOfLine theStyle )
{
  if ( GetHiddenLineStyle() != theStyle )
  {
    myDrawer->HiddenLineAspect()->SetTypeOfLine( theStyle );

    SetToUpdate();
  }
}

// =======================================================================
// function : SetTextHeight
// purpose  :
// =======================================================================
void GEOM_Annotation::SetTextHeight( const Standard_Real theHeight )
{
  if ( GetTextHeight() != theHeight )
  {
    myDrawer->TextAspect()->SetHeight( theHeight );

    SetToUpdate();
  }
}

// =======================================================================
// function : SetFontAspect
// purpose  :
// =======================================================================
void GEOM_Annotation::SetFontAspect( const Font_FontAspect theFontAspect )
{
  if ( GetFontAspect() != theFontAspect )
  {
    myDrawer->TextAspect()->Aspect()->SetTextFontAspect( theFontAspect );

    SetToUpdate();
  }
}

// =======================================================================
// function : SetFont
// purpose  :
// =======================================================================
void GEOM_Annotation::SetFont( const TCollection_AsciiString& theFont )
{
  if ( GetFont() != theFont )
  {
    myDrawer->TextAspect()->Aspect()->SetFont( theFont );

    SetToUpdate();
  }
}

// =======================================================================
// function : SetDepthCulling
// purpose  :
// =======================================================================
void GEOM_Annotation::SetDepthCulling( const Standard_Boolean theToEnable )
{
  if ( GetDepthCulling() != theToEnable )
  {
    myIsDepthCulling = theToEnable;

    SetToUpdate();
  }
}

// =======================================================================
// function : SetDefaultZLayer
// purpose  :
// =======================================================================
void GEOM_Annotation::SetDefaultZLayer()
{
  SetZLayer( myIsScreenFixed ? Graphic3d_ZLayerId_Topmost : Graphic3d_ZLayerId_Default );

  SetToUpdate();
}

// =======================================================================
// function : GetDefaultPosition
// purpose  :
// =======================================================================
gp_Pnt GEOM_Annotation::GetDefaultPosition( const Standard_Boolean theIsScreenFixed,
                                            const gp_Pnt& theAttachPnt,
                                            const Standard_Real theOffset,
                                            const Handle(V3d_View)& theView )
{
  Standard_Integer aWinWidth = 0;
  Standard_Integer aWinHeight = 0;
  theView->Window()->Size( aWinWidth, aWinHeight );

  gp_Pnt aPositionProj = theView->Camera()->Project( theAttachPnt );
  aPositionProj.SetX( (aPositionProj.X() / 2.) * aWinWidth  + theOffset );
  aPositionProj.SetY( (aPositionProj.Y() / 2.) * aWinHeight + theOffset );
  aPositionProj.SetZ( 0.0 );

  if ( theIsScreenFixed )
  {
    return aPositionProj;
  }

  gp_Pnt aAttachProj = theView->Camera()->Project ( theAttachPnt );
  gp_Pnt aPosition3d = theView->Camera()->UnProject ( gp_Pnt ( aPositionProj.X() / aWinWidth * 2.,
                                                               aPositionProj.Y() / aWinHeight * 2., 
                                                               aAttachProj.Z() ));

  return aPosition3d;
}

// =======================================================================
// function : ConvertPosition2d
// purpose  :
// =======================================================================
gp_Pnt GEOM_Annotation::ConvertPosition2d( const gp_Pnt& thePosition,
                                           const gp_Pnt& /*theAttach*/,
                                           const Handle(V3d_View)& theView )
{
  Standard_Integer aWinWidth = 0;
  Standard_Integer aWinHeight = 0;
  theView->Window()->Size( aWinWidth, aWinHeight );

  gp_Pnt aPositionProj = theView->Camera()->Project( thePosition );
  aPositionProj.SetX( (aPositionProj.X() / 2.) * aWinWidth );
  aPositionProj.SetY( (aPositionProj.Y() / 2.) * aWinHeight );
  aPositionProj.SetZ( 0.0 );
  return aPositionProj;
}

// =======================================================================
// function : ConvertPosition3d
// purpose  :
// =======================================================================
gp_Pnt GEOM_Annotation::ConvertPosition3d( const gp_Pnt& thePosition,
                                           const gp_Pnt& theAttach,
                                           const Handle(V3d_View)& theView )
{
  Standard_Integer aWinWidth = 0;
  Standard_Integer aWinHeight = 0;
  theView->Window()->Size( aWinWidth, aWinHeight );

  gp_Pnt aAttachProj = theView->Camera()->Project( theAttach );
  gp_Pnt aPosition3d =  theView->Camera()->UnProject(
    gp_Pnt ( thePosition.X() / aWinWidth * 2., thePosition.Y() / aWinHeight * 2., aAttachProj.Z() ) );

  return aPosition3d;
}

// =======================================================================
// function : Compute
// purpose  :
// =======================================================================
void GEOM_Annotation::Compute( const Handle(PrsMgr_PresentationManager3d)& /*thePresentationManager*/,
                               const Handle(Prs3d_Presentation)&           thePresentation,
                               const Standard_Integer                      theMode )
{
  thePresentation->Clear();

  if (theMode < 0)
  {
    return;
  }

  Handle(OpenGl_Group) aGroup = Handle(OpenGl_Group)::DownCast( Prs3d_Root::NewGroup( thePresentation ) );
  if (aGroup.IsNull())
  {
    return;
  }

  Handle(Prs3d_TextAspect) anAsp = myDrawer->TextAspect();
  NCollection_String aUtfText( myText.ToExtString() );
  OpenGl_Annotation* aAnnotationDraw =
    new OpenGl_Annotation( this, static_cast<Standard_Integer>( anAsp->Height() ), aGroup->GlStruct()->GlDriver() );

  aAnnotationDraw->SetDepthMode( 0 );
  aGroup->SetGroupPrimitivesAspect( myDrawer->TextAspect()->Aspect() );
  aGroup->SetGroupPrimitivesAspect( myDrawer->LineAspect()->Aspect() );
  aGroup->SetGroupPrimitivesAspect( myDrawer->PointAspect()->Aspect() );
  aGroup->AddElement( aAnnotationDraw );

  if ( !myIsDepthCulling )
  {
    OpenGl_Annotation* aAnnotationDraw =
      new OpenGl_Annotation( this, static_cast<Standard_Integer>( anAsp->Height() ), aGroup->GlStruct()->GlDriver() );

    aAnnotationDraw->SetDepthMode( GL_GREATER );
    aGroup->SetPrimitivesAspect( myDrawer->TextAspect()->Aspect() );
    aGroup->SetPrimitivesAspect( myDrawer->HiddenLineAspect()->Aspect() );
    aGroup->SetPrimitivesAspect( myDrawer->PointAspect()->Aspect() );
    aGroup->AddElement( aAnnotationDraw );
  }

  Bnd_Box aBox = TextBoundingBox();
  if ( myIsScreenFixed )
  {
    gp_Trsf aOffset2d;
    aOffset2d.SetTranslation( gp_Vec( myPosition.X(), myPosition.Y(), 0.0 ) );
    aBox = aBox.Transformed( aOffset2d );
  }

  const gp_Pnt aBoxMin = aBox.CornerMin();
  const gp_Pnt aBoxMax = aBox.CornerMax();
  aGroup->ChangeBoundingBox() = Graphic3d_BndBox4f (
    Graphic3d_Vec4( static_cast<Standard_ShortReal>( aBoxMin.X() ),
                    static_cast<Standard_ShortReal>( aBoxMin.Y() ),
                    static_cast<Standard_ShortReal>( aBoxMin.Z() ), 1.0F ),
    Graphic3d_Vec4( static_cast<Standard_ShortReal>( aBoxMax.X() ),
                    static_cast<Standard_ShortReal>( aBoxMax.Y() ),
                    static_cast<Standard_ShortReal>( aBoxMax.Z() ), 1.0F ) );
}

// =======================================================================
// function : ComputeSelection
// purpose  :
// =======================================================================
void GEOM_Annotation::ComputeSelection( const Handle(SelectMgr_Selection)& theSelection,
                                        const Standard_Integer             theMode )
{
  if (theMode != GlobalSelectionMode())
  {
    return;
  }

  theSelection->Clear();

  Bnd_Box aBox = TextBoundingBox();
  if ( myIsScreenFixed )
  {
    gp_Trsf aOffset2d;
    aOffset2d.SetTranslation( gp_Vec( myPosition.X(), myPosition.Y(), 0.0 ) );
    aBox = aBox.Transformed( aOffset2d );
  }

  const Handle(GEOM_AnnotationOwner) anEntityOwner = new GEOM_AnnotationOwner( myShape, this, 10 );
  const Handle(GEOM_AnnotationSensEntity) aSensitive =
    new GEOM_AnnotationSensEntity( anEntityOwner, aBox, myIsDepthCulling );

  theSelection->Add( aSensitive );
}

// =======================================================================
// function : TextBoundingBox
// purpose  :
// =======================================================================
Bnd_Box GEOM_Annotation::TextBoundingBox() const
{
  Handle(Prs3d_TextAspect) anAsp = myDrawer->TextAspect();
  Font_FTFont aFont;
  unsigned int aResolution = GetContext()->CurrentViewer()->DefaultRenderingParams().Resolution;
  if ( aFont.Init( anAsp->Aspect()->Font().ToCString(),
                   anAsp->Aspect()->GetTextFontAspect(),
                  (unsigned int)anAsp->Height(),
                  aResolution ) )
  {
    const NCollection_String aText( (Standard_Utf16Char* )myText.ToExtString() );
    const Font_Rect aFontRect = aFont.BoundingBox( aText, Graphic3d_HTA_CENTER, Graphic3d_VTA_CENTER );
    Bnd_Box aBox;
    aBox.Add( gp_Pnt( aFontRect.Left, aFontRect.Bottom, 0.0 ) );
    aBox.Add( gp_Pnt( aFontRect.Right, aFontRect.Top, 0.0 ) );
    return aBox;
  }

  return Bnd_Box();
}

// =======================================================================
// function : BeginDrag
// purpose  :
// =======================================================================
void GEOM_Annotation::BeginDrag()
{
  myStartPosition = myPosition;
}

// =======================================================================
// function : Drag
// purpose  :
// =======================================================================
void GEOM_Annotation::Drag( const Standard_Integer theDx,
                            const Standard_Integer theDy,
                            const Handle(V3d_View)& theView )
{
  if (myIsScreenFixed)
  {
    SetPosition( myStartPosition.Translated( gp_Vec( theDx, theDy, 0.0 ) ), Standard_False );
  }
  else
  {
    Standard_Integer aWidth, aHeight;
    theView->Window()->Size( aWidth, aHeight );
    gp_Pnt aNormalized = theView->Camera()->Project( myStartPosition );
    gp_Pnt aNormalizedDrag =
      aNormalized.Translated( gp_Vec( static_cast<Standard_Real>(theDx) * 2.0 / aWidth,
                                      static_cast<Standard_Real>(theDy) * 2.0 / aHeight,
                                      0.0 ) );

    SetPosition( theView->Camera()->UnProject( aNormalizedDrag ), Standard_False );
  }
}

// =======================================================================
// function : EndDrag
// purpose  :
// =======================================================================
void GEOM_Annotation::EndDrag()
{
  UpdateSelection();
}

// =======================================================================
// function : UndoDrag
// purpose  :
// =======================================================================
void GEOM_Annotation::UndoDrag()
{
  SetPosition( myStartPosition, Standard_True );
}

// =======================================================================
// subclass : OpenGl_Annotation
// function : Constructor
// purpose  : 
// =======================================================================
GEOM_Annotation::OpenGl_Annotation::OpenGl_Annotation( GEOM_Annotation* theAnnotation,
                                                       const Standard_Integer theTextHeight,
                                                       const OpenGl_GraphicDriver* theDriver )
: OpenGl_Element(),
  myDepthMode( 0 ),
  myAISObject( theAnnotation ),
  myText( theAnnotation->myText.ToExtString() ),
  myTextLineY( 0.f ),
  myTextDPI( 0 )
{
  // graphical resources for drawing text and underline
  myTextParams.Height = theTextHeight;
  myTextParams.HAlign = Graphic3d_HTA_CENTER;
  myTextParams.VAlign = Graphic3d_VTA_CENTER;
  myTextDraw = new OpenGl_Text( myText.ToCString(), OpenGl_Vec3(), myTextParams );
  myTextLineDraw = new OpenGl_PrimitiveArray( theDriver );

  // graphical resources for drawing extension line and marker
  Handle(Graphic3d_ArrayOfSegments)
  aExtVertexArray = new Graphic3d_ArrayOfSegments( 2 );
  aExtVertexArray->AddVertex( 0.0, 0.0, 0.0 );
  aExtVertexArray->AddVertex( 0.0, 0.0, 1.0 );
  myExtLineDraw = new OpenGl_PrimitiveArray( theDriver, Graphic3d_TOPA_SEGMENTS,
    aExtVertexArray->Indices(), aExtVertexArray->Attributes(), aExtVertexArray->Bounds() );

  Handle(Graphic3d_ArrayOfPoints)
  aExtMakerArray = new Graphic3d_ArrayOfPoints( 1 );
  aExtMakerArray->AddVertex( 0.0, 0.0, 1.0 );
  myExtMarkerDraw = new OpenGl_PrimitiveArray( theDriver, Graphic3d_TOPA_POINTS,
    aExtMakerArray->Indices(), aExtMakerArray->Attributes(), aExtMakerArray->Bounds() );
}

// =======================================================================
// subclass : OpenGl_Annotation
// function : Destructor
// purpose  : 
// =======================================================================
GEOM_Annotation::OpenGl_Annotation::~OpenGl_Annotation()
{
  Release( NULL );
}

// =======================================================================
// subclass : OpenGl_Annotation
// function : Release
// purpose  : Releases GL resources with the given GL context.
// =======================================================================
void GEOM_Annotation::OpenGl_Annotation::Release( OpenGl_Context* theCtx )
{
  if (myTextDraw)
  {
    myTextDraw->Release( theCtx );
    myTextLineDraw->Release( theCtx );
    myExtLineDraw->Release( theCtx );
    myExtMarkerDraw->Release( theCtx );
  }
  myTextDraw      = NULL;
  myTextLineDraw  = NULL;
  myExtLineDraw   = NULL;
  myExtMarkerDraw = NULL;
}

// =======================================================================
// subclass : OpenGl_Annotation
// function : Render
// purpose  : Renders the annotation graphical elements.
// =======================================================================
void GEOM_Annotation::OpenGl_Annotation::Render( const Handle(OpenGl_Workspace)& theWorkspace ) const
{
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

  // ---------------------------------------------------------------------
  // initialize text's font and configure some properties when DPI changes
  // ---------------------------------------------------------------------

  const unsigned int aDPI = theWorkspace->View()->RenderingParams().Resolution;
  if (myTextDPI != aDPI)
  {
    const OpenGl_AspectText* anAspect = theWorkspace->AspectText();

    // getting string size will also initialize font library
    myTextDraw->StringSize( aContext,
      myText, *anAspect, myTextParams, aDPI,
      myTextSize.x, myTextSize.a, myTextSize.d );

    myTextDPI = aDPI;
    myTextSize.y = myTextSize.a - myTextSize.d;
    switch (myTextParams.HAlign)
    {
      case Graphic3d_HTA_LEFT:   myTextUnderline.x() = 0.f; break;
      case Graphic3d_HTA_CENTER: myTextUnderline.x() = -myTextSize.x / 2.f; break;
      case Graphic3d_HTA_RIGHT:  myTextUnderline.x() = -myTextSize.x; break;
      default:
        break;
    }
    switch (myTextParams.VAlign)
    {
      case Graphic3d_VTA_TOPFIRSTLINE:
      case Graphic3d_VTA_TOP:    myTextUnderline.y() = -myTextSize.y; break;
      case Graphic3d_VTA_CENTER: myTextUnderline.y() = -myTextSize.y / 2.f; break;
      case Graphic3d_VTA_BOTTOM: myTextUnderline.y() = myTextSize.d; break;
      default:
        break;
    }

    Handle(Graphic3d_ArrayOfSegments)
    aVertexArray = new Graphic3d_ArrayOfSegments( 2 );
    aVertexArray->AddVertex( myTextUnderline.x(), myTextUnderline.y(), 0.0f );
    aVertexArray->AddVertex( myTextUnderline.x() + myTextSize.x, myTextUnderline.y(), 0.0f );
    myTextLineDraw->InitBuffers( aContext, Graphic3d_TOPA_SEGMENTS,
      aVertexArray->Indices(), aVertexArray->Attributes(), aVertexArray->Bounds() );
  }

  // ---------------------------------------------
  // perform view culling test by attachment point
  // ---------------------------------------------

  const OpenGl_Vec4 aAttach( static_cast<float>( myAISObject->myAttach.X() ),
                             static_cast<float>( myAISObject->myAttach.Y() ),
                             static_cast<float>( myAISObject->myAttach.Z() ), 1.F );

  const Handle(Graphic3d_Camera) aCamera = theWorkspace->View()->Camera();
  const OpenGl_Mat4& aCameraProjMat = aCamera->ProjectionMatrixF();
  const OpenGl_Mat4& aCameraViewMat = aCamera->OrientationMatrixF();
  const OpenGl_Vec4 aAttachView = aCameraViewMat * aAttach;
  if (myAISObject->myIsAutoHide)
  {
    const OpenGl_Vec4 aAttachClip = aCameraProjMat * aAttachView;
    if (Abs( aAttachClip.x() ) > aAttachClip.w()
     || Abs( aAttachClip.y() ) > aAttachClip.w()
     || Abs( aAttachClip.z() ) > aAttachClip.w())
    {
      return;
    }
  }

  const bool toHighlight = theWorkspace->ToHighlight();

  if (toHighlight && myAISObject->myHilightMode == HighlightLabel)
  {
    theWorkspace->SetHighlight( false );
    theWorkspace->ApplyAspectLine();
  }

  GLint myOldDepthMode = 0;

  if ( myDepthMode )
  {
    aContext->core11fwd->glGetIntegerv( GL_DEPTH_FUNC, &myOldDepthMode );
    aContext->core11fwd->glDepthFunc( myDepthMode );
  }

  // -------------------------------------------------------------
  // render text label in current persistence matrix and underline
  // -------------------------------------------------------------

  if ( myAISObject->myIsScreenFixed )
  {
    // use text position property instead of matrix setup 
    // to avoid jittering when dragging text
    myTextDraw->SetPosition( OpenGl_Vec3( static_cast<float>( myAISObject->myPosition.X() ),
                                          static_cast<float>( myAISObject->myPosition.Y() ),
                                          static_cast<float>( myAISObject->myPosition.Z() ) ) );
  }

  myTextDraw->Render( theWorkspace );

  // ------------------------------------------------------------
  // render annotation text's underline
  // ------------------------------------------------------------

  if ( myAISObject->myIsScreenFixed )
  {
    // setup local transformation (in 2D persistence reference)
    // to represent position of annotation label on screen
    const OpenGl_Mat4& aViewMat = aContext->WorldViewState.Current();
    OpenGl_Mat4 aPositionMat;
    aPositionMat.SetValue( 0, 3, static_cast<float>( myAISObject->myPosition.X() ) );
    aPositionMat.SetValue( 1, 3, static_cast<float>( myAISObject->myPosition.Y() ) );
    aPositionMat.SetValue( 2, 3, static_cast<float>( myAISObject->myPosition.Z() ) );
    OpenGl_Mat4 aPosViewMat = aViewMat * aPositionMat;
    aContext->WorldViewState.Push();
    aContext->WorldViewState.SetCurrent( aPosViewMat );
    aContext->ApplyModelViewMatrix();
  }

  myTextLineDraw->Render( theWorkspace );

  // ------------------------------------------------------------
  // render dynamic extension line using synthetic transformation
  // ------------------------------------------------------------

  OpenGl_Vec4 aCenter (0.f, 0.f, 0.f, 1.f);
  switch (myTextParams.HAlign)
  {
    case Graphic3d_HTA_LEFT:   aCenter.x() =  myTextSize.x / 2.f; break;
    case Graphic3d_HTA_CENTER: aCenter.x() = 0.f; break;
    case Graphic3d_HTA_RIGHT:  aCenter.x() = -myTextSize.x / 2.f; break;
    default: break;
  }
  switch (myTextParams.VAlign)
  {
    case Graphic3d_VTA_TOPFIRSTLINE:
    case Graphic3d_VTA_TOP:    aCenter.y() = -myTextSize.y / 2.f; break;
    case Graphic3d_VTA_CENTER: aCenter.y() = 0.f; break;
    case Graphic3d_VTA_BOTTOM: aCenter.y() =  myTextSize.y / 2.f; break;
    default: break;
  }

  // compute label's center in view coordinate space
  const OpenGl_Mat4& aViewMat = aContext->WorldViewState.Current();
  const OpenGl_Vec4 aCenterView = aViewMat * aCenter;

  // the value below defines whether the extension line should be hanging
  // on the left side of the label or on the right
  const bool isLeftHanded = aAttachView.x() < aCenterView.x();

  // compute extension line point at the text label in view coordinate space
  const OpenGl_Vec4 aHingeView = aViewMat * OpenGl_Vec4(
    ( isLeftHanded ? myTextUnderline.x() : myTextUnderline.x() + myTextSize.x ), myTextUnderline.y(), 0.0f, 1.0f );

  // prepare matrix to specify geometry of extension line in view space
  // by multiplication of unit z coordinate vector on given matrix.
  OpenGl_Mat4 aExtGeometryMat;
  aExtGeometryMat.SetColumn( 2, aAttachView - aHingeView );
  aExtGeometryMat.SetColumn( 3, aHingeView );

  // setup and draw
  aContext->ModelWorldState.Push();
  aContext->ModelWorldState.SetIdentity();
  aContext->WorldViewState.Push();
  aContext->WorldViewState.SetCurrent( aExtGeometryMat );
  aContext->ApplyModelViewMatrix();

  myExtLineDraw->Render( theWorkspace );
  myExtMarkerDraw->Render( theWorkspace );

  // ------------------------------------------------------------
  // restore original state
  // ------------------------------------------------------------

  aContext->ModelWorldState.Pop();
  aContext->WorldViewState.Pop();

  if ( myOldDepthMode )
  {
    aContext->core11fwd->glDepthFunc( myOldDepthMode );
  }

  if ( myAISObject->myIsScreenFixed )
  {
    aContext->WorldViewState.Pop();
  }

  aContext->ApplyModelViewMatrix();

  if ( toHighlight != theWorkspace->ToHighlight() )
  {
    theWorkspace->SetHighlight( toHighlight );
  }
}

// =======================================================================
// subclass : GEOM_AnnotationOwner
// function : HilightWithColor
// purpose  : Perform highlighting of the presentation.
// =======================================================================
void GEOM_Annotation::GEOM_AnnotationOwner::HilightWithColor( const Handle(PrsMgr_PresentationManager3d)& thePM,
                                                              const Handle(Graphic3d_HighlightStyle)& theStyle,
                                                              const Standard_Integer theMode )
{
  if ( myPrsSh.IsNull() )
  {
    Handle(Prs3d_Drawer) aDrawer = new Prs3d_Drawer;
    aDrawer->Link( Selectable()->HilightAttributes() );

    Handle(Prs3d_IsoAspect) aUIsoAspect = new Prs3d_IsoAspect(
      aDrawer->UIsoAspect()->Aspect()->Color(),
      aDrawer->UIsoAspect()->Aspect()->Type(),
      aDrawer->UIsoAspect()->Aspect()->Width(), 0 );

    Handle(Prs3d_IsoAspect) aVIsoAspect = new Prs3d_IsoAspect(
      aDrawer->UIsoAspect()->Aspect()->Color(),
      aDrawer->UIsoAspect()->Aspect()->Type(),
      aDrawer->UIsoAspect()->Aspect()->Width(), 0 );

    aDrawer->SetIsoOnPlane( Standard_False );
    aDrawer->SetUIsoAspect( aUIsoAspect );
    aDrawer->SetVIsoAspect( aVIsoAspect );
    myPrsSh = new StdSelect_Shape( myShape, aDrawer );
  }

  myPrsSh->SetZLayer ( Selectable()->ZLayer() );

  thePM->Color( Selectable(), theStyle, theMode, NULL, Graphic3d_ZLayerId_Topmost );

  thePM->Color( myPrsSh, theStyle, theMode, Selectable(), Graphic3d_ZLayerId_Topmost );
}

// =======================================================================
// subclass : GEOM_AnnotationOwner
// function : Unhilight
// purpose  : Removes highlighting from the type of shape.
// =======================================================================
void GEOM_Annotation::GEOM_AnnotationOwner::Unhilight ( const Handle(PrsMgr_PresentationManager)& thePM,
                                                        const Standard_Integer theMode )
{
  SelectMgr_EntityOwner::Unhilight( thePM, theMode );

  thePM->Unhighlight( myPrsSh, theMode );
}

// =======================================================================
// subclass : GEOM_AnnotationOwner
// function : Clear
// purpose  : Clears the presentation manager object aPM of all shapes
// with the given selection mode.
// =======================================================================
void GEOM_Annotation::GEOM_AnnotationOwner::Clear ( const Handle(PrsMgr_PresentationManager)& thePM,
                                                    const Standard_Integer theMode )
{
  SelectMgr_EntityOwner::Clear( thePM, theMode );

  if ( !myPrsSh.IsNull() ) {
    thePM->Clear( myPrsSh, theMode );
  }

  myPrsSh.Nullify();
}
