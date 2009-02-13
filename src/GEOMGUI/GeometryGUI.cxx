//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// GEOM GEOMGUI : GUI for Geometry component
// File   : GeometryGUI.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "GeometryGUI.h"
#include "GEOMGUI_OCCSelector.h"
#include "GEOMGUI_Selection.h"
#include "GEOM_Displayer.h"
#include "GEOM_AISShape.hxx"

#include "GEOM_Actor.h"

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>

#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>
#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewManager.h>

#include <SOCC_ViewModel.h>
#include <SOCC_ViewWindow.h>

#include <SVTK_ViewWindow.h>
#include <SVTK_RenderWindowInteractor.h>
#include <SVTK_InteractorStyle.h>
#include <SVTK_ViewModel.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>

#include <LightApp_SelectionMgr.h>
#include <LightApp_VTKSelector.h>
#include <LightApp_DataObject.h>
#include <LightApp_Preferences.h>

#include <SALOME_LifeCycleCORBA.hxx>
#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>

#include <SALOMEDSClient_ClientFactory.hxx>
#include <SALOMEDSClient_IParameters.hxx>

// External includes
#include <QMenu>
#include <QAction>
#include <QFileInfo>
#include <QString>
#include <QPainter>

#include <AIS_Drawer.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <OSD_SharedLibrary.hxx>
#include <NCollection_DataMap.hxx>

#include <utilities.h>

#include <vtkCamera.h>
#include <vtkRenderer.h>

#include "GEOMImpl_Types.hxx"

using namespace Qt;

extern "C" {
  Standard_EXPORT CAM_Module* createModule() {
    return new GeometryGUI();
  }
}



GEOM::GEOM_Gen_var GeometryGUI::myComponentGeom = GEOM::GEOM_Gen::_nil();

GEOM::GEOM_Gen_var GeometryGUI::GetGeomGen()
{
  // Bug 12290: exception in Mesh GUI on GEOMBase::GetShape() if Geometry GUI hasn't been loaded
  if (CORBA::is_nil(myComponentGeom))
    InitGeomGen();
  return GeometryGUI::myComponentGeom;
}

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

//=======================================================================
// function : JoinObjectParameters
// purpose  :
//=======================================================================
char* GeometryGUI::JoinObjectParameters(const QStringList& theParametersList)
{
  return theParametersList.join(":").toLatin1().data();
}
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

  myDisplayer = 0;
  myLocalSelectionMode = GEOM_ALLOBJECTS;
}

//=======================================================================
// function : GeometryGUI::~GeometryGUI()
// purpose  : Destructor
//=======================================================================
GeometryGUI::~GeometryGUI()
{
  while (!myOCCSelectors.isEmpty())
    delete myOCCSelectors.takeFirst();

  while (!myVTKSelectors.isEmpty())
    delete myVTKSelectors.takeFirst();
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
    QString libs;
#ifndef WNT
    libs = getenv( "LD_LIBRARY_PATH" );
    if( !libs.isEmpty() ) {
      QStringList dirList = libs.split( ":", QString::SkipEmptyParts ); // skip empty entries
#else
    libs = getenv( "PATH" );
    if( !libs.isEmpty() ) {
      QStringList dirList = libs.split( ";", QString::SkipEmptyParts ); // skip empty entries
#endif
      for( int i = dirList.count()-1; i >= 0; i-- ) {
	QString dir = dirList[ i ];
	QFileInfo fi( Qtx::addSlash( dirList[ i ] ) + libraryName );
	if( fi.exists() ) {
	  OSD_SharedLibrary aSharedLibrary( fi.fileName().toLatin1().data() );
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
  SUIT_Application* anApp = application();
  if (!anApp) return;
  SUIT_Desktop* desk = anApp->desktop();

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
    SUIT_MessageBox::critical( desk, tr( "GEOM_ERROR" ), tr( "GEOM_ERR_GET_ENGINE" ), tr( "GEOM_BUT_OK" ) );
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
      id == 2171 ||  // POPUP VIEWER - SELECT ONLY - VERTEX
      id == 2172 ||  // POPUP VIEWER - SELECT ONLY - EDGE
      id == 2173 ||  // POPUP VIEWER - SELECT ONLY - WIRE
      id == 2174 ||  // POPUP VIEWER - SELECT ONLY - FACE
      id == 2175 ||  // POPUP VIEWER - SELECT ONLY - SHELL
      id == 2176 ||  // POPUP VIEWER - SELECT ONLY - SOLID
      id == 2177 ||  // POPUP VIEWER - SELECT ONLY - COMPOUND
      id == 2178 ||  // POPUP VIEWER - SELECT ONLY - SELECT ALL
      id == 31   ||  // MENU EDIT - COPY
      id == 33   ||  // MENU EDIT - DELETE
      id == 411  ||  // MENU SETTINGS - ADD IN STUDY
      id == 412  ||  // MENU SETTINGS - SHADING COLOR
      id == 5103 ||  // MENU TOOLS - CHECK GEOMETRY
      id == 8032 ||  // POPUP VIEWER - COLOR
      id == 8033 ||  // POPUP VIEWER - TRANSPARENCY
      id == 8034 ||  // POPUP VIEWER - ISOS
      id == 8035 ||  // POPUP VIEWER - AUTO COLOR
      id == 8036 ||  // POPUP VIEWER - DISABLE AUTO COLOR
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
	   id == 4025 ||  // MENU PRIMITIVE - CONE
	   id == 4026 ||  // MENU PRIMITIVE - FACE
	   id == 4027 ) { // MENU PRIMITIVE - DISK
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
           id == 405 ||   // MENU ENTITY - 3D SKETCHER
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
	   id == 5028 ||  // MENU TRANSFORMATION - MULTI-ROTATION
           id == 5029 ) { // CONTEXT(POPUP) MENU - RELOAD_IMPORTED
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
	   id == 507 ||   // MENU OPERATION - CLIPPING RANGE
	   id == 508 ||   // MENU OPERATION - GET SHAPES ON SHAPE
	   id == 509 ) {  // MENU OPERATION - FILLET 2D
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
           id == 609 ||   // MENU MEASURE - FREE BOUNDARIES
           id == 610 ||   // MENU MEASURE - FREE FACES
           id == 611 ||   // MENU REPAIR - CHANGE ORIENTATION
	   id == 602 ||   // MENU REPAIR - GLUE FACES
	   id == 612 ) {  // MENU REPAIR - REMOVE EXTRA EDGES
#ifndef WNT
	library = getLibrary( "libRepairGUI.so" );
#else
	library = getLibrary( "RepairGUI.dll" );
#endif
  }
  else if( id == 701   ||  // MENU MEASURE - PROPERTIES
	   id == 702   ||  // MENU MEASURE - CDG
	   id == 703   ||  // MENU MEASURE - INERTIA
	   id == 704   ||  // MENU MEASURE - NORMALE
	   id == 7041  ||  // MENU MEASURE - BOUNDING BOX
	   id == 7042  ||  // MENU MEASURE - MIN DISTANCE
	   id == 7043  ||  // MENU MEASURE - ANGLE
	   id == 705   ||  // MENU MEASURE - TOLERANCE
	   id == 706   ||  // MENU MEASURE - WHATIS
	   id == 707   ||  // MENU MEASURE - CHECK
	   id == 7072  ||  // MENU MEASURE - CHECK COMPOUND OF BLOCKS
	   id == 708 ) {   // MENU MEASURE - POINT COORDINATES
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
           id == 9995 ) {  // MENU BLOCKS - EXPLODE ON BLOCKS
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
    SUIT_MessageBox::critical( desk, tr( "GEOM_ERROR" ), tr( "GEOM_ERR_LIB_NOT_FOUND" ), tr( "GEOM_BUT_OK" ) );
}


//=================================================================================
// function : GeometryGUI::OnKeyPress()
// purpose  : Called when any key is pressed by user [static]
//=================================================================================
void GeometryGUI::OnKeyPress( SUIT_ViewWindow* win, QKeyEvent* pe )
{
  GUIMap::Iterator it;
  bool bOk = true;
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it ) {
    SUIT_Application* anApp = application();
    if (!anApp) return;
    bOk = bOk && it.value()->OnKeyPress( pe, anApp->desktop(), win );
  }
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
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it ) {
    SUIT_Application* anApp = application();
    if (!anApp) return;
    bOk = bOk && it.value()->OnMouseMove( pe, anApp->desktop(), win );
  }
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
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it ) {
    SUIT_Application* anApp = application();
    if (!anApp) return;
    processed = processed || it.value()->OnMousePress( pe, anApp->desktop(), win );
  }
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
  QIcon icon;
  QWidget* parent = application()->desktop();
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  QPixmap pix;
  if ( icon_id.length() )
    pix = resMgr->loadPixmap( "GEOM", tr( icon_id.toLatin1().constData() ) );
  else
    pix = resMgr->loadPixmap( "GEOM", tr( (QString( "ICO_" )+po_id).toLatin1().constData() ), false );
  if ( !pix.isNull() )
    icon = QIcon( pix );

  QString tooltip    = tr( (QString( "TOP_" )+po_id).toLatin1().constData() ),
          menu       = tr( (QString( "MEN_" )+po_id).toLatin1().constData() ),
          status_bar = tr( (QString( "STB_" )+po_id).toLatin1().constData() );

  createAction( id, tooltip, icon, menu, status_bar, key, parent, toggle, this, SLOT( OnGUIEvent() )  );
}



//=======================================================================
// function : GeometryGUI::initialize()
// purpose  : Called when GEOM module is created
//=======================================================================
void GeometryGUI::initialize( CAM_Application* app )
{
  SalomeApp_Module::initialize( app );

  // ----- create actions --------------

  createGeomAction( 111, "IMPORT", "", (CTRL + Key_I) );
  createGeomAction( 121, "EXPORT", "", (CTRL + Key_E) );

  createGeomAction( 33, "DELETE", "", Qt::Key_Delete );

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
  createGeomAction( 4026, "FACE" );
  createGeomAction( 4027, "DISK" );

  createGeomAction( 4031, "EXTRUSION" );
  createGeomAction( 4032, "REVOLUTION" );
  createGeomAction( 4033, "FILLING" );
  createGeomAction( 4034, "PIPE" );

  createGeomAction( 800, "GROUP_CREATE" );
  createGeomAction( 801, "GROUP_EDIT" );

  createGeomAction( 5029, "RELOAD_IMPORTED" );

  createGeomAction( 9997, "Q_FACE" );
  createGeomAction( 9999, "HEX_SOLID" );

  createGeomAction( 404, "SKETCH" );
  createGeomAction( 405, "3DSKETCH" );
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
  createGeomAction( 508, "GET_SHAPES_ON_SHAPES" );
  createGeomAction( 509, "FILLET_2D" );

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
  createGeomAction( 611, "CHANGE_ORIENTATION" );
  createGeomAction( 612, "REMOVE_EXTRA_EDGES" );

  createGeomAction( 708, "POINT_COORDS" );
  createGeomAction( 701, "BASIC_PROPS" );
  createGeomAction( 702, "MASS_CENTER" );
  createGeomAction( 703, "INERTIA" );
  createGeomAction( 704, "NORMALE" );
  createGeomAction( 7041, "BND_BOX" );
  createGeomAction( 7042, "MIN_DIST" );
  createGeomAction( 7043, "MEASURE_ANGLE" );

  createGeomAction( 705, "TOLERANCE" );
  createGeomAction( 706, "WHAT_IS" );
  createGeomAction( 707, "CHECK" );
  createGeomAction( 7072, "CHECK_COMPOUND" );

#ifdef _DEBUG_ // PAL16821
  createGeomAction( 5103, "CHECK_GEOMETRY" );
#endif
  createGeomAction( 412, "SHADING_COLOR" );

  createGeomAction( 211, "SHADING" );
  createGeomAction( 212, "DISPLAY_ALL" );
  createGeomAction( 214, "ERASE_ALL" );
  createGeomAction( 216, "DISPLAY" );
  createGeomAction( 2171, "VERTEX_SEL_ONLY" ,"", 0, true );
  createGeomAction( 2172, "EDGE_SEL_ONLY", "", 0, true );
  createGeomAction( 2173, "WIRE_SEL_ONLY", "",  0, true );
  createGeomAction( 2174, "FACE_SEL_ONLY", "", 0, true );
  createGeomAction( 2175, "SHELL_SEL_ONLY", "",  0, true );
  createGeomAction( 2176, "SOLID_SEL_ONLY", "", 0, true );
  createGeomAction( 2177, "COMPOUND_SEL_ONLY", "",  0, true );
  createGeomAction( 2178, "ALL_SEL_ONLY", "",  0, true );
  createGeomAction( 213, "DISPLAY_ONLY" );
  createGeomAction( 215, "ERASE" );

  createGeomAction( 901, "POP_RENAME", "", Qt::Key_F2 );
  createGeomAction( 80311, "POP_WIREFRAME", "", 0, true );
  createGeomAction( 80312, "POP_SHADING", "", 0, true );
  createGeomAction( 8032, "POP_COLOR" );
  createGeomAction( 8033, "POP_TRANSPARENCY" );
  createGeomAction( 8034, "POP_ISOS" );
  createGeomAction( 8035, "POP_AUTO_COLOR" );
  createGeomAction( 8036, "POP_DISABLE_AUTO_COLOR" );
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
  createMenu( 4026, primId, -1 );
  createMenu( 4027, primId, -1 );

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
  createMenu( 405, newEntId, -1 );
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
  createMenu( 508, operId, -1 );
  createMenu( separator(), operId, -1 );
  createMenu( 509, transId, -1 );
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
  //createMenu( 609, repairId, -1 );
  //createMenu( 610, repairId, -1 );
  createMenu( 611, repairId, -1 );
  createMenu( 612, repairId, -1 );

  int measurId = createMenu( tr( "MEN_MEASURES" ), -1, -1, 10 );
  createMenu( 708, measurId, -1 );
  createMenu( 701, measurId, -1 );
  createMenu( separator(), measurId, -1 );
  createMenu( 702, measurId, -1 );
  createMenu( 703, measurId, -1 );
  createMenu( 704, measurId, -1 );
  // NPAL16572: move "Check free boundaries" and "Check free faces" from "Repair" to "Measure"
  createMenu( separator(), measurId, -1 );
  createMenu( 609, measurId, -1 );
  createMenu( 610, measurId, -1 );
  // NPAL16572 END
  createMenu( separator(), measurId, -1 );

  int dimId = createMenu( tr( "MEN_DIMENSIONS" ), measurId, -1 );
  createMenu( 7041, dimId, -1 );
  createMenu( 7042, dimId, -1 );
  createMenu( 7043, dimId, -1 );
  createMenu( separator(), measurId, -1 );

  createMenu( 705, measurId, -1 );
  createMenu( separator(), measurId, -1 );
  createMenu( 706, measurId, -1 );
  createMenu( 707, measurId, -1 );
  createMenu( 7072, measurId, -1 );

#ifdef _DEBUG_ // PAL16821
  int toolsId = createMenu( tr( "MEN_TOOLS" ), -1, -1, 50 );
  createMenu( separator(), toolsId, -1 );
  createMenu( 5103, toolsId, -1 );
#endif
  //int prefId = createMenu( tr( "MEN_PREFERENCES" ), -1, -1, 50 );
  //createMenu( separator(), prefId, -1 );
  //int geomId = createMenu( tr( "MEN_PREFERENCES_GEOM" ), prefId, -1 );
  //createMenu( 412, geomId, -1 );
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
  createTool( 4026, primTbId );
  createTool( 4027, primTbId );

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

  // ---- create popup menus --------------------------

  QString clientOCCorVTK = "(client='OCCViewer' or client='VTKViewer')";
  QString clientOCCorVTK_AndSomeVisible = clientOCCorVTK + " and selcount>0 and isVisible";

  QString clientOCCorVTKorOB = "(client='ObjectBrowser' or client='OCCViewer' or client='VTKViewer')";
  QString clientOCCorVTKorOB_AndSomeVisible = clientOCCorVTKorOB + " and selcount>0 and isVisible";

  QString autoColorPrefix =
    "(client='ObjectBrowser' or client='OCCViewer') and type='Shape' and selcount=1 and isOCC=true";

  QtxPopupMgr* mgr = popupMgr();
  mgr->insert( action(  901 ), -1, -1 );  // rename
  mgr->setRule( action( 901 ), QString("$type in {'Shape' 'Group'} and selcount=1"), QtxPopupMgr::VisibleRule );
  mgr->insert( action(   33 ), -1, -1 );  // delete
  mgr->setRule( action(  33 ), QString("$type in {'Shape' 'Group'} and selcount>0"), QtxPopupMgr::VisibleRule );
  mgr->insert( action(  8001 ), -1, -1 ); // create group
  mgr->setRule( action( 8001 ), QString("client='ObjectBrowser' and type='Shape' and selcount=1 and isOCC=true"), QtxPopupMgr::VisibleRule );
  mgr->insert( action(  801 ), -1, -1 );  // edit group
  mgr->setRule( action( 801 ),  QString("client='ObjectBrowser' and type='Group' and selcount=1 and isOCC=true"), QtxPopupMgr::VisibleRule );
  mgr->insert( separator(), -1, -1 );     // -----------
  dispmodeId = mgr->insert(  tr( "MEN_DISPLAY_MODE" ), -1, -1 ); // display mode menu
  mgr->insert( action(  80311 ), dispmodeId, -1 ); // wireframe
  mgr->setRule( action( 80311 ), clientOCCorVTK_AndSomeVisible, QtxPopupMgr::VisibleRule );
  mgr->setRule( action( 80311 ), clientOCCorVTK + " and displaymode='Wireframe'", QtxPopupMgr::ToggleRule );
  mgr->insert( action(  80312 ), dispmodeId, -1 ); // shading
  mgr->setRule( action( 80312 ), clientOCCorVTK_AndSomeVisible, QtxPopupMgr::VisibleRule );
  mgr->setRule( action( 80312 ), clientOCCorVTK + " and displaymode='Shading'", QtxPopupMgr::ToggleRule );
  mgr->insert( separator(), -1, -1 );     // -----------
  mgr->insert( action(  8032 ), -1, -1 ); // color
  mgr->setRule( action( 8032 ), clientOCCorVTKorOB_AndSomeVisible + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );
  mgr->insert( action(  8033 ), -1, -1 ); // transparency
  mgr->setRule( action( 8033 ), clientOCCorVTK_AndSomeVisible, QtxPopupMgr::VisibleRule );
  mgr->insert( action(  8034 ), -1, -1 ); // isos
  mgr->setRule( action( 8034 ), clientOCCorVTK_AndSomeVisible + " and selcount>0 and isVisible", QtxPopupMgr::VisibleRule );
  mgr->insert( separator(), -1, -1 );     // -----------
  mgr->insert( action(  8035 ), -1, -1 ); // auto color
  mgr->setRule( action( 8035 ), autoColorPrefix + " and isAutoColor=false", QtxPopupMgr::VisibleRule );
  mgr->insert( action(  8036 ), -1, -1 ); // disable auto color
  mgr->setRule( action( 8036 ), autoColorPrefix + " and isAutoColor=true", QtxPopupMgr::VisibleRule );
  mgr->insert( separator(), -1, -1 );     // -----------


  QString canDisplay = "($component={'GEOM'}) and (selcount>0) and ({true} in $canBeDisplayed) ",
          onlyComponent = "((type='Component') and selcount=1)",
          rule = canDisplay + "and ((($type in {%1}) and( %2 )) or " + onlyComponent + ")",
          types = "'Shape' 'Group'";

  mgr->insert( action(  216 ), -1, -1 ); // display
  mgr->setRule( action( 216 ), rule.arg( types ).arg( "not isVisible" ), QtxPopupMgr::VisibleRule );

  mgr->insert( action(  215 ), -1, -1 ); // erase
  mgr->setRule( action( 215 ), rule.arg( types ).arg( "isVisible" ), QtxPopupMgr::VisibleRule );

  mgr->insert( action(  214 ), -1, -1 ); // erase All
  mgr->setRule( action( 214 ), clientOCCorVTK, QtxPopupMgr::VisibleRule );

  QString selectOnly = "(client='OCCViewer' or client='VTKViewer') and (selcount=0)";

  int selectolnyId = mgr->insert( tr("MEN_SELECT_ONLY"), -1, -1);                //select only menu
  mgr->insert( action(2171), selectolnyId, -1);                                  //Vertex
  mgr->setRule(action(2171), selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(2171), selectOnly + " and selectionmode='VERTEX'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(2172), selectolnyId, -1);                                  //Edge
  mgr->setRule(action(2172), selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(2172), selectOnly + " and selectionmode='EDGE'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(2173), selectolnyId, -1);                                  //Wire
  mgr->setRule(action(2173), selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(2173), selectOnly + " and selectionmode='WIRE'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(2174), selectolnyId, -1);                                  //Face
  mgr->setRule(action(2174), selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(2174), selectOnly + " and selectionmode='FACE'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(2175), selectolnyId, -1);                                  //Shell
  mgr->setRule(action(2175), selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(2175), selectOnly + " and selectionmode='SHELL'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(2176), selectolnyId, -1);                                  //Solid
  mgr->setRule(action(2176), selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(2176), selectOnly + " and selectionmode='SOLID'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(2177), selectolnyId, -1);                                  //Compound
  mgr->setRule(action(2177), selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(2177), selectOnly + " and selectionmode='COMPOUND'", QtxPopupMgr::ToggleRule);
  mgr->insert( separator(), selectolnyId, -1);
  mgr->insert( action(2178), selectolnyId, -1);                                  //Clear selection filter
  mgr->setRule(action(2178), selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(2178), selectOnly + " and selectionmode='ALL'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(  213 ), -1, -1 ); // display only
  mgr->setRule( action( 213 ), rule.arg( types ).arg( "true" ), QtxPopupMgr::VisibleRule );
  mgr->insert( separator(), -1, -1 );

  mgr->hide( mgr->actionId( action( myEraseAll ) ) );
}

//=======================================================================
// function : GeometryGUI::activateModule()
// purpose  : Called when GEOM module is activated
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

  // Reset actions accelerator keys
  //action(111)->setAccel(QKeySequence(CTRL + Key_I)); // Import
  //action(121)->setAccel(QKeySequence(CTRL + Key_E)); // Export
  action(111)->setEnabled( true ); // Import: CTRL + Key_I
  action(121)->setEnabled( true ); // Export: CTRL + Key_E
  action( 33)->setEnabled( true ); // Delete: Key_Delete
  action(901)->setEnabled( true ); // Rename: Key_F2

  GUIMap::Iterator it;
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it )
    it.value()->activate( application()->desktop() );

  LightApp_SelectionMgr* sm = getApp()->selectionMgr();

  SUIT_ViewManager* vm;
  ViewManagerList OCCViewManagers, VTKViewManagers;

  application()->viewManagers( OCCViewer_Viewer::Type(), OCCViewManagers );
  QListIterator<SUIT_ViewManager*> itOCC( OCCViewManagers );
  while ( itOCC.hasNext() && (vm = itOCC.next()) )
    myOCCSelectors.append( new GEOMGUI_OCCSelector( ((OCCViewer_ViewManager*)vm)->getOCCViewer(), sm ) );

  application()->viewManagers( SVTK_Viewer::Type(), VTKViewManagers );
  QListIterator<SUIT_ViewManager*> itVTK( VTKViewManagers );
  while ( itVTK.hasNext() && (vm = itVTK.next()) )
    myVTKSelectors.append( new LightApp_VTKSelector( dynamic_cast<SVTK_Viewer*>( vm->getViewModel() ), sm ) );

  //NPAL 19674
  SALOME_ListIO selected;
  sm->selectedObjects( selected );
  sm->clearSelected();
  
  // disable OCC selectors
  getApp()->selectionMgr()->setEnabled( false, OCCViewer_Viewer::Type() );
  QListIterator<GEOMGUI_OCCSelector*> itOCCSel( myOCCSelectors );
  while ( itOCCSel.hasNext() )
    if ( GEOMGUI_OCCSelector* sr = itOCCSel.next() )
      sr->setEnabled(true);

  // disable VTK selectors
  getApp()->selectionMgr()->setEnabled( false, SVTK_Viewer::Type() );
  QListIterator<LightApp_VTKSelector*> itVTKSel( myVTKSelectors );
  while ( itVTKSel.hasNext() )
    if ( LightApp_VTKSelector* sr = itVTKSel.next() )
      sr->setEnabled(true);

  sm->setSelectedObjects( selected, true );   //NPAL 19674

  return true;
}


//=======================================================================
// function : GeometryGUI::deactivateModule()
// purpose  : Called when GEOM module is deactivated
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
    it.value()->deactivate();

  // Unset actions accelerator keys
  //action(111)->setAccel(QKeySequence()); // Import
  //action(121)->setAccel(QKeySequence()); // Export
  action(111)->setEnabled( false ); // Import: CTRL + Key_I
  action(121)->setEnabled( false ); // Export: CTRL + Key_E
  action( 33)->setEnabled( false ); // Delete: Key_Delete
  action(901)->setEnabled( false ); // Rename: Key_F2

  qDeleteAll(myOCCSelectors);
  myOCCSelectors.clear();
  getApp()->selectionMgr()->setEnabled( true, OCCViewer_Viewer::Type() );

  qDeleteAll(myVTKSelectors);
  myVTKSelectors.clear();
  getApp()->selectionMgr()->setEnabled( true, SVTK_Viewer::Type() );

  return SalomeApp_Module::deactivateModule( study );
}

//=======================================================================
// function : GeometryGUI::BuildPresentation()
// purpose  :
//=======================================================================
void GeometryGUI::BuildPresentation( const Handle(SALOME_InteractiveObject)& io, SUIT_ViewWindow* win )
{
  //GEOM_Displayer().Display( io, false, win );
}

//=======================================================================
// function : onWindowActivated()
// purpose  : update menu items' status - disable non-OCC-viewer-compatible actions
//=======================================================================
void GeometryGUI::onWindowActivated( SUIT_ViewWindow* win )
{
  if ( !win )
    return;

  const bool ViewOCC = ( win->getViewManager()->getType() == OCCViewer_Viewer::Type() );
  //const bool ViewVTK = ( win->getViewManager()->getType() == SVTK_Viewer::Type() );

  // disable non-OCC viewframe menu commands
//  action( 404 )->setEnabled( ViewOCC ); // SKETCHER
  action( 603 )->setEnabled( ViewOCC ); // SuppressFace
  action( 604 )->setEnabled( ViewOCC ); // SuppressHole
  action( 606 )->setEnabled( ViewOCC ); // CloseContour
  action( 607 )->setEnabled( ViewOCC ); // RemoveInternalWires
  action( 608 )->setEnabled( ViewOCC ); // AddPointOnEdge
//  action( 609 )->setEnabled( ViewOCC ); // Free boundaries

  action( 800 )->setEnabled( ViewOCC ); // Create Group
  action( 801 )->setEnabled( ViewOCC ); // Edit Group

  action( 9998 )->setEnabled( ViewOCC ); // MENU BLOCKS - MULTI-TRANSFORMATION
}

void GeometryGUI::windows( QMap<int, int>& mappa ) const
{
  mappa.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
  mappa.insert( SalomeApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea );
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
    QListIterator<GEOMGUI_OCCSelector*> itOCCSel( myOCCSelectors );
    while ( itOCCSel.hasNext() )
      if ( GEOMGUI_OCCSelector* sr = itOCCSel.next() )
	sr->setEnabled(true);
  }
  else if ( vm->getType() == SVTK_Viewer::Type() )
  {
    LightApp_SelectionMgr* sm = getApp()->selectionMgr();
    myVTKSelectors.append( new LightApp_VTKSelector( dynamic_cast<SVTK_Viewer*>( vm->getViewModel() ), sm ) );

    // disable VTK selectors
    getApp()->selectionMgr()->setEnabled( false, SVTK_Viewer::Type() );
    QListIterator<LightApp_VTKSelector*> itVTKSel( myVTKSelectors );
    while ( itVTKSel.hasNext() )
      if ( LightApp_VTKSelector* sr = itVTKSel.next() )
	sr->setEnabled(true);
  }
}

void GeometryGUI::onViewManagerRemoved( SUIT_ViewManager* vm )
{
  SUIT_ViewModel* viewer = vm->getViewModel();
  if ( vm->getType() == OCCViewer_Viewer::Type() )
  {
    QListIterator<GEOMGUI_OCCSelector*> itOCCSel( myOCCSelectors );
    while ( itOCCSel.hasNext() )
      if ( GEOMGUI_OCCSelector* sr = itOCCSel.next() )
	if ( sr->viewer() == viewer )
	{
	  delete myOCCSelectors.takeAt( myOCCSelectors.indexOf( sr ) );
	  break;
	}
  }
  if ( vm->getType() == SVTK_Viewer::Type() )
  {
    QListIterator<LightApp_VTKSelector*> itVTKSel( myVTKSelectors );
    while ( itVTKSel.hasNext() )
      if ( LightApp_VTKSelector* sr = itVTKSel.next() )
	if ( sr->viewer() == viewer )
	{
	  delete myVTKSelectors.takeAt( myVTKSelectors.indexOf( sr ) );
	  break;
	}
  }
}

QString GeometryGUI::engineIOR() const
{
  if ( !CORBA::is_nil( GetGeomGen() ) )
    return QString( getApp()->orb()->object_to_string( GetGeomGen() ) );
  return "";
}

LightApp_Selection* GeometryGUI::createSelection() const
{
  return new GEOMGUI_Selection();
}

void GeometryGUI::contextMenuPopup( const QString& client, QMenu* menu, QString& title )
{
  SalomeApp_Module::contextMenuPopup(client, menu, title);

  SALOME_ListIO lst;
  getApp()->selectionMgr()->selectedObjects(lst);
  if (lst.Extent() < 1)
    return;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(application()->activeStudy());
  _PTR(Study) study = appStudy->studyDS();

  bool isImported = true;
  SALOME_ListIteratorOfListIO anIt (lst);
  for (; anIt.More() && isImported; anIt.Next()) {
    Handle(SALOME_InteractiveObject) io = anIt.Value();
    _PTR(SObject) aSObj = study->FindObjectID(io->getEntry());
    if (aSObj) {
      if (lst.Extent() == 1) {
        // Set context menu title
        if (client == "OCCViewer" || client == "VTKViewer")
          title = QString(aSObj->GetName().c_str());
      }

      CORBA::Object_var anObj = GeometryGUI::ClientSObjectToObject(aSObj);
      GEOM::GEOM_Object_var aGeomObj = GEOM::GEOM_Object::_narrow(anObj);
      if (CORBA::is_nil(aGeomObj) || aGeomObj->GetType() != GEOM_IMPORT)
        isImported = false;
    }
  }

  if (isImported) {
    menu->addAction(action(5029)); // Reload imported shape
  }
}

void GeometryGUI::createPreferences()
{
  int tabId = addPreference( tr( "PREF_TAB_SETTINGS" ) );

  int genGroup = addPreference( tr( "PREF_GROUP_GENERAL" ), tabId );
  setPreferenceProperty( genGroup, "columns", 2 );

  int dispmode = addPreference( tr( "PREF_DISPLAY_MODE" ), genGroup,
				LightApp_Preferences::Selector, 
				"Geometry", "display_mode" );

  addPreference( tr( "PREF_SHADING_COLOR" ), genGroup,
		 LightApp_Preferences::Color, "Geometry", "shading_color" );

  addPreference( tr( "PREF_WIREFRAME_COLOR" ), genGroup,
		 LightApp_Preferences::Color, "Geometry", "wireframe_color" );

  addPreference( tr( "PREF_FREE_BOUND_COLOR" ), genGroup,
		 LightApp_Preferences::Color, "Geometry", "free_bound_color" );

  addPreference( tr( "PREF_LINE_COLOR"), genGroup,
		 LightApp_Preferences::Color, "Geometry", "line_color" );

  addPreference( tr( "PREF_POINT_COLOR"), genGroup,
		 LightApp_Preferences::Color, "Geometry", "point_color" );

  addPreference( tr( "PREF_ISOS_COLOR" ), genGroup,
		 LightApp_Preferences::Color, "Geometry", "isos_color" );

  int step = addPreference( tr( "PREF_STEP_VALUE" ), genGroup,
			    LightApp_Preferences::IntSpin, "Geometry", "SettingsGeomStep" );

  int VertexGroup = addPreference( tr( "PREF_GROUP_VERTEX" ), tabId );
  setPreferenceProperty( VertexGroup, "columns", 2 );

  int typeOfMarker = addPreference( tr( "PREF_TYPE_OF_MARKER" ), VertexGroup,
                                    LightApp_Preferences::Selector, "Geometry", "type_of_marker" );

  int markerScale = addPreference( tr( "PREF_MARKER_SCALE" ), VertexGroup,
                                   LightApp_Preferences::DblSpin, "Geometry", "marker_scale" );

  // Set property for default display mode
  QStringList aModesList;
  aModesList.append( tr("MEN_WIREFRAME") );
  aModesList.append( tr("MEN_SHADING") );

  QList<QVariant> anIndexesList;
  anIndexesList.append(0);
  anIndexesList.append(1);

  setPreferenceProperty( dispmode, "strings", aModesList );
  setPreferenceProperty( dispmode, "indexes", anIndexesList );

  // Set property for step value for spinboxes
  setPreferenceProperty( step, "min", 1 );
  setPreferenceProperty( step, "max", 10000 );
  setPreferenceProperty( step, "precision", 3 );

  // Set property for type of vertex marker
  QStringList aTypeOfMarkerList;
  QList<QVariant> anTypeOfMarkerIndexesList;

  aTypeOfMarkerList.append( tr("TOM_PLUS") );
  anTypeOfMarkerIndexesList.append(Aspect_TOM_PLUS);

  aTypeOfMarkerList.append( tr("TOM_POINT") );
  anTypeOfMarkerIndexesList.append(Aspect_TOM_POINT);

  aTypeOfMarkerList.append( tr("TOM_STAR") );
  anTypeOfMarkerIndexesList.append(Aspect_TOM_STAR);

  aTypeOfMarkerList.append( tr("TOM_O") );
  anTypeOfMarkerIndexesList.append(Aspect_TOM_O);

  aTypeOfMarkerList.append( tr("TOM_X") );
  anTypeOfMarkerIndexesList.append(Aspect_TOM_X);

  aTypeOfMarkerList.append( tr("TOM_O_POINT") );
  anTypeOfMarkerIndexesList.append(Aspect_TOM_O_POINT);

  aTypeOfMarkerList.append( tr("TOM_O_PLUS") );
  anTypeOfMarkerIndexesList.append(Aspect_TOM_O_PLUS);

  aTypeOfMarkerList.append( tr("TOM_O_STAR") );
  anTypeOfMarkerIndexesList.append(Aspect_TOM_O_STAR);

  aTypeOfMarkerList.append( tr("TOM_O_X") );
  anTypeOfMarkerIndexesList.append(Aspect_TOM_O_X);


  setPreferenceProperty( typeOfMarker, "strings", aTypeOfMarkerList );
  setPreferenceProperty( typeOfMarker, "indexes", anTypeOfMarkerIndexesList );

  // Set property for Vertex Marker scale
  setPreferenceProperty( markerScale, "min", 1. );
  setPreferenceProperty( markerScale, "max", 7. );
  setPreferenceProperty( markerScale, "precision", 0.01 );
  setPreferenceProperty( markerScale, "step", 0.5 );

}

void GeometryGUI::preferencesChanged( const QString& section, const QString& param )
{
  if (section == "Geometry") {
    SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
    if (param == QString("SettingsGeomStep")) {
      double spin_step = aResourceMgr->doubleValue(section, param, 100.);
      EmitSignalDefaultStepValueChanged(spin_step);
    }
  }
}

LightApp_Displayer* GeometryGUI::displayer()
{
  if( !myDisplayer )
    myDisplayer = new GEOM_Displayer( dynamic_cast<SalomeApp_Study*>( getApp()->activeStudy() ) );
  return myDisplayer;
}

void GeometryGUI::setLocalSelectionMode(const int mode)
{
  myLocalSelectionMode = mode;
}
int GeometryGUI::getLocalSelectionMode() const
{
  return myLocalSelectionMode;
}

const char gSeparator = '_'; // character used to separate parameter names
const char gDigitsSep = ':'; // character used to separate numeric parameter values (color = r:g:b)

/*!
 * \brief Store visual parameters
 *
 * This method is called just before the study document is saved.
 * Store visual parameters in AttributeParameter attribue(s)
 */
void GeometryGUI::storeVisualParameters (int savePoint)
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(application()->activeStudy());
  if (!appStudy || !appStudy->studyDS())
    return;
  _PTR(Study) studyDS = appStudy->studyDS();

  // componentName is used for encoding of entries when storing them in IParameters
  std::string componentName = myComponentGeom->ComponentDataType();
  //_PTR(SComponent) aSComponent = studyDS->FindComponent("GEOM");
  //if (!aSComponent) return;

  // IParameters
  _PTR(AttributeParameter) ap = studyDS->GetModuleParameters("Interface Applicative",
                                                             componentName.c_str(),
                                                             savePoint);
  _PTR(IParameters) ip = ClientFactory::getIParameters(ap);

  // viewers counters are used for storing view_numbers in IParameters
  int vtkViewers(0), occViewers(0);

  QList<SUIT_ViewManager*> lst;
  QList<SUIT_ViewManager*>::Iterator it;

  // main cycle to store parameters of displayed objects
  lst.clear();
  vtkViewers = occViewers = 0;
  getApp()->viewManagers(lst);
  for (it = lst.begin(); it != lst.end(); it++)
  {
    SUIT_ViewManager* vman = *it;
    QString vType = vman->getType();

    // saving VTK actors properties
    if (vType == SVTK_Viewer::Type())
    {
      QVector<SUIT_ViewWindow*> views = vman->getViews();
      for (int i = 0, iEnd = vman->getViewsCount(); i < iEnd; i++)
      {
	if (SVTK_ViewWindow* vtkView = dynamic_cast<SVTK_ViewWindow*>(views[i]))
        {
	  vtkActorCollection* allActors = vtkView->getRenderer()->GetActors();
	  allActors->InitTraversal();
	  while (vtkActor* actor = allActors->GetNextActor())
          {
	    if (actor->GetVisibility()) // store only visible actors
            {
              GEOM_Actor* aGeomActor = 0;
              if (actor->IsA("GEOM_Actor"))
                aGeomActor = GEOM_Actor::SafeDownCast(actor);
	      if (aGeomActor && aGeomActor->hasIO())
              {
                Handle(SALOME_InteractiveObject) io = aGeomActor->getIO();
                if (io->hasEntry())
                {
                  // entry is "encoded" = it does NOT contain component adress, since it is a
                  // subject to change on next component loading
                  std::string entry = ip->encodeEntry(io->getEntry(), componentName);

                  std::string param, occParam = vType.toLatin1().data();
                  occParam += gSeparator;
                  occParam += QString::number(vtkViewers).toLatin1().data();
                  occParam += gSeparator;

                  param = occParam + "Visibility";
                  ip->setParameter(entry, param, "On");

                  param = occParam + "DisplayMode";
                  ip->setParameter(entry, param, QString::number
                                   (aGeomActor->getDisplayMode()).toLatin1().data());

                  vtkFloatingPointType r, g, b;
                  aGeomActor->GetColor(r, g, b);
                  QString colorStr = QString::number(r);
                  colorStr += gDigitsSep; colorStr += QString::number(g);
                  colorStr += gDigitsSep; colorStr += QString::number(b);
                  param = occParam + "Color";
                  ip->setParameter(entry, param, colorStr.toLatin1().data());

                  param = occParam + "Opacity";
                  ip->setParameter(entry, param, QString::number(aGeomActor->GetOpacity()).toLatin1().data());

                  int nbUIso(0), nbVIso(0);
                  aGeomActor->GetNbIsos(nbUIso,nbVIso);
                  QString isosStr = QString::number(nbUIso);
                  isosStr += gDigitsSep;
                  isosStr += QString::number(nbVIso);
                  param = occParam + "Isos";
                  ip->setParameter(entry, param, isosStr.toLatin1().data());
                } // if (io->hasEntry())
	      } // GEOM_Actor && hasIO
	    } // isVisible
	  } // while.. actors traversal
	} // if (vtkView)
      } // for (views)
      vtkViewers++;
    } // if (SVTK view model)
    else if (vType == SOCC_Viewer::Type()) // processing OCC viewers
    {
      QVector<SUIT_ViewWindow*> views = vman->getViews();
      for (int i = 0, iEnd = vman->getViewsCount(); i < iEnd; i++)
      {
	SOCC_ViewWindow* occView = dynamic_cast<SOCC_ViewWindow*>(views[i]);
	if (occView)
        {
          //Handle(AIS_InteractiveContext) ic =
          //  ((OCCViewer_Viewer*)(occView->getViewManager()->getViewModel()))->getAISContext();
          OCCViewer_Viewer* viewModel = (OCCViewer_Viewer*)(vman->getViewModel());
          //OCCViewer_Viewer* viewModel = ((OCCViewer_ViewManager*)vman)->getOCCViewer();
          Handle(AIS_InteractiveContext) ic = viewModel->getAISContext();

          AIS_ListOfInteractive aList;
          ic->DisplayedObjects(aList);

          AIS_ListIteratorOfListOfInteractive ite (aList);
          for (; ite.More(); ite.Next())
          {
            if (ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape)))
            {
              Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
              if (aSh->hasIO())
              {
                Handle(SALOME_InteractiveObject) io =
                  Handle(SALOME_InteractiveObject)::DownCast(aSh->getIO());
                if (io->hasEntry())
                {
                  // entry is "encoded": it does NOT contain component adress,
                  // since it is a subject to change on next component loading
                  std::string entry = ip->encodeEntry(io->getEntry(), componentName);

                  std::string param, occParam = vType.toLatin1().data();
                  occParam += gSeparator;
                  occParam += QString::number(occViewers).toLatin1().data();
                  occParam += gSeparator;

                  // Visibility
                  param = occParam + "Visibility";
                  ip->setParameter(entry, param, "On");

                  // DisplayMode
                  param = occParam + "DisplayMode";
                  int dm = aSh->DisplayMode();
                  ip->setParameter(entry, param, QString::number(dm).toLatin1().data());

                  // Color
                  // is a property of GEOM_Object, it is stored by GEOM engine

                  // Transparency
                  param = occParam + "Transparency";
                  ip->setParameter(entry, param, QString::number(aSh->Transparency()).toLatin1().data());

                  // Isos
                  Handle(AIS_Drawer) aDrawer = aSh->Attributes();
                  int nbUIso = aDrawer->UIsoAspect()->Number();
                  int nbVIso = aDrawer->VIsoAspect()->Number();
                  QString isosStr = QString::number(nbUIso);
                  isosStr += gDigitsSep;
                  isosStr += QString::number(nbVIso);
                  param = occParam + "Isos";
                  ip->setParameter(entry, param, isosStr.toLatin1().data());
                } // if (io->hasEntry())
              } // if (io)
	    } // if (GEOM_AISShape)
	  } // for (AIS_ListOfInteractive)
	} // if ( occView )
      } // for ( views )
      occViewers++;
    } // if (SOCC view model)
    else
    {
      // unknown viewer type
    }
  } // for (viewManagers)
}

/*!
 * \brief Restore visual parameters
 *
 * This method is called after the study document is opened.
 * Restore visual parameters from AttributeParameter attribue(s)
 */
void GeometryGUI::restoreVisualParameters (int savePoint)
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(application()->activeStudy());
  if (!appStudy || !appStudy->studyDS())
    return;
  _PTR(Study) studyDS = appStudy->studyDS();

  // componentName is used for encoding of entries when storing them in IParameters
  std::string componentName = myComponentGeom->ComponentDataType();
  //_PTR(SComponent) aSComponent = studyDS->FindComponent("GEOM");
  //if (!aSComponent) return;

  // IParameters
  _PTR(AttributeParameter) ap = studyDS->GetModuleParameters("Interface Applicative",
                                                             componentName.c_str(),
                                                             savePoint);
  _PTR(IParameters) ip = ClientFactory::getIParameters(ap);

  std::vector<std::string> entries = ip->getEntries();

  for (std::vector<std::string>::iterator entIt = entries.begin(); entIt != entries.end(); ++entIt)
  {
    // entry is a normal entry - it should be "decoded" (setting base adress of component)
    QString entry (ip->decodeEntry(*entIt).c_str());

    // Check that the entry corresponds to a real object in the Study
    // as the object may be deleted or modified after the visual state is saved.
    _PTR(SObject) so = studyDS->FindObjectID(entry.toLatin1().data());
    if (!so) continue; //Skip the not existent entry

    std::vector<std::string> paramNames = ip->getAllParameterNames( *entIt );
    std::vector<std::string> paramValues = ip->getAllParameterValues( *entIt );

    std::vector<std::string>::iterator namesIt = paramNames.begin();
    std::vector<std::string>::iterator valuesIt = paramValues.begin();

    // actors are stored in a map after displaying of them for
    // quicker access in the future: map < viewID to actor >
    NCollection_DataMap<int, GEOM_Actor*          > vtkActors;
    NCollection_DataMap<int, Handle(GEOM_AISShape)> occActors;

    for (; namesIt != paramNames.end(); ++namesIt, ++valuesIt)
    {
      // visual parameters are stored in strings as follows: ViewerType_ViewIndex_ParamName.
      // '_' is used as separator and should not be used in viewer type or parameter names.
      QStringList lst = QString((*namesIt).c_str()).split(gSeparator, QString::SkipEmptyParts);
      if (lst.size() != 3)
        continue;

      QString viewerTypStr = lst[0];
      QString viewIndexStr = lst[1];
      QString paramNameStr = lst[2];

      bool ok;
      int viewIndex = viewIndexStr.toUInt(&ok);
      if (!ok) // bad conversion of view index to integer
	continue;

      // viewers
      if (viewerTypStr == SVTK_Viewer::Type())
      {
        GEOM_Actor* vActor = 0;
        if (vtkActors.IsBound(viewIndex))
          vActor = vtkActors.Find(viewIndex);

        if (paramNameStr == "Visibility")
        {
          if (!vActor && displayer())
          {
            QList<SUIT_ViewManager*> lst;
            getApp()->viewManagers(viewerTypStr, lst);

            // SVTK ViewManager always has 1 ViewWindow, so view index is index of view manager
            if (viewIndex >= 0 && viewIndex < lst.count()) {
              SUIT_ViewManager* vman = lst.at(viewIndex);
              SUIT_ViewModel* vmodel = vman->getViewModel();
              // SVTK view model can be casted to SALOME_View
              displayer()->Display(entry, true, dynamic_cast<SALOME_View*>(vmodel));

              // store displayed actor in a temporary map for quicker
              // access later when restoring other parameters
              SVTK_ViewWindow* vtkView = (SVTK_ViewWindow*) vman->getActiveView();
              vtkRenderer* Renderer = vtkView->getRenderer();
              vtkActorCollection* theActors = Renderer->GetActors();
              theActors->InitTraversal();
              bool isFound = false;
              vtkActor *ac = theActors->GetNextActor();
              for (; ac != NULL && !isFound; ac = theActors->GetNextActor()) {
                if (ac->IsA("GEOM_Actor")) {
                  GEOM_Actor* aGeomAc = GEOM_Actor::SafeDownCast(ac);
                  if (aGeomAc->hasIO()) {
                    Handle(SALOME_InteractiveObject) io =
                      Handle(SALOME_InteractiveObject)::DownCast(aGeomAc->getIO());
                    if (io->hasEntry() && strcmp(io->getEntry(), entry.toLatin1().data()) == 0) {
                      isFound = true;
                      vtkActors.Bind(viewIndex, aGeomAc);
                    }
                  }
                }
              }
            }
          }
        } // if (paramNameStr == "Visibility")
	else
        {
          // the rest properties "work" with GEOM_Actor
	  if (vActor)
          {
            QString val ((*valuesIt).c_str());

            if (paramNameStr == "DisplayMode") {
              vActor->setDisplayMode(val.toInt());
            }
            else if (paramNameStr == "Color") {
              QStringList colors = val.split(gDigitsSep, QString::SkipEmptyParts);
              if (colors.count() == 3)
                vActor->SetColor(colors[0].toFloat(), colors[1].toFloat(), colors[2].toFloat());
            }
            else if (paramNameStr == "Opacity") {
              vActor->SetOpacity(val.toFloat());
            }
            else if (paramNameStr == "Isos") {
              QStringList isos = val.split(gDigitsSep, QString::SkipEmptyParts);
              if (isos.count() == 2) {
                int aIsos[2] = {isos[0].toInt(), isos[1].toInt()};
                vActor->SetNbIsos(aIsos);
              }
            }
          }
	} // other parameters than Visibility
      }
      else if (viewerTypStr == SOCC_Viewer::Type())
      {
        //Handle(AIS_InteractiveObject) occActor;
        Handle(GEOM_AISShape) occActor;
        if (occActors.IsBound(viewIndex))
          occActor = occActors.Find(viewIndex);

        // ViewModel and InteractiveContext
        SOCC_Viewer* occVModel = 0;
        Handle(AIS_InteractiveContext) ic;

        QList<SUIT_ViewManager*> lst;
        getApp()->viewManagers(viewerTypStr, lst);

        // SOCC ViewManager always has 1 ViewWindow, so view index is index of view manager
        if (viewIndex >= 0 && viewIndex < lst.count()) {
          SUIT_ViewManager* vman = lst.at(viewIndex);
          SUIT_ViewModel* vmodel = vman->getViewModel();
          occVModel = dynamic_cast<SOCC_Viewer*>(vmodel);
          if (occVModel)
            ic = occVModel->getAISContext();
        }

        if (ic.IsNull())
          continue;

        if (paramNameStr == "Visibility")
        {
	  if (occActor.IsNull() && displayer())
          {
            displayer()->Display(entry, true, occVModel);

            // store displayed actor in a temporary map for quicker
            // access later when restoring other parameters
            AIS_ListOfInteractive aList;
            ic->DisplayedObjects(aList);
            bool isFound = false;
            AIS_ListIteratorOfListOfInteractive ite (aList);
            for (; ite.More() && !isFound; ite.Next()) {
              if (ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
                Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
                if (aSh->hasIO()) {
                  Handle(SALOME_InteractiveObject) io =
                    Handle(SALOME_InteractiveObject)::DownCast(aSh->getIO());
                  if (io->hasEntry() && strcmp(io->getEntry(), entry.toLatin1().data()) == 0) {
                    isFound = true;
                    occActors.Bind(viewIndex, aSh);
                  }
                }
              }
            }
          }
        } // if (paramNameStr == "Visibility")
	else
        {
          // the rest properties "work" with GEOM_AISShape
	  if (!occActor.IsNull())
          {
            QString val ((*valuesIt).c_str());

            if (paramNameStr == "DisplayMode") {
              ic->SetDisplayMode(occActor, AIS_DisplayMode(val.toInt()), false);
              //ic->Redisplay(occActor, Standard_False, Standard_True);
            }
            // Color is restored by the GEOM engine
            else if (paramNameStr == "Transparency") {
              ic->SetTransparency(occActor, val.toFloat(), false);
              ic->Redisplay(occActor, Standard_False, Standard_True);
            }
            else if (paramNameStr == "Isos") {
              QStringList isos = val.split(gDigitsSep, QString::SkipEmptyParts);
              if (isos.count() == 2) {
                Handle(AIS_Drawer) aDrawer = occActor->Attributes();
                int nbUIso = isos[0].toInt();
                int nbVIso = isos[1].toInt();
                Handle(Prs3d_IsoAspect) uIsoAspect = aDrawer->UIsoAspect();
                Handle(Prs3d_IsoAspect) vIsoAspect = aDrawer->VIsoAspect();
                uIsoAspect->SetNumber(nbUIso);
                vIsoAspect->SetNumber(nbVIso);
                aDrawer->SetUIsoAspect(uIsoAspect);
                aDrawer->SetVIsoAspect(vIsoAspect);
                ic->SetLocalAttributes(occActor, aDrawer);
                ic->Redisplay(occActor);
              }
            }
          }
	} // other parameters than Visibility
      }
      else
      {
        // unknown viewer type
      }
    } // for names/parameters iterator
  } // for entries iterator

  // update all VTK and OCC views
  QList<SUIT_ViewManager*> lst;
  getApp()->viewManagers(lst);
  for (QList<SUIT_ViewManager*>::Iterator it = lst.begin(); it != lst.end(); it++) {
    SUIT_ViewModel* vmodel = (*it)->getViewModel();
    if (!vmodel)
      continue;
    if (vmodel->getType() == SVTK_Viewer::Type()) {
      SVTK_ViewWindow* vtkView = (SVTK_ViewWindow*) (*it)->getActiveView();
      vtkView->getRenderer()->ResetCameraClippingRange();
      vtkView->Repaint();
    }
    else if (vmodel->getType() == SOCC_Viewer::Type()) {
      //SOCC_ViewWindow* occView = (SOCC_ViewWindow*) (*it)->getActiveView();
      SALOME_View* occVMod = dynamic_cast<SALOME_View*>(vmodel);
      if (occVMod)
        occVMod->Repaint();
    }
  }
}
