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
#ifndef GEOM_Annotation_HeaderFile
#define GEOM_Annotation_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <Bnd_Box.hxx>
#include <Font_FontAspect.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_String.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_TextParam.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_Shape.hxx>
#include <TCollection_ExtendedString.hxx>

class OpenGl_GraphicDriver;
class OpenGl_PrimitiveArray;
class OpenGl_Text;
class V3d_View;

/*!
 * \class GEOM_Annotation
 * \brief Interactive object, representating annotation entity
 */
class GEOM_Annotation : public AIS_InteractiveObject
{
public:

  DEFINE_STANDARD_RTTIEXT( GEOM_Annotation, AIS_InteractiveObject )

  //! Enumerates supported highlighting modes.
  //! - HighlightAll   : all elements of the annotation are highlighted.
  //! - HighlightLabel : only annotation label is highlighted.
  enum HighlightMode
  {
    HighlightAll   = 0,
    HighlightLabel = 1
  };

public:

  //! Constructor.
  Standard_EXPORT GEOM_Annotation();

  //! Destructor.
  virtual ~GEOM_Annotation() {}

  //! Sets annotation text string.
  //! \param theText [in] the string displayed in annotation label.
  Standard_EXPORT void SetText( const TCollection_ExtendedString& theText );

  //! Returns annotation text string.
  const TCollection_ExtendedString& GetText() const { return myText; }

  //! Sets position of the annotation text label.
  //! \param thePosition [in] the cartesian point defining the position of lower left
  //!        corner of the text label. When displayed in fixed screen position mode
  //!        (\sa SetScreenFixed) the position is defined as {x,y} pixel coordinate
  //!        of window space, otherwise 3D point defined in world's coordinate system
  //!        is used.
  Standard_EXPORT void SetPosition( const gp_Pnt& thePosition );

  //! Returns position of the annotation text label.
  const gp_Pnt& GetPosition() const { return myPosition; }

  //! Sets or disables "2D screen fixed" positioning mode. In this mode the annotation
  //! is fixed at predefined pixel location in the window coordinate space. Other mode
  //! is "3D screen aligned" positioning, when the label is aligned in plane of the
  //! screen, while its position is a 3D point defined in world's coordinate system.
  Standard_EXPORT void SetIsScreenFixed( const Standard_Boolean theIsFixed );

  //! Retuns value of "screen fixed" positioning mode.
  Standard_Boolean GetIsScreenFixed() const { return myIsScreenFixed; }

  //! Converts annotation position to screen fixed coordinates and
  //! modifies its "2D screen fixed" flag correspondingly.
  //! @param theView [in] the view to perform projection of coordinates.
  Standard_EXPORT void Set2dPosition( const Handle(V3d_View)& theView );

  //! Converts annotation position from screen fixed coordinates
  //! to 3D position and modifies its "2D screen fixed" flag
  //! correspondingly.
  //! @param theView [in] the view to perform projection of coordinates.
  Standard_EXPORT void Set3dPosition( const Handle(V3d_View)& theView );

  //! Sets attachment point of extension line.
  //! \param thePoint [in] the 3D cartesian point defined in world's coordinate system
  //!        (a point on annotated geometry).
  Standard_EXPORT void SetAttachPoint( const gp_Pnt& thePoint );

  //! Returns attachment point of extension line.
  const gp_Pnt& GetAttachPoint() const { return myAttach; }

  //! Sets shape (annotated shape) that will be used for hilighting.
  Standard_EXPORT void SetHilightShape( const TopoDS_Shape& theShape );

  //! Returns the hilighting shape.
  const TopoDS_Shape HilightShape() const { return myShape; }

public:

  //! Sets color for the presentation.
  Standard_EXPORT virtual void SetColor( const Quantity_Color& theColor ) Standard_OVERRIDE;

  //! Sets text color.
  Standard_EXPORT void SetTextColor( const Quantity_Color& theColor );

  //! Returns color for the text's label.
  Quantity_Color GetTextColor() const { return myDrawer->TextAspect()->Aspect()->Color(); }

  //! Sets line's color.
  Standard_EXPORT void SetLineColor( const Quantity_Color& theColor);

   //! Returns color for the connection line.
  Quantity_Color GetLineColor() const { return myDrawer->LineAspect()->Aspect()->Color(); }

  //! Sets line width to be used for drawing the annotation's extension line and underline.
  Standard_EXPORT void SetLineWidth( const Standard_Real theLineWidth );

  //! Returns line width for drawing the annotation's extension line and underline.
  Standard_Real GetLineWidth() const { return myDrawer->LineAspect()->Aspect()->Width(); }

  //! Sets style of connection line.
  Standard_EXPORT void SetLineStyle( const Aspect_TypeOfLine theStyle );

  //! Retusn style of connection line.
  Aspect_TypeOfLine GetLineStyle() const { return myDrawer->LineAspect()->Aspect()->Type(); }

  //! Sets style of hidden connection line.
  Standard_EXPORT void SetHiddenLineStyle( const Aspect_TypeOfLine theStyle );

  //! Retusn style of hidden connection line.
  Aspect_TypeOfLine GetHiddenLineStyle() const { return myDrawer->HiddenLineAspect()->Aspect()->Type(); }

  //! Sets text height in pixels.
  Standard_EXPORT void SetTextHeight( const Standard_Real theHeight );

  //! Returns text's height in pixels.
  Standard_Real GetTextHeight() const { return myDrawer->TextAspect()->Height(); }

  //! Sets font aspect for label.
  Standard_EXPORT void SetFontAspect( const Font_FontAspect theFontAspect );

  //! Returns label's font aspect.
  Font_FontAspect GetFontAspect() const { return myDrawer->TextAspect()->Aspect()->GetTextFontAspect(); }

  //! Sets font used for drawing the label.
  Standard_EXPORT void SetFont( const TCollection_AsciiString& theFont );

  //! Returns font used for drawing the label.
  TCollection_AsciiString GetFont() const { return myDrawer->TextAspect()->Aspect()->Font(); }

  //! Sets annotation auto-hiding option.
  //! \param theIsEnable [in] the option flag. If passed true, the annotation 
  //!        will be automatically hidden in the view if the attachment point
  //!        goes outside of the view.
  void SetAutoHide( const Standard_Boolean theIsEnable ) { myIsAutoHide = theIsEnable; }

  //! Returns current state of the auto-hiding option.
  Standard_Boolean GetAutoHide() const { return myIsAutoHide; }

  //! Sets highlight mode used for display of presentation.
  //! \param theMode [in] the one of the supported highlight modes.
  void SetHighlightMode( const HighlightMode theMode ) { myHilightMode = theMode; }

  //! Returns highlight mode
  HighlightMode GetHilightMode() const { return myHilightMode; }

  //! Sets special flag that allows disabling depth testing when rendering
  //! the graphical presentation. When disable the hidden parts such as
  //! lines and text become visible and a rendered with another drawing
  //! aspect. This mode should be explicitly used with setting top layer
  //! for the presentation. Otherwise the behavior is undefined.
  Standard_EXPORT void SetDepthCulling( const Standard_Boolean theToEnable );

  //! Returns depth culling state.
  Standard_Boolean GetDepthCulling() const { return myIsDepthCulling; }

// Helper tools:
public:

  //! Restores default z-layer for this presentation.
  Standard_EXPORT void SetDefaultZLayer();

  //! Returns default position for placing annotation when its attachment point
  //! has been determined.
  //! @param theIsScreenFixed [in] the state of annotation's "2D screen fixed" mode.
  //! @param theAttachPnt [in] the attachment point.
  //! @param theOffset [in] the offset value for placing the position relative to attachment.s
  //! @param theView [in] the view for projecting coordinates.
  Standard_EXPORT static gp_Pnt GetDefaultPosition( const Standard_Boolean theIsScreenFixed,
                                                    const gp_Pnt& theAttachPnt,
                                                    const Standard_Real theOffset,
                                                    const Handle(V3d_View)& theView );

  //! Converts 3d position to 2d on screen point.
  //! @param thePosition [in] the 3d position.
  //! @param theAttach [in] the attachment point.
  //! @param theView [in] the view for projecting coordinates.
  Standard_EXPORT static gp_Pnt ConvertPosition2d( const gp_Pnt& thePosition,
                                                   const gp_Pnt& theAttach,
                                                   const Handle(V3d_View)& theView );

  //! Converts 2d position to 3d point.
  //! @param thePosition [in] the 2d position.
  //! @param theAttach [in] the attachment point.
  //! @param theView [in] the view for projecting coordinates.
  Standard_EXPORT static gp_Pnt ConvertPosition3d( const gp_Pnt& thePosition,
                                                   const gp_Pnt& theAttach,
                                                   const Handle(V3d_View)& theView );

// Interactive dragging:
public:

  //! Prepares necessary data to perform dragging.
  Standard_EXPORT void BeginDrag();

  //! Drags annotation presentation in the screen plane using the given pixel delta.
  //! \param theDx, theDy [in] the drag offset in pixels (from beginning of dragging).
  //! \param theView [in] the current view for evaluating drag with 3D position mode.
  Standard_EXPORT void Drag( const Standard_Integer theDx,
                             const Standard_Integer theDy,
                             const Handle(V3d_View)& theView );

  //! Perform necessary update when dragging is finished.
  Standard_EXPORT void EndDrag();

  //! Perform necessary update when dragging need undo.
  Standard_EXPORT void UndoDrag();

private:

  virtual void Compute( const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                        const Handle(Prs3d_Presentation)& thePresentation,
                        const Standard_Integer theMode = 0 ) Standard_OVERRIDE;

  virtual void ComputeSelection( const Handle(SelectMgr_Selection)& theSelection,
                                 const Standard_Integer theMode ) Standard_OVERRIDE;

  virtual void setLocalTransformation( const Handle(Geom_Transformation)& /*theTransformation*/ ) Standard_OVERRIDE {}

  virtual void SetTransformPersistence( const Handle(Graphic3d_TransformPers)& /*theTrsfPers*/ ) Standard_OVERRIDE {}

  Bnd_Box TextBoundingBox() const;

  void SetPosition( const gp_Pnt& thePosition, const Standard_Boolean theUpdateSelection );

private:

  gp_Pnt myAttach; //!< Attachment point of extension line.
  gp_Pnt myPosition; //!< Position of text label.
  gp_Pnt myStartPosition; //!< Position before starting dragging operation.
  Standard_Boolean myIsScreenFixed; //!< Flag indicating whether "screen fixed" positioning mode is turned on or off.
  Standard_Boolean myIsAutoHide; //!< Flag indicating whether "auto-hiding" option is turned on.
  Standard_Boolean myIsDepthCulling; //!< Flag indiciating whether the "depth culling" is turned on.
  HighlightMode myHilightMode; //!< Highlight mode for presentation.
  TCollection_ExtendedString myText; //!< Text string of the label presentation.
  TopoDS_Shape myShape; //!< Hilighting shape.

private:

  //! Custom element implementing dynamic rendering of 3D annotation
  //! and invoking dynamic callback in presentation class.
  class OpenGl_Annotation : public OpenGl_Element
  {
  public:

    //! Constructor. Some of the input properties are assigned by reference for dynamic draw
    //! (it is not likely that GL element will ever outlive the interactive object).
    //! \param theAnnotation [in] the instance of interactive presentation class.
    //! \param theTextHeight [in] the height of the text label.
    //! \param theDriver [in] the instance of graphical driver required for initialization.
    OpenGl_Annotation( GEOM_Annotation* theAnnotation,
                       const Standard_Integer theTextHeight,
                       const OpenGl_GraphicDriver* theDriver );

    //! Destructor. Releases GL resources with NULL context.
    virtual ~OpenGl_Annotation();

    //! Releases GL resources with the given GL context.
    virtual void Release( OpenGl_Context* theCtx ) Standard_OVERRIDE;

    //! Renders the annotation graphical elements.
    virtual void Render( const Handle(OpenGl_Workspace)& theWorkspace ) const Standard_OVERRIDE;

    void SetDepthMode( const int theMode ) { myDepthMode = theMode; }

  private:

    struct TextSize {
      float x; // width
      float y; // height
      float a; // ascent
      float d; // descent
    };

    GEOM_Annotation* myAISObject;           //!< Instance of presentation class.
    NCollection_String myText;              //!< Text string of annotation label.
    OpenGl_TextParam myTextParams;          //!< Text draw parameters.
    OpenGl_Text* myTextDraw;                //!< Text draw element.
    OpenGl_PrimitiveArray* myTextLineDraw;  //!< Text underline draw element.
    OpenGl_PrimitiveArray* myExtLineDraw;   //!< Extension line draw element.
    OpenGl_PrimitiveArray* myExtMarkerDraw; //!< Extension marker draw element.
    int myDepthMode;                        //!< Depth mode for drawing hidden line presentation.
    mutable float myTextLineY;              //!< Text's underlines relative position.
    mutable TextSize myTextSize;            //!< Text's size parameters
    mutable Graphic3d_Vec2 myTextUnderline; //!< Text's underline position.
    mutable unsigned int myTextDPI;         //!< Text's DPI scale used for last rendering.
  };

  friend class OpenGl_Annotation; // allow opengl element to get private data and invoke callback methods

public:

  //! Custom entity owner implementing correct highlight for topmost mode.
  class GEOM_AnnotationOwner : public SelectMgr_EntityOwner
  {
  public:

    //! Constructor.
    GEOM_AnnotationOwner( const TopoDS_Shape& theShape,
                          const Handle(SelectMgr_SelectableObject)& theSelectable,
                          const Standard_Integer thePriority )
      : SelectMgr_EntityOwner( theSelectable, thePriority ),
        myShape( theShape ) {}

    //! Perform highlighting of the presentation.
    //! \param thePresentationMgr [in] the presentation manager.
    //! \param theColor [in] the highlighting color.
    //! \param theMode [in] the display mode.
    virtual void
      HilightWithColor( const Handle(PrsMgr_PresentationManager3d)& thePM,
                        const Handle(Graphic3d_HighlightStyle)& theStyle,
                        const Standard_Integer theMode = 0 ) Standard_OVERRIDE;

    //! Removes highlighting from the type of shape.
    virtual void
      Unhilight ( const Handle(PrsMgr_PresentationManager)& thePM,
                  const Standard_Integer theMode = 0 ) Standard_OVERRIDE;

    //! Clears the presentation manager object aPM of all shapes
    //! with the given selection mode.
    virtual void
      Clear ( const Handle(PrsMgr_PresentationManager)& thePM,
              const Standard_Integer theMode = 0 ) Standard_OVERRIDE;

  private:

    TopoDS_Shape myShape;
    Handle(StdSelect_Shape) myPrsSh;
  };

  //! Custom sensitive entity with implementing option to support selection
  //! with depth culling flag turned off.
  class GEOM_AnnotationSensEntity : public Select3D_SensitiveBox
  {
  public:

    //! Constructor.
    GEOM_AnnotationSensEntity( const Handle(SelectMgr_EntityOwner)& theOwner,
                               const Bnd_Box& theBox,
                               const Standard_Boolean theIsDepthCulling )
      : Select3D_SensitiveBox( theOwner, theBox ),
        myIsDepthCulling( theIsDepthCulling ) {}

    //! Checks whether the box overlaps current selecting volume.
    virtual Standard_Boolean Matches( SelectBasics_SelectingVolumeManager& theMgr,
                                      SelectBasics_PickResult& thePickResult ) Standard_OVERRIDE
    {
      const Standard_Boolean isMatches = Select3D_SensitiveBox::Matches( theMgr, thePickResult );
      if ( !myIsDepthCulling )
      {
        thePickResult = SelectBasics_PickResult( -DBL_MAX, thePickResult.DistToGeomCenter() );
      }
      return isMatches;
    }

  private:

    Standard_Boolean myIsDepthCulling;
  };
};

DEFINE_STANDARD_HANDLE( GEOM_Annotation, AIS_InteractiveObject )

#endif
