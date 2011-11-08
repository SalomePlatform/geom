// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : GEOM_Displayer.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "GEOM_Displayer.h"

#include "GeometryGUI.h"

#include <GEOM_TypeFilter.h>
#include <GEOM_EdgeFilter.h>
#include <GEOM_FaceFilter.h>
#include <GEOM_CompoundFilter.h>
#include <GEOM_PreviewFilter.h>
#include <GEOM_LogicalFilter.h>
#include <GEOM_OCCFilter.h>

#include <GEOM_Actor.h>
#include <GEOM_AISShape.hxx>
#include <GEOM_AISVector.hxx>
#include <GEOM_AISTrihedron.hxx>
#include <GEOM_VTKTrihedron.hxx>

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

#include <SALOME_ListIteratorOfListIO.hxx>
#include <SALOME_ListIO.hxx>
#include <SALOME_Prs.h>

#include <SOCC_Prs.h>
#include <SOCC_ViewModel.h>

#include <SVTK_Prs.h>
#include <SVTK_ViewModel.h>

// OCCT Includes
#include <AIS_Drawer.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <StdSelect_TypeOfEdge.hxx>
#include <StdSelect_TypeOfFace.hxx>
#include <StdSelect_DisplayMode.hxx>
#include <TopoDS_Face.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Axis2Placement.hxx>
#include <gp_Pln.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TopoDS_Iterator.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS.hxx>

#include <Prs3d_ShadingAspect.hxx>

#include <BRepMesh_IncrementalMesh.hxx>

// VTK Includes
#include <vtkActorCollection.h>
#include <vtkProperty.h>

// CORBA Headers
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)

#include <GEOMImpl_Types.hxx>

#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
#include <TColStd_HArray1OfByte.hxx>
#else
#include <Graphic3d_HArray1OfBytes.hxx>
#endif

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
static std::string getEntry( GEOM::GEOM_Object_ptr object )
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
static std::string getName( GEOM::GEOM_Object_ptr object )
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
  int aType = resMgr->integerValue("Geometry", "type_of_marker", (int)Aspect_TOM_PLUS);
  myTypeOfMarker = (Aspect_TypeOfMarker)(std::min((int)Aspect_TOM_RING3, std::max((int)Aspect_TOM_POINT, aType)));
  myScaleOfMarker = (resMgr->integerValue("Geometry", "marker_scale", 1)-(int)GEOM::MS_10)*0.5 + 1.0;
  myScaleOfMarker = std::min(7.0, std::max(1., myScaleOfMarker));

  myColor = -1;
  // This color is used for shape displaying. If it is equal -1 then
  // default color is used.
  myTexture = "";

  myWidth = -1;
  myType = -1;

  myToActivate = true;
  // This parameter is used for activisation/deactivisation of objects to be displayed
  
  #if OCC_VERSION_LARGE > 0x06050100 // Functionnality available only in OCCT 6.5.2
  // Activate parallel vizualisation only for testing purpose  
  // and if the corresponding env variable is set to 1 
  char* parallel_visu = getenv("PARALLEL_VISU");
  if (parallel_visu && atoi(parallel_visu))
  {
    MESSAGE("Parallel visualisation on");
    BRepMesh_IncrementalMesh::SetParallelDefault(Standard_True);
  }
  #endif

  myViewFrame = 0;
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
      vf->Display( prs );
      vf->AfterDisplay( this, prs );

      if ( updateViewer )
        vf->Repaint();

      int aMgrId = getViewManagerId(vf);
      SalomeApp_Study* aStudy = getStudy();
      aStudy->setObjectProperty(aMgrId, theIO->getEntry(), VISIBILITY_PROP, 1 );

      setVisibilityState(theIO->getEntry(), Qtx::ShownState);

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
void GEOM_Displayer::Display( GEOM::GEOM_Object_ptr theObj, const bool updateViewer )
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
      vf->Erase( prs, forced );
      vf->AfterErase( this, prs );
      if ( updateViewer )
        vf->Repaint();
      delete prs;  // delete presentation because displayer is its owner

      int aMgrId = getViewManagerId(vf);
      SalomeApp_Study* aStudy = getStudy();
      aStudy->setObjectProperty(aMgrId, theIO->getEntry(), VISIBILITY_PROP, 0 );

      setVisibilityState(theIO->getEntry(), Qtx::HiddenState);
    }
  }
}

//=================================================================
/*!
 *  GEOM_Displayer::Erase
 *  Erase geometry object in the current viewer
 */
//=================================================================
void GEOM_Displayer::Erase( GEOM::GEOM_Object_ptr theObj,
                            const bool forced,
                            const bool updateViewer )
{
  std::string entry = getEntry( theObj );
  if ( entry != "" )
  {
    Erase(new SALOME_InteractiveObject(entry.c_str(), "GEOM", getName(theObj).c_str()),
          forced, updateViewer);
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
                                const bool updateViewer )
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
            if ( view->isVisible( theIO ) || view == GetActiveView() )
            {
              Erase( theIO, true, false, view );
              Display( theIO, updateViewer, view );
            }
          }
        }
      }
    }
  }
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
void GEOM_Displayer::Redisplay( const SALOME_ListIO& theIOList, const bool updateViewer )
{
  SALOME_ListIteratorOfListIO Iter( theIOList );
  for ( ; Iter.More(); Iter.Next() )
    Redisplay( Iter.Value(), false );

  if ( updateViewer )
    UpdateViewer();
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
  if ( !occPrs )
    return;

  if ( myType == GEOM_MARKER && !myShape.IsNull() && myShape.ShapeType() == TopAbs_FACE )
  {
    TopoDS_Face aFace = TopoDS::Face( myShape );
    Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( aFace ) );
    if ( !aPlane.IsNull() )
    {
      gp_Ax3 aPos = aPlane->Pln().Position();
      Handle(Geom_Axis2Placement) aPlc = new Geom_Axis2Placement( aPos.Ax2() );

      Handle(GEOM_AISTrihedron) aTrh;

      if ( occPrs->IsNull() )
      {
        aTrh = new GEOM_AISTrihedron( aPlc );

        if ( HasColor() )
          aTrh->SetColor( (Quantity_NameOfColor)GetColor() );

        if ( HasWidth() )
          aTrh->SetWidth( GetWidth() );

        if ( !myIO.IsNull() )
        {
          aTrh->setIO( myIO );
          aTrh->SetOwner( myIO );
        }

        occPrs->AddObject( aTrh );
      }
      else
      {
        AIS_ListOfInteractive aList;
        occPrs->GetObjects( aList );
        AIS_ListIteratorOfListOfInteractive anIter( aList );
        for ( ; anIter.More(); anIter.Next() )
        {
          aTrh = Handle(GEOM_AISTrihedron)::DownCast( anIter.Value() );
          if ( !aTrh.IsNull() )
          {
            aTrh->SetComponent( aPlc );
            aTrh->SetToUpdate();
          }
        }
      }

      occPrs->SetToActivate( ToActivate() );
    }
  }
  else
  {
    // if presentation is empty we try to create new one
    if ( occPrs->IsNull() )
    {
      SalomeApp_Study* aStudy = getStudy();
      if(!aStudy)
        return;
      if ( !myShape.IsNull() ) {

        bool onlyVertex = (myShape.ShapeType() == TopAbs_VERTEX || isCompoundOfVertices( myShape ));

        QString anEntry;
        int aMgrId = -1;
        if(!myIO.IsNull()) {
          aMgrId = getViewManagerId(myViewFrame);
          anEntry = myIO->getEntry();
        }
        bool useStudy = !anEntry.isEmpty() && aMgrId != -1;
        bool useObjColor = false;
        bool useObjMarker = false;

        PropMap aPropMap;
        PropMap aDefPropMap;

        if(useStudy){
          aPropMap = aStudy->getObjectPropMap(aMgrId,anEntry);
          aDefPropMap = getDefaultPropertyMap(SOCC_Viewer::Type());
          MergePropertyMaps(aPropMap, aDefPropMap);
        }

        //Handle(GEOM_AISShape) AISShape = new GEOM_AISShape( myShape, "" );
        Handle(GEOM_AISShape) AISShape;
        if (myType == GEOM_VECTOR)
          AISShape = new GEOM_AISVector (myShape, "");
        else {
          if (myShape.ShapeType() != TopAbs_VERTEX && // fix pb with not displayed points
              !TopoDS_Iterator(myShape).More())
            return;// NPAL15983 (Bug when displaying empty groups)
          AISShape = new GEOM_AISShape (myShape, "");
        }
        // Temporary staff: vertex must be infinite for correct visualization
        AISShape->SetInfiniteState( myShape.Infinite() ); // || myShape.ShapeType() == TopAbs_VERTEX // VSR: 05/04/2010: Fix 20668 (Fit All for points & lines)

        // Setup shape properties here ..., e.g. display mode, color, transparency, etc
        if(useStudy) {
          AISShape->SetDisplayMode( aPropMap.value(DISPLAY_MODE_PROP).toInt() );
          AISShape->SetDisplayVectors(aPropMap.value(VECTOR_MODE_PROP).toInt());

          //Color property
          if(aPropMap.contains(COLOR_PROP)) {
            Quantity_Color  quant_col = SalomeApp_Tools::color( aPropMap.value(COLOR_PROP).value<QColor>());
            AISShape->SetShadingColor( quant_col );
          } else
            useObjColor = true;
        }else {
          MESSAGE("myDisplayMode = "<< myDisplayMode)
          AISShape->SetDisplayMode( myDisplayMode );
          AISShape->SetShadingColor( myShadingColor );
        }

        // Set color and number for iso lines
        SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
        QColor col = aResMgr->colorValue( "Geometry", "isos_color",
                                          QColor(int(0.5*255), int(0.5*255), int(0.5*255)) );
        Quantity_Color aColor = SalomeApp_Tools::color( col );

        //get the ISOS number, set transparency if need
        int anUIsoNumber, aVIsoNumber;
        if(useStudy) {
          QString anIsos = aPropMap.value(ISOS_PROP).toString();
          QStringList uv =  anIsos.split(DIGIT_SEPARATOR);
          anUIsoNumber = uv[0].toInt();
          aVIsoNumber = uv[1].toInt();
          //AISShape->SetTransparency(aPropMap.value(TRANSPARENCY_PROP).toDouble());
        } else {
          anUIsoNumber = aResMgr->integerValue("OCCViewer", "iso_number_u", 1);
          aVIsoNumber  = aResMgr->integerValue("OCCViewer", "iso_number_v", 1);
        }

        Handle(Prs3d_IsoAspect) anAspect = AISShape->Attributes()->UIsoAspect();
        anAspect->SetNumber( anUIsoNumber );
        anAspect->SetColor( aColor );
        AISShape->Attributes()->SetUIsoAspect( anAspect );

        anAspect = AISShape->Attributes()->VIsoAspect();
        anAspect->SetNumber( aVIsoNumber );
        anAspect->SetColor( aColor );
        AISShape->Attributes()->SetVIsoAspect( anAspect );

        if ( HasColor() )
        {
          AISShape->SetColor( (Quantity_NameOfColor)GetColor() );
          if ( onlyVertex )
          {
            if(aPropMap.contains(MARKER_TYPE_PROP)) {
              QStringList aList = aPropMap.value(MARKER_TYPE_PROP).toString().split(DIGIT_SEPARATOR);
              if(aList.size() == 2) { //Standard marker string contains "TypeOfMarker:ScaleOfMarker"
                Handle(Prs3d_PointAspect) anAspect = AISShape->Attributes()->PointAspect();
                int  aTypeOfMarker = aList[0].toInt();
                double  aScaleOfMarker = aList[1].toDouble();
                anAspect->SetScale( aScaleOfMarker );
                anAspect->SetTypeOfMarker((Aspect_TypeOfMarker) (aTypeOfMarker-1));
                anAspect->SetColor((Quantity_NameOfColor)GetColor());
                AISShape->Attributes()->SetPointAspect(anAspect);
              } else { //Custom marker string contains "IdOfTexsture"
                int textureId = aList[0].toInt();
                Standard_Integer aWidth, aHeight;
#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
                Handle(TColStd_HArray1OfByte) aTexture =
#else
                Handle(Graphic3d_HArray1OfBytes) aTexture =
#endif
                  GeometryGUI::getTexture(aStudy, textureId, aWidth, aHeight);
                if (!aTexture.IsNull()) {
                  static int TextureId = 0;
                  Handle(Prs3d_PointAspect) aTextureAspect =
                    new Prs3d_PointAspect ((Quantity_NameOfColor)GetColor(),
                                           ++TextureId,
                                           aWidth, aHeight,
                                           aTexture);
                  AISShape->Attributes()->SetPointAspect(aTextureAspect);
                } else {
                  useObjMarker = true;
                }
              }
            } else {
              useObjMarker = true;
            }
          }
        }
        
        else
        {
          if ( onlyVertex )
          {
            col = aResMgr->colorValue( "Geometry", "point_color", QColor( 255, 255, 0 ) );
            aColor = SalomeApp_Tools::color( col );

            if(aPropMap.contains(MARKER_TYPE_PROP)) {
              QStringList aList = aPropMap.value(MARKER_TYPE_PROP).toString().split(DIGIT_SEPARATOR);
              if(aList.size() == 2) { //Standard marker string contains "TypeOfMarker:ScaleOfMarker"
                int  aTypeOfMarker = aList[0].toInt();
                double  aScaleOfMarker = aList[1].toDouble();
                Handle(Prs3d_PointAspect) anAspect = AISShape->Attributes()->PointAspect();
                anAspect->SetScale( aScaleOfMarker );
                anAspect->SetTypeOfMarker((Aspect_TypeOfMarker) (aTypeOfMarker-1) );
                anAspect->SetColor( aColor );
                AISShape->Attributes()->SetPointAspect( anAspect );
              } else { //Custom marker string contains "IdOfTexsture"
                int textureId = aList[0].toInt();
                Standard_Integer aWidth, aHeight;
#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
                Handle(TColStd_HArray1OfByte) aTexture =
#else
                Handle(Graphic3d_HArray1OfBytes) aTexture =
#endif
                  GeometryGUI::getTexture(aStudy, textureId, aWidth, aHeight);
                if (!aTexture.IsNull()) {
                  static int TextureId = 0;
                  Handle(Prs3d_PointAspect) aTextureAspect =
                    new Prs3d_PointAspect (aColor, ++TextureId, aWidth, aHeight, aTexture);
                  AISShape->Attributes()->SetPointAspect( aTextureAspect );
                } else {
                  useObjMarker = true;
                }
              }
            } else {
              useObjMarker = true;
            }
          }
          else {
            // Set line aspect
            col = aResMgr->colorValue( "Geometry", "wireframe_color", QColor( 255, 255, 0 ) );
            aColor = SalomeApp_Tools::color( col );

            Handle(Prs3d_LineAspect) anAspect = AISShape->Attributes()->LineAspect();
            anAspect->SetColor( aColor );
            AISShape->Attributes()->SetLineAspect( anAspect );

            // Set unfree boundaries aspect
            anAspect = AISShape->Attributes()->UnFreeBoundaryAspect();
            anAspect->SetColor( aColor );
            AISShape->Attributes()->SetUnFreeBoundaryAspect( anAspect );

            // Set free boundaries aspect
            col = aResMgr->colorValue( "Geometry", "free_bound_color", QColor( 0, 255, 0 ) );
            aColor = SalomeApp_Tools::color( col );

            anAspect = AISShape->Attributes()->FreeBoundaryAspect();
            anAspect->SetColor( aColor );
            AISShape->Attributes()->SetFreeBoundaryAspect( anAspect );

            // Set wire aspect
            col = aResMgr->colorValue( "Geometry", "line_color", QColor( 255, 0, 0 ) );
            aColor = SalomeApp_Tools::color( col );

            anAspect = AISShape->Attributes()->WireAspect();
            anAspect->SetColor( aColor );
            AISShape->Attributes()->SetWireAspect( anAspect );

            // bug [SALOME platform 0019868]
            // Set deviation angle. Default one is 12 degrees (Prs3d_Drawer.cxx:18)
            //AISShape->SetOwnDeviationAngle( 10*PI/180 );

            // IMP 0020626
            double aDC = 0;
            if(useStudy) {
              aDC = aPropMap.value(DEFLECTION_COEFF_PROP).toDouble();
            }
            else {
              aDC = aResMgr->doubleValue("Geometry", "deflection_coeff", 0.001);
            }

            aDC = std::max( aDC, DEFLECTION_MIN ); // to avoid to small values of the coefficient
            AISShape->SetOwnDeviationCoefficient(aDC);
          }
        }

        if ( HasTexture() )
        {
          AISShape->SetTextureFileName(TCollection_AsciiString(myTexture.c_str()));
          AISShape->SetTextureMapOn();
          AISShape->DisableTextureModulate();
          AISShape->SetDisplayMode(3);
        }
        
        if ( HasWidth() )
          AISShape->SetWidth( GetWidth() );

        if ( !myIO.IsNull() )
        {
          AISShape->setIO( myIO );
          AISShape->SetOwner( myIO );
        }
        else if ( !myName.empty() )
        {
          // Workaround to allow selection of temporary objects
          static int tempId = 0;
          char buf[50];
          sprintf( buf, "TEMP_%d", tempId++ );
          Handle( SALOME_InteractiveObject ) anObj =
            new SALOME_InteractiveObject( buf, "GEOM", myName.c_str() );
          AISShape->setIO( anObj );
          AISShape->SetOwner( anObj );
        }

        Handle( SALOME_InteractiveObject ) anIO = AISShape->getIO();
        if ( !anIO.IsNull() ) {
          _PTR(SObject) SO ( aStudy->studyDS()->FindObjectID( anIO->getEntry() ) );
          if ( SO ) {
            // get CORBA reference to data object
            CORBA::Object_var object = GeometryGUI::ClientSObjectToObject(SO);
            if ( !CORBA::is_nil( object ) ) {
              // downcast to GEOM object
              GEOM::GEOM_Object_var aGeomObject = GEOM::GEOM_Object::_narrow( object );
              bool hasColor = false;
              SALOMEDS::Color aSColor = getColor(aGeomObject,hasColor);
              if( hasColor && useObjColor) {
                Quantity_Color aQuanColor( aSColor.R, aSColor.G, aSColor.B, Quantity_TOC_RGB );
                AISShape->SetColor( aQuanColor );
                AISShape->SetShadingColor( aQuanColor );
                if ( onlyVertex ) {
                  Handle(Prs3d_PointAspect) anAspect = AISShape->Attributes()->PointAspect();
                  anAspect->SetColor( aQuanColor );
                  anAspect->SetScale( myScaleOfMarker );
                  anAspect->SetTypeOfMarker( myTypeOfMarker );
                  AISShape->Attributes()->SetPointAspect( anAspect );
                }
              } else if(!hasColor) {
                //In case if color wasn't defined in the property map of the object
                //and GEOM_Object color also wasn't defined get default color from Resource Mgr.
                QColor col = aResMgr->colorValue( "Geometry", "shading_color", QColor( 255, 0, 0 ) );
                Quantity_Color aQuanColor = SalomeApp_Tools::color( col );
                AISShape->SetShadingColor( aQuanColor );
                aStudy->setObjectProperty( aMgrId, anIO->getEntry(), COLOR_PROP, col );
              }

              // ... marker type
              if(useObjMarker) {
                GEOM::marker_type aType = aGeomObject->GetMarkerType();
                GEOM::marker_size aSize = aGeomObject->GetMarkerSize();
                if ( aType > GEOM::MT_NONE && aType < GEOM::MT_USER && aSize > GEOM::MS_NONE && aSize <= GEOM::MS_70 ) {
                  Aspect_TypeOfMarker aMType = (Aspect_TypeOfMarker)( (int)aType-1 );
                  double aMSize = ((int)aSize+1)*0.5;
                  Handle(Prs3d_PointAspect) anAspect = AISShape->Attributes()->PointAspect();
                  anAspect->SetScale( aMSize );
                  anAspect->SetTypeOfMarker( aMType );
                  Quantity_Color aQuanColor = SalomeApp_Tools::color( aResMgr->colorValue( "Geometry", "point_color", QColor( 255, 255, 0 ) ) );
                  if ( hasColor )
                    aQuanColor = Quantity_Color( aSColor.R, aSColor.G, aSColor.B, Quantity_TOC_RGB );
                  anAspect->SetColor( aQuanColor );
                  AISShape->Attributes()->SetPointAspect( anAspect );
                }
                else if ( aType == GEOM::MT_USER ) {
                  int aTextureId = aGeomObject->GetMarkerTexture();
                  Quantity_Color aQuanColor = SalomeApp_Tools::color( aResMgr->colorValue( "Geometry", "point_color", QColor( 255, 255, 0 ) ) );
                  if ( hasColor ) aQuanColor = Quantity_Color( aSColor.R, aSColor.G, aSColor.B, Quantity_TOC_RGB );
                  Standard_Integer aWidth, aHeight;
#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
                  Handle(TColStd_HArray1OfByte) aTexture =
#else
                  Handle(Graphic3d_HArray1OfBytes) aTexture =
#endif
                    GeometryGUI::getTexture(getStudy(), aTextureId, aWidth, aHeight);
                  if (!aTexture.IsNull()) {
                    static int TextureId = 0;
                    Handle(Prs3d_PointAspect) aTextureAspect =
                      new Prs3d_PointAspect(aQuanColor, ++TextureId, aWidth, aHeight, aTexture );
                    AISShape->Attributes()->SetPointAspect( aTextureAspect );
                  }
                } else { //Use marker from the preferences
                  Handle(Prs3d_PointAspect) anAspect = AISShape->Attributes()->PointAspect();
                  anAspect->SetScale( myScaleOfMarker );
                  anAspect->SetTypeOfMarker( myTypeOfMarker );
                  Quantity_Color aQuanColor = SalomeApp_Tools::color( aResMgr->colorValue( "Geometry", "point_color", QColor( 255, 255, 0 ) ) );
                  if ( hasColor )
                    aQuanColor = Quantity_Color( aSColor.R, aSColor.G, aSColor.B, Quantity_TOC_RGB );
                  anAspect->SetColor( aQuanColor );
                  AISShape->Attributes()->SetPointAspect( anAspect );
                }
              }
            }
          }
        }
        // AISShape->SetName(???); ??? necessary to set name ???
        occPrs->AddObject( AISShape );

        // In accordance with ToActivate() value object will be activated/deactivated
        // when it will be displayed
        occPrs->SetToActivate( ToActivate() );
      }
    }
    // if presentation is found -> set again shape for it
    else
    {
      if ( !myShape.IsNull() )
      {
        AIS_ListOfInteractive IOList;
        occPrs->GetObjects( IOList );
        AIS_ListIteratorOfListOfInteractive Iter( IOList );
        for ( ; Iter.More(); Iter.Next() )
        {
          Handle(GEOM_AISShape) AISShape = Handle(GEOM_AISShape)::DownCast( Iter.Value() );
          if ( AISShape.IsNull() )
            continue;
          if ( AISShape->Shape() != myShape )
          {
            AISShape->Set( myShape );
            AISShape->UpdateSelection();
            AISShape->SetToUpdate();
          }
          if ( !myIO.IsNull() )
          {
            AISShape->setIO( myIO );
            AISShape->SetOwner( myIO );
          }
        }
      }
    }
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
  SalomeApp_Study* aStudy = getStudy();
  int aMgrId = -1;
  SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( prs );

  if ( !vtkPrs || myShape.IsNull() || !aStudy)
    return;

  bool useStudy = false;
  PropMap aPropMap;

  vtkActorCollection* theActors = 0;

  if ( myType == GEOM_MARKER && myShape.ShapeType() == TopAbs_FACE ) {
    //myToActivate = false; // ouv: commented to make the trihedron pickable (see IPAL18657)
    GEOM_VTKTrihedron* aTrh = GEOM_VTKTrihedron::New();

    if ( HasColor() ) {
      Quantity_Color aColor( (Quantity_NameOfColor)GetColor() );
      aTrh->SetColor( aColor.Red(), aColor.Green(), aColor.Blue() );
    }

    Handle(Geom_Plane) aPlane =
      Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( TopoDS::Face( myShape ) ) );
    if ( aPlane.IsNull() )
      return;

    gp_Ax2 anAx2 = aPlane->Pln().Position().Ax2();
    aTrh->SetPlacement( new Geom_Axis2Placement( anAx2 ) );

    //    if ( SVTK_Viewer* vf = dynamic_cast<SVTK_Viewer*>( GetActiveView() ) )
    //      aTrh->SetSize( 0.5 * vf->GetTrihedronSize() );

    vtkPrs->AddObject( aTrh );

    theActors = vtkActorCollection::New();
    theActors->AddItem( aTrh );
  }
  else {
    PropMap aDefPropMap = getDefaultPropertyMap(SVTK_Viewer::Type());

    QString anEntry;
    if(!myIO.IsNull()) {
      aMgrId = getViewManagerId(myViewFrame);
      anEntry = myIO->getEntry();
    }
    useStudy = !anEntry.isEmpty() && aMgrId != -1;


    theActors = vtkActorCollection::New();
    GEOM_Actor* aGeomActor = GEOM_Actor::New();
    aGeomActor->SetShape(myShape,aDefPropMap.value(DEFLECTION_COEFF_PROP).toDouble(),myType == GEOM_VECTOR);
    theActors->AddItem(aGeomActor);
    aGeomActor->Delete();

    if(useStudy) {
      aPropMap = aStudy->getObjectPropMap(aMgrId,anEntry);
      MergePropertyMaps(aPropMap, aDefPropMap);
    }
  }

  theActors->InitTraversal();

  vtkActor* anActor = (vtkActor*)theActors->GetNextActor();

  vtkProperty* aProp = 0;

  if ( HasColor() || HasWidth() )
  {
    aProp = vtkProperty::New();
    aProp->SetRepresentationToWireframe();
  }

  if ( HasColor() )
  {
    Quantity_Color aColor( (Quantity_NameOfColor)GetColor() );
    aProp->SetColor( aColor.Red(), aColor.Green(), aColor.Blue() );
  }

  if ( HasWidth() )
  {
    aProp->SetLineWidth( GetWidth() );
  }

  while ( anActor != NULL )
  {
    SALOME_Actor* GActor = SALOME_Actor::SafeDownCast( anActor );

    GActor->setIO( myIO );

    if ( aProp )
    {
      GActor->SetProperty( aProp );
      GActor->SetPreviewProperty( aProp );
    }

    GEOM_Actor* aGeomGActor = GEOM_Actor::SafeDownCast( anActor );
    if ( aGeomGActor != 0 )
      {
        if ( aProp ) {
          aGeomGActor->SetShadingProperty( aProp );
          aGeomGActor->SetWireframeProperty( aProp );
        }
        int aIsos[2]= { 1, 1 };
        if(useStudy) {
          QString anIsos = aPropMap.value(ISOS_PROP).toString();
          QStringList uv =  anIsos.split(DIGIT_SEPARATOR);
          aIsos[0] = uv[0].toInt(); aIsos[1] = uv[1].toInt();
          aGeomGActor->SetNbIsos(aIsos);
          aGeomGActor->SetOpacity(1.0 - aPropMap.value(TRANSPARENCY_PROP).toDouble());
          aGeomGActor->SetVectorMode(aPropMap.value(VECTOR_MODE_PROP).toInt());
          aGeomGActor->setDisplayMode(aPropMap.value(DISPLAY_MODE_PROP).toInt());
          aGeomGActor->SetDeflection(aPropMap.value(DEFLECTION_COEFF_PROP).toDouble());

          vtkFloatingPointType aColor[3] = {1.,0.,0.};
          if(aPropMap.contains(COLOR_PROP)) {
            QColor c = aPropMap.value(COLOR_PROP).value<QColor>();
            aColor[0] = c.red()/255.; aColor[1] = c.green()/255.; aColor[2] = c.blue()/255.;
          } else { //Get Color from geom object
            Handle( SALOME_InteractiveObject ) anIO = aGeomGActor->getIO();
            if ( !anIO.IsNull() ) {
              _PTR(SObject) SO ( aStudy->studyDS()->FindObjectID( anIO->getEntry() ) );
              if ( SO ) {
                // get CORBA reference to data object
                CORBA::Object_var object = GeometryGUI::ClientSObjectToObject(SO);
                if ( !CORBA::is_nil( object ) ) {
                  // downcast to GEOM object
                  GEOM::GEOM_Object_var aGeomObject = GEOM::GEOM_Object::_narrow( object );
                  bool hasColor = false;
                  SALOMEDS::Color aSColor = getColor(aGeomObject,hasColor);
                  if(hasColor) {
                    aColor[0] = aSColor.R; aColor[1] = aSColor.G; aColor[2] = aSColor.B;
                  } else {
                    SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
                    if(aResMgr) {
                      QColor c = aResMgr->colorValue( "Geometry", "shading_color", QColor( 255, 0, 0 ) );
                      aColor[0] = c.red()/255.; aColor[1] = c.green()/255.; aColor[2] = c.blue()/255.;
                      aStudy->setObjectProperty( aMgrId, anIO->getEntry(), COLOR_PROP, c );
                    }
                  }
                }
              }
            }
          }
          aGeomGActor->SetColor(aColor[0],aColor[1],aColor[2]);
        }
      }

    if ( myToActivate )
      GActor->PickableOn();
    else
      GActor->PickableOff();

    vtkPrs->AddObject( GActor );

    anActor = (vtkActor*)theActors->GetNextActor();
  }

  if ( aProp )
    aProp->Delete();

  theActors->Delete();
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
                // downcast to GEOM object
                GEOM::GEOM_Object_var GeomObject = GEOM::GEOM_Object::_narrow( object );
                if ( !GeomObject->_is_nil() )
                {
                  // finally set shape
                  setShape( GEOM_Client::get_client().GetShape( GeometryGUI::GetGeomGen(), GeomObject ) );
                  myType = GeomObject->GetType();
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
    vf->LocalSelection( prs, theMode );
    delete prs;  // delete presentation because displayer is its owner
  }
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
void GEOM_Displayer::LocalSelection( const SALOME_ListIO& theIOList, const int theMode )
{
  SALOME_ListIteratorOfListIO Iter( theIOList );
  for ( ; Iter.More(); Iter.Next() )
    LocalSelection( Iter.Value(), theMode );
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
  SalomeApp_Study* aStudy = getStudy();
  if (!aStudy) return;
  SOCC_Viewer* vf = dynamic_cast<SOCC_Viewer*>( v );
  if ( vf && !p->IsNull() ) {
    int aMgrId = getViewManagerId( vf );
    Handle(AIS_InteractiveContext) ic = vf->getAISContext();
    const SOCC_Prs* prs = dynamic_cast<const SOCC_Prs*>( p );
    if ( !ic.IsNull() && prs ) {
      AIS_ListOfInteractive objects;
      prs->GetObjects( objects );
      AIS_ListIteratorOfListOfInteractive it( objects );
      for ( ; it.More(); it.Next() ) {
        Handle(GEOM_AISShape) sh = Handle(GEOM_AISShape)::DownCast( it.Value() );
        if ( sh.IsNull() ) continue;
        Handle(SALOME_InteractiveObject) IO = sh->getIO();
        if ( IO.IsNull() ) continue;
        PropMap aPropMap = aStudy->getObjectPropMap( aMgrId, IO->getEntry() );
        if ( aPropMap.contains( TRANSPARENCY_PROP ) ) {
          double transparency = aPropMap.value(TRANSPARENCY_PROP).toDouble();
          ic->SetTransparency( sh, transparency, true );
        }
      }
    }
  }
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
 *  GEOM_Displayer::SetTexture
 *  Set color for shape displaying. If it is equal -1 then default color is used.
 *  Available values are from Quantity_NameOfColor enumeration
 */
//=================================================================
void GEOM_Displayer::SetTexture( const std::string& texureFileName )
{
  if(texureFileName!="")
  {
    myTexture = texureFileName;
  }
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

bool GEOM_Displayer::canBeDisplayed( const QString& entry, const QString& viewer_type ) const
{
  return viewer_type == SOCC_Viewer::Type() || viewer_type == SVTK_Viewer::Type();
}

int GEOM_Displayer::SetDisplayMode( const int theMode )
{
  int aPrevMode = myDisplayMode;
  if ( theMode != -1 )
    myDisplayMode = theMode;
  else
  {
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    myDisplayMode = resMgr->integerValue( "Geometry", "display_mode", 0 );
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
  return aPrevMode;
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



PropMap GEOM_Displayer::getDefaultPropertyMap(const QString& viewer_type) {
  PropMap aDefaultMap;
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  //1. Visibility
  aDefaultMap.insert(VISIBILITY_PROP , 1);

  //2. Nb Isos
  int anUIsoNumber;
  int aVIsoNumber;
  if(viewer_type == SOCC_Viewer::Type()) {
    anUIsoNumber = aResMgr->integerValue("OCCViewer", "iso_number_u", 1);
    aVIsoNumber = aResMgr->integerValue("OCCViewer", "iso_number_v", 1);
  } else if( viewer_type==SVTK_Viewer::Type()) {
    anUIsoNumber = aResMgr->integerValue("VTKViewer", "iso_number_u", 1);
    aVIsoNumber = aResMgr->integerValue("VTKViewer", "iso_number_u", 1);
  }
  QString anIsos("%1%2%3");
  anIsos = anIsos.arg(anUIsoNumber);anIsos = anIsos.arg(DIGIT_SEPARATOR);anIsos = anIsos.arg(aVIsoNumber);
  aDefaultMap.insert(ISOS_PROP , anIsos);

  //3. Transparency
  aDefaultMap.insert( TRANSPARENCY_PROP , 0.0 );

  //4. Display Mode
  aDefaultMap.insert( DISPLAY_MODE_PROP , aResMgr->integerValue("Geometry", "display_mode", 0));

  //5. Vector Mode
  aDefaultMap.insert( VECTOR_MODE_PROP , 0);

  //6. Color
  QColor col = aResMgr->colorValue( "Geometry", "shading_color", QColor( 255, 0, 0 ) );
  aDefaultMap.insert( COLOR_PROP , col);

  //7. Deflection Coeff
  double aDC;

  if(viewer_type == SOCC_Viewer::Type()) {
    aDC = aResMgr->doubleValue("Geometry", "deflection_coeff", 0.001);
  } else if( viewer_type==SVTK_Viewer::Type()) {
    aDC = 0.001;
  }

  aDefaultMap.insert( DEFLECTION_COEFF_PROP , aDC);

  return aDefaultMap;
}

bool GEOM_Displayer::MergePropertyMaps(PropMap& theOrigin, PropMap& theDefault) {
  int nbInserted = 0;
  if(!theOrigin.contains(VISIBILITY_PROP)) {
    theOrigin.insert(VISIBILITY_PROP, 0);
    nbInserted++;
  }
  if(!theOrigin.contains(TRANSPARENCY_PROP)) {
    theOrigin.insert(TRANSPARENCY_PROP, theDefault.value(TRANSPARENCY_PROP));
    nbInserted++;
  }
  if(!theOrigin.contains(DISPLAY_MODE_PROP)) {
    theOrigin.insert(DISPLAY_MODE_PROP, theDefault.value(DISPLAY_MODE_PROP));
    nbInserted++;
  }
  if(!theOrigin.contains(ISOS_PROP)) {
    theOrigin.insert(ISOS_PROP, theDefault.value(ISOS_PROP));
    nbInserted++;
  }
  if(!theOrigin.contains(VECTOR_MODE_PROP)) {
    theOrigin.insert(VECTOR_MODE_PROP, theDefault.value(VECTOR_MODE_PROP));
    nbInserted++;
  }
  if(!theOrigin.contains(DEFLECTION_COEFF_PROP)) {
    theOrigin.insert(DEFLECTION_COEFF_PROP, theDefault.value(DEFLECTION_COEFF_PROP));
    nbInserted++;
  }
  return (nbInserted > 0);
}


SALOMEDS::Color GEOM_Displayer::getColor(GEOM::GEOM_Object_var theGeomObject, bool& hasColor) {
  SALOMEDS::Color aSColor;
  hasColor = false;

  SUIT_Session* session = SUIT_Session::session();
  SUIT_Application* app = session->activeApplication();

  if ( app && !theGeomObject->_is_nil()) {
    SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );

    if ( study ) {
      aSColor = theGeomObject->GetColor();
      hasColor = aSColor.R >= 0 && aSColor.G >= 0 && aSColor.B >= 0;
      if( !hasColor && theGeomObject->GetType() == GEOM_GROUP ) { // auto color for group
        GEOM::GEOM_Gen_var theGeomGen = GeometryGUI::GetGeomGen();
        GEOM::GEOM_IGroupOperations_var anOperations = theGeomGen->GetIGroupOperations( study->id() );
        GEOM::GEOM_Object_var aMainObject = anOperations->GetMainShape( theGeomObject );
        if ( !aMainObject->_is_nil() && aMainObject->GetAutoColor() )
          {
            QList<SALOMEDS::Color> aReservedColors;

            SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( app );
            CORBA::String_var IOR = app->orb()->object_to_string( aMainObject );
            if ( strcmp(IOR.in(), "") != 0 )
              {
                _PTR(Study) aStudy = study->studyDS();
                _PTR(SObject) aMainSObject( aStudy->FindObjectIOR( std::string(IOR) ) );
                _PTR(ChildIterator) it( aStudy->NewChildIterator( aMainSObject ) );
                for( ; it->More(); it->Next() )
                  {
                    _PTR(SObject) aChildSObject( it->Value() );
                    GEOM::GEOM_Object_var aChildObject =
                      GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aChildSObject));
                    if( CORBA::is_nil( aChildObject ) )
                      continue;

                    if( aChildObject->GetType() != GEOM_GROUP )
                      continue;

                    SALOMEDS::Color aReservedColor = aChildObject->GetColor();
                    aReservedColors.append( aReservedColor );
                  }
              }

            aSColor = getUniqueColor( aReservedColors );
            hasColor = true;
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
