//  GEOM GEOMGUI : GUI for Geometry component
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
//  File   : GeometryGUI.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "GeometryGUI.h"
#include "GEOMGUI_OCCSelector.h"
#include "GEOMGUI_Selection.h"
#include "GEOM_Displayer.h"

#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>

#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>
#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewManager.h>

#include <SVTK_ViewWindow.h>
#include <SVTK_RenderWindowInteractor.h>
#include <SVTK_InteractorStyle.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewManager.h>

#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_VTKSelector.h>
#include <SalomeApp_Study.h>
#include <LightApp_Preferences.h>
#include <SALOME_LifeCycleCORBA.hxx>
#include <SALOME_ListIO.hxx>

// External includes
#include <qfileinfo.h>
#include <qpainter.h>

#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <OSD_SharedLibrary.hxx>

#include <utilities.h>

#include <vtkCamera.h>
#include <vtkRenderer.h>


extern "C" {
  Standard_EXPORT CAM_Module* createModule() {
    return new GeometryGUI();
  }
}



GEOM::GEOM_Gen_var GeometryGUI::myComponentGeom = GEOM::GEOM_Gen::_nil(); 

GEOM::GEOM_Gen_var   GeometryGUI::GetGeomGen()        { return GeometryGUI::myComponentGeom; }

bool GeometryGUI::InitGeomGen() 
{
  GeometryGUI aGG;
  if( CORBA::is_nil( myComponentGeom ) ) return false;   
  return true; 
}			   

//=======================================================================
// function : ClientSObjectToObject
// purpose  : 
//=======================================================================
CORBA::Object_var GeometryGUI::ClientSObjectToObject (_PTR(SObject) theSObject)
{
  _PTR(GenericAttribute) anAttr;
  CORBA::Object_var anObj;
  try {
    std::string aValue = theSObject->GetIOR();
    if (strcmp(aValue.c_str(), "") != 0) {
      CORBA::ORB_ptr anORB = SalomeApp_Application::orb();
      anObj = anORB->string_to_object(aValue.c_str());
    }
  } catch(...) {
    INFOS("ClientSObjectToObject - Unknown exception was occured!!!");
  }
  return anObj._retn();
}

//=======================================================================
// function : ClientStudyToStudy
// purpose  : 
//=======================================================================
SALOMEDS::Study_var GeometryGUI::ClientStudyToStudy (_PTR(Study) theStudy)
{
  SALOME_NamingService *aNamingService = SalomeApp_Application::namingService();
  CORBA::Object_var aSMObject = aNamingService->Resolve("/myStudyManager");
  SALOMEDS::StudyManager_var aStudyManager = SALOMEDS::StudyManager::_narrow(aSMObject);
  int aStudyID = theStudy->StudyId();
  SALOMEDS::Study_var aDSStudy = aStudyManager->GetStudyByID(aStudyID);
  return aDSStudy._retn();
}

//=================================================================================
// class   : CustomItem
// purpose : Set Font to a text.
//=================================================================================
class CustomItem : public QCustomMenuItem
{
public:
  CustomItem(const QString& s, const QFont& f) : myString(s), myFont(f) {}
  ~CustomItem() {}

  void paint(QPainter* p, const QColorGroup& cg, bool act, bool /*enabled*/, int x, int y, int w, int h)
  {
    p->save();
    p->fillRect( x, y, w, h, act ? cg.highlight() : cg.mid() );
    p->setPen( act ? cg.highlightedText() : cg.buttonText() );
    p->setFont( myFont );
    p->drawText( x, y, w, h, AlignHCenter | AlignVCenter | ShowPrefix | DontClip | SingleLine, myString );
    p->restore();
  }

  QSize sizeHint()
  {
    return QFontMetrics( myFont ).size( AlignHCenter | AlignVCenter | ShowPrefix | DontClip | SingleLine, myString );
  }

  bool fullSpan() const
  {
    return true;
  }

private:
  QString myString;
  QFont   myFont;
};

//=======================================================================
// function : GeometryGUI::GeometryGUI()
// purpose  : Constructor
//=======================================================================
GeometryGUI::GeometryGUI() :
  SalomeApp_Module( "GEOM" )
{
  if ( CORBA::is_nil( myComponentGeom ) )
  { 
    SALOME_LifeCycleCORBA* ls = new SALOME_LifeCycleCORBA( getApp()->namingService() );
    Engines::Component_var comp = ls->FindOrLoad_Component( "FactoryServer", "GEOM" );
    myComponentGeom  = GEOM::GEOM_Gen::_narrow( comp );
  }
  
  myState           = -1;
  myActiveDialogBox = 0;
  myFatherior       = "";

  gp_Pnt origin = gp_Pnt(0., 0., 0.);
  gp_Dir direction = gp_Dir(0., 0., 1.);
  myWorkingPlane = gp_Ax3(origin, direction);

  myOCCSelectors.setAutoDelete( true );
  myVTKSelectors.setAutoDelete( true );

  myDisplayer = 0;
}

//=======================================================================
// function : GeometryGUI::~GeometryGUI()
// purpose  : Destructor
//=======================================================================
GeometryGUI::~GeometryGUI()
{
}

//=======================================================================
// function : GeometryGUI::getLibrary()
// purpose  : get or load GUI library by name [ internal ]
//=======================================================================
typedef GEOMGUI* (*LibraryGUI)( GeometryGUI* );
GEOMGUI* GeometryGUI::getLibrary( const QString& libraryName )
{
  if ( !myGUIMap.contains( libraryName ) ) {
    // try to load library if it is not loaded yet
    QCString libs;
#ifndef WNT
    if( ( libs = getenv( "LD_LIBRARY_PATH" ) ) ) {
	  QStringList dirList = QStringList::split( ":", libs, false ); // skip empty entries
#else
	if( ( libs = getenv( "PATH" ) ) ) {
	  QStringList dirList = QStringList::split( ";", libs, false ); // skip empty entries
#endif
      for( int i = dirList.count()-1; i >= 0; i-- ) {
	QString dir = dirList[ i ];
	QFileInfo fi( Qtx::addSlash( dirList[ i ] ) + libraryName );
	if( fi.exists() ) {
	  OSD_SharedLibrary aSharedLibrary( (char*)fi.fileName().latin1() );
	  bool res = aSharedLibrary.DlOpen( OSD_RTLD_LAZY );
	  if( !res ) {
	    MESSAGE( "Can't open library : " << aSharedLibrary.DlError() );
	    continue; // continue search further
	  }
	  OSD_Function osdF = aSharedLibrary.DlSymb( "GetLibGUI" );
	  if ( osdF != NULL ) {
	    LibraryGUI func = (GEOMGUI* (*) (GeometryGUI*))osdF;
	    GEOMGUI* libGUI = (*func)(this);
	    if ( libGUI ) {
	      myGUIMap[ libraryName ] = libGUI;
	      break; // found and loaded!
	    }
	  }
	}
      }
    }
  }
  if ( myGUIMap.contains( libraryName ) )
    // library is successfully loaded
    return myGUIMap[ libraryName ];
  return 0;
}

//=======================================================================
// function : GeometryGUI::ActiveWorkingPlane()
// purpose  : Activate Working Plane View
//=======================================================================
void GeometryGUI::ActiveWorkingPlane()
{
  gp_Dir DZ = myWorkingPlane.Direction();
  gp_Dir DY = myWorkingPlane.YDirection();

  SUIT_ViewWindow* window = application()->desktop()->activeWindow();
  bool ViewOCC = ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );
  bool ViewVTK = ( window && window->getViewManager()->getType() == SVTK_Viewer::Type() );

  if( ViewOCC ) {
    OCCViewer_ViewWindow* vw = dynamic_cast<OCCViewer_ViewWindow*>( window );
    if ( vw ) {
      Handle(V3d_View) view3d =  vw->getViewPort()->getView();

      view3d->SetProj(DZ.X(), DZ.Y(), DZ.Z());
      view3d->SetUp(DY.X(), DY.Y(), DY.Z());

      vw->onViewFitAll();
    }
  }
  else if( ViewVTK ) {
    SVTK_ViewWindow* vw = dynamic_cast<SVTK_ViewWindow*>( window );
    if ( vw ) {
      vtkCamera* camera = vw->getRenderer()->GetActiveCamera();

      camera->SetPosition(DZ.X(), DZ.Y(), DZ.Z());
      camera->SetViewUp(DY.X(), DY.Y(), DY.Z());
      camera->SetFocalPoint(0,0,0);

      vw->onFitAll();
    }
  }
}

//=======================================================================
// function : GeometryGUI::SetActiveDialogBox()
// purpose  : Set active dialog box
//=======================================================================
void GeometryGUI::SetActiveDialogBox( QDialog* aDlg )
{
  myActiveDialogBox = (QDialog*)aDlg;
}

//=======================================================================
// function : GeometryGUI::EmitSignalDeactivateDialog()
// purpose  : Emit a signal to deactivate the active dialog Box
//=======================================================================
void GeometryGUI::EmitSignalDeactivateDialog()
{
  emit SignalDeactivateActiveDialog();
}

//=======================================================================
// function : GeometryGUI::EmitSignalCloseAllDialogs()
// purpose  : Emit a signal to close all non modal dialogs box
//=======================================================================
void GeometryGUI::EmitSignalCloseAllDialogs()
{
  emit SignalCloseAllDialogs();
}

//=======================================================================
// function : GeometryGUI::EmitSignalDefaultStepValueChanged()
// purpose  : Emit a signal to inform that default real spin box step has
//            been changed
//=======================================================================
void GeometryGUI::EmitSignalDefaultStepValueChanged(double newVal)
{
  emit SignalDefaultStepValueChanged(newVal);
}

//=======================================================================
// function : GeometryGUI::OnGUIEvent()
// purpose  : common slot for all menu/toolbar actions
//=======================================================================
void GeometryGUI::OnGUIEvent()
{
  const QObject* obj = sender();
  if ( !obj || !obj->inherits( "QAction" ) )
    return;
  int id = actionId((QAction*)obj);
  if ( id != -1 )
    OnGUIEvent( id );
}

//=======================================================================
// function : GeometryGUI::OnGUIEvent()
// purpose  : manage all events on GUI [static]
//=======================================================================
void GeometryGUI::OnGUIEvent( int id )
{
  SUIT_Desktop* desk = application()->desktop();

  // check type of the active viewframe
  SUIT_ViewWindow* window = desk->activeWindow();
  bool ViewOCC = ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );
  bool ViewVTK = ( window && window->getViewManager()->getType() == SVTK_Viewer::Type() );
  // if current viewframe is not of OCC and not of VTK type - return immediately
  // fix for IPAL8958 - allow some commands to execute even when NO viewer is active (rename for example)
  bool NotViewerDependentCommand = ( id == 901 || id == 216 || id == 213 ); 
  if ( !ViewOCC && !ViewVTK && !NotViewerDependentCommand )
      return;

  // fix for IPAL9103, point 2
  if ( CORBA::is_nil( GetGeomGen() ) ) {
    SUIT_MessageBox::error1( desk, tr( "GEOM_ERROR" ), tr( "GEOM_ERR_GET_ENGINE" ), tr( "GEOM_BUT_OK" ) );
    return;
  }

  GEOMGUI* library = 0;
  // try to get-or-load corresponding GUI library
  if( id == 111  ||  // MENU FILE - IMPORT BREP
      id == 112  ||  // MENU FILE - IMPORT IGES
      id == 113  ||  // MENU FILE - IMPORT STEP
      id == 121  ||  // MENU FILE - EXPORT BREP
      id == 122  ||  // MENU FILE - EXPORT IGES
      id == 123  ||  // MENU FILE - EXPORT STEP
      id == 31   ||  // MENU EDIT - COPY
      id == 33   ||  // MENU EDIT - DELETE
      id == 411  ||  // MENU SETTINGS - ADD IN STUDY
      id == 412  ||  // MENU SETTINGS - SHADING COLOR
      id == 413  ||  // MENU SETTINGS - ISOS
      id == 414  ||  // MENU SETTINGS - STEP VALUE FOR SPIN BOXES
      id == 5103 ||  // MENU TOOLS - CHECK GEOMETRY
      id == 8032 ||  // POPUP VIEWER - COLOR
      id == 8033 ||  // POPUP VIEWER - TRANSPARENCY
      id == 8034 ||  // POPUP VIEWER - ISOS
      id == 804  ||  // POPUP VIEWER - ADD IN STUDY
      id == 901  ||  // OBJECT BROWSER - RENAME
      id == 9024 ) { // OBJECT BROWSER - OPEN
    //cout << "id " << id << " received" << endl;
#ifndef WNT
	library = getLibrary( "libGEOMToolsGUI.so" );
#else
	library = getLibrary( "GEOMToolsGUI.dll" );
#endif
  }
  else if( id == 211  ||  // MENU VIEW - WIREFRAME/SHADING
	   id == 212  ||  // MENU VIEW - DISPLAY ALL
	   id == 213  ||  // MENU VIEW - DISPLAY ONLY
	   id == 214  ||  // MENU VIEW - ERASE ALL
	   id == 215  ||  // MENU VIEW - ERASE
	   id == 216  ||  // MENU VIEW - DISPLAY
	   id == 80311 ||  // POPUP VIEWER - WIREFRAME
	   id == 80312 ) { // POPUP VIEWER - SHADING
#ifndef WNT
	library = getLibrary( "libDisplayGUI.so" );
#else
	library = getLibrary( "DisplayGUI.dll" );
#endif
  }
  else if( id == 4011 ||  // MENU BASIC - POINT
	   id == 4012 ||  // MENU BASIC - LINE
	   id == 4013 ||  // MENU BASIC - CIRCLE
	   id == 4014 ||  // MENU BASIC - ELLIPSE
	   id == 4015 ||  // MENU BASIC - ARC
	   id == 4016 ||  // MENU BASIC - VECTOR
	   id == 4017 ||  // MENU BASIC - PLANE
	   id == 4018 ||  // MENU BASIC - WPLANE
	   id == 4019 ||  // MENU BASIC - CURVE
	   id == 4020 ) { // MENU BASIC - REPAIR
#ifndef WNT
	library = getLibrary( "libBasicGUI.so" );
#else
	library = getLibrary( "BasicGUI.dll" );
#endif
  }
  else if( id == 4021 ||  // MENU PRIMITIVE - BOX
	   id == 4022 ||  // MENU PRIMITIVE - CYLINDER
	   id == 4023 ||  // MENU PRIMITIVE - SPHERE
	   id == 4024 ||  // MENU PRIMITIVE - TORUS
	   id == 4025 ) { // MENU PRIMITIVE - CONE
#ifndef WNT
	library = getLibrary( "libPrimitiveGUI.so" );
#else
	library = getLibrary( "PrimitiveGUI.dll" );
#endif
  }
  else if( id == 4031 ||  // MENU GENERATION - PRISM
	   id == 4032 ||  // MENU GENERATION - REVOLUTION
	   id == 4033 ||  // MENU GENERATION - FILLING
	   id == 4034 ) { // MENU GENERATION - PIPE
#ifndef WNT
	library = getLibrary( "libGenerationGUI.so" );
#else
	library = getLibrary( "GenerationGUI.dll" );
#endif
  }
  else if( id == 404 ||   // MENU ENTITY - SKETCHER
	   id == 407 ) {  // MENU ENTITY - EXPLODE
#ifndef WNT
	library = getLibrary( "libEntityGUI.so" );
#else
	library = getLibrary( "EntityGUI.dll" );
#endif
  }
  else if( id == 4081 ||  // MENU BUILD - EDGE
	   id == 4082 ||  // MENU BUILD - WIRE
	   id == 4083 ||  // MENU BUILD - FACE
	   id == 4084 ||  // MENU BUILD - SHELL
	   id == 4085 ||  // MENU BUILD - SOLID
	   id == 4086 ) { // MENU BUILD - COMPUND
#ifndef WNT
	library = getLibrary( "libBuildGUI.so" );
#else
	library = getLibrary( "BuildGUI.dll" );
#endif
  }
  else if( id == 5011 ||  // MENU BOOLEAN - FUSE
	   id == 5012 ||  // MENU BOOLEAN - COMMON
	   id == 5013 ||  // MENU BOOLEAN - CUT
	   id == 5014 ) { // MENU BOOLEAN - SECTION
#ifndef WNT
	library = getLibrary( "libBooleanGUI.so" );
#else
	library = getLibrary( "BooleanGUI.dll" );
#endif
  }
  else if( id == 5021 ||  // MENU TRANSFORMATION - TRANSLATION
	   id == 5022 ||  // MENU TRANSFORMATION - ROTATION
	   id == 5023 ||  // MENU TRANSFORMATION - LOCATION
	   id == 5024 ||  // MENU TRANSFORMATION - MIRROR
	   id == 5025 ||  // MENU TRANSFORMATION - SCALE
	   id == 5026 ||  // MENU TRANSFORMATION - OFFSET
	   id == 5027 ||  // MENU TRANSFORMATION - MULTI-TRANSLATION
	   id == 5028 ) { // MENU TRANSFORMATION - MULTI-ROTATION
#ifndef WNT
	library = getLibrary( "libTransformationGUI.so" );
#else
	library = getLibrary( "TransformationGUI.dll" );
#endif
  }
  else if( id == 503 ||   // MENU OPERATION - PARTITION
	   id == 504 ||   // MENU OPERATION - ARCHIMEDE
	   id == 505 ||   // MENU OPERATION - FILLET
	   id == 506 ||   // MENU OPERATION - CHAMFER  
	   id == 507 ) {  // MENU OPERATION - CLIPPING RANGE
#ifndef WNT
	library = getLibrary( "libOperationGUI.so" );
#else
	library = getLibrary( "OperationGUI.dll" );
#endif
  }
  else if( id == 601 ||   // MENU REPAIR - SEWING
	   id == 603 ||   // MENU REPAIR - SUPPRESS FACES
	   id == 604 ||   // MENU REPAIR - SUPPRESS HOLE
           id == 605 ||   // MENU REPAIR - SHAPE PROCESSING
           id == 606 ||   // MENU REPAIR - CLOSE CONTOUR
           id == 607 ||   // MENU REPAIR - REMOVE INTERNAL WIRES
           id == 608 ||   // MENU REPAIR - ADD POINT ON EDGE
           id == 609 ||   // MENU REPAIR - FREE BOUNDARIES
           id == 610 ||   // MENU REPAIR - FREE FACES
	   id == 602 ) {  // MENU REPAIR - GLUE FACES
#ifndef WNT
	library = getLibrary( "libRepairGUI.so" );
#else
	library = getLibrary( "RepairGUI.dll" );
#endif
  }
  else if( id == 701   ||  // MENU MEASURE - PROPERTIES
	   id == 702   ||  // MENU MEASURE - CDG
	   id == 703   ||  // MENU MEASURE - INERTIA
	   id == 7041  ||  // MENU MEASURE - BOUNDING BOX
	   id == 7042  ||  // MENU MEASURE - MIN DISTANCE
	   id == 705   ||  // MENU MEASURE - TOLERANCE
	   id == 706   ||  // MENU MEASURE - WHATIS
	   id == 707   ||  // MENU MEASURE - CHECK
	   id == 7072  ||  // MENU MEASURE - CHECK COMPOUND OF BLOCKS
	   id == 708 ) {  // MENU MEASURE - POINT COORDINATES
#ifndef WNT
	library = getLibrary( "libMeasureGUI.so" );
#else
	library = getLibrary( "MeasureGUI.dll" );
#endif
  }
  else if( id == 800  ||  // MENU GROUP - CREATE
	   id == 8001 ||  // POPUP MENU - CREATE GROUP
	   id == 801 ) {  // MENU GROUP - EDIT
#ifndef WNT
	library = getLibrary( "libGroupGUI.so" );
#else
	library = getLibrary( "GroupGUI.dll" );
#endif
  }
  else if( id == 9999  ||  // MENU BLOCKS - HEXAHEDRAL SOLID
           id == 9998  ||  // MENU BLOCKS - MULTI-TRANSFORMATION
           id == 9997  ||  // MENU BLOCKS - QUADRANGLE FACE
           id == 99991 ||  // MENU BLOCKS - PROPAGATE
           id == 9995 ) { // MENU BLOCKS - EXPLODE ON BLOCKS
#ifndef WNT
	library = getLibrary( "libBlocksGUI.so" );
#else
	library = getLibrary( "BlocksGUI.dll" );
#endif
  }

  // call method of corresponding GUI library
  if ( library ) 
    library->OnGUIEvent( id, desk );
  else 
    SUIT_MessageBox::error1( desk, tr( "GEOM_ERROR" ), tr( "GEOM_ERR_LIB_NOT_FOUND" ), tr( "GEOM_BUT_OK" ) );
}


//=================================================================================
// function : GeometryGUI::OnKeyPress()
// purpose  : Called when any key is pressed by user [static]
//=================================================================================
void GeometryGUI::OnKeyPress( SUIT_ViewWindow* win, QKeyEvent* pe )
{
  GUIMap::Iterator it;
  bool bOk = true;
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it )
    bOk = bOk && it.data()->OnKeyPress( pe, application()->desktop(), win );
//  return bOk;
}


//=================================================================================
// function : GeometryGUI::OnMouseMove()
// purpose  : Manages mouse move events [static]
//=================================================================================
void GeometryGUI::OnMouseMove( SUIT_ViewWindow* win, QMouseEvent* pe )
{  
  GUIMap::Iterator it;
  bool bOk = true;
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it )
    bOk = bOk && it.data()->OnMouseMove( pe, application()->desktop(), win );
//  return bOk;
}


//=================================================================================
// function : GeometryGUI::0nMousePress()
// purpose  : Manage mouse press events [static]
//=================================================================================
void GeometryGUI::OnMousePress( SUIT_ViewWindow* win, QMouseEvent* pe )
{
  GUIMap::Iterator it;
  // OnMousePress() should return false if this event should be processed further
  // (see OCCViewer_Viewer3d::onMousePress() for explanation)
  bool processed = false;
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it )
    processed = processed || it.data()->OnMousePress( pe, application()->desktop(), win );
//  return processed;
}

/*
static void UpdateVtkSelection()
{
  QPtrList<SUIT_ViewWindow> winList = application()->desktop()->windows();
  SUIT_ViewWindow* win = 0;
  for ( win = winList.first(); win; win = winList.next() ) {
    if ( win->getViewManager()->getTypeView() == VIEW_VTK ) {
      SVTK_ViewWindow* vw = dynamic_cast<SVTK_ViewWindow*>( window );
      if ( vw ) {
	SVTK_RenderWindowInteractor* anInteractor = vw->getRWInteractor();
	anInteractor->SetSelectionProp();
	anInteractor->SetSelectionTolerance();
	SVTK_InteractorStyleSALOME* aStyle = anInteractor->GetInteractorStyleSALOME();
	if (aStyle) {
	  aStyle->setPreselectionProp();
	}
      }
    }
  }
}

//=================================================================================
// function : GeometryGUI::SetSettings()
// purpose  : Called when GEOM module is activated [static]
//=================================================================================
bool GeometryGUI::SetSettings()
{
  QMenuBar*     Mb = parent->getMainMenuBar();
  SUIT_Study*   ActiveStudy = application()->activeStudy();
    
// Wireframe or Shading
  int DisplayMode = 0;
  SUIT_ViewWindow* window = application()->desktop()->activeWindow();
  bool ViewOCC = ( window && window->getViewManager()->getType() == VIEW_OCC );
  bool ViewVTK = ( window && window->getViewManager()->getType() == VIEW_VTK );
  if ( ViewOCC ) {
    OCCViewer_ViewManager* vm = dynamic_cast<OCCViewer_ViewManager*>( window->getViewManager() );
    if ( vm ) {
      Handle(AIS_InteractiveContext) ic = vm->getOCCViewer()->getAISContext();
      DisplayMode = ic->DisplayMode();
    }
  }
  else if ( ViewVTK ) {
    SVTK_ViewWindow* vw = dynamic_cast<SVTK_ViewWindow*>( window );
    if ( vw ) {
      SVTK_RenderWindowInteractor* myRenderInter = vw->getRWInteractor();
      DisplayMode = myRenderInter->GetDisplayMode();
    }
  }

  if( DisplayMode == 1 )
    getApp()->
    Mb->changeItem( 211, tr( "GEOM_MEN_WIREFRAME" ) );
  else
    Mb->changeItem( 211, tr( "GEOM_MEN_SHADING" ) );


  // Add in Study  - !!!ALWAYS TRUE!!! /////// VSR : TO BE REMOVED
  QString AddInStudy = QAD_CONFIG->getSetting("Geometry:SettingsAddInStudy");
  int Settings_AddInStudy;
  //  if(!AddInStudy.isEmpty())
  //    Settings_AddInStudy = AddInStudy.toInt();
  //  else
  
  Settings_AddInStudy = 1;
  Mb->setItemChecked(411, Settings_AddInStudy);

  // step value 
  QString S = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  if(S.isEmpty())
    QAD_CONFIG->addSetting("Geometry:SettingsGeomStep", "100");

  // isos 
  int count = ActiveStudy->getStudyFramesCount();
  for(int i = 0; i < count; i++) {
    if(ActiveStudy->getStudyFrame(i)->getTypeView() == VIEW_OCC) {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)ActiveStudy->getStudyFrame(i)->getRightFrame()->getViewFrame())->getViewer();
      Handle (AIS_InteractiveContext) ic = v3d->getAISContext();

      QString IsoU = QAD_CONFIG->getSetting("Geometry:SettingsIsoU");
      QString IsoV = QAD_CONFIG->getSetting("Geometry:SettingsIsoV");
      if(!IsoU.isEmpty())
	ic->DefaultDrawer()->UIsoAspect()->SetNumber(IsoU.toInt());
      if(!IsoV.isEmpty())
	ic->DefaultDrawer()->VIsoAspect()->SetNumber(IsoV.toInt());
    }
  }

  setActionsEnabled();

  // PAL5356: update VTK selection
  ::UpdateVtkSelection();
  bool bOk = true;
  GUIMap::Iterator it;
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it )
    bOk = bOk && it.data()->SetSettings( parent );
    
  // MZN: Enable/disable "Clipping range" menu item(from GEOM_CLIPPING variable)	
  if (getenv( "GEOM_CLIPPING" ) == NULL)
    {
      QMenuItem* mi = Mb->findItem(50);
      if (mi && mi->popup())
      mi->popup()->removeItem(507);    	
    } 
    
  return bOk;
}
*/

//=======================================================================
// function : createGeomAction
// purpose  : 
//=======================================================================
void GeometryGUI::createGeomAction( const int id, const QString& po_id, const QString& icon_id, const int key, const bool toggle  )
{
  QIconSet icon;
  QWidget* parent = application()->desktop();
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  QPixmap pix;
  if ( icon_id.length() ) 
    pix = resMgr->loadPixmap( "GEOM", tr( icon_id ) );
  else
    pix = resMgr->loadPixmap( "GEOM", tr( QString( "ICO_" )+po_id ), false );
  if ( !pix.isNull() )
    icon = QIconSet( pix );

  QString tooltip    = tr( QString( "TOP_" )+po_id ),
          menu       = tr( QString( "MEN_" )+po_id ),
          status_bar = tr( QString( "STB_" )+po_id );

  createAction( id, tooltip, icon, menu, status_bar, key, parent, toggle, this, SLOT( OnGUIEvent() )  );
}



//=======================================================================
// function : GeometryGUI::Deactivate()
// purpose  : Called when GEOM module is deactivated [ static ]
//=======================================================================
void GeometryGUI::initialize( CAM_Application* app )
{
  SalomeApp_Module::initialize( app );

  // ----- create actions --------------

  createGeomAction( 111, "IMPORT", "", (CTRL + Key_I) );
  createGeomAction( 121, "EXPORT", "", (CTRL + Key_E) );

  createGeomAction( 33, "DELETE" );

  createGeomAction( 4011, "POINT" );
  createGeomAction( 4012, "LINE" );
  createGeomAction( 4013, "CIRCLE" );
  createGeomAction( 4014, "ELLIPSE" );
  createGeomAction( 4015, "ARC" );
  createGeomAction( 4019, "CURVE" );
  createGeomAction( 4016, "VECTOR" );
  createGeomAction( 4017, "PLANE" );
  createGeomAction( 4018, "WORK_PLANE" );
  createGeomAction( 4020, "LOCAL_CS" );

  createGeomAction( 4021, "BOX" );
  createGeomAction( 4022, "CYLINDER" );
  createGeomAction( 4023, "SPHERE" );
  createGeomAction( 4024, "TORUS" );
  createGeomAction( 4025, "CONE" );

  createGeomAction( 4031, "EXTRUSION" );
  createGeomAction( 4032, "REVOLUTION" );
  createGeomAction( 4033, "FILLING" );
  createGeomAction( 4034, "PIPE" );

  createGeomAction( 800, "GROUP_CREATE" );
  createGeomAction( 801, "GROUP_EDIT" );

  createGeomAction( 9997, "Q_FACE" );
  createGeomAction( 9999, "HEX_SOLID" );

  createGeomAction( 404, "SKETCH" );
  createGeomAction( 407, "EXPLODE" );

  createGeomAction( 4081, "EDGE" );
  createGeomAction( 4082, "WIRE" );
  createGeomAction( 4083, "FACE" );
  createGeomAction( 4084, "SHELL" );
  createGeomAction( 4085, "SOLID" );
  createGeomAction( 4086, "COMPOUND" );

  createGeomAction( 5011, "FUSE" );
  createGeomAction( 5012, "COMMON" );
  createGeomAction( 5013, "CUT" );
  createGeomAction( 5014, "SECTION" );

  createGeomAction( 5021, "TRANSLATION" );
  createGeomAction( 5022, "ROTATION" );
  createGeomAction( 5023, "MODIFY_LOCATION" );
  createGeomAction( 5024, "MIRROR" );
  createGeomAction( 5025, "SCALE" );
  createGeomAction( 5026, "OFFSET" );
  createGeomAction( 5027, "MUL_TRANSLATION" );
  createGeomAction( 5028, "MUL_ROTATION" );

  createGeomAction( 503, "PARTITION" );
  createGeomAction( 504, "ARCHIMEDE" );
  createGeomAction( 505, "FILLET" );
  createGeomAction( 506, "CHAMFER" );
  //createGeomAction( 507, "CLIPPING" );

  createGeomAction( 9998, "MUL_TRANSFORM" );
  createGeomAction( 9995, "EXPLODE_BLOCKS" );
  createGeomAction( 99991, "PROPAGATE" );

  createGeomAction( 601, "SEWING" );
  createGeomAction( 602, "GLUE_FACES" );
  createGeomAction( 603, "SUPPRESS_FACES" );
  createGeomAction( 604, "SUPPERSS_HOLES" );
  createGeomAction( 605, "SHAPE_PROCESS" );
  createGeomAction( 606, "CLOSE_CONTOUR" );
  createGeomAction( 607, "SUPPRESS_INT_WIRES" );
  createGeomAction( 608, "POINT_ON_EDGE" );
  createGeomAction( 609, "CHECK_FREE_BNDS" );
  createGeomAction( 610, "CHECK_FREE_FACES" );
  
  createGeomAction( 708, "POINT_COORDS" );
  createGeomAction( 701, "BASIC_PROPS" );
  createGeomAction( 702, "MASS_CENTER" );
  createGeomAction( 703, "INERTIA" );
  createGeomAction( 7041, "BND_BOX" );
  createGeomAction( 7042, "MIN_DIST" );

  createGeomAction( 705, "TOLERANCE" );
  createGeomAction( 706, "WHAT_IS" );
  createGeomAction( 707, "CHECK" );
  createGeomAction( 7072, "CHECK_COMPOUND" );

  createGeomAction( 5103, "CHECK_GEOMETRY" );
  
  createGeomAction( 412, "SHADING_COLOR" );
  createGeomAction( 413, "ISOS" );
  createGeomAction( 414, "STEP_VALUE" );

  createGeomAction( 211, "SHADING" );
  createGeomAction( 212, "DISPLAY_ALL" );
  createGeomAction( 214, "ERASE_ALL" );
  createGeomAction( 216, "DISPLAY" );
  createGeomAction( 213, "DISPLAY_ONLY" );
  createGeomAction( 215, "ERASE" );

  createGeomAction( 901, "POP_RENAME" );
  createGeomAction( 80311, "POP_WIREFRAME", "", 0, true );
  createGeomAction( 80312, "POP_SHADING", "", 0, true );
  createGeomAction( 8032, "POP_COLOR" );
  createGeomAction( 8033, "POP_TRANSPARENCY" );
  createGeomAction( 8034, "POP_ISOS" );
  createGeomAction( 8001, "POP_CREATE_GROUP" );

  // make wireframe-shading items to be exclusive (only one at a time is selected)
  //QActionGroup* dispModeGr = new QActionGroup( this, "", true );
  //dispModeGr->add( action( 80311 ) );
  //dispModeGr->add( action( 80312 ) );
  // ---- create menu --------------------------

  int fileId = createMenu( tr( "MEN_FILE" ), -1, -1 );
  createMenu( separator(), fileId, 10 );
  createMenu( 111, fileId, 10 );
  createMenu( 121, fileId, 10 );
  createMenu( separator(), fileId, -1 );

  int editId = createMenu( tr( "MEN_EDIT" ), -1, -1 );
  createMenu( 33, editId, -1 );

  int newEntId = createMenu( tr( "MEN_NEW_ENTITY" ), -1, -1, 10 );

  int basicId = createMenu( tr( "MEN_BASIC" ), newEntId, -1 );
  createMenu( 4011, basicId, -1 );
  createMenu( 4012, basicId, -1 );
  createMenu( 4013, basicId, -1 );
  createMenu( 4014, basicId, -1 );
  createMenu( 4015, basicId, -1 );
  createMenu( 4019, basicId, -1 );
  createMenu( separator(), basicId, -1 );
  createMenu( 4016, basicId, -1 );
  createMenu( 4017, basicId, -1 );
  createMenu( 4018, basicId, -1 );
  createMenu( 4020, basicId, -1 );

  int primId = createMenu( tr( "MEN_PRIMITIVES" ), newEntId, -1 );
  createMenu( 4021, primId, -1 );  
  createMenu( 4022, primId, -1 );  
  createMenu( 4023, primId, -1 );  
  createMenu( 4024, primId, -1 );  
  createMenu( 4025, primId, -1 );  

  int genId = createMenu( tr( "MEN_GENERATION" ), newEntId, -1 );
  createMenu( 4031, genId, -1 );  
  createMenu( 4032, genId, -1 );  
  createMenu( 4033, genId, -1 );  
  createMenu( 4034, genId, -1 );  
  createMenu( separator(), newEntId, -1 );

  int groupId = createMenu( tr( "MEN_GROUP" ), newEntId, -1 );
  createMenu( 800, groupId, -1 );  
  createMenu( 801, groupId, -1 );  
  createMenu( separator(), newEntId, -1 );

  int blocksId = createMenu( tr( "MEN_BLOCKS" ), newEntId, -1 );
  createMenu( 9997, blocksId, -1 );  
  createMenu( 9999, blocksId, -1 );  

  createMenu( separator(), newEntId, -1 );
  createMenu( 404, newEntId, -1 );  
  createMenu( separator(), newEntId, -1 );
  createMenu( 407, newEntId, -1 );  

  int buildId = createMenu( tr( "MEN_BUILD" ), newEntId, -1 );
  createMenu( 4081, buildId, -1 );  
  createMenu( 4082, buildId, -1 );  
  createMenu( 4083, buildId, -1 );  
  createMenu( 4084, buildId, -1 );  
  createMenu( 4085, buildId, -1 );  
  createMenu( 4086, buildId, -1 );  

  int operId = createMenu( tr( "MEN_OPERATIONS" ), -1, -1, 10 );

  int boolId = createMenu( tr( "MEN_BOOLEAN" ), operId, -1 );
  createMenu( 5011, boolId, -1 );  
  createMenu( 5012, boolId, -1 );  
  createMenu( 5013, boolId, -1 );  
  createMenu( 5014, boolId, -1 );  

  int transId = createMenu( tr( "MEN_TRANSFORMATION" ), operId, -1 );
  createMenu( 5021, transId, -1 );  
  createMenu( 5022, transId, -1 );  
  createMenu( 5023, transId, -1 );  
  createMenu( 5024, transId, -1 );  
  createMenu( 5025, transId, -1 );  
  createMenu( 5026, transId, -1 );  
  createMenu( separator(), transId, -1 );
  createMenu( 5027, transId, -1 );  
  createMenu( 5028, transId, -1 );  

  createMenu( 503, operId, -1 );  
  createMenu( 504, operId, -1 );  
  createMenu( separator(), operId, -1 );
  createMenu( 505, transId, -1 );  
  createMenu( 506, transId, -1 );  
  //createMenu( 507, transId, -1 );  

  int blockId = createMenu( tr( "MEN_BLOCKS" ), operId, -1 );
  createMenu( 9998, blockId, -1 );  
  createMenu( 9995, blockId, -1 );  
  createMenu( 99991, blockId, -1 );  

  int repairId = createMenu( tr( "MEN_REPAIR" ), -1, -1, 10 );
  createMenu( 605, repairId, -1 );  
  createMenu( 603, repairId, -1 );  
  createMenu( 606, repairId, -1 );  
  createMenu( 607, repairId, -1 );  
  createMenu( 604, repairId, -1 );  
  createMenu( 601, repairId, -1 );  
  createMenu( 602, repairId, -1 );  
  createMenu( 608, repairId, -1 );  
  createMenu( 609, repairId, -1 );  
  createMenu( 610, repairId, -1 );  

  int measurId = createMenu( tr( "MEN_MEASURES" ), -1, -1, 10 );
  createMenu( 708, measurId, -1 );  
  createMenu( 701, measurId, -1 );  
  createMenu( separator(), measurId, -1 );
  createMenu( 702, measurId, -1 );  
  createMenu( 703, measurId, -1 );  
  createMenu( separator(), measurId, -1 );

  int dimId = createMenu( tr( "MEN_DIMENSIONS" ), measurId, -1 );
  createMenu( 7041, dimId, -1 );  
  createMenu( 7042, dimId, -1 );
  createMenu( separator(), measurId, -1 );
  
  createMenu( 705, measurId, -1 );  
  createMenu( separator(), measurId, -1 );
  createMenu( 706, measurId, -1 );  
  createMenu( 707, measurId, -1 );  
  createMenu( 7072, measurId, -1 );  

  int toolsId = createMenu( tr( "MEN_TOOLS" ), -1, -1, 50 );
  createMenu( separator(), toolsId, -1 );
  createMenu( 5103, toolsId, -1 );  
  
  //int prefId = createMenu( tr( "MEN_PREFERENCES" ), -1, -1, 50 );
  //createMenu( separator(), prefId, -1 );
  //int geomId = createMenu( tr( "MEN_PREFERENCES_GEOM" ), prefId, -1 );
  //createMenu( 412, geomId, -1 );  
  //createMenu( 413, geomId, -1 );  
  //createMenu( 414, geomId, -1 );  
  //createMenu( separator(), prefId, -1 );

  int viewId = createMenu( tr( "MEN_VIEW" ), -1, -1 );
  createMenu( separator(), viewId, -1 );

  int dispmodeId = createMenu( tr( "MEN_DISPLAY_MODE" ), viewId, -1 );
  createMenu( 211, dispmodeId, -1 );  
  
  createMenu( separator(), viewId, -1 );
  createMenu( 212, viewId, -1 );  
  createMenu( 214, viewId, -1 );  
  createMenu( separator(), viewId, -1 );

/*
  PAL9111:
  because of these items are accessible through object browser and viewers
  we have removed they from main menu

  createMenu( 216, viewId, -1 );  
  createMenu( 213, viewId, -1 );  
  createMenu( 215, viewId, -1 );
*/

  // ---- create toolbars --------------------------

  int basicTbId = createTool( tr( "TOOL_BASIC" ) );
  createTool( 4011, basicTbId );
  createTool( 4012, basicTbId );
  createTool( 4013, basicTbId );
  createTool( 4014, basicTbId );
  createTool( 4015, basicTbId );
  createTool( 4019, basicTbId );
  createTool( 4016, basicTbId );
  createTool( 4017, basicTbId );
  createTool( 4018, basicTbId );
  createTool( 4020, basicTbId );

  int primTbId = createTool( tr( "TOOL_PRIMITIVES" ) );
  createTool( 4021, primTbId );  
  createTool( 4022, primTbId );  
  createTool( 4023, primTbId );  
  createTool( 4024, primTbId );  
  createTool( 4025, primTbId );  

  int boolTbId = createTool( tr( "TOOL_BOOLEAN" ) );
  createTool( 5011, boolTbId );  
  createTool( 5012, boolTbId );  
  createTool( 5013, boolTbId );  
  createTool( 5014, boolTbId );  

  int genTbId = createTool( tr( "TOOL_GENERATION" ) );
  createTool( 4031, genTbId );  
  createTool( 4032, genTbId );  
  createTool( 4033, genTbId );  
  createTool( 4034, genTbId );  

  int transTbId = createTool( tr( "TOOL_TRANSFORMATION" ) );
  createTool( 5021, transTbId );  
  createTool( 5022, transTbId );  
  createTool( 5023, transTbId );  
  createTool( 5024, transTbId );  
  createTool( 5025, transTbId );  
  createTool( 5026, transTbId );  
  createTool( separator(), transTbId );
  createTool( 5027, transTbId );  
  createTool( 5028, transTbId );

  QtxPopupMgr* mgr = popupMgr();
  mgr->insert( action(  901 ), -1, -1 ); // rename
  mgr->setRule( action( 901 ), "$type in {'Shape' 'Group'} and selcount=1", true );
  mgr->insert( action(  8001 ), -1, -1 ); // create group
  mgr->setRule( action( 8001 ), "client='ObjectBrowser' and type='Shape' and selcount=1 and isOCC=true", true );
  mgr->insert( action(  801 ), -1, -1 ); // edit group
  mgr->setRule( action( 801 ),  "client='ObjectBrowser' and type='Group' and selcount=1 and isOCC=true", true );
  mgr->insert( separator(), -1, -1 );        // -----------
  dispmodeId = mgr->insert(  tr( "MEN_DISPLAY_MODE" ), -1, -1 ); // display mode menu
  mgr->insert( action(  80311 ), dispmodeId, -1 ); // wireframe
  mgr->setRule( action( 80311 ), "(client='OCCViewer' or client='VTKViewer') and selcount>0 and isVisible", true );
  mgr->setRule( action( 80311 ), "(client='OCCViewer' or client='VTKViewer') and displaymode='Wireframe'", false );
  mgr->insert( action(  80312 ), dispmodeId, -1 ); // shading
  mgr->setRule( action( 80312 ), "(client='OCCViewer' or client='VTKViewer') and selcount>0 and isVisible", true );
  mgr->setRule( action( 80312 ), "(client='OCCViewer' or client='VTKViewer') and displaymode='Shading'", false );
  mgr->insert( separator(), -1, -1 );        // -----------
  mgr->insert( action(  8032 ), -1, -1 ); // color
  mgr->setRule( action( 8032 ), "(client='OCCViewer' or client='VTKViewer') and selcount>0 and isVisible", true );
  mgr->insert( action(  8033 ), -1, -1 ); // transparency
  mgr->setRule( action( 8033 ), "(client='OCCViewer' or client='VTKViewer') and selcount>0 and isVisible", true );
  mgr->insert( action(  8034 ), -1, -1 ); // isos
  mgr->setRule( action( 8034 ), "client='OCCViewer' and selcount>0 and isVisible", true );
  mgr->insert( separator(), -1, -1 );        // -----------
  


  QString canDisplay = "($component={'GEOM'}) and (selcount>0) and ({true} in $canBeDisplayed) ",
          onlyComponent = "((type='Component') and selcount=1)",
          rule = canDisplay + "and ((($type in {%1}) and( %2 )) or " + onlyComponent + ")",
          types = "'Shape' 'Group'";

  mgr->insert( action(  216 ), -1, -1 ); // display
  mgr->setRule( action( 216 ), rule.arg( types ).arg( "not isVisible" ), true );

  mgr->insert( action(  215 ), -1, -1 ); // erase
  mgr->setRule( action( 215 ), rule.arg( types ).arg( "isVisible" ), true );

  mgr->insert( action(  214 ), -1, -1 ); // erase All
  mgr->setRule( action( 214 ), "client='OCCViewer' or client='VTKViewer'", true );

  mgr->insert( action(  213 ), -1, -1 ); // display only
  mgr->setRule( action( 213 ), rule.arg( types ).arg( "true" ), true );
  mgr->insert( separator(), -1, -1 );

  mgr->hide( mgr->actionId( action( myEraseAll ) ) );
}

//=======================================================================
// function : GeometryGUI::Deactivate()
// purpose  : Called when GEOM module is deactivated [ static ]
//=======================================================================
bool GeometryGUI::activateModule( SUIT_Study* study )
{
  if ( CORBA::is_nil( myComponentGeom ) )
    return false;

  bool res = SalomeApp_Module::activateModule( study );

  if ( !res )
    return false;

  setMenuShown( true );
  setToolShown( true );

  connect( application()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ), 
	  this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ) );

  GUIMap::Iterator it;
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it )
    it.data()->activate( application()->desktop() );

  LightApp_SelectionMgr* sm = getApp()->selectionMgr();
  SUIT_ViewManager* vm;
  ViewManagerList OCCViewManagers, VTKViewManagers;
  application()->viewManagers( OCCViewer_Viewer::Type(), OCCViewManagers );
  for ( vm = OCCViewManagers.first(); vm; vm = OCCViewManagers.next() )
    myOCCSelectors.append( new GEOMGUI_OCCSelector( ((OCCViewer_ViewManager*)vm)->getOCCViewer(), sm ) );
  application()->viewManagers( SVTK_Viewer::Type(), VTKViewManagers );
  for ( vm = VTKViewManagers.first(); vm; vm = VTKViewManagers.next() )
    myVTKSelectors.append( new LightApp_VTKSelector( dynamic_cast<SVTK_Viewer*>( vm->getViewModel() ), sm ) );

  // disable OCC selectors
  getApp()->selectionMgr()->setEnabled( false, OCCViewer_Viewer::Type() );
  for ( GEOMGUI_OCCSelector* sr = myOCCSelectors.first(); sr; sr = myOCCSelectors.next() )
    sr->setEnabled(true);
  
  // disable VTK selectors
  getApp()->selectionMgr()->setEnabled( false, SVTK_Viewer::Type() );
  for ( LightApp_VTKSelector* sr = myVTKSelectors.first(); sr; sr = myVTKSelectors.next() )
    sr->setEnabled(true);
  
  return true;
}


//=======================================================================
// function : GeometryGUI::Deactivate()
// purpose  : Called when GEOM module is deactivated [ static ]
//=======================================================================
bool GeometryGUI::deactivateModule( SUIT_Study* study )
{
  setMenuShown( false );
  setToolShown( false );

  disconnect( application()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ), 
	     this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ) );

  EmitSignalCloseAllDialogs();

  GUIMap::Iterator it;
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it )
    it.data()->deactivate();  

  myOCCSelectors.clear();
  getApp()->selectionMgr()->setEnabled( true, OCCViewer_Viewer::Type() );

  myVTKSelectors.clear();
  getApp()->selectionMgr()->setEnabled( true, SVTK_Viewer::Type() );

  return SalomeApp_Module::deactivateModule( study );
}

//=================================================================================
// function : GeometryGUI::DefinePopup()
// purpose  : Called from desktop to define popup menu [static]
//=================================================================================
/*
void GeometryGUI::DefinePopup(QString& theContext, QString& theParent, QString& theObject)
{
  QAD_Study* ActiveStudy = QAD_Application::getDesktop()->getActiveStudy();
  SALOME_Selection* Sel  = SALOME_Selection::Selection(ActiveStudy->getSelection());

  theObject  = "";
  theContext = "";

  if ( theParent == "Viewer" ) {
    if ( Sel->IObjectCount() == 0 )
      theContext = "NothingSelected";
  }

  if ( Sel->IObjectCount() == 1 ) {
    Handle(SALOME_InteractiveObject) IO = Sel->firstIObject();
    if( IO->hasEntry() ) {
      SALOMEDS::SObject_var sobj = ActiveStudy->getStudyDocument()->FindObjectID( IO->getEntry() );
      if ( !sobj->_is_nil() ) {
	SALOMEDS::SComponent_var scomp = sobj->GetFatherComponent();
	if ( !strcmp(scomp->GetID(), IO->getEntry() ) ) {
	  // component is selected
	  theObject = "Component";
	}
	else {
	  GEOM::GEOM_Object_var aGObj = GEOM::GEOM_Object::_narrow( sobj->GetObject() );
	  if ( !CORBA::is_nil( aGObj ) ) {
	    switch( aGObj->GetType() ) {
	    case GEOM_GROUP:
	      theObject = "Group";
	      break;
	    default:
	      theObject = "Shape";
	      break;
	    }
	  }
	}
      }
    }
  }
}

//=================================================================================
// function : GeometryGUI::CustomPopup()
// purpose  : Called from desktop to create popup menu [static]
//=================================================================================
bool GeometryGUI::CustomPopup(QAD_Desktop* parent, QPopupMenu* popup, const QString& theContext,
			      const QString& theParent, const QString& theObject)
{
  GeometryGUI* geomGUI = GetGeomGUI();

  // Deactivate any non modal dialog box to get the neutral point
  geomGUI->EmitSignalDeactivateDialog();
  QAD_Study* anActiveStudy    = parent->getActiveStudy();
  QAD_StudyFrame* aStudyFrame = anActiveStudy->getActiveStudyFrame();
  QAD_ViewFrame* aViewFrame   = aStudyFrame->getRightFrame()->getViewFrame();
  SALOME_Selection* Sel       = SALOME_Selection::Selection(anActiveStudy->getSelection());
  QString parentComponent     = ((SALOMEGUI_Desktop*)parent)->getComponentFromSelection();
  bool isOCCViewer            = aViewFrame->getTypeView() == VIEW_OCC;
  bool isVTKViewer            = aViewFrame->getTypeView() == VIEW_VTK;
  int aDisplayMode            = 0;
  QString objectName;

  if ( aViewFrame->getTypeView() == VIEW_OCC )
    aDisplayMode = ((OCCViewer_ViewFrame*)aViewFrame)->getViewer()->getAISContext()->DisplayMode();
  else if ( aViewFrame->getTypeView() == VIEW_VTK )
    aDisplayMode = (dynamic_cast<SVTK_ViewFrame*>( aViewFrame )->getRWInteractor()->GetDisplayMode();

  int nbSel = Sel->IObjectCount();

  if( nbSel == 0 ) {
    ////// NOTHING SELECTED
    popup->clear();
  } 
  else if ( nbSel == 1 ) {
    ////// SINGLE OBJECT SELECTION
    if ( parentComponent != parent->getActiveComponent() )  {
      ////// selected object does not belong to GEOM module:
      // remove all commands except Display/Erase...
      while ( 1 ) {
	int id = popup->idAt( 0 );
	if ( id <= QAD_TopLabel_Popup_ID )
	  popup->removeItemAt( 0 );
	else
	  break;
      }
    }
    else {
      ////// selected object belong to the GEOM module
      // get interactive object
      Handle(SALOME_InteractiveObject) IObject = Sel->firstIObject();
      objectName = IObject->getName();
      // if object has entry get SObject
      SALOMEDS::SObject_var SO;
      if ( IObject->hasEntry() )
	SO = anActiveStudy->getStudyDocument()->FindObjectID( IObject->getEntry() );

      if ( theObject == "Component" ) {
	////// menu for component
	if ( !isOCCViewer && !isVTKViewer ) {
	  popup->removeItem( QAD_DisplayOnly_Popup_ID );
	  popup->removeItem( QAD_Display_Popup_ID );
	  popup->removeItem( QAD_Erase_Popup_ID );
	}
      }
      else {
	////// not component (should be shape)
	if ( IObject->hasEntry() )  /////// VSR : TO BE REMOVED
	  popup->removeItem( 804 ); // "Add in Study"

	// Here could be analysis of the geom shape's type
	// ... //

	SALOMEDS::GenericAttribute_var aTmpAttr;
	if( SO->_is_nil() || SO->GetFatherComponent()->FindAttribute( aTmpAttr, "AttributeIOR") )
	  popup->removeItem( 9024 ); // "Open" /////// VSR : TO BE REMOVED

	if ( !isOCCViewer && theParent == "ObjectBrowser" ) {
	  if ( theObject == "Shape" )
	    popup->removeItem( 800 ); // Create Group
	  else if ( theObject == "Group" )
	    popup->removeItem( 801 ); // Edit Group
	}

	if ( isOCCViewer || isVTKViewer ) {
	  ////// current viewer is OCC or VTK
	  SALOME_Prs* prs = aViewFrame->CreatePrs( IObject->getEntry() );
	  if ( aViewFrame->isVisible( IObject ) ) {
	    ////// object is already displayed in the viewer
	    popup->removeItem( QAD_Display_Popup_ID );
	    if ( isOCCViewer ) {
	      ////// OCC viewer only
	      OCCViewer_Prs* occPrs = dynamic_cast<OCCViewer_Prs*>( prs );
	      if ( occPrs && !occPrs->IsNull() ) {
		AIS_ListOfInteractive ioList;
		occPrs->GetObjects( ioList );
		QMenuItem* mi = popup->findItem( 803 );
		if ( mi && mi->popup() ) {
		  if ( ioList.First()->DisplayMode() == 0 )
		    mi->popup()->setItemChecked( 80311, true ); // "Wireframe"
		  else if ( ioList.First()->DisplayMode() == 1 )
		    mi->popup()->setItemChecked( 80312, true ); // "Shading"
		  else if ( ioList.First()->DisplayMode() < 0 )
		    mi->popup()->setItemChecked( aDisplayMode == 0 ? 80311 : 80312 , true ); // "Wireframe" or "Shading"
		}
	      }
	    }
	    else {
	      ////// VTK viewer only
	      popup->removeItem( 8034 ); // "Isos"
	      SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( prs );
	      if ( vtkPrs && !vtkPrs->IsNull() ) {
		vtkActorCollection* actorList = vtkPrs->GetObjects();
		actorList->InitTraversal();
		SALOME_Actor* ac = SALOME_Actor::SafeDownCast( actorList->GetNextActor() );
		QMenuItem* mi = popup->findItem( 803 );
		if ( ac && mi && mi->popup() ) {
		  if ( ac->getDisplayMode() == 0 )
		    mi->popup()->setItemChecked( 80311, true ); // "Wireframe"
		  else if ( ac->getDisplayMode() == 1 )
		    mi->popup()->setItemChecked( 80312, true ); // "Shading"
		  else
		    mi->popup()->setItemChecked( aDisplayMode == 0 ? 80311 : 80312 , true ); // "Wireframe" or "Shading"
		}
	      }
	    }
	  }
	  else {
	    ////// object is not yet displayed in the viewer
	    popup->removeItem( 803 );  // "Display Mode"
	    popup->removeItem( 8032 ); // "Color"
	    popup->removeItem( 8033 ); // "Transparency"
	    popup->removeItem( 8034 ); // "Isos"
	    popup->removeItem( QAD_Erase_Popup_ID );
	  }
	  delete prs;
	}
	else {
	  ////// other viewer type (neither OCC nor VTK)
	  popup->removeItem( 803 );  // "Display Mode"
	  popup->removeItem( 8032 ); // "Color"
	  popup->removeItem( 8033 ); // "Transparency"
	  popup->removeItem( 8034 ); // "Isos"
	  popup->removeItem( QAD_Display_Popup_ID );
	  popup->removeItem( QAD_DisplayOnly_Popup_ID );
	  popup->removeItem( QAD_Erase_Popup_ID );
	}
      }
    }
  }
  else {
    ////// MULTIPLE SELECTION
    if ( parentComponent != parent->getActiveComponent() )  {
      ////// not GEOM module objects or objects belong to different modules
      // remove all commands except Display/Erase...
      while ( 1 ) {
	int id = popup->idAt( 0 );
	if ( id <= QAD_TopLabel_Popup_ID )
	  popup->removeItemAt( 0 );
	else
	  break;
      }
      if ( parentComponent.isNull() )  {
	////// objects belong to different modules
	popup->removeItem(QAD_Display_Popup_ID);
	popup->removeItem(QAD_DisplayOnly_Popup_ID);
	popup->removeItem(QAD_Erase_Popup_ID);
      }
      else {
	objectName = tr( "GEOM_MEN_POPUP_NAME" ).arg( nbSel );
      }
    }
    else {
      ////// all selected objects belong to GEOM module
      popup->removeItem( 901 ); // "Rename"

      SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
      bool isComponent  = false;
      bool needOpen     = false;
      bool needDisplay  = false;
      bool needErase    = false;
      int needToPublish = 0;

      for( ;It.More();It.Next() ) {
	Handle(SALOME_InteractiveObject) anIObject = It.Value();

	if ( aViewFrame->isVisible( anIObject ) )
	  needErase   = true;
	else
	  needDisplay = true;

	if( anIObject->hasEntry() ) {
	  needToPublish = -1; /////// VSR : TO BE REMOVED
	  SALOMEDS::SObject_var obj = anActiveStudy->getStudyDocument()->FindObjectID( anIObject->getEntry() );
	  SALOMEDS::GenericAttribute_var aTmpAttr;
	  if ( !obj->_is_nil() && !obj->GetFatherComponent()->FindAttribute( aTmpAttr, "AttributeIOR" ) )
	    needOpen = true;  /////// VSR : TO BE REMOVED
	  if ( !obj->_is_nil() && QString( obj->GetID() ) == QString( obj->GetFatherComponent()->GetID() ) )
	    isComponent = true;
	}
	else {
	  if ( needToPublish != -1 ) needToPublish = 1;
	}
      }
      if( needOpen || ( !isOCCViewer && !isVTKViewer ) ) {
	////// Data is not loaded yet or current viewer is neither OCC nor VTK
	popup->removeItem( 803 );  // "Display Mode"
	popup->removeItem( 8032 ); // "Color"
	popup->removeItem( 8033 ); // "Transparency"
	popup->removeItem( 8034 ); // "Isos"
	popup->removeItem( 804 );  // "Add in Study"
	popup->removeItem( QAD_DisplayOnly_Popup_ID );
	popup->removeItem( QAD_Display_Popup_ID );
	popup->removeItem( QAD_Erase_Popup_ID );
      }
      else {
	popup->removeItem( 9024 );   // "Open"
	if ( needToPublish <= 0 )
	  popup->removeItem( 804 );  // "Add in Study"

	if( isComponent ) {
	  popup->removeItem( 803 );  // "Display Mode"
	  popup->removeItem( 8032 ); // "Color"
	  popup->removeItem( 8033 ); // "Transparency"
	  popup->removeItem( 8034 ); // "Isos"
	  popup->removeItem( QAD_DisplayOnly_Popup_ID );
	}

	if ( !needDisplay )
	  popup->removeItem( QAD_Display_Popup_ID );
	if ( !needErase )
	  popup->removeItem( QAD_Erase_Popup_ID );
	if ( !isOCCViewer )
	  popup->removeItem( 8034 ); // "Isos"
      }
    }
  }

  // check popup for unnecessary separators
  QAD_Tools::checkPopup( popup );
  // find popup menu's TopLabel item (with title)
  int topItem = popup->indexOf( QAD_TopLabel_Popup_ID );
  if ( topItem >= 0 ) {
    // remove popup menu's title item
    popup->removeItem( QAD_TopLabel_Popup_ID );
    if ( theParent == "Viewer" && !objectName.isEmpty() && popup->count() > 0 ) {
      // set bold font for popup menu's title
      QFont f = popup->font(); f.setBold( TRUE );
      popup->removeItem( QAD_TopLabel_Popup_ID );
      popup->insertItem( new CustomItem( objectName, f ), QAD_TopLabel_Popup_ID, topItem );
    }
  }

  return false;
}

*/

//=======================================================================
// function : GeometryGUI::BuildPresentation()
// purpose  : 
//=======================================================================
void GeometryGUI::BuildPresentation( const Handle(SALOME_InteractiveObject)& io, SUIT_ViewWindow* win )
{
  //GEOM_Displayer().Display( io, false, win );
}

//=======================================================================
// function : setCommandsEnabled()
// purpose  : update menu items' status - disable non-OCC-viewer-compatible actions
//=======================================================================
void GeometryGUI::onWindowActivated( SUIT_ViewWindow* win )
{
  if ( !win )
    return;

  const bool ViewOCC = ( win->getViewManager()->getType() == OCCViewer_Viewer::Type() );
//  const bool ViewVTK = ( win->getViewManager()->getType() == SVTK_Viewer::Type() );
  
  // disable non-OCC viewframe menu commands
//  action( 404 )->setEnabled( ViewOCC ); // SKETCHER
  action( 603 )->setEnabled( ViewOCC ); // SuppressFace
  action( 604 )->setEnabled( ViewOCC ); // SuppressHole
  action( 606 )->setEnabled( ViewOCC ); // CloseContour
  action( 607 )->setEnabled( ViewOCC ); // RemoveInternalWires
  action( 608 )->setEnabled( ViewOCC ); // AddPointOnEdge
//  action( 609 )->setEnabled( ViewOCC ); // Free boundaries
  action( 413 )->setEnabled( ViewOCC ); // Isos Settings

  action( 800 )->setEnabled( ViewOCC ); // Create Group
  action( 801 )->setEnabled( ViewOCC ); // Edit Group

  action( 9998 )->setEnabled( ViewOCC ); // MENU BLOCKS - MULTI-TRANSFORMATION
}

void GeometryGUI::windows( QMap<int, int>& mappa ) const
{
  mappa.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::DockLeft );
  mappa.insert( SalomeApp_Application::WT_PyConsole, Qt::DockBottom );
}

void GeometryGUI::viewManagers( QStringList& lst ) const
{
  lst.append( OCCViewer_Viewer::Type() );
}

void GeometryGUI::onViewManagerAdded( SUIT_ViewManager* vm )
{
  if ( vm->getType() == OCCViewer_Viewer::Type() )
  {
    qDebug( "connect" );
    connect( vm, SIGNAL( keyPress  ( SUIT_ViewWindow*, QKeyEvent* ) ),
	     this, SLOT( OnKeyPress( SUIT_ViewWindow*, QKeyEvent* ) ) );
    connect( vm, SIGNAL( mousePress( SUIT_ViewWindow*, QMouseEvent* ) ),
	     this, SLOT( OnMousePress( SUIT_ViewWindow*, QMouseEvent* ) ) );
    connect( vm, SIGNAL( mouseMove ( SUIT_ViewWindow*, QMouseEvent* ) ),
	     this, SLOT( OnMouseMove( SUIT_ViewWindow*, QMouseEvent* ) ) );


    LightApp_SelectionMgr* sm = getApp()->selectionMgr();
    myOCCSelectors.append( new GEOMGUI_OCCSelector( ((OCCViewer_ViewManager*)vm)->getOCCViewer(), sm ) );

    // disable OCC selectors
    getApp()->selectionMgr()->setEnabled( false, OCCViewer_Viewer::Type() );
    for ( GEOMGUI_OCCSelector* sr = myOCCSelectors.first(); sr; sr = myOCCSelectors.next() )
      sr->setEnabled(true);
  }
  else if ( vm->getType() == SVTK_Viewer::Type() )
  {
    LightApp_SelectionMgr* sm = getApp()->selectionMgr();
    myVTKSelectors.append( new LightApp_VTKSelector( dynamic_cast<SVTK_Viewer*>( vm->getViewModel() ), sm ) );

    // disable VTK selectors
    getApp()->selectionMgr()->setEnabled( false, SVTK_Viewer::Type() );
    for ( LightApp_VTKSelector* sr = myVTKSelectors.first(); sr; sr = myVTKSelectors.next() )
      sr->setEnabled(true);
  }
}

void GeometryGUI::onViewManagerRemoved( SUIT_ViewManager* vm )
{
  SUIT_ViewModel* viewer = vm->getViewModel();
  if ( vm->getType() == OCCViewer_Viewer::Type() )
  {
    for ( GEOMGUI_OCCSelector* sr = myOCCSelectors.first(); sr; sr = myOCCSelectors.next() )
      if ( sr->viewer() == viewer )
      {
	myOCCSelectors.remove( sr );
	break;
      }
  }
  if ( vm->getType() == SVTK_Viewer::Type() )
  {
    for ( LightApp_VTKSelector* sr = myVTKSelectors.first(); sr; sr = myVTKSelectors.next() )
      if ( sr->viewer() == viewer )
      {
	myVTKSelectors.remove( sr );
	break;
      }
  }
}

QString GeometryGUI::engineIOR() const
{
  if ( !CORBA::is_nil( GetGeomGen() ) )
    return QString( getApp()->orb()->object_to_string( GetGeomGen() ) );
  return QString( "" );
}

LightApp_Selection* GeometryGUI::createSelection() const
{
  return new GEOMGUI_Selection();
}

void GeometryGUI::contextMenuPopup( const QString& client, QPopupMenu* menu, QString& title )
{
  SalomeApp_Module::contextMenuPopup( client, menu, title );
  SALOME_ListIO lst;
  getApp()->selectionMgr()->selectedObjects( lst );
  if ( ( client == "OCCViewer" || client == "VTKViewer" ) && lst.Extent() == 1 ) {
    Handle(SALOME_InteractiveObject) io = lst.First();
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( application()->activeStudy() );
    _PTR(Study) study = appStudy->studyDS();
    _PTR(SObject) obj = study->FindObjectID( io->getEntry() );
    if ( obj )
      title = QString( obj->GetName().c_str() );
  }
}

void GeometryGUI::createPreferences()
{
  int tabId = addPreference( tr( "PREF_TAB_SETTINGS" ) );

  int genGroup = addPreference( tr( "PREF_GROUP_GENERAL" ), tabId );
  addPreference( tr( "PREF_SHADING_COLOR" ), genGroup,
		 LightApp_Preferences::Color, "Geometry", "shading_color" );
  int step = addPreference( tr( "PREF_STEP_VALUE" ), genGroup,
			    LightApp_Preferences::IntSpin, "Geometry", "SettingsGeomStep" );
  int dispmode = addPreference( tr( "PREF_DISPLAY_MODE" ), genGroup,
			    LightApp_Preferences::Selector, "Geometry", "display_mode" );

  setPreferenceProperty( genGroup, "columns", 1 );

  setPreferenceProperty( step, "min", 0.001 );
  setPreferenceProperty( step, "max", 10000 );
  setPreferenceProperty( step, "precision", 3 );

  // Set property for default display mode
  QStringList aModesList;
  aModesList.append( tr("MEN_WIREFRAME") );
  aModesList.append( tr("MEN_SHADING") );
  
  QValueList<QVariant> anIndexesList;
  anIndexesList.append(0);
  anIndexesList.append(1);
  
  setPreferenceProperty( dispmode, "strings", aModesList );
  setPreferenceProperty( dispmode, "indexes", anIndexesList );
}

void GeometryGUI::preferencesChanged( const QString& section, const QString& param )
{
}

LightApp_Displayer* GeometryGUI::displayer()
{
  if( !myDisplayer )
    myDisplayer = new GEOM_Displayer( dynamic_cast<SalomeApp_Study*>( getApp()->activeStudy() ) );
  return myDisplayer;
}
