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

// GEOM GEOMGUI : GUI for Geometry component
// File   : GEOM_Displayer.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "GEOM_Displayer.h"
#include "GEOMGUI_DimensionProperty.h"
#include "GeometryGUI.h"

#include <GEOM_Constants.h>
#include <GEOM_TypeFilter.h>
#include <GEOM_EdgeFilter.h>
#include <GEOM_FaceFilter.h>
#include <GEOM_CompoundFilter.h>
#include <GEOM_PreviewFilter.h>
#include <GEOM_LogicalFilter.h>
#include <GEOM_OCCFilter.h>

#include <GEOM_Actor.h>
#include <GEOM_AISDimension.hxx>
#include <GEOM_TopWireframeShape.hxx>
#include <GEOM_AISVector.hxx>
#include <GEOM_AISTrihedron.hxx>
#include <GEOM_VTKTrihedron.hxx>
#include <GEOM_VTKPropertyMaterial.hxx>

#include <GEOMUtils.hxx>

#include <Material_Model.h>

#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ResourceMgr.h>

#include <Basics_OCCTVersion.hxx>

#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_DataObject.h>
#include <SalomeApp_TypeFilter.h>
#include <SalomeApp_Tools.h>
#include "utilities.h"

#include <SALOME_ListIO.hxx>
#include <SALOME_Prs.h>
#include "utilities.h"

#include <SOCC_Prs.h>
#include <SOCC_ViewModel.h>

#include <SVTK_Prs.h>
#include <SVTK_ViewModel.h>

#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>
#include <OCCViewer_Utilities.h>

// OCCT Includes
#include <AIS_Dimension.hxx>
#include <AIS_LengthDimension.hxx>
#include <AIS_DiameterDimension.hxx>
#include <AIS_AngleDimension.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Aspect_PolygonOffsetMode.hxx>
#if OCC_VERSION_MAJOR < 7
  #include <Aspect_ColorScale.hxx>
#endif
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <StdSelect_TypeOfEdge.hxx>
#include <StdSelect_TypeOfFace.hxx>
#include <StdSelect_DisplayMode.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <gp_Pln.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TopoDS_Iterator.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>

#include <Prs3d_ShadingAspect.hxx>

#include <BRepMesh_IncrementalMesh.hxx>

// VTK Includes
#include <vtkActorCollection.h>
#include <vtkProperty.h>

// CORBA Headers
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)

#include <GEOMImpl_Types.hxx>

#include <TColStd_HArray1OfByte.hxx>

// If the next macro is defined, autocolor feature works for all sub-shapes;
// if it is undefined, autocolor feature works for groups only
#define GENERAL_AUTOCOLOR
// Below macro, when uncommented, switches on simplified (more performant) algorithm
// of auto-color picking up
#define SIMPLE_AUTOCOLOR

// Hard-coded value of shape deflection coefficient for VTK viewer
const double VTK_MIN_DEFLECTION = 0.001;
// If the next macro is defined, the deflection coefficient for VTK presentation
// is limited by VTK_MIN_DEFLECTION
//#define LIMIT_DEFLECTION_FOR_VTK

// Pixmap caching support
namespace
{
  typedef NCollection_Map<Handle(GEOM_AISShape)>                    SetOfAISShapes;
  typedef NCollection_DataMap<Handle(Image_PixMap), SetOfAISShapes> PixmapUsageMap;
  typedef QMap<QString, Handle(Image_PixMap)>                       PixmapCacheMap;

  static inline PixmapUsageMap& getPixmapUsageMap()
  {
    static PixmapUsageMap aMap;
    return aMap;
  }

  static inline PixmapCacheMap& getPixmapCacheMap()
  {
    static PixmapCacheMap aMap;
    return aMap;
  }

  //===========================================================================
  // Function : getDefaultTexture
  // Purpose  : Get default texture
  //===========================================================================
  static inline Handle(Image_PixMap) getDefaultTexture()
  {
    static Handle(Image_PixMap) aPixmap;
    if ( aPixmap.IsNull() ) {
      QPixmap px(":images/default_texture.png");
      if ( !px.isNull() )
	aPixmap = OCCViewer_Utilities::imageToPixmap( px.toImage() );
    }
    return aPixmap;
  }

  //===========================================================================
  // Function : cacheTextureFor
  // Purpose  : Load and cache image for the specified presentation.
  //===========================================================================
  static inline Handle(Image_PixMap) cacheTextureFor( const QString& thePath,
                                                      const Handle(GEOM_AISShape)& theShape )
  {
    if ( thePath.isEmpty() )
      return NULL;

    PixmapUsageMap& aPixmapUsersMap = getPixmapUsageMap();
    PixmapCacheMap& aPixmapCacheMap = getPixmapCacheMap();

    Handle(Image_PixMap) aPixmap = aPixmapCacheMap.value( thePath, NULL );
    if ( !aPixmap.IsNull() ) {
      // point that the texture is used by the presentation
      if ( !aPixmapUsersMap.IsBound( aPixmap ) )
        aPixmapUsersMap.Bind( aPixmap, SetOfAISShapes() );

      aPixmapUsersMap.ChangeFind( aPixmap ).Add( theShape );

      return aPixmap;
    }

    // convert texture to compatible image format
    QImage anImage = QImage( thePath ).convertToFormat( QImage::Format_ARGB32 );
    if ( anImage.isNull() )
      return NULL;

    aPixmap = OCCViewer_Utilities::imageToPixmap( anImage );

    aPixmapCacheMap.insert( thePath, aPixmap );

    if ( !aPixmapUsersMap.IsBound( aPixmap ) )
      aPixmapUsersMap.Bind( aPixmap, SetOfAISShapes() );

    aPixmapUsersMap.ChangeFind( aPixmap ).Add( theShape );

    return aPixmap;
  }

  //===========================================================================
  // Function : releaseTextures
  // Purpose  : Releases cached textures found for the specified presentation.
  //===========================================================================
  static inline void releaseTextures( const SALOME_OCCPrs* thePrs )
  {
    const SOCC_Prs* anOccPrs = dynamic_cast<const SOCC_Prs*>( thePrs );

    AIS_ListOfInteractive aListOfIO;

    anOccPrs->GetObjects( aListOfIO );

    AIS_ListIteratorOfListOfInteractive aIterateIO( aListOfIO );

    PixmapUsageMap& aPixmapUsersMap = getPixmapUsageMap();
    PixmapCacheMap& aPixmapCacheMap = getPixmapCacheMap();

    for ( ; aIterateIO.More(); aIterateIO.Next() )
    {
      Handle(GEOM_AISShape) aAISShape =
        Handle(GEOM_AISShape)::DownCast( aIterateIO.Value() );

      if ( aAISShape.IsNull() )
        continue;

#ifdef USE_TEXTURED_SHAPE
      const Handle(Image_PixMap)& aPixmap = aAISShape->TexturePixMap();
      if ( aPixmap.IsNull() )
        continue;

      if ( !aPixmapUsersMap.IsBound( aPixmap ) )
        continue;

      SetOfAISShapes& aUsersShapes = aPixmapUsersMap.ChangeFind( aPixmap );

      aUsersShapes.Remove( aAISShape );

      if ( aUsersShapes.IsEmpty() ) {
        aPixmapUsersMap.UnBind( aPixmap );
        aPixmapCacheMap.remove( aPixmapCacheMap.key( aPixmap ) );
      }
#endif
    }
  }

  uint randomize( uint size )
  {
    static bool initialized = false;
    if ( !initialized ) {
      qsrand( QDateTime::currentDateTime().toTime_t() );
      initialized = true;
    }
    uint v = qrand();
    v = uint( (double)( v ) / RAND_MAX * size );
    v = qMax( uint(0), qMin ( v, size-1 ) );
    return v;
  }
} // namespace

//================================================================
// Function : getActiveStudy
// Purpose  : Get active study, returns 0 if no open study frame
//================================================================
static inline SalomeApp_Study* getActiveStudy()
{
  SUIT_Session* session = SUIT_Session::session();
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( session->activeApplication() );
  if ( app )
    return ( SalomeApp_Study* )app->activeStudy();
  return 0;
}

static inline int getViewManagerId( SALOME_View* theViewFrame) {
  SUIT_ViewModel* aModel = dynamic_cast<SUIT_ViewModel*>(theViewFrame);
  SUIT_ViewManager* aViewMgr = 0;
  if (aModel != 0)
    aViewMgr = aModel->getViewManager();
  return ((aViewMgr == 0) ? -1 :aViewMgr->getGlobalId());
}

//================================================================
// Function : getTopAbsMode
// Purpose  : Get TopAbs_ShapeEnum value corresponding to the
//            one from GEOMImpl_Types.h
//================================================================
static inline int getTopAbsMode( const int implType )
{
  switch ( implType )
  {
    case GEOM_COMPOUND  : return TopAbs_COMPOUND;
    case GEOM_SOLID     : return TopAbs_SOLID;
    case GEOM_SHELL     : return TopAbs_SHELL;
    case GEOM_FACE      : return TopAbs_FACE;
    case GEOM_WIRE      : return TopAbs_WIRE;
    case GEOM_EDGE      : return TopAbs_EDGE;
    case GEOM_POINT     : return TopAbs_VERTEX;
    default             : return -1;
  }
}

int GEOM_Displayer::getMinMaxShapeType( const TopoDS_Shape& shape, bool ismin )
{
  if ( shape.IsNull() )
    return TopAbs_SHAPE;

  int ret = shape.ShapeType();

  if ( shape.ShapeType() == TopAbs_COMPOUND || shape.ShapeType() == TopAbs_COMPSOLID ) {
    TopoDS_Iterator it(shape, Standard_True, Standard_False);
    for (; it.More(); it.Next()) {
      TopoDS_Shape sub_shape = it.Value();
      if ( sub_shape.IsNull() ) continue;
      int stype = getMinMaxShapeType( sub_shape, ismin );
      if ( stype == TopAbs_SHAPE ) continue;
      if ( ismin && stype > ret )
        ret = stype;
      else if ( !ismin && ( ret < TopAbs_SOLID || stype < ret ) )
        ret = stype;
    }
  }

  return ret;
}

bool GEOM_Displayer::isCompoundOfVertices( const TopoDS_Shape& theShape )
{
  return theShape.ShapeType() == TopAbs_COMPOUND && getMinMaxShapeType( theShape, false ) == TopAbs_VERTEX;
}

//================================================================
// Function : getFilter
// Purpose  : Get filter corresponding to the type of object
//            from GEOMImpl_Types.h
//================================================================
SUIT_SelectionFilter* GEOM_Displayer::getFilter( const int theMode )
{
  SUIT_SelectionFilter* aFilter;

  int aTopAbsMode = getTopAbsMode( theMode );
  if ( aTopAbsMode != -1 )
    aFilter = new GEOM_TypeFilter( getStudy(), aTopAbsMode, true ); //@ aFilter = new GEOM_TypeFilter( ( TopAbs_ShapeEnum )aTopAbsMode );
  else
    switch ( theMode )
      {
      case GEOM_LINE      : aFilter = new GEOM_EdgeFilter( getStudy(), StdSelect_Line ); break;
      case GEOM_CIRCLE    : aFilter = new GEOM_EdgeFilter( getStudy(), StdSelect_Circle ); break;

      case GEOM_PLANE     : aFilter = new GEOM_FaceFilter( getStudy(), StdSelect_Plane ); break;
      case GEOM_CYLINDER  : aFilter = new GEOM_FaceFilter( getStudy(), StdSelect_Cylinder ); break;
      case GEOM_SPHERE    : aFilter = new GEOM_FaceFilter( getStudy(), StdSelect_Sphere ); break;
      case GEOM_TORUS     : aFilter = new GEOM_FaceFilter( getStudy(), StdSelect_Torus ); break;
      case GEOM_REVOLUTION: aFilter = new GEOM_FaceFilter( getStudy(), StdSelect_Revol ); break;
      case GEOM_CONE      : aFilter = new GEOM_FaceFilter( getStudy(), StdSelect_Cone ); break;

      case GEOM_PREVIEW   : aFilter = new GEOM_PreviewFilter( getStudy() ); break;

      case GEOM_ALLSHAPES : aFilter = new GEOM_SelectionFilter(getStudy(), true ); break;
      case GEOM_ALLGEOM   : aFilter = new SalomeApp_TypeFilter( getStudy(), "GEOM" ); break;

      default             : aFilter = new GEOM_TypeFilter( getStudy(), theMode ); break;
      }

  return aFilter;
}

//================================================================
// Function : getComplexFilter
// Purpose  : Get compound filter corresponding to the type of
//            object from GEOMImpl_Types.h
//================================================================
SUIT_SelectionFilter* GEOM_Displayer::getComplexFilter( const QList<int>* aSubShapes)
{
  GEOM_CompoundFilter* aFilter;

  if(aSubShapes != NULL ) {
    aFilter = new GEOM_CompoundFilter(getStudy());
    QList<int> aTopAbsTypes;
    QList<int>::const_iterator it;
    for(it = aSubShapes->constBegin(); it != aSubShapes->constEnd(); ++it ) {
      int topAbsMode = getTopAbsMode(*it);
      if(topAbsMode != -1 )
        aTopAbsTypes.append(topAbsMode);
    }
    aFilter->addSubTypes(aTopAbsTypes);
  }

  return aFilter;
}

//================================================================
// Function : getEntry
// Purpose  :
//================================================================
static std::string getEntry( GEOM::GEOM_BaseObject_ptr object )
{
  SUIT_Session* session = SUIT_Session::session();
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( session->activeApplication() );
  if ( app )
  {
    CORBA::String_var IOR = app->orb()->object_to_string( object );
    if ( strcmp(IOR.in(), "") != 0 )
    {
      SalomeApp_Study* study = ( SalomeApp_Study* )app->activeStudy();
      _PTR(SObject) SO ( study->studyDS()->FindObjectIOR( std::string(IOR) ) );
      if ( SO )
        return SO->GetID();
    }
  }
  return "";
}

//================================================================
// Function : getName
// Purpose  :
//================================================================
static std::string getName( GEOM::GEOM_BaseObject_ptr object )
{
  SUIT_Session* session = SUIT_Session::session();
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( session->activeApplication() );
  if ( app )
  {
    CORBA::String_var IOR = app->orb()->object_to_string( object );
    if ( strcmp(IOR.in(), "") != 0 )
    {
      SalomeApp_Study* study = ( SalomeApp_Study* )app->activeStudy();
      _PTR(SObject) aSObj ( study->studyDS()->FindObjectIOR( std::string(IOR) ) );

      _PTR(GenericAttribute) anAttr;

      if ( aSObj && aSObj->FindAttribute( anAttr, "AttributeName") )
      {
        _PTR(AttributeName) aNameAttr( anAttr );
        return aNameAttr->Value();
      }
    }
  }

  return "";
}

//=================================================================
/*!
 *  GEOM_Displayer::GEOM_Displayer
 *  Constructor
 */
//=================================================================
GEOM_Displayer::GEOM_Displayer( SalomeApp_Study* st )
{
  if( st )
    myApp = dynamic_cast<SalomeApp_Application*>( st->application() );
  else
    myApp = 0;

  /* Shading Color */
  SUIT_Session* session = SUIT_Session::session();
  SUIT_ResourceMgr* resMgr = session->resourceMgr();

  QColor col = resMgr->colorValue( "Geometry", "shading_color", QColor( 255, 0, 0 ) );
  myShadingColor = SalomeApp_Tools::color( col );

  myDisplayMode = resMgr->integerValue("Geometry", "display_mode", 0);
  myHasDisplayMode = false;

  int aType = resMgr->integerValue("Geometry", "type_of_marker", (int)Aspect_TOM_PLUS);
  
  myTypeOfMarker = (Aspect_TypeOfMarker)(std::min((int)Aspect_TOM_RING3, std::max((int)Aspect_TOM_POINT, aType)));
  myScaleOfMarker = (resMgr->integerValue("Geometry", "marker_scale", 1)-(int)GEOM::MS_10)*0.5 + 1.0;
  myScaleOfMarker = std::min(7.0, std::max(1., myScaleOfMarker));

  // Next properties provide a way to customize displaying of presentations;
  // for instance, this is useful for preview
  myColor = -1;
  myTexture = "";
  myNbIsos = -1;
  myWidth = -1;
  myTransparency = -1;
  myType = -1;
  myIsosColor = -1;
  myIsosWidth = -1;

  // This parameter is used for activisation/deactivisation (selection) of objects to be displayed
  myToActivate = true;

  // Activate parallel vizualisation only for testing purpose
  // and if the corresponding env variable is set to 1
  char* parallel_visu = getenv("PARALLEL_VISU");
  if (parallel_visu && atoi(parallel_visu))
  {
    MESSAGE("Parallel visualisation on");
    BRepMesh_IncrementalMesh::SetParallelDefault(Standard_True);
  }

  myViewFrame = 0;

#if OCC_VERSION_MAJOR >= 7
  myColorScale = new AIS_ColorScale;
  myColorScale->SetZLayer (Graphic3d_ZLayerId_TopOSD);
  myColorScale->SetTransformPersistence (Graphic3d_TMF_2d, gp_Pnt (-1,-1,0));
#endif

  myFieldDataType = GEOM::FDT_Double;
  myFieldDimension = 0;
  myFieldStepRangeMin = 0;
  myFieldStepRangeMax = 0;
}

//=================================================================
/*!
 *  GEOM_Displayer::~GEOM_Displayer
 *  Destructor
 */
//=================================================================
GEOM_Displayer::~GEOM_Displayer()
{
}

//=================================================================
/*!
 *  GEOM_Displayer::Display
 *  Display interactive object in the current viewer
 */
//=================================================================
void GEOM_Displayer::Display( const Handle(SALOME_InteractiveObject)& theIO,
                             const bool updateViewer,
                             SALOME_View* theViewFrame )
{
  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();
  if ( vf )
  {
    SALOME_Prs* prs = buildPresentation( theIO->getEntry(), vf );

    if ( prs )
    {
      vf->BeforeDisplay( this, prs );
      vf->Display( this, prs );
      vf->AfterDisplay( this, prs );

      if ( updateViewer )
        vf->Repaint();

      delete prs;  // delete presentation because displayer is its owner
    }
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Display
 *  This overloaded Display() method can be useful for operations
 *  not using dialog boxes.
 */
//=================================================================
void GEOM_Displayer::Display( GEOM::GEOM_BaseObject_ptr theObj, const bool updateViewer )
{
  if ( theObj->_is_nil() )
    return;

  std::string entry = getEntry( theObj );
  if ( entry != "" ) {
    Display(new SALOME_InteractiveObject(entry.c_str(), "GEOM", getName(theObj).c_str()),
            updateViewer);
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Erase
 *  Erase interactive object in the current viewer
 */
//=================================================================
void GEOM_Displayer::Erase( const Handle(SALOME_InteractiveObject)& theIO,
                            const bool forced,
                            const bool updateViewer,
                            SALOME_View* theViewFrame )
{
  if ( theIO.IsNull() )
    return;

  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();

  if ( vf ) {
    SALOME_Prs* prs = vf->CreatePrs( theIO->getEntry() );
    if ( prs ) {
      vf->BeforeErase( this, prs );
      vf->Erase( this, prs, forced );
      vf->AfterErase( this, prs );
      if ( updateViewer )
        vf->Repaint();
      delete prs;  // delete presentation because displayer is its owner
    }
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Erase
 *  Erase geometry object in the current viewer
 */
//=================================================================
void GEOM_Displayer::Erase( GEOM::GEOM_BaseObject_ptr theObj,
                            const bool forced,
                            const bool updateViewer,
                            SALOME_View* theViewFrame)
{
  std::string entry = getEntry( theObj );
  if ( entry != "" )
  {
    Erase(new SALOME_InteractiveObject(entry.c_str(), "GEOM", getName(theObj).c_str()),
          forced, updateViewer, theViewFrame);
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Redisplay
 *  Redisplay (erase and then display again) interactive object
 *  in the current viewer
 */
//=================================================================
void GEOM_Displayer::Redisplay( const Handle(SALOME_InteractiveObject)& theIO,
                                const bool updateViewer,
                                const bool checkActiveViewer )
{
  // Remove the object permanently (<forced> == true)
  SUIT_Session* ses = SUIT_Session::session();
  SUIT_Application* app = ses->activeApplication();
  if ( app )
  {
    SUIT_Desktop* desk = app->desktop();
    QList<SUIT_ViewWindow*> wnds = desk->windows();
    SUIT_ViewWindow* wnd;
    QListIterator<SUIT_ViewWindow*> it( wnds );
    while ( it.hasNext() && (wnd = it.next()) )
    {
      SUIT_ViewManager* vman = wnd->getViewManager();
      if ( vman )
      {
        SUIT_ViewModel* vmodel = vman->getViewModel();
        if ( vmodel )
        {
          SALOME_View* view = dynamic_cast<SALOME_View*>(vmodel);
          if ( view )
          {
            if ( view->isVisible( theIO ) || ( checkActiveViewer && view == GetActiveView() ) )
            {
              Redisplay( theIO, updateViewer, view );
            }
          }
        }
      }
    }
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Redisplay
 *  Redisplay (erase and then display again) interactive object
 *  in the specified view
 */
//=================================================================
void GEOM_Displayer::Redisplay( const Handle(SALOME_InteractiveObject)& theIO,
                                const bool theUpdateViewer,
                                SALOME_View* theViewFrame )
{
  SALOME_View* vf = theViewFrame ? theViewFrame : GetActiveView();
  if ( !vf )
  {
    return;
  }

  Erase( theIO, true, false, theViewFrame );
  Display( theIO, theUpdateViewer, theViewFrame );
}

//=================================================================
/*!
 *  GEOM_Displayer::Display
 *  Calls Display() method for each object in the given list
 */
//=================================================================
void GEOM_Displayer::Display( const SALOME_ListIO& theIOList, const bool updateViewer )
{
  SALOME_ListIteratorOfListIO Iter( theIOList );
  for ( ; Iter.More(); Iter.Next() ) {
    Display( Iter.Value(), false );
  }
  if ( updateViewer )
    UpdateViewer();
}

void GEOM_Displayer::UpdateVisibility( SALOME_View* v, const SALOME_Prs* p, bool on )
{
  SalomeApp_Study* aStudy = getStudy();
  int vId = -1;
  if ( v ) vId = getViewManagerId( v );

  if ( p ) {
    QString entry = p->GetEntry();
    if ( !entry.isEmpty() ) {
      if ( vId != -1 )
	aStudy->setObjectProperty( vId, entry, GEOM::propertyName( GEOM::Visibility ), on );
      setVisibilityState( entry, on ? Qtx::ShownState : Qtx::HiddenState );
    }
  }
  else {
    if ( vId != -1 ) {
      aStudy->setObjectProperty( vId, GEOM::propertyName( GEOM::Visibility ), on );
    }
  }
}

Quantity_Color GEOM_Displayer::qColorFromResources( const QString& property, const QColor& defColor )
{
  // VSR: this method can be improved in future:
  // to improve performance, the default values from resource manager should be cached in the displayer
  return SalomeApp_Tools::color( SUIT_Session::session()->resourceMgr()->colorValue( "Geometry", property, defColor ) );
}

QColor GEOM_Displayer::colorFromResources( const QString& property, const QColor& defColor )
{
  // VSR: this method can be improved in future:
  // to improve performance, the default values from resource manager should be cached in the displayer
  return SUIT_Session::session()->resourceMgr()->colorValue( "Geometry", property, defColor );
}

void GEOM_Displayer::updateShapeProperties( const Handle(GEOM_AISShape)& AISShape, bool create )
{
  // check that shape is not null
  if ( AISShape.IsNull() ) return;
  
  // check that study is active
  SalomeApp_Study* study = getStudy();
  if ( !study ) return;

  if ( myShape.ShapeType() != TopAbs_VERTEX && // fix pb with not displayed points
       !TopoDS_Iterator(myShape).More() )
    return; // NPAL15983 (Bug when displaying empty groups)

  // set interactive object

  Handle( SALOME_InteractiveObject ) anIO;

  if ( !myIO.IsNull() ) {
    AISShape->setIO( myIO );
    AISShape->SetOwner( myIO );
    anIO = myIO;
  }
  else if ( !myName.empty() ) {
    // workaround to allow selection of temporary objects
    static int tempId = 0;
    anIO = new SALOME_InteractiveObject( QString( "TEMP_%1" ).arg( tempId++ ).toLatin1().data(), "GEOM", myName.c_str() );
    AISShape->setIO( anIO );
    AISShape->SetOwner( anIO );
  }

  // flag:  only vertex or compound of vertices is processed (specific handling)
  bool onlyVertex = myShape.ShapeType() == TopAbs_VERTEX || isCompoundOfVertices( myShape );
  // presentation study entry (empty for temporary objects like preview)
  QString entry = !anIO.IsNull() ? QString( anIO->getEntry() ) : QString();
  // flag: temporary object
  bool isTemporary = entry.isEmpty() || entry.startsWith( "TEMP_" );
  // currently active view window's ID (-1 if no active view)
  int aMgrId = !anIO.IsNull() ? getViewManagerId( myViewFrame ) : -1;

  // get presentation properties
  PropMap propMap = getObjectProperties( study, entry, myViewFrame );

  // Temporary staff: vertex must be infinite for correct visualization
  AISShape->SetInfiniteState( myShape.Infinite() ); // || myShape.ShapeType() == TopAbs_VERTEX // VSR: 05/04/2010: Fix 20668 (Fit All for points & lines)

  // set material
  Material_Model material;
  // if predefined color isn't set in displayer(via GEOM_Displayer::SetColor() function)
  if( !HasColor() )
    material.fromProperties( propMap.value( GEOM::propertyName( GEOM::Material ) ).toString() );
  // - set front material properties
  AISShape->SetCurrentFacingModel( Aspect_TOFM_FRONT_SIDE );
  AISShape->SetMaterial( material.getMaterialOCCAspect( true ) );
  // - set back material properties
  AISShape->SetCurrentFacingModel( Aspect_TOFM_BACK_SIDE );
  AISShape->SetMaterial( material.getMaterialOCCAspect( false ) );
  // - switch to default (both sides) facing mode
  AISShape->SetCurrentFacingModel( Aspect_TOFM_BOTH_SIDE );

  // set colors

  // - shading color
  if ( HasColor()  ) {
    // predefined color, manually set to displayer via GEOM_Displayer::SetColor() function;
    // we set it to the shape not taking into account material properties
    AISShape->SetShadingColor( (Quantity_NameOfColor)GetColor() );
  }
  else if ( !material.isPhysical() ) {
    // shading color from properties is used only for non-physical materials
    AISShape->SetShadingColor( SalomeApp_Tools::color( propMap.value( GEOM::propertyName( GEOM::ShadingColor ) ).value<QColor>() ) );
  }

  // - wireframe color
  Handle(Prs3d_LineAspect) anAspect = AISShape->Attributes()->LineAspect();
  anAspect->SetColor( HasColor() ? (Quantity_NameOfColor)GetColor() : 
                      SalomeApp_Tools::color( propMap.value( GEOM::propertyName( GEOM::WireframeColor ) ).value<QColor>() ) );
  AISShape->Attributes()->SetLineAspect( anAspect );
  
  // - unfree boundaries color
  anAspect = AISShape->Attributes()->UnFreeBoundaryAspect();
  anAspect->SetColor( HasColor() ? (Quantity_NameOfColor)GetColor() : 
                      SalomeApp_Tools::color( propMap.value( GEOM::propertyName( GEOM::WireframeColor ) ).value<QColor>() ) );
  AISShape->Attributes()->SetUnFreeBoundaryAspect( anAspect );
  
  // - free boundaries color
  anAspect = AISShape->Attributes()->FreeBoundaryAspect();
  anAspect->SetColor( HasColor() ? (Quantity_NameOfColor)GetColor() : 
                      SalomeApp_Tools::color( propMap.value( GEOM::propertyName( GEOM::FreeBndColor ) ).value<QColor>() ) );
  AISShape->Attributes()->SetFreeBoundaryAspect( anAspect );
  
  // - standalone edges color
  anAspect = AISShape->Attributes()->WireAspect();
  anAspect->SetColor( HasColor() ? (Quantity_NameOfColor)GetColor() : 
                      SalomeApp_Tools::color( propMap.value( GEOM::propertyName( GEOM::LineColor ) ).value<QColor>() ) );
  AISShape->Attributes()->SetWireAspect( anAspect );
  
  // - color for edges in shading+edges mode
  AISShape->SetEdgesInShadingColor( SalomeApp_Tools::color( propMap.value( GEOM::propertyName( GEOM::OutlineColor ) ).value<QColor>() ) );

  // - color of labels (textual fields and shape name)
  AISShape->SetLabelColor( qColorFromResources( "label_color", QColor( 255, 255, 255 ) ) );

  // set display mode
  AISShape->SetDisplayMode( HasDisplayMode() ? 
                            // predefined display mode, manually set to displayer via GEOM_Displayer::SetDisplayMode() function 
                            GetDisplayMode() :
                            // display mode from properties
                            propMap.value( GEOM::propertyName( GEOM::DisplayMode ) ).toInt() );

  // - face boundaries color and line width
  anAspect = AISShape->Attributes()->FaceBoundaryAspect();
  anAspect->SetColor( SalomeApp_Tools::color( propMap.value( GEOM::propertyName( GEOM::OutlineColor ) ).value<QColor>() ) );
  anAspect->SetWidth( HasWidth() ? GetWidth() : propMap.value( GEOM::propertyName( GEOM::LineWidth ) ).toInt() );
  AISShape->Attributes()->SetFaceBoundaryAspect( anAspect );

  // set display vectors flag
  AISShape->SetDisplayVectors( propMap.value( GEOM::propertyName( GEOM::EdgesDirection ) ).toBool() );

  // set display vertices flag
  bool isVerticesMode = propMap.value( GEOM::propertyName( GEOM::Vertices ) ).toBool();
  AISShape->SetDisplayVertices( isVerticesMode );

  // set display name flag
  bool isNameMode = propMap.value( GEOM::propertyName( GEOM::ShowName ) ).toBool();
  AISShape->SetDisplayName( isNameMode );

  // set transparency
  if( HasTransparency() ) {
    AISShape->SetTransparency( GetTransparency() );
  } else {
    AISShape->SetTransparency( propMap.value( GEOM::propertyName( GEOM::Transparency ) ).toDouble() );
  }

  // set iso properties
  int uIsos = propMap.value( GEOM::propertyName( GEOM::NbIsos ) ).toString().split( GEOM::subSectionSeparator() )[0].toInt();
  int vIsos = propMap.value( GEOM::propertyName( GEOM::NbIsos ) ).toString().split( GEOM::subSectionSeparator() )[1].toInt();
  Quantity_Color isosColor = SalomeApp_Tools::color( propMap.value( GEOM::propertyName( GEOM::IsosColor ) ).value<QColor>() );
  int isosWidth = propMap.value( GEOM::propertyName( GEOM::IsosWidth ) ).toInt();
  Handle(Prs3d_IsoAspect) uIsoAspect = AISShape->Attributes()->UIsoAspect();
  Handle(Prs3d_IsoAspect) vIsoAspect = AISShape->Attributes()->VIsoAspect();

  if ( HasIsosColor() ) {
    uIsoAspect->SetColor( (Quantity_NameOfColor)GetIsosColor() );
    vIsoAspect->SetColor( (Quantity_NameOfColor)GetIsosColor() );
  }
  else {
    uIsoAspect->SetColor( isosColor );
    vIsoAspect->SetColor( isosColor );
  }

  if ( HasIsosWidth() ) {
    uIsoAspect->SetWidth( GetIsosWidth() );
    vIsoAspect->SetWidth( GetIsosWidth() );
  }
  else {
    uIsoAspect->SetWidth( isosWidth );
    vIsoAspect->SetWidth( isosWidth );
  }
  
  if ( HasNbIsos() ) {
    uIsoAspect->SetNumber( GetNbIsos() );
    vIsoAspect->SetNumber( GetNbIsos() );
  }
  else {
    uIsoAspect->SetNumber( uIsos );
    vIsoAspect->SetNumber( vIsos );
  }

  AISShape->Attributes()->SetUIsoAspect( uIsoAspect );
  AISShape->Attributes()->SetVIsoAspect( vIsoAspect );

  // set deflection coefficient
  // ... to avoid to small values of the coefficient, its lower value is limited 
  AISShape->SetOwnDeviationCoefficient( qMax( propMap.value( GEOM::propertyName( GEOM::Deflection ) ).toDouble(), GEOM::minDeflection() ) );

  // set texture
  QString aImagePath;
  if ( HasTexture() ) {
    // predefined display texture, manually set to displayer via GEOM_Displayer::SetTexture() function 
    aImagePath = GetTexture().c_str();
    if ( ! entry.isEmpty() ) {
      // check that study is active
      SalomeApp_Study* study = getActiveStudy();
      if ( study ) {
        // Store the texture in object properties for next displays
        study->setObjectProperty( aMgrId, entry, GEOM::propertyName( GEOM::Texture ), QString( GetTexture().c_str() ) );
        study->setObjectProperty( aMgrId, entry, GEOM::propertyName( GEOM::DisplayMode ), 3 );
        
        // Update propeties map
        propMap = getObjectProperties( study, entry, myViewFrame );
      }
    }
  }
  else {
    aImagePath = propMap.value( GEOM::propertyName( GEOM::Texture ) ).toString();
  }

#ifdef USE_TEXTURED_SHAPE
  Handle(Image_PixMap) aPixmap;
  if ( !aImagePath.isEmpty() )
    aPixmap = cacheTextureFor( aImagePath, AISShape );
  else
    aPixmap = getDefaultTexture();

  // apply image to shape
  if ( !aPixmap.IsNull() ) {
    AISShape->SetTexturePixMap( aPixmap );
    AISShape->SetTextureMapOn();
    AISShape->DisableTextureModulate();
  }
  else {
    AISShape->SetTextureMapOff();
  }
#endif

  // set line width
  AISShape->SetWidth( HasWidth() ?
                      // predefined line width, manually set to displayer via GEOM_Displayer::SetLineWidth() function 
                      GetWidth() :
                      // libe width from properties
                      propMap.value( GEOM::propertyName( GEOM::LineWidth ) ).toInt() );

  // set top-level flag
  AISShape->setTopLevel( propMap.value( GEOM::propertyName( GEOM::TopLevel ) ).toBool() );

  // set point marker (for vertex / compound of vertices only)
  if ( onlyVertex || isVerticesMode ) {
    QStringList aList = propMap.value( GEOM::propertyName( GEOM::PointMarker ) ).toString().split( GEOM::subSectionSeparator() );
    if ( aList.size() == 2 ) {
      // standard marker string contains "TypeOfMarker:ScaleOfMarker"
      int aTypeOfMarker = aList[0].toInt();
      double aScaleOfMarker = (aList[1].toInt() + 1) * 0.5;
      Handle(Prs3d_PointAspect) anAspect = AISShape->Attributes()->PointAspect();
      anAspect->SetScale( aScaleOfMarker );
      anAspect->SetTypeOfMarker( (Aspect_TypeOfMarker)( aTypeOfMarker-1 ) );
      anAspect->SetColor( HasColor() ? 
                          // predefined color, manually set to displayer via GEOM_Displayer::SetColor() function
                          (Quantity_NameOfColor)GetColor() : 
                          // color from properties
                          SalomeApp_Tools::color( propMap.value( GEOM::propertyName( GEOM::PointColor ) ).value<QColor>() ) );
      AISShape->Attributes()->SetPointAspect( anAspect );
    }
    else if ( aList.size() == 1 ) {
      // custom marker string contains "IdOfTexture"
      int textureId = aList[0].toInt();
      Standard_Integer aWidth, aHeight;
      Handle(TColStd_HArray1OfByte) aTexture =
        GeometryGUI::getTexture( study, textureId, aWidth, aHeight );
      if ( !aTexture.IsNull() ) {
        Handle(Prs3d_PointAspect) aTextureAspect =
          new Prs3d_PointAspect( HasColor() ? 
				 // predefined color, manually set to displayer via GEOM_Displayer::SetColor() function
				 (Quantity_NameOfColor)GetColor() : 
				 // color from properties 
                                 SalomeApp_Tools::color( propMap.value( GEOM::propertyName( GEOM::PointColor ) ).value<QColor>() ),
                                 aWidth, aHeight,
                                 aTexture );
        AISShape->Attributes()->SetPointAspect( aTextureAspect );
      }
    }
  }

  // set field step data
  AISShape->setFieldStepInfo( myFieldDataType,
                              myFieldDimension,
                              myFieldStepData,
                              myFieldStepName,
                              myFieldStepRangeMin,
                              myFieldStepRangeMax );

  if ( create && !isTemporary && aMgrId != -1 ) {
    // set properties to the study
    study->setObjectProperties( aMgrId, entry, propMap );
  }

  // AISShape->SetName(???); ??? necessary to set name ???
}

void GEOM_Displayer::updateActorProperties( GEOM_Actor* actor, bool create )
{
  // check that actor is not null
  if ( !actor ) return;
  
  // check that study is active
  SalomeApp_Study* study = getStudy();
  if ( !study ) return;

  // set interactive object

  Handle( SALOME_InteractiveObject ) anIO;

  if ( !myIO.IsNull() ) {
    actor->setIO( myIO );
    anIO = myIO;
  }
  else if ( !myName.empty() ) {
    // workaround to allow selection of temporary objects
    static int tempId = 0;
    anIO = new SALOME_InteractiveObject( QString( "TEMP_VTK_%1" ).arg( tempId++ ).toLatin1().data(), "GEOM", myName.c_str() );
    actor->setIO( anIO );
  }

  // presentation study entry (empty for temporary objects like preview)
  QString entry = !anIO.IsNull() ? QString( anIO->getEntry() ) : QString();
  // flag: temporary object
  bool isTemporary = entry.isEmpty() || entry.startsWith( "TEMP_" );
  // currently active view window's ID (-1 if no active view)
  int aMgrId = !anIO.IsNull() ? getViewManagerId( myViewFrame ) : -1;

  // get presentation properties
  PropMap propMap = getObjectProperties( study, entry, myViewFrame );
  QColor c;

  /////////////////////////////////////////////////////////////////////////
  // VSR: for VTK viewer currently deflection coefficient is hardcoded
  //      due to performance problem
  // actor->SetShape(myShape,aDefPropMap.value(GEOM::propertyName( GEOM::Deflection )).toDouble(),myType == GEOM_VECTOR);
  /////////////////////////////////////////////////////////////////////////
  if ( !actor->getTopo().IsSame( myShape ) )
#ifdef LIMIT_DEFLECTION_FOR_VTK
    actor->SetShape( myShape, VTK_MIN_DEFLECTION, myType == GEOM_VECTOR );
#else
    actor->SetShape( myShape, qMax( propMap.value( GEOM::propertyName( GEOM::Deflection ) ).toDouble(), GEOM::minDeflection() ), myType == GEOM_VECTOR );
#endif

  // set material
  Material_Model material;
  material.fromProperties( propMap.value( GEOM::propertyName( GEOM::Material ) ).toString() );
  std::vector<vtkProperty*> mprops;
  mprops.push_back( material.getMaterialVTKProperty( true ) );
  mprops.push_back( material.getMaterialVTKProperty( false) );
  actor->SetMaterial( mprops );

  // set iso-lines properties

  // - set number of iso-lines
  int nbIsos[2]= { 1, 1 };
  if ( HasNbIsos() ) {
    nbIsos[0] = GetNbIsos();
    nbIsos[1] = GetNbIsos();
  }
  else {
    QStringList isos = propMap.value( GEOM::propertyName( GEOM::NbIsos ) ).toString().split( GEOM::subSectionSeparator() );
    nbIsos[0] = isos[0].toInt();
    nbIsos[1] = isos[1].toInt();
  }
  actor->SetNbIsos( nbIsos );

  // - set iso-lines width
  actor->SetIsosWidth( HasIsosWidth() ? GetIsosWidth() : propMap.value( GEOM::propertyName( GEOM::IsosWidth ) ).toInt() );

  // - set iso-lines color
  if ( HasIsosColor() )
    c = SalomeApp_Tools::color( Quantity_Color((Quantity_NameOfColor)GetIsosColor()) );
  else
    c = propMap.value( GEOM::propertyName( GEOM::IsosColor ) ).value<QColor>();
  actor->SetIsosColor( c.redF(), c.greenF(), c.blueF() );

  // set colors

  if ( HasColor()  ) {
    // - same color for all sub-actors
    Quantity_Color aColor( (Quantity_NameOfColor)GetColor() );
    actor->SetColor( aColor.Red(), aColor.Green(), aColor.Blue() );
  }
  else {
    // shading color (for non-physical materials)
    if ( !material.isPhysical() ) {
      c = propMap.value( GEOM::propertyName( GEOM::ShadingColor ) ).value<QColor>();
      actor->GetFrontMaterial()->SetColor( c.redF(), c.greenF(), c.blueF() );
      actor->GetBackMaterial()->SetColor( c.redF(), c.greenF(), c.blueF() );
    }

    // - standalone edge color
    c = propMap.value( GEOM::propertyName( GEOM::LineColor ) ).value<QColor>();
    actor->SetIsolatedEdgeColor( c.redF(), c.greenF(), c.blueF() );

    c = propMap.value( GEOM::propertyName( GEOM::WireframeColor ) ).value<QColor>();
    // - shared edges color ???
    actor->SetSharedEdgeColor( c.redF(), c.greenF(), c.blueF() );

    c = propMap.value( GEOM::propertyName( GEOM::FreeBndColor ) ).value<QColor>();
    // - free edges color ???
    actor->SetFreeEdgeColor( c.redF(), c.greenF(), c.blueF() );

    // - point color
    c = propMap.value( GEOM::propertyName( GEOM::PointColor ) ).value<QColor>();
    actor->SetPointColor( c.redF(), c.greenF(), c.blueF() );
  }

  // - color for edges in shading+edges mode
  c = propMap.value( GEOM::propertyName( GEOM::OutlineColor ) ).value<QColor>();
  actor->SetEdgesInShadingColor( c.redF(), c.greenF(), c.blueF() );

  // - color of labels (shape name)
  c = colorFromResources( "label_color", QColor( 255, 255, 255 ) );
  actor->SetLabelColor( c.redF(), c.greenF(), c.blueF() );

  // set opacity
  if( HasTransparency() ) {
    actor->SetOpacity( 1.0 - GetTransparency() );
  } else {
    actor->SetOpacity( 1.0 - propMap.value( GEOM::propertyName( GEOM::Transparency ) ).toDouble() );
  }

  // set line width
  actor->SetWidth( HasWidth() ?
                   // predefined line width, manually set to displayer via GEOM_Displayer::SetLineWidth() function 
                   GetWidth() :
                   // libe width from properties
                   propMap.value( GEOM::propertyName( GEOM::LineWidth ) ).toInt() );
  
  // set display vectors flag
  actor->SetVectorMode( propMap.value( GEOM::propertyName( GEOM::EdgesDirection ) ).toBool() );

  // set display vertices flag
  actor->SetVerticesMode( propMap.value( GEOM::propertyName( GEOM::Vertices ) ).toBool() );

  // set display name flag
  actor->SetNameMode( propMap.value( GEOM::propertyName( GEOM::ShowName ) ).toBool() );

  // set display mode
  int displayMode = HasDisplayMode() ? 
    // predefined display mode, manually set to displayer via GEOM_Displayer::SetDisplayMode() function 
    GetDisplayMode() :
    // display mode from properties
    propMap.value( GEOM::propertyName( GEOM::DisplayMode ) ).toInt();

  // specific processing of 'shading with edges' mode, as VTK provides only the following standard display modes:
  // Points - 0, Wireframe - 1, Surface - 2, Insideframe - 3, SurfaceWithEdges - 4
  // GEOM actor allows alternative display modes (see VTKViewer::Representation enum) and enum in GEOM_Actor:
  // eWireframe - 0, eShading - 1, eShadingWithEdges - 3

  if ( displayMode == 2 )
      // this is 'Shading with edges' mode => we have to do the correct mapping to EDisplayMode
      // enum in GEOM_Actor (and further to VTKViewer::Representation enum)
    displayMode++;
  actor->setDisplayMode( displayMode );

  if ( myToActivate )
    actor->PickableOn();
  else
    actor->PickableOff();

  if ( create && !isTemporary && aMgrId != -1 ) {
    // set properties to the study
    study->setObjectProperties( aMgrId, entry, propMap );
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::updateDimensions
 *  Creates or renews dimension presentation for the IO.
 */
//=================================================================
void GEOM_Displayer::updateDimensions( const Handle(SALOME_InteractiveObject)& theIO,
                                       SALOME_OCCPrs* thePrs,
                                       const gp_Ax3& theShapeLCS )
{
  SalomeApp_Study* aStudy = getStudy();
  if ( !aStudy )
  {
    return;
  }

  if ( theIO.IsNull() )
  {
    return;
  }

  SOCC_Prs* anOccPrs = dynamic_cast<SOCC_Prs*>( thePrs );

  AIS_ListOfInteractive aListOfIO;

  anOccPrs->GetObjects( aListOfIO );

  AIS_ListIteratorOfListOfInteractive aIterateIO( aListOfIO );

  // remove outdated presentations of dimensions
  for ( ; aIterateIO.More(); aIterateIO.Next() )
  {
    const Handle(AIS_InteractiveObject)& anIO = aIterateIO.Value();
    if ( !anIO->IsKind( STANDARD_TYPE( AIS_Dimension ) ) )
    {
      continue;
    }

    aListOfIO.Remove( aIterateIO );

    if ( !aIterateIO.More() )
    {
      break;
    }
  }

  // prepare dimension styling
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  QColor  aQColor       = aResMgr->colorValue  ( "Geometry", "dimensions_color", QColor( 0, 255, 0 ) );
  int     aLineWidth    = aResMgr->integerValue( "Geometry", "dimensions_line_width", 1 );
  QFont   aFont         = aResMgr->fontValue   ( "Geometry", "dimensions_font", QFont("Y14.5M-2009", 14) );
  double  anArrowLength = aResMgr->doubleValue ( "Geometry", "dimensions_arrow_length", 5 );
  bool    isUnitsShown  = aResMgr->booleanValue( "Geometry", "dimensions_show_units", false );
  QString aUnitsLength  = aResMgr->stringValue ( "Geometry", "dimensions_length_units", "m" );
  QString aUnitsAngle   = aResMgr->stringValue ( "Geometry", "dimensions_angle_units", "deg" );
  bool    aUseText3d    = aResMgr->booleanValue( "Geometry", "dimensions_use_text3d", false );

  // restore dimension presentation from saved attribute or property data
  AIS_ListOfInteractive aRestoredDimensions;

  QVariant aProperty = aStudy->getObjectProperty( GEOM::sharedPropertiesId(),
                                                  theIO->getEntry(),
                                                  GEOM::propertyName( GEOM::Dimensions ),
                                                  QVariant() );

  GEOMGUI_DimensionProperty aRecords;

  if ( aProperty.isValid() && aProperty.canConvert<GEOMGUI_DimensionProperty>() )
  {
    aRecords = aProperty.value<GEOMGUI_DimensionProperty>();
  }
  else
  {
    aRecords.LoadFromAttribute( getStudy(), theIO->getEntry() );
  }
  
  // create up-to-date dimension presentations
  for ( int aPrsIt = 0; aPrsIt < aRecords.GetNumber(); ++aPrsIt )
  {
    if ( !aRecords.IsVisible( aPrsIt ) )
    {
      continue;
    }

    // init dimension by type
    Handle(AIS_Dimension) aPrs;
    switch( aRecords.GetType( aPrsIt ) )
    {
      case GEOMGUI_DimensionProperty::DimensionType_Length :
      {
        Handle(GEOM_AISLength) aLength = new GEOM_AISLength( aPrsIt );
        aRecords.GetRecord( aPrsIt )->AsLength()->Update( aLength, theShapeLCS );
        aPrs = aLength;
        break;
      }

      case GEOMGUI_DimensionProperty::DimensionType_Diameter :
      {
        Handle(GEOM_AISDiameter) aDiam = new GEOM_AISDiameter( aPrsIt );
        aRecords.GetRecord( aPrsIt )->AsDiameter()->Update( aDiam, theShapeLCS );
        aPrs = aDiam;
        break;
      }

      case GEOMGUI_DimensionProperty::DimensionType_Angle :
      {
        Handle(GEOM_AISAngle) anAng = new GEOM_AISAngle( aPrsIt );
        aRecords.GetRecord( aPrsIt )->AsAngle()->Update( anAng, theShapeLCS );
        aPrs = anAng;
        break;
      }
    }

    aPrs->SetOwner( theIO );

    Quantity_Color aColor( aQColor.redF(), aQColor.greenF(), aQColor.blueF(), Quantity_TOC_RGB );

    Handle(Prs3d_DimensionAspect) aStyle = new Prs3d_DimensionAspect();

    aStyle->SetCommonColor( aColor );
    aStyle->MakeUnitsDisplayed( (Standard_Boolean) isUnitsShown );
    aStyle->MakeText3d( aUseText3d );
    aStyle->MakeTextShaded( Standard_True );
    int fsize = aFont.pixelSize() != -1 ? aFont.pixelSize() : aFont.pointSize();
    aStyle->SetExtensionSize( fsize * 0.5 );
    aStyle->TextAspect()->SetFont( aFont.family().toLatin1().data() );
    aStyle->TextAspect()->SetHeight( fsize );
    aStyle->ArrowAspect()->SetLength( anArrowLength );
    aStyle->LineAspect()->SetWidth( aLineWidth );
    aStyle->SetTextHorizontalPosition( aPrs->DimensionAspect()->TextHorizontalPosition() );
    aStyle->SetTextVerticalPosition( aPrs->DimensionAspect()->TextVerticalPosition() );
    aStyle->SetArrowOrientation( aPrs->DimensionAspect()->ArrowOrientation() );
    aPrs->SetDimensionAspect( aStyle );
    aPrs->SetPolygonOffsets( Aspect_POM_Fill, -1.0, -1.0 );
    aPrs->Attributes()->SetDimLengthDisplayUnits( aUnitsLength.toLatin1().data() );
    aPrs->Attributes()->SetDimAngleDisplayUnits( aUnitsAngle.toLatin1().data() );

    if ( aPrs->IsKind( STANDARD_TYPE(AIS_AngleDimension) ) )
    {
      // show degree symbol for dimension instead of label "deg"
      if ( aUnitsAngle == "deg" )
      {
        aPrs->SetSpecialSymbol(0xB0);
        aPrs->SetDisplaySpecialSymbol( isUnitsShown ? AIS_DSS_After : AIS_DSS_No );
        aStyle->MakeUnitsDisplayed(Standard_False);
      }
      else
      {
        aPrs->SetDisplaySpecialSymbol(AIS_DSS_No);
        aStyle->MakeUnitsDisplayed( (Standard_Boolean) isUnitsShown );
      }
    }
    else
    {
      aStyle->MakeUnitsDisplayed( (Standard_Boolean) isUnitsShown );
    }

    aListOfIO.Append( aPrs );
  }

  // update presentation
  anOccPrs->Clear();

  for ( aIterateIO.Initialize( aListOfIO ); aIterateIO.More(); aIterateIO.Next() )
  {
    anOccPrs->AddObject( aIterateIO.Value() );
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Erase
 *  Calls Erase() method for each object in the given list
 */
//=================================================================
void GEOM_Displayer::Erase( const SALOME_ListIO& theIOList,
                            const bool forced,
                            const bool updateViewer )
{
  SALOME_ListIteratorOfListIO Iter( theIOList );
  for ( ; Iter.More(); Iter.Next() )
    Erase( Iter.Value(), forced, false );

  if ( updateViewer )
    UpdateViewer();
}

//=================================================================
/*!
 *  GEOM_Displayer::Redisplay
 *  Calls Redisplay() method for each object in the given list
 */
//=================================================================
void GEOM_Displayer::Redisplay( const SALOME_ListIO& theIOList,
                                const bool updateViewer,
                                const bool checkActiveViewer )
{
  SALOME_ListIteratorOfListIO Iter( theIOList );
  for ( ; Iter.More(); Iter.Next() )
    Redisplay( Iter.Value(), false, checkActiveViewer );

  if ( updateViewer )
    UpdateViewer();
}

//=================================================================
/*!
 *  GEOM_Displayer::Redisplay
 *  Calls Redisplay() method for each object in the given list
 */
//=================================================================
void GEOM_Displayer::Redisplay( const SALOME_ListIO& theIOList,
                                const bool theUpdateViewer,
                                SALOME_View* theViewFrame )
{
  SALOME_ListIteratorOfListIO anIter( theIOList );
  for ( ; anIter.More(); anIter.Next() )
  {
    Redisplay( anIter.Value(), false, theViewFrame );
  }

  if ( theUpdateViewer )
  {
    UpdateViewer();
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Update
 *  Update OCC presentaion
 *  [ Reimplemented from SALOME_Displayer ]
 */
//=================================================================
void GEOM_Displayer::Update( SALOME_OCCPrs* prs )
{
  SOCC_Prs* occPrs = dynamic_cast<SOCC_Prs*>( prs );
  SalomeApp_Study* study = getStudy();

  if ( !occPrs || myShape.IsNull() || !study )
    return;

  if ( myType == GEOM_MARKER && myShape.ShapeType() == TopAbs_FACE )
  {
    // 
    // specific processing for local coordinate system presentation
    // 

    TopoDS_Face aFace = TopoDS::Face( myShape );
    Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( aFace ) );
    if ( !aPlane.IsNull() )
    {
      gp_Ax3 aPos = aPlane->Pln().Position();
      Handle(Geom_Axis2Placement) aPlc = new Geom_Axis2Placement( aPos.Ax2() );

      Handle(GEOM_AISTrihedron) aTrh;

      if ( occPrs->IsNull() )
      {
        // new presentation is being created
        aTrh = new GEOM_AISTrihedron( aPlc );
        occPrs->AddObject( aTrh );
      }
      else
      {
        // presentation is being updated
        AIS_ListOfInteractive aList;
        occPrs->GetObjects( aList );
        AIS_ListIteratorOfListOfInteractive anIter( aList );
        for ( ; anIter.More() && aTrh.IsNull(); anIter.Next() ) {
          aTrh = Handle(GEOM_AISTrihedron)::DownCast( anIter.Value() );
        }
      }
        
      if ( !aTrh.IsNull() ) {
        // predefined color, manually set to displayer via GEOM_Displayer::SetColor() function
        if ( HasColor() )
          aTrh->SetColor( (Quantity_NameOfColor)GetColor() );
        // predefined line width, manually set to displayer via GEOM_Displayer::SetLineWidth() function 
        if ( HasWidth() )
          aTrh->SetWidth( GetWidth() );
        
        if ( !myIO.IsNull() )
        {
          aTrh->setIO( myIO );
          aTrh->SetOwner( myIO );
        }
        aTrh->SetComponent( aPlc );
        aTrh->SetToUpdate();
      }
      occPrs->SetToActivate( ToActivate() );
    }
  }
  else
  {
    // 
    // processing for usual geometry presentation
    // 

    // if presentation is empty we try to create new one
    if ( occPrs->IsNull() )
    {
      // create presentation (specific for vectors)
      Handle(GEOM_AISShape) AISShape = ( myType == GEOM_VECTOR ) ? new GEOM_AISVector( myShape, "" )
                                                                 : new GEOM_AISShape ( myShape, "" );

      if( myType == GEOM_FIELD_STEP )
        AISShape->SetHilightMode( GEOM_AISShape::CustomHighlight );
      // update shape properties
      updateShapeProperties( AISShape, true );

      // add shape to the presentation
      occPrs->AddObject( AISShape );

      // In accordance with ToActivate() value object will be activated/deactivated
      // when it will be displayed
      occPrs->SetToActivate( ToActivate() );

      if ( AISShape->isTopLevel() && GEOM_AISShape::topLevelDisplayMode() == GEOM_AISShape::TopShowAdditionalWActor ) {
        // 21671: EDF 1829 GEOM : Bring to front selected objects (continuation):

        // create additional wireframe shape
        Handle(GEOM_TopWireframeShape) aWirePrs = new GEOM_TopWireframeShape(myShape);
        aWirePrs->SetWidth(AISShape->Width());
        if ( !myIO.IsNull() ) {
          aWirePrs->setIO( myIO );
          aWirePrs->SetOwner( myIO );
        }

        // add shape to the presentation
        occPrs->AddObject( aWirePrs );
      }
    }
    // if presentation is found -> set again shape for it
    else
    {
      AIS_ListOfInteractive IOList;
      occPrs->GetObjects( IOList );
      AIS_ListIteratorOfListOfInteractive Iter( IOList );
      for ( ; Iter.More(); Iter.Next() )
      {
        Handle(GEOM_AISShape) AISShape = Handle(GEOM_AISShape)::DownCast( Iter.Value() );
        if ( AISShape.IsNull() )
          continue;

        // re-set shape (it might be changed)
        if ( AISShape->Shape() != myShape )
          AISShape->Set( myShape );

        // update shape properties
        updateShapeProperties( AISShape, false );

        // force updating
        AISShape->UpdateSelection();
        AISShape->SetToUpdate();
      }
    }

    updateDimensions( myIO, occPrs, gp_Ax3().Transformed( myShape.Location().Transformation() ) );
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Update
 *  Update VTK presentaion
 *  [ Reimplemented from SALOME_Displayer ]
 */
//=================================================================
void GEOM_Displayer::Update( SALOME_VTKPrs* prs )
{
  SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( prs );
  SalomeApp_Study* study = getStudy();

  if ( !vtkPrs || myShape.IsNull() || !study )
    return;

  if ( myType == GEOM_MARKER && myShape.ShapeType() == TopAbs_FACE )
  {
    // 
    // specific processing for local coordinate system presentation
    // 

    TopoDS_Face aFace = TopoDS::Face( myShape );
    Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( aFace ) );
    if ( !aPlane.IsNull() ) {
      gp_Ax3 aPos = aPlane->Pln().Position();
      Handle(Geom_Axis2Placement) aPlc = new Geom_Axis2Placement( aPos.Ax2() );

      GEOM_VTKTrihedron* aTrh = 0;

      if ( vtkPrs->IsNull() ) {
        // new presentation is being created
        aTrh = GEOM_VTKTrihedron::New();
        vtkPrs->AddObject( aTrh );
      }
      else {
        // presentation is being updated
        vtkActorCollection* actors = vtkPrs->GetObjects();
        if ( actors ) {
          actors->InitTraversal();
          vtkActor* a = actors->GetNextActor();
          while ( a && !aTrh ) {
            aTrh = GEOM_VTKTrihedron::SafeDownCast( a );
            a = actors->GetNextActor();
          }
        }
      }
      
      if ( aTrh ) {
        // predefined color, manually set to displayer via GEOM_Displayer::SetColor() function
        if ( HasColor() ) {
          Quantity_Color aColor( (Quantity_NameOfColor)GetColor() );
          aTrh->SetColor( aColor.Red(), aColor.Green(), aColor.Blue() );
        }
#ifdef VTK_TRIHEDRON_WIDTH
        // 
        // VSR: currently isn't supported
        //
        // predefined line width, manually set to displayer via GEOM_Displayer::SetLineWidth() function 
        if ( HasWidth() )
          aTrh->SetWidth( GetWidth() );
#endif

        if ( !myIO.IsNull() )
          aTrh->setIO( myIO );

        aTrh->SetPlacement( aPlc );
      }
    }
  }
  else
  {
    // 
    // processing for usual geometry presentation
    // 

    // if presentation is empty we try to create new one
    if ( vtkPrs->IsNull() )
    {
      // create an actor
      GEOM_Actor* actor = GEOM_Actor::New();
      // update actor properties
      updateActorProperties( actor, true );
      // add actor to the presentation
      vtkPrs->AddObject( actor );
    }
    else {
      // presentation is being updated
      vtkActorCollection* actors = vtkPrs->GetObjects();
      if ( actors ) {
        actors->InitTraversal();
        vtkActor* a = actors->GetNextActor();
        while ( a ) {
          GEOM_Actor* actor = GEOM_Actor::SafeDownCast( a );
          if ( actor ) {
            // update actor properties
            updateActorProperties( actor, false );
            a = actors->GetNextActor();
          }
        }
      }
    }
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::BuildPrs
 *  Build presentation accordint to the current viewer type
 */
//=================================================================
SALOME_Prs* GEOM_Displayer::BuildPrs( GEOM::GEOM_Object_ptr theObj )
{
  if ( theObj->_is_nil() )
    return 0;

  myViewFrame = GetActiveView();
  if ( myViewFrame == 0 )
    return 0;

  SALOME_Prs* aPrs = myViewFrame->CreatePrs();
  if ( aPrs == 0 )
    return 0;

  internalReset();
  setShape( GEOM_Client::get_client().GetShape( GeometryGUI::GetGeomGen(), theObj ) );
  myType = theObj->GetType();

  // Update presentation
  UpdatePrs( aPrs );

  return aPrs;
}

//=================================================================
/*!
 *  GEOM_Displayer::BuildPrs
 *  Build presentation accordint to the current viewer type
 */
//=================================================================
SALOME_Prs* GEOM_Displayer::BuildPrs( const TopoDS_Shape& theShape )
{
  myViewFrame = GetActiveView();
  if ( theShape.IsNull() || myViewFrame == 0 )
    return 0;

  SALOME_Prs* aPrs = myViewFrame->CreatePrs();
  if ( aPrs == 0 )
    return 0;

  internalReset();
  setShape( theShape );
  myType = -1;

  UpdatePrs( aPrs );

  return aPrs;
}

//=================================================================
/*!
 *  GEOM_Displayer::buildPresentation
 *  Builds/finds object's presentation for the current viewer
 *  Calls corresponding Update() method by means of double dispatch
 *  [ internal ]
 */
//=================================================================
SALOME_Prs* GEOM_Displayer::buildPresentation( const QString& entry,
                                               SALOME_View* theViewFrame )
{
  SALOME_Prs* prs = 0;
  internalReset();

  myViewFrame = theViewFrame ? theViewFrame : GetActiveView();

  if ( myViewFrame )
  {
    prs = LightApp_Displayer::buildPresentation( entry, theViewFrame );
    if ( prs )
    {
      Handle( SALOME_InteractiveObject ) theIO = new SALOME_InteractiveObject();
      theIO->setEntry( entry.toLatin1().constData() );
      if ( !theIO.IsNull() )
      {
        // set interactive object
        setIO( theIO );
        //  Find SOBject (because shape should be published previously)
        SUIT_Session* session = SUIT_Session::session();
        SUIT_Application* app = session->activeApplication();
        if ( app )
        {
          SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
          if ( study )
          {
            _PTR(SObject) SO ( study->studyDS()->FindObjectID( theIO->getEntry() ) );
            if ( SO )
            {
              // get CORBA reference to data object
              CORBA::Object_var object = GeometryGUI::ClientSObjectToObject(SO);
              if ( !CORBA::is_nil( object ) )
              {
                // downcast to GEOM base object
                GEOM::GEOM_BaseObject_var GeomBaseObject = GEOM::GEOM_BaseObject::_narrow( object );
                if ( !GeomBaseObject->_is_nil() )
                {
                  myType = GeomBaseObject->GetType();

                  // downcast to GEOM object
                  GEOM::GEOM_Object_var GeomObject = GEOM::GEOM_Object::_narrow( GeomBaseObject );
                  if ( myType == GEOM_FIELD_STEP )
                  {
                    // get the GEOM object from the field's shape
                    GEOM::GEOM_FieldStep_var GeomFieldStep = GEOM::GEOM_FieldStep::_narrow( GeomBaseObject );
                    if ( !GeomFieldStep->_is_nil() )
                    {
                      GEOM::GEOM_Field_var GeomField = GeomFieldStep->GetField();
                      if ( !GeomField->_is_nil() )
                        GeomObject = GeomField->GetShape();
                    }

                    // read the field step information
                    readFieldStepInfo( GeomFieldStep );
                  }

                  if ( !GeomObject->_is_nil() )
                  {
                    theIO->setName( GeomObject->GetName() );
                    // finally set shape
                    setShape( GEOM_Client::get_client().GetShape( GeometryGUI::GetGeomGen(), GeomObject ) );
                  }
                }
              }
            }
          }
        }
      }
      UpdatePrs( prs );  // Update presentation by using of the double dispatch
    }
  }
  return prs;
}

//=================================================================
/*!
 *  GEOM_Displayer::buildSubshapePresentation
 *  Builds/finds object's presentation for the current viewer
 *  Calls corresponding Update() method by means of double dispatch
 *  For not published objects (for Mantis issue 0020435)
 */
//=================================================================
SALOME_Prs* GEOM_Displayer::buildSubshapePresentation(const TopoDS_Shape& aShape,
                                                      const QString& entry,
                                                      SALOME_View* theViewFrame)
{
  SALOME_Prs* prs = 0;
  internalReset();

  myViewFrame = theViewFrame ? theViewFrame : GetActiveView();

  if (myViewFrame)
  {
    prs = LightApp_Displayer::buildPresentation(entry, theViewFrame);
    if (prs)
    {
      Handle(SALOME_InteractiveObject) theIO = new SALOME_InteractiveObject();
      theIO->setEntry(entry.toLatin1().constData());
      if (!theIO.IsNull())
      {
        // set interactive object
        setIO(theIO);
        // finally set shape
        setShape(aShape);
        myType = GEOM_SUBSHAPE;
      }
      UpdatePrs(prs);  // Update presentation by using of the double dispatch
    }
  }
  return prs;
}

//=================================================================
/*!
 *  GEOM_Displayer::internalReset
 *  Resets internal data
 *  [internal]
 */
//=================================================================
void GEOM_Displayer::internalReset()
{
  myIO.Nullify();
  myShape.Nullify();

  myFieldDataType = GEOM::FDT_Double;
  myFieldDimension = 0;
  myFieldStepData.clear();
  myFieldStepName.Clear();
  myFieldStepRangeMin = 0;
  myFieldStepRangeMax = 0;
}

//=================================================================
/*!
 *  GEOM_Displayer::LocalSelection
 *  Activate selection of CAD shapes with activisation of selection
 *  of their sub-shapes (with opened local context for OCC viewer)
 */
//=================================================================
void GEOM_Displayer::LocalSelection( const Handle(SALOME_InteractiveObject)& theIO, const std::list<int> modes )
{
  SUIT_Session* session = SUIT_Session::session();
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( session->activeApplication() );
  if ( !app )
    return;

  LightApp_SelectionMgr* sm = app->selectionMgr();

  // remove all filters from selection
  sm->clearFilters();

  SALOME_View* vf = GetActiveView();
  if ( vf ) {
    if (!theIO.IsNull() && !vf->isVisible(theIO))
      Display(theIO);
    SALOME_Prs* prs = vf->CreatePrs( theIO.IsNull() ? 0 : theIO->getEntry() );
    vf->LocalSelection( prs, modes );
    delete prs;  // delete presentation because displayer is its owner
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::LocalSelection
 *  Activate selection of CAD shapes with activisation of selection
 *  of their sub-shapes (with opened local context for OCC viewer)
 */
//=================================================================
void GEOM_Displayer::LocalSelection( const Handle(SALOME_InteractiveObject)& theIO, const int theMode )
{
  std::list<int> modes;
  modes.push_back( theMode );
  LocalSelection( theIO, modes );
}

//=================================================================
/*!
 *  GEOM_Displayer::globalSelection
 *  Activate selection of CAD shapes without activisation of selection
 *  of their sub-shapes (without opened local context for OCC viewer)
 */
//=================================================================
void GEOM_Displayer::GlobalSelection( const int theMode, const bool update )
{
  TColStd_MapOfInteger aModes;
  aModes.Add( theMode );
  GlobalSelection( aModes, update );
}

//=================================================================
/*!
 *  GEOM_Displayer::globalSelection
 *  Activate selection of CAD shapes without activisation of selection
 *  of their sub-shapes (without opened local context for OCC viewer)
 */
//=================================================================
void GEOM_Displayer::GlobalSelection( const TColStd_MapOfInteger& theModes,
                                      const bool update, const QList<int>* theSubShapes )
{
  SUIT_Session* session = SUIT_Session::session();
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( session->activeApplication() );
  if ( !app )
    return;

  SALOME_View* vf = GetActiveView();
  if ( vf == 0 )
    return;

  // Close local context
  vf->GlobalSelection( update );

  // Set selection filters in accordance with current mode
  LightApp_SelectionMgr* sm = app->selectionMgr();
  if ( !sm )
    return;

  // Remove from selection temporary objects if necessary
  if ( !theModes.Contains( GEOM_PREVIEW ) )
    clearTemporary( sm );

  //@ aSel->ClearIndex();

  sm->clearFilters();

  // Remove filters from AIS_InteractiveContext
  Handle(AIS_InteractiveContext) ic;
  SOCC_Viewer* viewer = dynamic_cast<SOCC_Viewer*>( vf );
  if ( viewer )
    {
      ic = viewer->getAISContext();
      if ( !ic.IsNull() )
        ic->RemoveFilters();
    }

  if ( theModes.Contains( GEOM_ALLOBJECTS ) )
    return;

  SUIT_SelectionFilter* aFilter;
  if ( theModes.Extent() == 1 )
    {
      int aMode = TColStd_MapIteratorOfMapOfInteger( theModes ).Key();

      if ( aMode == GEOM_COMPOUNDFILTER )
        aFilter = getComplexFilter( theSubShapes );
      else
        aFilter = getFilter( aMode );
    }
  else if ( theModes.Extent() > 1 )
    {
      TColStd_MapOfInteger aTopAbsModes;
      TColStd_MapIteratorOfMapOfInteger anIter( theModes );
      QList<SUIT_SelectionFilter*> aListOfFilters;
      for ( ; anIter.More(); anIter.Next() )
        {
          SUIT_SelectionFilter* aFilter;
          int aMode = anIter.Key();
          if ( aMode == GEOM_COMPOUNDFILTER )
            aFilter = getComplexFilter( theSubShapes );
          else
            aFilter = getFilter( aMode );

          if ( aFilter )
            aListOfFilters.append( aFilter );
        }

      aFilter = new GEOM_LogicalFilter( aListOfFilters, GEOM_LogicalFilter::LO_OR );
    }
  else
    return;

  if ( aFilter )
    {
      sm->installFilter( aFilter );
      if ( !ic.IsNull() )
        {
          Handle(GEOM_OCCFilter) anOCCFilter = new GEOM_OCCFilter( sm );
          ic->AddFilter( anOCCFilter );
        }
    }
}

//=================================================================
/*!
 *  GEOM_Displayer::LocalSelection
 *  Activate selection of CAD shapes with activisation of selection
 *  of their sub-shapes (with opened local context for OCC viewer)
 */
//=================================================================
void GEOM_Displayer::LocalSelection( const SALOME_ListIO& theIOList, const std::list<int> modes )
{
  SALOME_ListIteratorOfListIO Iter( theIOList );
  for ( ; Iter.More(); Iter.Next() )
    LocalSelection( Iter.Value(), modes );
}

//=================================================================
/*!
 *  GEOM_Displayer::LocalSelection
 *  Activate selection of CAD shapes with activisation of selection
 *  of their sub-shapes (with opened local context for OCC viewer)
 */
//=================================================================
void GEOM_Displayer::LocalSelection( const SALOME_ListIO& theIOList, const int theMode )
{
  std::list<int> modes;
  modes.push_back( theMode );
  LocalSelection( theIOList, modes );
}

//=================================================================
/*!
 *  GEOM_Displayer::BeforeDisplay
 *  Called before displaying of pars. Close local context
 *  [ Reimplemented from SALOME_Displayer ]
 */
//=================================================================
void GEOM_Displayer::BeforeDisplay( SALOME_View* v, const SALOME_OCCPrs* )
{
  SOCC_Viewer* vf = dynamic_cast<SOCC_Viewer*>( v );
  if ( vf )
  {
    Handle(AIS_InteractiveContext) ic = vf->getAISContext();
    if ( !ic.IsNull() )
    {
      if ( ic->HasOpenedContext() )
      ic->CloseAllContexts();
    }
  }
}

void GEOM_Displayer::AfterDisplay( SALOME_View* v, const SALOME_OCCPrs* p )
{
  UpdateColorScale(false,false);
}

void GEOM_Displayer::BeforeErase( SALOME_View* v, const SALOME_OCCPrs* p )
{
  LightApp_Displayer::BeforeErase( v, p );
  releaseTextures( p );
}

void GEOM_Displayer::AfterErase( SALOME_View* v, const SALOME_OCCPrs* p )
{
  LightApp_Displayer::AfterErase( v, p );
  UpdateColorScale(false,false);
}

//=================================================================
/*!
 *  GEOM_Displayer::SetColor
 *  Set color for shape displaying. If it is equal -1 then default color is used.
 *  Available values are from Quantity_NameOfColor enumeration
 */
//=================================================================
void GEOM_Displayer::SetColor( const int color )
{
  if ( color == -1 )
    UnsetColor();
  else
  {
    myColor = color;
    myShadingColor = Quantity_Color( (Quantity_NameOfColor)color );
  }
}

int GEOM_Displayer::GetColor() const
{
  return myColor;
}

bool GEOM_Displayer::HasColor() const
{
  return myColor != -1;
}

void GEOM_Displayer::UnsetColor()
{
  myColor = -1;

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  QColor col = resMgr->colorValue( "Geometry", "shading_color", QColor( 255, 0, 0 ) );
  myShadingColor = SalomeApp_Tools::color( col );
}

//=================================================================
/*!
 *  GEOM_Displayer::SetTransparency
 *  Set transparency for shape displaying.
 */
//=================================================================
double GEOM_Displayer::SetTransparency( const double transparency )
{
  double prevTransparency = myTransparency;
  myTransparency = transparency;
  return prevTransparency;
}

//=================================================================
/*!
 *  GEOM_Displayer::GetTransparency
 *  Get transparency for shape displaying.
 */
//=================================================================
double GEOM_Displayer::GetTransparency() const
{
  return myTransparency;
}

//=================================================================
/*!
 *  GEOM_Displayer::HasTransparency
 *  Check if transparency for shape displaying is set.
 */
//=================================================================
bool GEOM_Displayer::HasTransparency() const
{
  return myTransparency >= 0;
}

//=================================================================
/*!
 *  GEOM_Displayer::UnsetTransparency
 *  Unset transparency for shape displaying.
 */
//=================================================================
double GEOM_Displayer::UnsetTransparency()
{
  return SetTransparency( -1 );
}

//=================================================================
/*!
 *  GEOM_Displayer::SetTexture
 */
//=================================================================
void GEOM_Displayer::SetTexture( const std::string& texureFileName )
{
  myTexture = texureFileName;
}

bool GEOM_Displayer::HasTexture() const
{
  return myTexture != "";
}

std::string GEOM_Displayer::GetTexture() const
{
  return myTexture;
}

//=================================================================
/*!
 *  GEOM_Displayer::SetWidth
 *  Set width of shape displaying. If it is equal -1 then default width is used.
 */
//=================================================================
void GEOM_Displayer::SetWidth( const double width )
{
  myWidth = width;
}

double GEOM_Displayer::GetWidth() const
{
  return myWidth;
}

bool GEOM_Displayer::HasWidth() const
{
  return myWidth != -1;
}

void GEOM_Displayer::UnsetWidth()
{
  myWidth = -1;
}

int GEOM_Displayer::GetIsosWidth() const
{
  return myIsosWidth;
}

void GEOM_Displayer::SetIsosWidth(const int width)
{
  myIsosWidth = width;
}

bool GEOM_Displayer::HasIsosWidth() const
{
  return myIsosWidth != -1;
}

int GEOM_Displayer::SetNbIsos( const int nbIsos )
{
  int prevNbIsos = myNbIsos;
  myNbIsos = nbIsos;
  return prevNbIsos;
}

int GEOM_Displayer::UnsetNbIsos()
{
  return SetNbIsos( -1 );
}

int GEOM_Displayer::GetNbIsos() const
{
  return myNbIsos;
}

bool GEOM_Displayer::HasNbIsos() const
{
  return myNbIsos >= 0;
}

int GEOM_Displayer::SetIsosColor( const int color )
{
  int prevColor = myIsosColor;
  myIsosColor = color;
  return prevColor;
}

int GEOM_Displayer::GetIsosColor() const
{
  return myIsosColor;
}

bool GEOM_Displayer::HasIsosColor() const
{
  return myIsosColor != -1;
}

int GEOM_Displayer::UnsetIsosColor()
{
  return SetIsosColor( -1 );
}

//=================================================================
/*!
 *  GEOM_Displayer::SetToActivate
 *  This method is used for activisation/deactivisation of objects to be displayed
 */
//=================================================================
void GEOM_Displayer::SetToActivate( const bool toActivate )
{
  myToActivate = toActivate;
}
bool GEOM_Displayer::ToActivate() const
{
  return myToActivate;
}

//=================================================================
/*!
 *  GEOM_Displayer::clearTemporary
 *  Removes from selection temporary objects
 */
//=================================================================
void GEOM_Displayer::clearTemporary( LightApp_SelectionMgr* theSelMgr )
{
  SALOME_ListIO selected, toSelect;
  theSelMgr->selectedObjects( selected );

  for (  SALOME_ListIteratorOfListIO it( selected ) ; it.More(); it.Next() ) {
    Handle(SALOME_InteractiveObject) io = it.Value();
    if ( !io.IsNull() && io->hasEntry() && strncmp( io->getEntry(), "TEMP_", 5 ) != 0 )
      toSelect.Append( it.Value() );
  }

  theSelMgr->setSelectedObjects( toSelect, true );
}

void GEOM_Displayer::SetName( const char* theName )
{
  myName = theName;
}

void GEOM_Displayer::UnsetName()
{
  myName = "";
}

SalomeApp_Study* GEOM_Displayer::getStudy() const
{
  return dynamic_cast<SalomeApp_Study*>( myApp->activeStudy() );
}

void GEOM_Displayer::setIO( const Handle(SALOME_InteractiveObject)& theIO )
{
  myIO = theIO;
}

void GEOM_Displayer::setShape( const TopoDS_Shape& theShape )
{
  myShape = theShape;
}

void GEOM_Displayer::setFieldStepInfo( const GEOM::field_data_type theFieldDataType,
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

bool GEOM_Displayer::canBeDisplayed( const QString& entry, const QString& viewer_type ) const
{
  _PTR(SObject) anObj = getStudy()->studyDS()->FindObjectID( (const char*)entry.toLatin1() );
  GEOM::GEOM_Object_var aGeomObj = GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(anObj)); // enable displaying of GEOM objects
  GEOM::GEOM_FieldStep_var aFieldStepObj = GEOM::GEOM_FieldStep::_narrow(GeometryGUI::ClientSObjectToObject(anObj)); // enable displaying of GEOM field steps
  GEOM::GEOM_Gen_var aCompObj = GEOM::GEOM_Gen::_narrow(GeometryGUI::ClientSObjectToObject(anObj)); // enable displaying of whole GEOM component
  return ( !CORBA::is_nil( aGeomObj ) || !CORBA::is_nil( aFieldStepObj ) || !CORBA::is_nil( aCompObj ) ) &&
         (viewer_type == SOCC_Viewer::Type() || viewer_type == SVTK_Viewer::Type());
}

int GEOM_Displayer::SetDisplayMode( const int theMode )
{
  int aPrevMode = myDisplayMode;
  if ( theMode != -1 ) {
    myDisplayMode = theMode;
    myHasDisplayMode = true;
  }
  else {
    UnsetDisplayMode();
  }
  return aPrevMode;
}

int GEOM_Displayer::GetDisplayMode() const
{
  return myDisplayMode;
}

int GEOM_Displayer::UnsetDisplayMode()
{
  int aPrevMode = myDisplayMode;
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  myDisplayMode = resMgr->integerValue( "Geometry", "display_mode", 0 );
  myHasDisplayMode = false;
  return aPrevMode;
}

bool GEOM_Displayer::HasDisplayMode() const
{
  return myHasDisplayMode;
}

SALOMEDS::Color GEOM_Displayer::getPredefinedUniqueColor()
{
  static QList<QColor> colors;

  if ( colors.isEmpty() ) {

    for (int s = 0; s < 2 ; s++)
    {
      for (int v = 100; v >= 40; v = v - 20)
      {
        for (int h = 0; h < 359 ; h = h + 60)
        {
          colors.append(QColor::fromHsv(h, 255 - s * 127, v * 255 / 100));
        }
      }
    }
  }

  static int currentColor = randomize( colors.size() );

  SALOMEDS::Color color;
  color.R = (double)colors[currentColor].red()   / 255.0;
  color.G = (double)colors[currentColor].green() / 255.0;
  color.B = (double)colors[currentColor].blue()  / 255.0;

  currentColor = (currentColor+1) % colors.count();

  return color;
}

SALOMEDS::Color GEOM_Displayer::getUniqueColor( const QList<SALOMEDS::Color>& theReservedColors )
{
  int aHue = -1;
  int aTolerance = 64;
  int anIterations = 0;
  int aPeriod = 5;

  while( 1 )
  {
    anIterations++;
    if( anIterations % aPeriod == 0 )
    {
      aTolerance /= 2;
      if( aTolerance < 1 )
        break;
    }
    //std::cout << "Iteration N" << anIterations << " (tolerance=" << aTolerance << ")"<< std::endl;

    aHue = (int)( 360.0 * rand() / RAND_MAX );
    //std::cout << "Hue = " << aHue << std::endl;

    //std::cout << "Auto colors : ";
    bool ok = true;
    QList<SALOMEDS::Color>::const_iterator it = theReservedColors.constBegin();
    QList<SALOMEDS::Color>::const_iterator itEnd = theReservedColors.constEnd();
    for( ; it != itEnd; ++it )
    {
      SALOMEDS::Color anAutoColor = *it;
      QColor aQColor( (int)( anAutoColor.R * 255.0 ), (int)( anAutoColor.G * 255.0 ), (int)( anAutoColor.B * 255.0 ) );

      int h, s, v;
      aQColor.getHsv( &h, &s, &v );
      //std::cout << h << " ";
      if( abs( h - aHue ) < aTolerance )
      {
        ok = false;
        //std::cout << "break (diff = " << abs( h - aHue ) << ")";
        break;
      }
    }
    //std::cout << std::endl;

    if( ok )
      break;
  }

  //std::cout << "Hue of the returned color = " << aHue << std::endl;
  QColor aColor;
  aColor.setHsv( aHue, 255, 255 );

  SALOMEDS::Color aSColor;
  aSColor.R = (double)aColor.red() / 255.0;
  aSColor.G = (double)aColor.green() / 255.0;
  aSColor.B = (double)aColor.blue() / 255.0;

  return aSColor;
}

PropMap GEOM_Displayer::getObjectProperties( SalomeApp_Study* study,
                                             const QString& entry,
                                             SALOME_View* view )
{
  // get default properties for the explicitly specified default view type
  PropMap propMap = GEOM_Displayer::getDefaultPropertyMap();

  if ( study && view ) {
    SUIT_ViewModel* viewModel = dynamic_cast<SUIT_ViewModel*>( view );
    SUIT_ViewManager* viewMgr = ( viewModel != 0 ) ? viewModel->getViewManager() : 0;
    int viewId = ( viewMgr != 0 ) ? viewMgr->getGlobalId() : -1;
  
    if ( viewModel && viewId != -1 ) {
      // get properties from the study
      PropMap storedMap = study->getObjectProperties( viewId, entry );
      // overwrite default properties from stored ones (that are specified)
      for ( int prop = GEOM::Visibility; prop <= GEOM::LastProperty; prop++ ) {
        if ( storedMap.contains( GEOM::propertyName( (GEOM::Property)prop ) ) )
          propMap.insert( GEOM::propertyName( (GEOM::Property)prop ), 
                          storedMap.value( GEOM::propertyName( (GEOM::Property)prop ) ) );
      }
      // ... specific processing for color
      // ... current implementation is to use same stored color for all aspects
      // ... (TODO) possible future improvements about free boundaries, standalone edges etc colors can be here
      if ( storedMap.contains( GEOM::propertyName( GEOM::Color ) ) ) {
        propMap.insert( GEOM::propertyName( GEOM::ShadingColor ),   storedMap.value( GEOM::propertyName( GEOM::Color ) ) );
        propMap.insert( GEOM::propertyName( GEOM::WireframeColor ), storedMap.value( GEOM::propertyName( GEOM::Color ) ) );
        propMap.insert( GEOM::propertyName( GEOM::LineColor ),      storedMap.value( GEOM::propertyName( GEOM::Color ) ) );
        propMap.insert( GEOM::propertyName( GEOM::FreeBndColor ),   storedMap.value( GEOM::propertyName( GEOM::Color ) ) );
        propMap.insert( GEOM::propertyName( GEOM::PointColor ),     storedMap.value( GEOM::propertyName( GEOM::Color ) ) );
      }

      if ( !entry.isEmpty() ) {
        // get CORBA reference to geom object
        _PTR(SObject) SO( study->studyDS()->FindObjectID( entry.toStdString() ) );
        if ( SO ) {
          CORBA::Object_var object = GeometryGUI::ClientSObjectToObject( SO );
          if ( !CORBA::is_nil( object ) ) {
            GEOM::GEOM_Object_var geomObject = GEOM::GEOM_Object::_narrow( object );
            if ( !CORBA::is_nil( geomObject ) ) { // to check
              // check that geom object has color properly set
              bool hasColor = false;
              SALOMEDS::Color aSColor = getColor( geomObject, hasColor );
              // set color from geometry object (only once, if it is not yet set in GUI)
              // current implementation is to use same color for all aspects
              // (TODO) possible future improvements about free boundaries, standalone edges etc colors can be here
              if ( hasColor && !storedMap.contains( GEOM::propertyName( GEOM::Color ) ) ) {
                QColor objColor = QColor::fromRgbF( aSColor.R, aSColor.G, aSColor.B );
                propMap.insert( GEOM::propertyName( GEOM::ShadingColor ),   objColor );
                propMap.insert( GEOM::propertyName( GEOM::WireframeColor ), objColor );
                propMap.insert( GEOM::propertyName( GEOM::LineColor ),      objColor );
                propMap.insert( GEOM::propertyName( GEOM::FreeBndColor ),   objColor );
                propMap.insert( GEOM::propertyName( GEOM::PointColor ),     objColor );
              }
              // check that object has point marker properly set
              GEOM::marker_type mType = geomObject->GetMarkerType();
              GEOM::marker_size mSize = geomObject->GetMarkerSize();
              int mTextureId = geomObject->GetMarkerTexture();
              bool hasMarker = ( mType > GEOM::MT_NONE && mType < GEOM::MT_USER && mSize > GEOM::MS_NONE && mSize <= GEOM::MS_70 ) || 
                               ( mType == GEOM::MT_USER && mTextureId > 0 );
              // set point marker from geometry object (only once, if it is not yet set in GUI)
              if ( hasMarker && !storedMap.contains( GEOM::propertyName( GEOM::PointMarker ) ) ) {
                if ( mType > GEOM::MT_NONE && mType < GEOM::MT_USER ) {
                  // standard type
                  propMap.insert( GEOM::propertyName( GEOM::PointMarker ),
                                  QString( "%1%2%3" ).arg( (int)mType ).arg( GEOM::subSectionSeparator() ).arg( (int)mSize ) );
                }
                else if ( mType == GEOM::MT_USER ) {
                  // custom texture
                  propMap.insert( GEOM::propertyName( GEOM::PointMarker ), QString::number( mTextureId ) );
                }
              }
            }
          }
        }
      }
    }
  }
  return propMap;
}

PropMap GEOM_Displayer::getDefaultPropertyMap()
{
  PropMap propMap;

  // get resource manager
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  // fill in the properties map with default values

  // - visibility (false by default)
  propMap.insert( GEOM::propertyName( GEOM::Visibility ), false );

  // - nb isos (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::NbIsos ),
                  QString( "%1%2%3" ).
                  arg( resMgr->integerValue( "Geometry", "iso_number_u", 1 ) ).
                  arg( GEOM::subSectionSeparator() ).
                  arg( resMgr->integerValue( "Geometry", "iso_number_v", 1 ) ) );

  // - transparency (opacity = 1-transparency)
  propMap.insert( GEOM::propertyName( GEOM::Transparency ),
		  resMgr->integerValue( "Geometry", "transparency", 0 ) / 100. );

  // - display mode (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::DisplayMode ),
                  resMgr->integerValue( "Geometry", "display_mode", 0 ) );

  // - show edges direction flag (false by default)
  propMap.insert( GEOM::propertyName( GEOM::EdgesDirection ), false );

  // - show vertices flag (false by default)
  propMap.insert( GEOM::propertyName( GEOM::Vertices ), false );

  // - show name flag (false by default)
  propMap.insert( GEOM::propertyName( GEOM::ShowName ), false );

  // - shading color (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::ShadingColor ),
                  colorFromResources( "shading_color", QColor( 255, 255, 0 ) ) );

  // - wireframe color (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::WireframeColor ),
                  colorFromResources( "wireframe_color", QColor( 255, 255, 0 ) ) );

  // - standalone edges color (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::LineColor ),
                  colorFromResources( "line_color", QColor( 255, 0, 0 ) ) );

  // - free boundaries color (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::FreeBndColor ),
                  colorFromResources( "free_bound_color", QColor( 0, 255, 0 ) ) );

  // - points color (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::PointColor ),
                  colorFromResources( "point_color", QColor( 255, 255, 0 ) ) );

  // - isos color (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::IsosColor ),
                  colorFromResources( "isos_color", QColor( 200, 200, 200 ) ) );

  // - outlines color (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::OutlineColor ),
                  colorFromResources( "edges_in_shading_color", QColor( 180, 180, 180 ) ) );

  // - deflection coefficient (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::Deflection ),
                  resMgr->doubleValue( "Geometry", "deflection_coeff", 0.001 ) );

  // - material (take default value from preferences)
  Material_Model material;
  material.fromResources( resMgr->stringValue( "Geometry", "material", "Plastic" ) );
  propMap.insert( GEOM::propertyName( GEOM::Material ), material.toProperties() );

  // - edge width (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::LineWidth ),
                  resMgr->integerValue( "Geometry", "edge_width", 1 ) );

  // - isos width (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::IsosWidth ),
                  resMgr->integerValue( "Geometry", "isolines_width", 1 ) );

  // - point marker (take default value from preferences)
  propMap.insert( GEOM::propertyName( GEOM::PointMarker ),
                  QString( "%1%2%3" ).
                  arg( resMgr->integerValue( "Geometry", "type_of_marker", 1 ) + 1 ).
                  arg( GEOM::subSectionSeparator() ).
                  arg( resMgr->integerValue( "Geometry", "marker_scale", 1 ) ) );

  // - top-level flag (false by default)
  propMap.insert( GEOM::propertyName( GEOM::TopLevel ), false );

  return propMap;
}

SALOMEDS::Color GEOM_Displayer::getColor(GEOM::GEOM_Object_var theGeomObject, bool& hasColor) {
  SALOMEDS::Color aSColor;
  hasColor = false;

  SUIT_Session* session = SUIT_Session::session();
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( session->activeApplication() );

  if ( app && !theGeomObject->_is_nil()) {
    SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );

    if ( study ) {
      _PTR(Study) aStudy = study->studyDS();
      aSColor = theGeomObject->GetColor();
      hasColor = aSColor.R >= 0 && aSColor.G >= 0 && aSColor.B >= 0;
      if ( !hasColor ) {
#ifdef GENERAL_AUTOCOLOR // auto-color for all sub-shapes
        bool general_autocolor = true;
#else                    // auto-color for groups only
        bool general_autocolor = false;
#endif                   // GENERAL_AUTOCOLOR
        if ( general_autocolor || theGeomObject->GetType() == GEOM_GROUP ) {
          GEOM::GEOM_Object_var aMainObject = theGeomObject->GetMainShape();
          if ( !CORBA::is_nil( aMainObject ) && aMainObject->GetAutoColor() ) {
#ifdef SIMPLE_AUTOCOLOR  // simplified algorithm for auto-colors
            aSColor = getPredefinedUniqueColor();
            hasColor = true;
#else                    // old algorithm  for auto-colors
            QList<SALOMEDS::Color> aReservedColors;
            CORBA::String_var IOR = app->orb()->object_to_string( aMainObject );
            _PTR(SObject) aMainSObject( aStudy->FindObjectIOR( IOR.in() ) );
            if ( aMainSObject ) {
              _PTR(ChildIterator) it( aStudy->NewChildIterator( aMainSObject ) );
              for ( ; it->More(); it->Next() ) {
                _PTR(SObject) aChildSObject( it->Value() );
                GEOM::GEOM_Object_var aChildObject =
                  GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aChildSObject));
                if ( CORBA::is_nil( aChildObject ) )
                  continue;

                SALOMEDS::Color aReservedColor = aChildObject->GetColor();
                if ( aReservedColor.R >= 0 && aReservedColor.G >= 0 && aReservedColor.B >= 0 )
                  aReservedColors.append( aReservedColor );
              }
            }
            aSColor = getUniqueColor( aReservedColors );
            hasColor = true;
#endif                   // SIMPLE_AUTOCOLOR
          }
        }
      }
    }
  }
  return aSColor;
}


void GEOM_Displayer::EraseWithChildren(const Handle(SALOME_InteractiveObject)& theIO,
                                       const bool eraseOnlyChildren) {
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app )
    return;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !appStudy )
    return;

  LightApp_DataObject* parent = appStudy->findObjectByEntry(theIO->getEntry());

  if( !parent)
    return;

  // Erase from all views
  QList<SALOME_View*> views;
  SALOME_View* view;
  ViewManagerList vmans = app->viewManagers();
  SUIT_ViewManager* vman;
  foreach ( vman, vmans ) {
    SUIT_ViewModel* vmod = vman->getViewModel();
    view = dynamic_cast<SALOME_View*> ( vmod );
    if ( view )
      views.append( view );
  }

  if( views.count() == 0 )
    return;

  //Erase childrens w/o update views
  DataObjectList listObj = parent->children( true );
  SUIT_DataObject* obj;
  foreach( obj, listObj ) {
    LightApp_DataObject* l_obj = dynamic_cast<LightApp_DataObject*>(obj);
    if(l_obj)
      foreach ( view, views ) {
      Handle(SALOME_InteractiveObject) anIO =
        new SALOME_InteractiveObject(qPrintable(l_obj->entry()), "GEOM", "");
      Erase(anIO, false, false, view);
    }
  }

  //Erase parent with view update or repaint views
  foreach ( view, views ) {
    if(!eraseOnlyChildren)
      Erase(theIO, false, true, view);
    else
      view->Repaint();
  }
}

void GEOM_Displayer::readFieldStepInfo( GEOM::GEOM_FieldStep_var theGeomFieldStep )
{
  if( theGeomFieldStep->_is_nil() )
    return;

  GEOM::GEOM_Field_var aGeomField = theGeomFieldStep->GetField();
  if( aGeomField->_is_nil() )
    return;

  GEOM::GEOM_Object_var aGeomFieldShape = aGeomField->GetShape();
  if( aGeomFieldShape->_is_nil() )
    return;

  TCollection_AsciiString aFieldStepName( theGeomFieldStep->GetName() );
  TCollection_AsciiString aFieldName( aGeomField->GetName() );
  TCollection_AsciiString aShapeName( aGeomFieldShape->GetName() );

  aFieldStepName = aShapeName + "\n" + aFieldName + "\n" + aFieldStepName;

  GEOM::field_data_type aFieldDataType = aGeomField->GetDataType();

  int aFieldDimension = aGeomField->GetDimension();

  GEOM::string_array_var aFieldComponents = aGeomField->GetComponents();
  int aFieldNbComponents = aFieldComponents->length();

  QList<QVariant> aFieldStepData;
  if( aFieldDataType == GEOM::FDT_Bool )
  {
    GEOM::GEOM_BoolFieldStep_var aGeomBoolFieldStep = GEOM::GEOM_BoolFieldStep::_narrow( theGeomFieldStep );
    if ( !aGeomBoolFieldStep->_is_nil() )
    {
      GEOM::short_array_var aValues = aGeomBoolFieldStep->GetValues();
      for( size_t i = 0, n = aValues->length(); i < n; i++ )
        aFieldStepData << (bool)aValues[i];
    }
  }
  else if( aFieldDataType == GEOM::FDT_Int )
  {
    GEOM::GEOM_IntFieldStep_var aGeomIntFieldStep = GEOM::GEOM_IntFieldStep::_narrow( theGeomFieldStep );
    if ( !aGeomIntFieldStep->_is_nil() )
    {
      GEOM::ListOfLong_var aValues = aGeomIntFieldStep->GetValues();
      for( size_t i = 0, n = aValues->length(); i < n; i++ )
        aFieldStepData << (qlonglong)aValues[i];
    }
  }
  else if( aFieldDataType == GEOM::FDT_Double )
  {
    GEOM::GEOM_DoubleFieldStep_var aGeomDoubleFieldStep = GEOM::GEOM_DoubleFieldStep::_narrow( theGeomFieldStep );
    if ( !aGeomDoubleFieldStep->_is_nil() )
    {
      GEOM::ListOfDouble_var aValues = aGeomDoubleFieldStep->GetValues();
      for( size_t i = 0, n = aValues->length(); i < n; i++ )
        aFieldStepData << (double)aValues[i];
    }
  }
  else if( aFieldDataType == GEOM::FDT_String )
  {
    GEOM::GEOM_StringFieldStep_var aGeomStringFieldStep = GEOM::GEOM_StringFieldStep::_narrow( theGeomFieldStep );
    if ( !aGeomStringFieldStep->_is_nil() )
    {
      GEOM::string_array_var aValues = aGeomStringFieldStep->GetValues();
      for( size_t i = 0, n = aValues->length(); i < n; i++ )
        aFieldStepData << QString( aValues[i] );
    }
  }

  double aFieldStepRangeMin = 0, aFieldStepRangeMax = 0;
  aFieldStepData = groupFieldData( aFieldStepData,
                                   aFieldNbComponents,
                                   aFieldDataType == GEOM::FDT_String,
                                   aFieldStepRangeMin,
                                   aFieldStepRangeMax );

  setFieldStepInfo( aFieldDataType,
                    aFieldDimension,
                    aFieldStepData,
                    aFieldStepName,
                    aFieldStepRangeMin,
                    aFieldStepRangeMax );
}

QList<QVariant> GEOM_Displayer::groupFieldData( const QList<QVariant>& theFieldStepData,
                                                const int theFieldNbComponents,
                                                const bool theIsString,
                                                double& theFieldStepRangeMin,
                                                double& theFieldStepRangeMax )
{
  QList<QVariant> aResultList;
  theFieldStepRangeMin = 0;
  theFieldStepRangeMax = 0;

  if( theFieldStepData.isEmpty() || theFieldNbComponents < 1 )
    return aResultList;

  int aNbSubShapes = theFieldStepData.count() / theFieldNbComponents;

  QList<QVariant> aGroupedList;

  bool anIsBoolean = false;
  for( int aSubShape = 0; aSubShape < aNbSubShapes; aSubShape++ )
  {
    double aNorm = 0;
    QStringList aStringList;

    int aBaseIndex = aSubShape * theFieldNbComponents;
    for( int aComponent = 0; aComponent < theFieldNbComponents; aComponent++ )
    {
      int anIndex = aComponent + aBaseIndex;

      const QVariant& aVariant = theFieldStepData[ anIndex ];
      if( theIsString )
      {
        if( aVariant.type() == QVariant::String )
          aStringList << aVariant.toString();
      }
      else
      {
        double aValue = 0;
        if( aVariant.type() == QVariant::Bool )
        {
          aValue = aVariant.toBool() ? 1.0 : 0.0;
          aNorm += aValue;
          anIsBoolean = true;
        }
        else
        {
          if( aVariant.type() == QVariant::LongLong )
            aValue = double( aVariant.toLongLong() );
          else if( aVariant.type() == QVariant::Double )
            aValue = aVariant.toDouble();
          aNorm += aValue * aValue;
        }
      }
    }

    if( theIsString )
      aGroupedList << aStringList.join( "\n" );
    else
    {
      if( anIsBoolean )
        aNorm /= theFieldNbComponents;
      else
        aNorm = pow( aNorm, 0.5 );

      if( aGroupedList.isEmpty() )
        theFieldStepRangeMin = theFieldStepRangeMax = aNorm;
      else
      {
        theFieldStepRangeMin = Min( theFieldStepRangeMin, aNorm );
        theFieldStepRangeMax = Max( theFieldStepRangeMax, aNorm );
      }

      aGroupedList << aNorm;
    }
  }

  if( anIsBoolean )
  {
    theFieldStepRangeMin = 0.0;
    theFieldStepRangeMax = 1.0;
  }

  SUIT_Session* session = SUIT_Session::session();
  SUIT_ResourceMgr* resMgr = session->resourceMgr();
  Standard_Integer aNbIntervals = resMgr->integerValue( "Geometry", "scalar_bar_nb_intervals", 20 );

  QListIterator<QVariant> anIter( aGroupedList );
  while( anIter.hasNext() )
  {
    const QVariant& aVariant = anIter.next();
    if( theIsString )
      aResultList << aVariant;
    else
    {
      QColor aQColor;
      Quantity_Color aColor;
      if( FindColor( aVariant.toDouble(), theFieldStepRangeMin, theFieldStepRangeMax, anIsBoolean ? 2 : aNbIntervals, aColor ) )
        aQColor = QColor::fromRgbF( aColor.Red(), aColor.Green(), aColor.Blue() );
      aResultList << aQColor;
    }
  }
  return aResultList;
}

// Note: the method is copied from AIS_ColorScale class
Standard_Integer GEOM_Displayer::HueFromValue( const Standard_Integer aValue,
                                               const Standard_Integer aMin,
                                               const Standard_Integer aMax )
{
  Standard_Integer minLimit( 0 ), maxLimit( 230 );

  Standard_Integer aHue = maxLimit;
  if ( aMin != aMax )
    aHue = (Standard_Integer)( maxLimit - ( maxLimit - minLimit ) * ( aValue - aMin ) / ( aMax - aMin ) );

  aHue = Min( Max( minLimit, aHue ), maxLimit );

  return aHue;
}

// Note: the method is copied from AIS_ColorScale class
Standard_Boolean GEOM_Displayer::FindColor( const Standard_Real aValue, 
                                            const Standard_Real aMin,
                                            const Standard_Real aMax,
                                            const Standard_Integer ColorsCount,
                                            Quantity_Color& aColor )
{
  if( aValue<aMin || aValue>aMax || aMax<aMin )
    return Standard_False;

  else
  {
    Standard_Real IntervNumber = 0;
    if( aValue<aMin )
      IntervNumber = 0;
    else if( aValue>aMax )
      IntervNumber = ColorsCount-1;
    else if( Abs( aMax-aMin ) > Precision::Approximation() )
      IntervNumber = Floor( Standard_Real( ColorsCount ) * ( aValue - aMin ) / ( aMax - aMin ) ); // 'Ceiling' replaced with 'Floor'

    Standard_Integer Interv = Standard_Integer( IntervNumber );

    aColor = Quantity_Color( HueFromValue( Interv, 0, ColorsCount - 1 ), 1.0, 1.0, Quantity_TOC_HLS );

    return Standard_True;
  } 
}

void GEOM_Displayer::UpdateColorScale( const bool theIsRedisplayFieldSteps, const bool updateViewer ) 
{
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( myApp->activeStudy() );
  if( !aStudy )
    return;

  SOCC_Viewer* aViewModel = dynamic_cast<SOCC_Viewer*>( GetActiveView() );
  if( !aViewModel )
    return;

#if OCC_VERSION_MAJOR < 7
  Handle(V3d_Viewer) aViewer = aViewModel->getViewer3d();
  if( aViewer.IsNull() )
    return;

  aViewer->InitActiveViews();
  if( !aViewer->MoreActiveViews() )
    return;

  Handle(V3d_View) aView = aViewer->ActiveView();
  if( aView.IsNull() )
    return;
#endif

  Standard_Boolean anIsDisplayColorScale = Standard_False;
  TCollection_AsciiString aColorScaleTitle;
  Standard_Real aColorScaleMin = 0, aColorScaleMax = 0;
  Standard_Boolean anIsBoolean = Standard_False;

  SALOME_ListIO aSelectedObjects;
  myApp->selectionMgr()->selectedObjects( aSelectedObjects );
  if( aSelectedObjects.Extent() == 1 )
  {
    Handle(SALOME_InteractiveObject) anIO = aSelectedObjects.First();
    if( !anIO.IsNull() )
    {
      SOCC_Prs* aPrs = dynamic_cast<SOCC_Prs*>( aViewModel->CreatePrs( anIO->getEntry() ) );
      if( aPrs )
      {
        AIS_ListOfInteractive aList;
        aPrs->GetObjects( aList );
        AIS_ListIteratorOfListOfInteractive anIter( aList );
        for( ; anIter.More(); anIter.Next() )
        {
          Handle(GEOM_AISShape) aShape = Handle(GEOM_AISShape)::DownCast( anIter.Value() );
          if( !aShape.IsNull() )
          {
            GEOM::field_data_type aFieldDataType;
            int aFieldDimension;
            QList<QVariant> aFieldStepData;
            TCollection_AsciiString aFieldStepName;
            double aFieldStepRangeMin, aFieldStepRangeMax;
            aShape->getFieldStepInfo( aFieldDataType,
                                      aFieldDimension,
                                      aFieldStepData,
                                      aFieldStepName,
                                      aFieldStepRangeMin,
                                      aFieldStepRangeMax );
            if( !aFieldStepData.isEmpty() && aFieldDataType != GEOM::FDT_String )
            {
              anIsDisplayColorScale = Standard_True;
              aColorScaleTitle = aFieldStepName;
              aColorScaleMin = aFieldStepRangeMin;
              aColorScaleMax = aFieldStepRangeMax;
              anIsBoolean = aFieldDataType == GEOM::FDT_Bool;
            }
          }
        }
      }
    }
  }

  if( anIsDisplayColorScale )
  {
    SUIT_Session* session = SUIT_Session::session();
    SUIT_ResourceMgr* resMgr = session->resourceMgr();

    Standard_Real anXPos = resMgr->doubleValue( "Geometry", "scalar_bar_x_position", 0.05 );
    Standard_Real anYPos = resMgr->doubleValue( "Geometry", "scalar_bar_y_position", 0.1 );
    Standard_Real aWidth = resMgr->doubleValue( "Geometry", "scalar_bar_width", 0.2 );
    Standard_Real aHeight = resMgr->doubleValue( "Geometry", "scalar_bar_height", 0.5 );
    Standard_Integer aTextHeight = resMgr->integerValue( "Geometry", "scalar_bar_text_height", 14 );
    Standard_Integer aNbIntervals = resMgr->integerValue( "Geometry", "scalar_bar_nb_intervals", 20 );

#if OCC_VERSION_MAJOR < 7
    Handle(Aspect_ColorScale) myColorScale = aView->ColorScale();
    if( !myColorScale.IsNull() )
    {
#endif

    myColorScale->SetXPosition( anXPos );
    myColorScale->SetYPosition( anYPos );
    myColorScale->SetWidth( aWidth );
    myColorScale->SetHeight( aHeight );

    myColorScale->SetTextHeight( aTextHeight );
    myColorScale->SetNumberOfIntervals( anIsBoolean ? 2 : aNbIntervals );

    myColorScale->SetTitle( aColorScaleTitle );
    myColorScale->SetRange( aColorScaleMin, aColorScaleMax );

#if OCC_VERSION_MAJOR < 7
    }
    if( !aView->ColorScaleIsDisplayed() )
      aView->ColorScaleDisplay();
  }
  else
    if( aView->ColorScaleIsDisplayed() )
      aView->ColorScaleErase();
#else
    if( !aViewModel->getAISContext()->IsDisplayed( myColorScale ) )
      aViewModel->getAISContext()->Display( myColorScale );
  }
  else
    if( aViewModel->getAISContext()->IsDisplayed( myColorScale ) )
      aViewModel->getAISContext()->Erase( myColorScale );
#endif

  if( theIsRedisplayFieldSteps )
  {
    _PTR(Study) aStudyDS = aStudy->studyDS();
    QList<SUIT_ViewManager*> vmList;
    myApp->viewManagers( vmList );
    for( QList<SUIT_ViewManager*>::Iterator vmIt = vmList.begin(); vmIt != vmList.end(); vmIt++ )
    {
      if( SUIT_ViewManager* aViewManager = *vmIt )
      {
        const ObjMap& anObjects = aStudy->getObjectProperties( aViewManager->getGlobalId() );
        for( ObjMap::ConstIterator objIt = anObjects.begin(); objIt != anObjects.end(); objIt++ )
        {
          _PTR(SObject) aSObj( aStudyDS->FindObjectID( objIt.key().toLatin1().constData() ) );
          if( aSObj )
          {
            CORBA::Object_var anObject = GeometryGUI::ClientSObjectToObject( aSObj );
            if( !CORBA::is_nil( anObject ) )
            {
              GEOM::GEOM_FieldStep_var aFieldStep = GEOM::GEOM_FieldStep::_narrow( anObject );
              if( !aFieldStep->_is_nil() )
              {
                CORBA::String_var aStepEntry = aFieldStep->GetStudyEntry();
                Handle(SALOME_InteractiveObject) aStepIO =
                  new SALOME_InteractiveObject( aStepEntry.in(), "GEOM", "TEMP_IO" );
                Redisplay( aStepIO, false, false );
              }
            }
          }
        }
      }
    }
  }
  if(updateViewer)
    UpdateViewer();
}
