//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  GEOM GEOMGUI : GUI for Geometry component
//  File   : GeometryGUI.cxx
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include <Standard_math.hxx>  // E.A. must be included before Python.h to fix compilation on windows
#include "Python.h"
#include "GeometryGUI.h"
#include "GeometryGUI_Operations.h"
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
#include <Graphic3d_HArray1OfBytes.hxx>

#include <utilities.h>

#include <vtkCamera.h>
#include <vtkRenderer.h>

#include "GEOMImpl_Types.hxx"

extern "C" {
  Standard_EXPORT CAM_Module* createModule() {
    return new GeometryGUI();
  }
}

GeometryGUI::StudyTextureMap GeometryGUI::myTextureMap;

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

void GeometryGUI::Modified( bool theIsUpdateActions )
{
  if( SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() ) ) {
    if( SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() ) ) {
      appStudy->Modified();
      if( theIsUpdateActions )
        app->updateActions();
    }
  }
}

//=======================================================================
// function : GeometryGUI::GeometryGUI()
// purpose  : Constructor
//=======================================================================
GeometryGUI::GeometryGUI() :
  SalomeApp_Module( "GEOM" ),
  LightApp_Module( "GEOM" )
{
  if ( CORBA::is_nil( myComponentGeom ) )
  {
    Engines::Component_var comp = SalomeApp_Application::lcc()->FindOrLoad_Component( "FactoryServer", "GEOM" );
    myComponentGeom  = GEOM::GEOM_Gen::_narrow( comp );
  }

  myActiveDialogBox = 0;

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

  qDeleteAll(myGUIMap);
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
#ifndef WNT
    QString dirs = getenv( "LD_LIBRARY_PATH" );
    QString sep  = ":";
#else
    QString dirs = getenv( "PATH" );
    QString sep  = ";";
#endif
    if ( !dirs.isEmpty() ) {
      QStringList dirList = dirs.split(sep, QString::SkipEmptyParts ); // skip empty entries
      QListIterator<QString> it( dirList ); it.toBack();
      while ( it.hasPrevious() ) {
        QFileInfo fi( Qtx::addSlash( it.previous() ) + libraryName );
        if ( fi.exists() ) {
          OSD_SharedLibrary aSharedLibrary( fi.fileName().toLatin1().constData() );
          bool res = aSharedLibrary.DlOpen( OSD_RTLD_LAZY );
          if ( !res ) {
            MESSAGE( "Can't open library : " << aSharedLibrary.DlError() );
            continue; // continue search further
          }
          OSD_Function osdF = aSharedLibrary.DlSymb( "GetLibGUI" );
          if ( osdF != NULL ) {
            LibraryGUI func = (GEOMGUI* (*) (GeometryGUI*))osdF;
            GEOMGUI* libGUI = (*func)( this );
            if ( libGUI ) {
              myGUIMap[ libraryName ] = libGUI;
              break; // found and loaded!
            }
          }
        }
      }
    }
  }
  return myGUIMap.contains( libraryName ) ? myGUIMap[ libraryName ] : 0;
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
  QList<int> NotViewerDependentCommands;
  NotViewerDependentCommands << GEOMOp::OpRename
			     << GEOMOp::OpDelete
			     << GEOMOp::OpShow
			     << GEOMOp::OpShowOnly
			     << GEOMOp::OpShowChildren
			     << GEOMOp::OpHideChildren
			     << GEOMOp::OpPointMarker;
  if ( !ViewOCC && !ViewVTK && !NotViewerDependentCommands.contains( id ) )
      return;

  // fix for IPAL9103, point 2
  if ( CORBA::is_nil( GetGeomGen() ) ) {
    SUIT_MessageBox::critical( desk, tr( "GEOM_ERROR" ), tr( "GEOM_ERR_GET_ENGINE" ), tr( "GEOM_BUT_OK" ) );
    return;
  }

  QString libName;
  // find corresponding GUI library
  switch ( id ) {
  case GEOMOp::OpOriginAndVectors: // MENU BASIC - ORIGIN AND BASE VECTORS
    createOriginAndBaseVectors(); // internal operation
    return;
  case GEOMOp::OpImport:           // MENU FILE - IMPORT
  case GEOMOp::OpExport:           // MENU FILE - EXPORT
  case GEOMOp::OpSelectVertex:     // POPUP MENU - SELECT ONLY - VERTEX
  case GEOMOp::OpSelectEdge:       // POPUP MENU - SELECT ONLY - EDGE
  case GEOMOp::OpSelectWire:       // POPUP MENU - SELECT ONLY - WIRE
  case GEOMOp::OpSelectFace:       // POPUP MENU - SELECT ONLY - FACE
  case GEOMOp::OpSelectShell:      // POPUP MENU - SELECT ONLY - SHELL
  case GEOMOp::OpSelectSolid:      // POPUP MENU - SELECT ONLY - SOLID
  case GEOMOp::OpSelectCompound:   // POPUP MENU - SELECT ONLY - COMPOUND
  case GEOMOp::OpSelectAll:        // POPUP MENU - SELECT ONLY - SELECT ALL
  case GEOMOp::OpDelete:           // MENU EDIT - DELETE
  case GEOMOp::OpCheckGeom:        // MENU TOOLS - CHECK GEOMETRY
  case GEOMOp::OpDeflection:       // POPUP MENU - DEFLECTION COEFFICIENT
  case GEOMOp::OpColor:            // POPUP MENU - COLOR
  case GEOMOp::OpTransparency:     // POPUP MENU - TRANSPARENCY
  case GEOMOp::OpIncrTransparency: // SHORTCUT   - INCREASE TRANSPARENCY
  case GEOMOp::OpDecrTransparency: // SHORTCUT   - DECREASE TRANSPARENCY
  case GEOMOp::OpIsos:             // POPUP MENU - ISOS
  case GEOMOp::OpIncrNbIsos:       // SHORTCUT   - INCREASE NB ISOS
  case GEOMOp::OpDecrNbIsos:       // SHORTCUT   - DECREASE NB ISOS
  case GEOMOp::OpAutoColor:        // POPUP MENU - AUTO COLOR
  case GEOMOp::OpNoAutoColor:      // POPUP MENU - DISABLE AUTO COLOR
  case GEOMOp::OpShowChildren:     // POPUP MENU - SHOW CHILDREN
  case GEOMOp::OpHideChildren:     // POPUP MENU - HIDE CHILDREN
  case GEOMOp::OpPointMarker:      // POPUP MENU - POINT MARKER
  case GEOMOp::OpRename:           // POPUP MENU - RENAME
    libName = "GEOMToolsGUI";
    break;
  case GEOMOp::OpDisplayMode:      // MENU VIEW - WIREFRAME/SHADING
  case GEOMOp::OpShowAll:          // MENU VIEW - SHOW ALL
  case GEOMOp::OpShowOnly:         // MENU VIEW - DISPLAY ONLY
  case GEOMOp::OpHideAll:          // MENU VIEW - ERASE ALL
  case GEOMOp::OpHide:             // MENU VIEW - ERASE
  case GEOMOp::OpShow:             // MENU VIEW - DISPLAY
  case GEOMOp::OpSwitchVectors:    // MENU VIEW - VECTOR MODE
  case GEOMOp::OpWireframe:        // POPUP MENU - WIREFRAME
  case GEOMOp::OpShading:          // POPUP MENU - SHADING
  case GEOMOp::OpVectors:          // POPUP MENU - VECTORS
    libName = "DisplayGUI";
    break;
  case GEOMOp::OpPoint:            // MENU BASIC - POINT
  case GEOMOp::OpLine:             // MENU BASIC - LINE
  case GEOMOp::OpCircle:           // MENU BASIC - CIRCLE
  case GEOMOp::OpEllipse:          // MENU BASIC - ELLIPSE
  case GEOMOp::OpArc:              // MENU BASIC - ARC
  case GEOMOp::OpVector:           // MENU BASIC - VECTOR
  case GEOMOp::OpPlane:            // MENU BASIC - PLANE
  case GEOMOp::OpCurve:            // MENU BASIC - CURVE
  case GEOMOp::OpLCS:              // MENU BASIC - LOCAL COORDINATE SYSTEM
    libName = "BasicGUI";
    break;
  case GEOMOp::OpBox:              // MENU PRIMITIVE - BOX
  case GEOMOp::OpCylinder:         // MENU PRIMITIVE - CYLINDER
  case GEOMOp::OpSphere:           // MENU PRIMITIVE - SPHERE
  case GEOMOp::OpTorus:            // MENU PRIMITIVE - TORUS
  case GEOMOp::OpCone:             // MENU PRIMITIVE - CONE
  case GEOMOp::OpRectangle:        // MENU PRIMITIVE - FACE
  case GEOMOp::OpDisk:             // MENU PRIMITIVE - DISK
    libName = "PrimitiveGUI";
    break;
  case GEOMOp::OpPrism:            // MENU GENERATION - PRISM
  case GEOMOp::OpRevolution:       // MENU GENERATION - REVOLUTION
  case GEOMOp::OpFilling:          // MENU GENERATION - FILLING
  case GEOMOp::OpPipe:             // MENU GENERATION - PIPE
    libName = "GenerationGUI";
    break;
  case GEOMOp::Op2dSketcher:       // MENU ENTITY - SKETCHER
  case GEOMOp::Op3dSketcher:       // MENU ENTITY - 3D SKETCHER
  case GEOMOp::OpExplode:          // MENU ENTITY - EXPLODE
    libName = "EntityGUI";
    break;
  case GEOMOp::OpEdge:             // MENU BUILD - EDGE
  case GEOMOp::OpWire:             // MENU BUILD - WIRE
  case GEOMOp::OpFace:             // MENU BUILD - FACE
  case GEOMOp::OpShell:            // MENU BUILD - SHELL
  case GEOMOp::OpSolid:            // MENU BUILD - SOLID
  case GEOMOp::OpCompound:         // MENU BUILD - COMPUND
    libName = "BuildGUI";
    break;
  case GEOMOp::OpFuse:             // MENU BOOLEAN - FUSE
  case GEOMOp::OpCommon:           // MENU BOOLEAN - COMMON
  case GEOMOp::OpCut:              // MENU BOOLEAN - CUT
  case GEOMOp::OpSection:          // MENU BOOLEAN - SECTION
    libName = "BooleanGUI";
    break;
  case GEOMOp::OpTranslate:        // MENU TRANSFORMATION - TRANSLATION
  case GEOMOp::OpRotate:           // MENU TRANSFORMATION - ROTATION
  case GEOMOp::OpChangeLoc:        // MENU TRANSFORMATION - LOCATION
  case GEOMOp::OpMirror:           // MENU TRANSFORMATION - MIRROR
  case GEOMOp::OpScale:            // MENU TRANSFORMATION - SCALE
  case GEOMOp::OpOffset:           // MENU TRANSFORMATION - OFFSET
  case GEOMOp::OpMultiTranslate:   // MENU TRANSFORMATION - MULTI-TRANSLATION
  case GEOMOp::OpMultiRotate:      // MENU TRANSFORMATION - MULTI-ROTATION
  case GEOMOp::OpReimport:         // CONTEXT(POPUP) MENU - RELOAD_IMPORTED
    libName = "TransformationGUI";
    break;
  case GEOMOp::OpPartition:        // MENU OPERATION - PARTITION
  case GEOMOp::OpArchimede:        // MENU OPERATION - ARCHIMEDE
  case GEOMOp::OpFillet3d:         // MENU OPERATION - FILLET
  case GEOMOp::OpChamfer:          // MENU OPERATION - CHAMFER
  case GEOMOp::OpClipping:         // MENU OPERATION - CLIPPING RANGE
  case GEOMOp::OpShapesOnShape:    // MENU OPERATION - GET SHAPES ON SHAPE
  case GEOMOp::OpFillet2d:         // MENU OPERATION - FILLET 2D
  case GEOMOp::OpFillet1d:         // MENU OPERATION - FILLET 1D
    libName = "OperationGUI";
    break;
  case GEOMOp::OpSewing:           // MENU REPAIR - SEWING
  case GEOMOp::OpSuppressFaces:    // MENU REPAIR - SUPPRESS FACES
  case GEOMOp::OpSuppressHoles:    // MENU REPAIR - SUPPRESS HOLE
  case GEOMOp::OpShapeProcess:     // MENU REPAIR - SHAPE PROCESSING
  case GEOMOp::OpCloseContour:     // MENU REPAIR - CLOSE CONTOUR
  case GEOMOp::OpRemoveIntWires:   // MENU REPAIR - REMOVE INTERNAL WIRES
  case GEOMOp::OpAddPointOnEdge:   // MENU REPAIR - ADD POINT ON EDGE
  case GEOMOp::OpFreeBoundaries:   // MENU MEASURE - FREE BOUNDARIES
  case GEOMOp::OpFreeFaces:        // MENU MEASURE - FREE FACES
  case GEOMOp::OpOrientation:      // MENU REPAIR - CHANGE ORIENTATION
  case GEOMOp::OpGlueFaces:        // MENU REPAIR - GLUE FACES
  case GEOMOp::OpLimitTolerance:   // MENU REPAIR - LIMIT TOLERANCE
  case GEOMOp::OpRemoveExtraEdges: // MENU REPAIR - REMOVE EXTRA EDGES
    libName = "RepairGUI";
    break;
  case GEOMOp::OpProperties:       // MENU MEASURE - PROPERTIES
  case GEOMOp::OpCenterMass:       // MENU MEASURE - CDG
  case GEOMOp::OpInertia:          // MENU MEASURE - INERTIA
  case GEOMOp::OpNormale:          // MENU MEASURE - NORMALE
  case GEOMOp::OpBoundingBox:      // MENU MEASURE - BOUNDING BOX
  case GEOMOp::OpMinDistance:      // MENU MEASURE - MIN DISTANCE
  case GEOMOp::OpAngle:            // MENU MEASURE - ANGLE
  case GEOMOp::OpTolerance:        // MENU MEASURE - TOLERANCE
  case GEOMOp::OpWhatIs:           // MENU MEASURE - WHATIS
  case GEOMOp::OpCheckShape:       // MENU MEASURE - CHECK
  case GEOMOp::OpCheckCompound:    // MENU MEASURE - CHECK COMPOUND OF BLOCKS
  case GEOMOp::OpPointCoordinates: // MENU MEASURE - POINT COORDINATES
    libName = "MeasureGUI";
    break;
  case GEOMOp::OpGroupCreate:      // MENU GROUP - CREATE
  case GEOMOp::OpGroupCreatePopup: // POPUP MENU - CREATE GROUP
  case GEOMOp::OpGroupEdit:        // MENU GROUP - EDIT
    libName = "GroupGUI";
    break;
  case GEOMOp::OpHexaSolid:        // MENU BLOCKS - HEXAHEDRAL SOLID
  case GEOMOp::OpMultiTransform:   // MENU BLOCKS - MULTI-TRANSFORMATION
  case GEOMOp::OpQuadFace:         // MENU BLOCKS - QUADRANGLE FACE
  case GEOMOp::OpPropagate:        // MENU BLOCKS - PROPAGATE
  case GEOMOp::OpExplodeBlock:     // MENU BLOCKS - EXPLODE ON BLOCKS
    libName = "BlocksGUI";
    break;
  case GEOMOp::OpAdvancedNoOp:     // NO OPERATION (advanced operations base)
  case GEOMOp::OpPipeTShape:       // MENU NEW ENTITY - ADVANCED - PIPE TSHAPE
//   case GEOMOp::OpPipeTShapeGroups:     // MENU NEW ENTITY - ADVANCED - PIPE TSHAPE GROUPS
    //@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@//
    libName = "AdvancedGUI";
    break;
  default:
    break;
  }

  GEOMGUI* library = 0;
  if ( !libName.isEmpty() ) {
#ifndef WNT
    libName = QString( "lib" ) + libName + ".so";
#else
    libName = libName + ".dll";
#endif
    library = getLibrary( libName );
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
void GeometryGUI::OnKeyPress( SUIT_ViewWindow* w, QKeyEvent* e )
{
  if ( !application() )
    return;
  foreach ( GEOMGUI* lib, myGUIMap )
    lib->OnKeyPress( e, application()->desktop(), w );
}

//=================================================================================
// function : GeometryGUI::OnMouseMove()
// purpose  : Manages mouse move events [static]
//=================================================================================
void GeometryGUI::OnMouseMove( SUIT_ViewWindow* w, QMouseEvent* e )
{
  if ( !application() )
    return;
  foreach ( GEOMGUI* lib, myGUIMap )
    lib->OnMouseMove( e, application()->desktop(), w );
}

//=================================================================================
// function : GeometryGUI::OnMousePress()
// purpose  : Manage mouse press events [static]
//=================================================================================
void GeometryGUI::OnMousePress( SUIT_ViewWindow* w, QMouseEvent* e )
{
  if ( !application() )
    return;
  foreach ( GEOMGUI* lib, myGUIMap )
    lib->OnMousePress( e, application()->desktop(), w );
}

//=======================================================================
// function : createGeomAction
// purpose  :
//=======================================================================
void GeometryGUI::createGeomAction( const int id, const QString& label, const QString& icolabel, 
				    const int accel, const bool toggle, const QString& shortcutAction )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  QPixmap icon = icolabel.isEmpty() ? resMgr->loadPixmap( "GEOM", tr( (QString( "ICO_" )+label).toLatin1().constData() ), false )
                                    : resMgr->loadPixmap( "GEOM", tr( icolabel.toLatin1().constData() ) );
  createAction( id,
		tr( QString( "TOP_%1" ).arg( label ).toLatin1().constData() ),
		icon,
		tr( QString( "MEN_%1" ).arg( label ).toLatin1().constData() ),
		tr( QString( "STB_%1" ).arg( label ).toLatin1().constData() ),
		accel,
		application()->desktop(),
		toggle,
		this, SLOT( OnGUIEvent() ),
		shortcutAction );
}

//=======================================================================
// function : createOriginAndBaseVectors
// purpose  :
//=======================================================================
void GeometryGUI::createOriginAndBaseVectors()
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( application()->activeStudy() );
  if( appStudy ) {
    _PTR(Study) studyDS = appStudy->studyDS();
    if( studyDS && !CORBA::is_nil( GetGeomGen() ) ) {
      GEOM::GEOM_IBasicOperations_var aBasicOperations = GetGeomGen()->GetIBasicOperations( studyDS->StudyId() );
      if( !aBasicOperations->_is_nil() ) {
        SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
        double aLength = aResourceMgr->doubleValue( "Geometry", "base_vectors_length", 1.0 );
        GEOM::GEOM_Object_var anOrigin = aBasicOperations->MakePointXYZ( 0.0, 0.0, 0.0 );
        GEOM::GEOM_Object_var anOX = aBasicOperations->MakeVectorDXDYDZ( aLength, 0.0, 0.0 );
        GEOM::GEOM_Object_var anOY = aBasicOperations->MakeVectorDXDYDZ( 0.0, aLength, 0.0 );
        GEOM::GEOM_Object_var anOZ = aBasicOperations->MakeVectorDXDYDZ( 0.0, 0.0, aLength );

        SALOMEDS::Study_var aDSStudy = ClientStudyToStudy( studyDS );
        GetGeomGen()->PublishInStudy( aDSStudy, SALOMEDS::SObject::_nil(), anOrigin, "O" );
        GetGeomGen()->PublishInStudy( aDSStudy, SALOMEDS::SObject::_nil(), anOX, "OX" );
        GetGeomGen()->PublishInStudy( aDSStudy, SALOMEDS::SObject::_nil(), anOY, "OY" );
        GetGeomGen()->PublishInStudy( aDSStudy, SALOMEDS::SObject::_nil(), anOZ, "OZ" );

        getApp()->updateObjectBrowser( false );
      }
    }
  }
}

//=======================================================================
// function : GeometryGUI::initialize()
// purpose  : Called when GEOM module is created
//=======================================================================
void GeometryGUI::initialize( CAM_Application* app )
{
  SalomeApp_Module::initialize( app );

  // ----- create actions --------------

  createGeomAction( GEOMOp::OpImport,     "IMPORT", "", Qt::ControlModifier + Qt::Key_I );
  createGeomAction( GEOMOp::OpExport,     "EXPORT", "", Qt::ControlModifier + Qt::Key_E );

  createGeomAction( GEOMOp::OpDelete,     "DELETE", "", Qt::Key_Delete );

  createGeomAction( GEOMOp::OpPoint,      "POINT" );
  createGeomAction( GEOMOp::OpLine,       "LINE" );
  createGeomAction( GEOMOp::OpCircle,     "CIRCLE" );
  createGeomAction( GEOMOp::OpEllipse,    "ELLIPSE" );
  createGeomAction( GEOMOp::OpArc,        "ARC" );
  createGeomAction( GEOMOp::OpCurve,      "CURVE" );
  createGeomAction( GEOMOp::OpVector,     "VECTOR" );
  createGeomAction( GEOMOp::OpPlane,      "PLANE" );
  createGeomAction( GEOMOp::OpLCS,        "LOCAL_CS" );
  createGeomAction( GEOMOp::OpOriginAndVectors, "ORIGIN_AND_VECTORS" );

  createGeomAction( GEOMOp::OpBox,        "BOX" );
  createGeomAction( GEOMOp::OpCylinder,   "CYLINDER" );
  createGeomAction( GEOMOp::OpSphere,     "SPHERE" );
  createGeomAction( GEOMOp::OpTorus,      "TORUS" );
  createGeomAction( GEOMOp::OpCone,       "CONE" );
  createGeomAction( GEOMOp::OpRectangle,  "RECTANGLE" );
  createGeomAction( GEOMOp::OpDisk,       "DISK" );

  createGeomAction( GEOMOp::OpPrism,       "EXTRUSION" );
  createGeomAction( GEOMOp::OpRevolution,  "REVOLUTION" );
  createGeomAction( GEOMOp::OpFilling,     "FILLING" );
  createGeomAction( GEOMOp::OpPipe,        "PIPE" );

  createGeomAction( GEOMOp::OpGroupCreate, "GROUP_CREATE" );
  createGeomAction( GEOMOp::OpGroupEdit,   "GROUP_EDIT" );

  createGeomAction( GEOMOp::OpReimport,    "RELOAD_IMPORTED" );

  createGeomAction( GEOMOp::OpQuadFace,    "Q_FACE" );
  createGeomAction( GEOMOp::OpHexaSolid,   "HEX_SOLID" );

  createGeomAction( GEOMOp::Op2dSketcher,  "SKETCH" );
  createGeomAction( GEOMOp::Op3dSketcher,  "3DSKETCH" );
  createGeomAction( GEOMOp::OpExplode,     "EXPLODE" );

  createGeomAction( GEOMOp::OpEdge,        "EDGE" );
  createGeomAction( GEOMOp::OpWire,        "WIRE" );
  createGeomAction( GEOMOp::OpFace,        "FACE" );
  createGeomAction( GEOMOp::OpShell,       "SHELL" );
  createGeomAction( GEOMOp::OpSolid,       "SOLID" );
  createGeomAction( GEOMOp::OpCompound,    "COMPOUND" );

  createGeomAction( GEOMOp::OpFuse,        "FUSE" );
  createGeomAction( GEOMOp::OpCommon,      "COMMON" );
  createGeomAction( GEOMOp::OpCut,         "CUT" );
  createGeomAction( GEOMOp::OpSection,     "SECTION" );

  createGeomAction( GEOMOp::OpTranslate,      "TRANSLATION" );
  createGeomAction( GEOMOp::OpRotate,         "ROTATION" );
  createGeomAction( GEOMOp::OpChangeLoc,      "MODIFY_LOCATION" );
  createGeomAction( GEOMOp::OpMirror,         "MIRROR" );
  createGeomAction( GEOMOp::OpScale,          "SCALE" );
  createGeomAction( GEOMOp::OpOffset,         "OFFSET" );
  createGeomAction( GEOMOp::OpMultiTranslate, "MUL_TRANSLATION" );
  createGeomAction( GEOMOp::OpMultiRotate,    "MUL_ROTATION" );

  createGeomAction( GEOMOp::OpPartition,      "PARTITION" );
  createGeomAction( GEOMOp::OpArchimede,      "ARCHIMEDE" );
  createGeomAction( GEOMOp::OpFillet3d,       "FILLET" );
  createGeomAction( GEOMOp::OpChamfer,        "CHAMFER" );
  //createGeomAction( GEOMOp::OpClipping,        "CLIPPING" );
  createGeomAction( GEOMOp::OpShapesOnShape,  "GET_SHAPES_ON_SHAPES" );
  createGeomAction( GEOMOp::OpFillet1d,       "FILLET_1D" );
  createGeomAction( GEOMOp::OpFillet2d,       "FILLET_2D" );

  createGeomAction( GEOMOp::OpMultiTransform, "MUL_TRANSFORM" );
  createGeomAction( GEOMOp::OpExplodeBlock,   "EXPLODE_BLOCKS" );
  createGeomAction( GEOMOp::OpPropagate,      "PROPAGATE" );

  createGeomAction( GEOMOp::OpSewing,           "SEWING" );
  createGeomAction( GEOMOp::OpGlueFaces,        "GLUE_FACES" );
  createGeomAction( GEOMOp::OpLimitTolerance,   "LIMIT_TOLERANCE" );
  createGeomAction( GEOMOp::OpSuppressFaces,    "SUPPRESS_FACES" );
  createGeomAction( GEOMOp::OpSuppressHoles,    "SUPPERSS_HOLES" );
  createGeomAction( GEOMOp::OpShapeProcess,     "SHAPE_PROCESS" );
  createGeomAction( GEOMOp::OpCloseContour,     "CLOSE_CONTOUR" );
  createGeomAction( GEOMOp::OpRemoveIntWires,   "SUPPRESS_INT_WIRES" );
  createGeomAction( GEOMOp::OpAddPointOnEdge,   "POINT_ON_EDGE" );
  createGeomAction( GEOMOp::OpFreeBoundaries,   "CHECK_FREE_BNDS" );
  createGeomAction( GEOMOp::OpFreeFaces,        "CHECK_FREE_FACES" );
  createGeomAction( GEOMOp::OpOrientation,      "CHANGE_ORIENTATION" );
  createGeomAction( GEOMOp::OpRemoveExtraEdges, "REMOVE_EXTRA_EDGES" );

  createGeomAction( GEOMOp::OpPointCoordinates, "POINT_COORDS" );
  createGeomAction( GEOMOp::OpProperties,       "BASIC_PROPS" );
  createGeomAction( GEOMOp::OpCenterMass,       "MASS_CENTER" );
  createGeomAction( GEOMOp::OpInertia,          "INERTIA" );
  createGeomAction( GEOMOp::OpNormale,          "NORMALE" );
  createGeomAction( GEOMOp::OpBoundingBox,      "BND_BOX" );
  createGeomAction( GEOMOp::OpMinDistance,      "MIN_DIST" );
  createGeomAction( GEOMOp::OpAngle,            "MEASURE_ANGLE" );

  createGeomAction( GEOMOp::OpTolerance,        "TOLERANCE" );
  createGeomAction( GEOMOp::OpWhatIs,           "WHAT_IS" );
  createGeomAction( GEOMOp::OpCheckShape,       "CHECK" );
  createGeomAction( GEOMOp::OpCheckCompound,    "CHECK_COMPOUND" );

#ifdef _DEBUG_ // PAL16821
  createGeomAction( GEOMOp::OpCheckGeom,        "CHECK_GEOMETRY" );
#endif

  createGeomAction( GEOMOp::OpDisplayMode,      "SHADING" );
  createGeomAction( GEOMOp::OpShowAll,          "DISPLAY_ALL" );
  createGeomAction( GEOMOp::OpHideAll,          "ERASE_ALL" );
  createGeomAction( GEOMOp::OpShow,             "DISPLAY" );
  createGeomAction( GEOMOp::OpSwitchVectors,    "VECTOR_MODE");
  createGeomAction( GEOMOp::OpSelectVertex,     "VERTEX_SEL_ONLY" ,"", 0, true );
  createGeomAction( GEOMOp::OpSelectEdge,       "EDGE_SEL_ONLY", "", 0, true );
  createGeomAction( GEOMOp::OpSelectWire,       "WIRE_SEL_ONLY", "",  0, true );
  createGeomAction( GEOMOp::OpSelectFace,       "FACE_SEL_ONLY", "", 0, true );
  createGeomAction( GEOMOp::OpSelectShell,      "SHELL_SEL_ONLY", "",  0, true );
  createGeomAction( GEOMOp::OpSelectSolid,      "SOLID_SEL_ONLY", "", 0, true );
  createGeomAction( GEOMOp::OpSelectCompound,   "COMPOUND_SEL_ONLY", "",  0, true );
  createGeomAction( GEOMOp::OpSelectAll,        "ALL_SEL_ONLY", "",  0, true );
  createGeomAction( GEOMOp::OpShowOnly,         "DISPLAY_ONLY" );
  createGeomAction( GEOMOp::OpHide,             "ERASE" );

  createGeomAction( GEOMOp::OpRename,           "POP_RENAME", "", Qt::Key_F2 );
  createGeomAction( GEOMOp::OpWireframe,        "POP_WIREFRAME", "", 0, true );
  createGeomAction( GEOMOp::OpShading,          "POP_SHADING", "", 0, true );
  createGeomAction( GEOMOp::OpVectors,          "POP_VECTORS", "", 0, true );
  createGeomAction( GEOMOp::OpDeflection,       "POP_DEFLECTION" );
  createGeomAction( GEOMOp::OpColor,            "POP_COLOR" );
  createGeomAction( GEOMOp::OpTransparency,     "POP_TRANSPARENCY" );
  createGeomAction( GEOMOp::OpIsos,             "POP_ISOS" );
  createGeomAction( GEOMOp::OpAutoColor,        "POP_AUTO_COLOR" );
  createGeomAction( GEOMOp::OpNoAutoColor,      "POP_DISABLE_AUTO_COLOR" );
  createGeomAction( GEOMOp::OpGroupCreatePopup, "POP_CREATE_GROUP" );
  createGeomAction( GEOMOp::OpShowChildren,     "POP_SHOW_CHILDREN" );
  createGeomAction( GEOMOp::OpHideChildren,     "POP_HIDE_CHILDREN" );
  createGeomAction( GEOMOp::OpPointMarker,      "POP_POINT_MARKER" );

  createGeomAction( GEOMOp::OpPipeTShape, "PIPETSHAPE" );
  
  // Create actions for increase/decrease transparency shortcuts
  createGeomAction( GEOMOp::OpIncrTransparency, "", "", 0, false, 
		    "Geometry:Increase transparency");
  createGeomAction( GEOMOp::OpDecrTransparency, "", "", 0, false, 
		    "Geometry:Decrease transparency");

  // Create actions for increase/decrease number of isolines
  createGeomAction( GEOMOp::OpIncrNbIsos, "", "", 0, false, 
		    "Geometry:Increase number of isolines");
  createGeomAction( GEOMOp::OpDecrNbIsos, "", "", 0, false, 
		    "Geometry:Decrease number of isolines");

//   createGeomAction( GEOMOp::OpPipeTShapeGroups, "PIPETSHAPEGROUPS" );
  //@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@//

  // ---- create menus --------------------------

  int fileId = createMenu( tr( "MEN_FILE" ), -1, -1 );
  createMenu( separator(),      fileId, 10 );
  createMenu( GEOMOp::OpImport, fileId, 10 );
  createMenu( GEOMOp::OpExport, fileId, 10 );
  createMenu( separator(),      fileId, -1 );

  int editId = createMenu( tr( "MEN_EDIT" ), -1, -1 );
  createMenu( GEOMOp::OpDelete, editId, -1 );

  int newEntId = createMenu( tr( "MEN_NEW_ENTITY" ), -1, -1, 10 );

  int basicId = createMenu( tr( "MEN_BASIC" ), newEntId, -1 );
  createMenu( GEOMOp::OpPoint,   basicId, -1 );
  createMenu( GEOMOp::OpLine,    basicId, -1 );
  createMenu( GEOMOp::OpCircle,  basicId, -1 );
  createMenu( GEOMOp::OpEllipse, basicId, -1 );
  createMenu( GEOMOp::OpArc,     basicId, -1 );
  createMenu( GEOMOp::OpCurve,   basicId, -1 );
  createMenu( separator(),       basicId, -1 );
  createMenu( GEOMOp::OpVector,  basicId, -1 );
  createMenu( GEOMOp::OpPlane,   basicId, -1 );
  createMenu( GEOMOp::OpLCS,     basicId, -1 );
  createMenu( GEOMOp::OpOriginAndVectors, basicId, -1 );

  int primId = createMenu( tr( "MEN_PRIMITIVES" ), newEntId, -1 );
  createMenu( GEOMOp::OpBox,       primId, -1 );
  createMenu( GEOMOp::OpCylinder,  primId, -1 );
  createMenu( GEOMOp::OpSphere,    primId, -1 );
  createMenu( GEOMOp::OpTorus,     primId, -1 );
  createMenu( GEOMOp::OpCone,      primId, -1 );
  createMenu( GEOMOp::OpRectangle, primId, -1 );
  createMenu( GEOMOp::OpDisk,      primId, -1 );

  int genId = createMenu( tr( "MEN_GENERATION" ), newEntId, -1 );
  createMenu( GEOMOp::OpPrism,      genId, -1 );
  createMenu( GEOMOp::OpRevolution, genId, -1 );
  createMenu( GEOMOp::OpFilling,    genId, -1 );
  createMenu( GEOMOp::OpPipe,       genId, -1 );

  int advId = createMenu( tr( "MEN_ADVANCED" ), newEntId, -1 );
  createMenu( GEOMOp::OpPipeTShape, advId, -1 );
//   createMenu( GEOMOp::OpPipeTShapeGroups, advId, -1 );
  //@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@//

  createMenu( separator(), newEntId, -1 );

  int groupId = createMenu( tr( "MEN_GROUP" ), newEntId, -1 );
  createMenu( GEOMOp::OpGroupCreate, groupId, -1 );
  createMenu( GEOMOp::OpGroupEdit,   groupId, -1 );

  createMenu( separator(), newEntId, -1 );

  int blocksId = createMenu( tr( "MEN_BLOCKS" ), newEntId, -1 );
  createMenu( GEOMOp::OpQuadFace,  blocksId, -1 );
  createMenu( GEOMOp::OpHexaSolid, blocksId, -1 );

  createMenu( separator(),          newEntId, -1 );

  createMenu( GEOMOp::Op2dSketcher, newEntId, -1 );
  createMenu( GEOMOp::Op3dSketcher, newEntId, -1 );

  createMenu( separator(),          newEntId, -1 );

  createMenu( GEOMOp::OpExplode,    newEntId, -1 );

  int buildId = createMenu( tr( "MEN_BUILD" ), newEntId, -1 );
  createMenu( GEOMOp::OpEdge,     buildId, -1 );
  createMenu( GEOMOp::OpWire,     buildId, -1 );
  createMenu( GEOMOp::OpFace,     buildId, -1 );
  createMenu( GEOMOp::OpShell,    buildId, -1 );
  createMenu( GEOMOp::OpSolid,    buildId, -1 );
  createMenu( GEOMOp::OpCompound, buildId, -1 );

  int operId = createMenu( tr( "MEN_OPERATIONS" ), -1, -1, 10 );

  int boolId = createMenu( tr( "MEN_BOOLEAN" ), operId, -1 );
  createMenu( GEOMOp::OpFuse,    boolId, -1 );
  createMenu( GEOMOp::OpCommon,  boolId, -1 );
  createMenu( GEOMOp::OpCut,     boolId, -1 );
  createMenu( GEOMOp::OpSection, boolId, -1 );

  int transId = createMenu( tr( "MEN_TRANSFORMATION" ), operId, -1 );
  createMenu( GEOMOp::OpTranslate,      transId, -1 );
  createMenu( GEOMOp::OpRotate,         transId, -1 );
  createMenu( GEOMOp::OpChangeLoc,      transId, -1 );
  createMenu( GEOMOp::OpMirror,         transId, -1 );
  createMenu( GEOMOp::OpScale,          transId, -1 );
  createMenu( GEOMOp::OpOffset,         transId, -1 );
  createMenu( separator(),              transId, -1 );
  createMenu( GEOMOp::OpMultiTranslate, transId, -1 );
  createMenu( GEOMOp::OpMultiRotate,    transId, -1 );

  int blockId = createMenu( tr( "MEN_BLOCKS" ), operId, -1 );
  createMenu( GEOMOp::OpMultiTransform, blockId, -1 );
  createMenu( GEOMOp::OpExplodeBlock,   blockId, -1 );
  createMenu( GEOMOp::OpPropagate,      blockId, -1 );

  createMenu( separator(), operId, -1 );

  createMenu( GEOMOp::OpPartition,     operId, -1 );
  createMenu( GEOMOp::OpArchimede,     operId, -1 );
  createMenu( GEOMOp::OpShapesOnShape, operId, -1 );

  createMenu( separator(), operId, -1 );

  createMenu( GEOMOp::OpFillet1d,      operId, -1 );
  createMenu( GEOMOp::OpFillet2d,      operId, -1 );
  createMenu( GEOMOp::OpFillet3d,      operId, -1 );
  createMenu( GEOMOp::OpChamfer,       operId, -1 );
  //createMenu( GEOMOp::OpClipping,      operId, -1 );

  int repairId = createMenu( tr( "MEN_REPAIR" ), -1, -1, 10 );
  createMenu( GEOMOp::OpShapeProcess,    repairId, -1 );
  createMenu( GEOMOp::OpSuppressFaces,   repairId, -1 );
  createMenu( GEOMOp::OpCloseContour,    repairId, -1 );
  createMenu( GEOMOp::OpRemoveIntWires,  repairId, -1 );
  createMenu( GEOMOp::OpSuppressHoles,   repairId, -1 );
  createMenu( GEOMOp::OpSewing,          repairId, -1 );
  createMenu( GEOMOp::OpGlueFaces,       repairId, -1 );
  createMenu( GEOMOp::OpLimitTolerance,  repairId, -1 );
  createMenu( GEOMOp::OpAddPointOnEdge,  repairId, -1 );
  //createMenu( GEOMOp::OpFreeBoundaries,  repairId, -1 );
  //createMenu( GEOMOp::OpFreeFaces,       repairId, -1 );
  createMenu( GEOMOp::OpOrientation,      repairId, -1 );
  createMenu( GEOMOp::OpRemoveExtraEdges, repairId, -1 );

  int measurId = createMenu( tr( "MEN_MEASURES" ), -1, -1, 10 );
  createMenu( GEOMOp::OpPointCoordinates, measurId, -1 );
  createMenu( GEOMOp::OpProperties,       measurId, -1 );
  createMenu( separator(),                measurId, -1 );
  createMenu( GEOMOp::OpCenterMass,       measurId, -1 );
  createMenu( GEOMOp::OpInertia,          measurId, -1 );
  createMenu( GEOMOp::OpNormale,          measurId, -1 );
  createMenu( separator(),                measurId, -1 );
  createMenu( GEOMOp::OpFreeBoundaries,   measurId, -1 );
  createMenu( GEOMOp::OpFreeFaces,        measurId, -1 );
  createMenu( separator(),                measurId, -1 );

  int dimId = createMenu( tr( "MEN_DIMENSIONS" ), measurId, -1 );
  createMenu( GEOMOp::OpBoundingBox, dimId, -1 );
  createMenu( GEOMOp::OpMinDistance, dimId, -1 );
  createMenu( GEOMOp::OpAngle,       dimId, -1 );

  createMenu( separator(),             measurId, -1 );
  createMenu( GEOMOp::OpTolerance,     measurId, -1 );
  createMenu( separator(),             measurId, -1 );
  createMenu( GEOMOp::OpWhatIs,        measurId, -1 );
  createMenu( GEOMOp::OpCheckShape,    measurId, -1 );
  createMenu( GEOMOp::OpCheckCompound, measurId, -1 );

#ifdef _DEBUG_ // PAL16821
  int toolsId = createMenu( tr( "MEN_TOOLS" ), -1, -1, 50 );
  createMenu( separator(),         toolsId, -1 );
  createMenu( GEOMOp::OpCheckGeom, toolsId, -1 );
#endif

  int viewId = createMenu( tr( "MEN_VIEW" ), -1, -1 );
  createMenu( separator(),       viewId, -1 );

  int dispmodeId = createMenu( tr( "MEN_DISPLAY_MODE" ), viewId, -1 );
  createMenu( GEOMOp::OpDisplayMode,   dispmodeId, -1 );
  createMenu( separator(),             dispmodeId, -1 );
  createMenu( GEOMOp::OpSwitchVectors, dispmodeId, -1 );

  createMenu( separator(),       viewId, -1 );
  createMenu( GEOMOp::OpShowAll, viewId, -1 );
  createMenu( GEOMOp::OpHideAll, viewId, -1 );
  createMenu( separator(),       viewId, -1 );

/*
  PAL9111:
  because of these items are accessible through object browser and viewers
  we have removed they from main menu

  createMenu( GEOMOp::OpShow, viewId, -1 );
  createMenu( GEOMOp::OpShowOnly, viewId, -1 );
  createMenu( GEOMOp::OpHide, viewId, -1 );
*/

  // ---- create toolbars --------------------------

  int basicTbId = createTool( tr( "TOOL_BASIC" ) );
  createTool( GEOMOp::OpPoint,   basicTbId );
  createTool( GEOMOp::OpLine,    basicTbId );
  createTool( GEOMOp::OpCircle,  basicTbId );
  createTool( GEOMOp::OpEllipse, basicTbId );
  createTool( GEOMOp::OpArc,     basicTbId );
  createTool( GEOMOp::OpCurve,   basicTbId );
  createTool( GEOMOp::OpVector,  basicTbId );
  createTool( GEOMOp::OpPlane,   basicTbId );
  createTool( GEOMOp::OpLCS,     basicTbId );
  createTool( GEOMOp::OpOriginAndVectors, basicTbId );

  int primTbId = createTool( tr( "TOOL_PRIMITIVES" ) );
  createTool( GEOMOp::OpBox,       primTbId );
  createTool( GEOMOp::OpCylinder,  primTbId );
  createTool( GEOMOp::OpSphere,    primTbId );
  createTool( GEOMOp::OpTorus,     primTbId );
  createTool( GEOMOp::OpCone,      primTbId );
  createTool( GEOMOp::OpRectangle, primTbId );
  createTool( GEOMOp::OpDisk,      primTbId );

  int boolTbId = createTool( tr( "TOOL_BOOLEAN" ) );
  createTool( GEOMOp::OpFuse,    boolTbId );
  createTool( GEOMOp::OpCommon,  boolTbId );
  createTool( GEOMOp::OpCut,     boolTbId );
  createTool( GEOMOp::OpSection, boolTbId );

  int genTbId = createTool( tr( "TOOL_GENERATION" ) );
  createTool( GEOMOp::OpPrism,      genTbId );
  createTool( GEOMOp::OpRevolution, genTbId );
  createTool( GEOMOp::OpFilling,    genTbId );
  createTool( GEOMOp::OpPipe,       genTbId );

  int transTbId = createTool( tr( "TOOL_TRANSFORMATION" ) );
  createTool( GEOMOp::OpTranslate,      transTbId );
  createTool( GEOMOp::OpRotate,         transTbId );
  createTool( GEOMOp::OpChangeLoc,      transTbId );
  createTool( GEOMOp::OpMirror,         transTbId );
  createTool( GEOMOp::OpScale,          transTbId );
  createTool( GEOMOp::OpOffset,         transTbId );
  createTool( separator(),              transTbId );
  createTool( GEOMOp::OpMultiTranslate, transTbId );
  createTool( GEOMOp::OpMultiRotate,    transTbId );

  int operTbId = createTool( tr( "TOOL_OPERATIONS" ) );
  createTool( GEOMOp::Op2dSketcher,      operTbId );
  createTool( GEOMOp::Op3dSketcher,      operTbId );
  createTool( separator(),               operTbId );
  createTool( GEOMOp::OpExplode,         operTbId );
  createTool( separator(),               operTbId );
  createTool( GEOMOp::OpPartition,       operTbId );
  createTool( GEOMOp::OpArchimede,       operTbId );
  createTool( GEOMOp::OpShapesOnShape,   operTbId );
  createTool( separator(),               operTbId );
  createTool( GEOMOp::OpFillet1d,        operTbId );
  createTool( GEOMOp::OpFillet2d,        operTbId );
  createTool( GEOMOp::OpFillet3d,        operTbId );
  createTool( GEOMOp::OpChamfer,         operTbId );

  int buildTbId = createTool( tr( "TOOL_BUILD" ) );
  createTool( GEOMOp::OpEdge,     buildTbId );
  createTool( GEOMOp::OpWire,     buildTbId );
  createTool( GEOMOp::OpFace,     buildTbId );
  createTool( GEOMOp::OpShell,    buildTbId );
  createTool( GEOMOp::OpSolid,    buildTbId );
  createTool( GEOMOp::OpCompound, buildTbId );

  int measureTbId = createTool( tr( "TOOL_MEASURES" ) );
  createTool( GEOMOp::OpPointCoordinates, measureTbId );
  createTool( GEOMOp::OpProperties,       measureTbId );
  createTool( GEOMOp::OpCenterMass,       measureTbId );
  createTool( GEOMOp::OpInertia,          measureTbId );
  createTool( GEOMOp::OpNormale,          measureTbId );
  createTool( separator(),                measureTbId );
  createTool( GEOMOp::OpBoundingBox,      measureTbId );
  createTool( GEOMOp::OpMinDistance,      measureTbId );
  createTool( GEOMOp::OpAngle,            measureTbId );
  createTool( GEOMOp::OpTolerance  ,      measureTbId );
  createTool( separator(),                measureTbId );
  createTool( GEOMOp::OpFreeBoundaries,   measureTbId );
  createTool( GEOMOp::OpFreeFaces,        measureTbId );
  createTool( separator(),                measureTbId );
  createTool( GEOMOp::OpWhatIs,           measureTbId );
  createTool( GEOMOp::OpCheckShape,       measureTbId );
  createTool( GEOMOp::OpCheckCompound,    measureTbId );

  int advancedTbId = createTool( tr( "TOOL_ADVANCED" ) );
  createTool( GEOMOp::OpPipeTShape, advancedTbId );
  //@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@//

  // ---- create popup menus --------------------------

  QString clientOCCorVTK = "(client='OCCViewer' or client='VTKViewer')";
  QString clientOCCorVTK_AndSomeVisible = clientOCCorVTK + " and selcount>0 and isVisible";

  QString clientOCCorVTKorOB = "(client='ObjectBrowser' or client='OCCViewer' or client='VTKViewer')";
  QString clientOCCorVTKorOB_AndSomeVisible = clientOCCorVTKorOB + " and selcount>0 and isVisible";

  QString autoColorPrefix =
    "(client='ObjectBrowser' or client='OCCViewer') and type='Shape' and selcount=1 and isOCC=true";

  QtxPopupMgr* mgr = popupMgr();
  mgr->insert( action(  GEOMOp::OpRename ), -1, -1 );  // rename
  mgr->setRule( action( GEOMOp::OpRename ), QString("$type in {'Shape' 'Group'} and selcount=1"), QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpDelete ), -1, -1 );  // delete
  mgr->setRule( action( GEOMOp::OpDelete ), QString("$type in {'Shape' 'Group'} and selcount>0"), QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpGroupCreatePopup ), -1, -1 ); // create group
  mgr->setRule( action( GEOMOp::OpGroupCreatePopup ), QString("client='ObjectBrowser' and type='Shape' and selcount=1 and isOCC=true"), QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpShowChildren ), -1, -1 ); // show children
  mgr->setRule( action( GEOMOp::OpShowChildren ), QString("client='ObjectBrowser' and type='Shape' and selcount=1 and hasHiddenChildren=true"), QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpHideChildren ), -1, -1 ); // hide children
  mgr->setRule( action( GEOMOp::OpHideChildren ), QString("client='ObjectBrowser' and type='Shape' and selcount=1 and hasShownChildren=true"), QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpGroupEdit ), -1, -1 );  // edit group
  mgr->setRule( action( GEOMOp::OpGroupEdit ),  QString("client='ObjectBrowser' and type='Group' and selcount=1 and isOCC=true"), QtxPopupMgr::VisibleRule );
  mgr->insert( separator(), -1, -1 );     // -----------
  dispmodeId = mgr->insert(  tr( "MEN_DISPLAY_MODE" ), -1, -1 ); // display mode menu
  mgr->insert( action(  GEOMOp::OpWireframe ), dispmodeId, -1 ); // wireframe
  mgr->setRule( action( GEOMOp::OpWireframe ), clientOCCorVTK_AndSomeVisible, QtxPopupMgr::VisibleRule );
  mgr->setRule( action( GEOMOp::OpWireframe ), clientOCCorVTK + " and displaymode='Wireframe'", QtxPopupMgr::ToggleRule );
  mgr->insert( action(  GEOMOp::OpShading ), dispmodeId, -1 ); // shading
  mgr->setRule( action( GEOMOp::OpShading ), clientOCCorVTK_AndSomeVisible, QtxPopupMgr::VisibleRule );
  mgr->setRule( action( GEOMOp::OpShading ), clientOCCorVTK + " and displaymode='Shading'", QtxPopupMgr::ToggleRule );
  mgr->insert( separator(), dispmodeId, -1 );
  mgr->insert( action(  GEOMOp::OpVectors ), dispmodeId, -1 ); // vectors
  mgr->setRule( action( GEOMOp::OpVectors ), clientOCCorVTK_AndSomeVisible, QtxPopupMgr::VisibleRule );
  mgr->setRule( action( GEOMOp::OpVectors ), clientOCCorVTK + " and isVectorsMode", QtxPopupMgr::ToggleRule );
  mgr->insert( separator(), -1, -1 );     // -----------
  mgr->insert( action(  GEOMOp::OpColor ), -1, -1 ); // color
  mgr->setRule( action( GEOMOp::OpColor ), clientOCCorVTKorOB_AndSomeVisible + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpTransparency ), -1, -1 ); // transparency
  mgr->setRule( action( GEOMOp::OpTransparency ), clientOCCorVTK_AndSomeVisible, QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpIsos ), -1, -1 ); // isos
  mgr->setRule( action( GEOMOp::OpIsos ), clientOCCorVTK_AndSomeVisible + " and selcount>0 and isVisible", QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpDeflection ), -1, -1 ); // deflection
  mgr->setRule( action( GEOMOp::OpDeflection ), "selcount>0 and isVisible and client='OCCViewer'", QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpPointMarker ), -1, -1 ); // point marker
  //mgr->setRule( action( GEOMOp::OpPointMarker ), QString( "selcount>0 and $typeid in {%1}" ).arg(GEOM_POINT ), QtxPopupMgr::VisibleRule );
  mgr->setRule( action( GEOMOp::OpPointMarker ), QString( "selcount>0 and ( $typeid in {%1} or compoundOfVertices=true ) " ).arg(GEOM::VERTEX).arg(GEOM::COMPOUND), QtxPopupMgr::VisibleRule );
  mgr->insert( separator(), -1, -1 );     // -----------
  mgr->insert( action(  GEOMOp::OpAutoColor ), -1, -1 ); // auto color
  mgr->setRule( action( GEOMOp::OpAutoColor ), autoColorPrefix + " and isAutoColor=false", QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpNoAutoColor ), -1, -1 ); // disable auto color
  mgr->setRule( action( GEOMOp::OpNoAutoColor ), autoColorPrefix + " and isAutoColor=true", QtxPopupMgr::VisibleRule );
  mgr->insert( separator(), -1, -1 );     // -----------

  QString canDisplay = "($component={'GEOM'}) and (selcount>0) and ({true} in $canBeDisplayed) ",
          onlyComponent = "((type='Component') and selcount=1)",
          rule = canDisplay + "and ((($type in {%1}) and( %2 )) or " + onlyComponent + ")",
          types = "'Shape' 'Group'";

  mgr->insert( action(  GEOMOp::OpShow ), -1, -1 ); // display
  mgr->setRule( action( GEOMOp::OpShow ), rule.arg( types ).arg( "not isVisible" ), QtxPopupMgr::VisibleRule );

  mgr->insert( action(  GEOMOp::OpHide ), -1, -1 ); // erase
  mgr->setRule( action( GEOMOp::OpHide ), rule.arg( types ).arg( "isVisible" ), QtxPopupMgr::VisibleRule );

  mgr->insert( action(  GEOMOp::OpHideAll ), -1, -1 ); // erase All
  mgr->setRule( action( GEOMOp::OpHideAll ), clientOCCorVTK, QtxPopupMgr::VisibleRule );

  QString selectOnly = "(client='OCCViewer' or client='VTKViewer') and (selcount=0)";

  int selectonlyId = mgr->insert( tr("MEN_SELECT_ONLY"), -1, -1);                //select only menu
  mgr->insert( action(GEOMOp::OpSelectVertex),   selectonlyId, -1);                                  //Vertex
  mgr->setRule(action(GEOMOp::OpSelectVertex),   selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(GEOMOp::OpSelectVertex),   selectOnly + " and selectionmode='VERTEX'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(GEOMOp::OpSelectEdge),     selectonlyId, -1);                                  //Edge
  mgr->setRule(action(GEOMOp::OpSelectEdge),     selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(GEOMOp::OpSelectEdge),     selectOnly + " and selectionmode='EDGE'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(GEOMOp::OpSelectWire),     selectonlyId, -1);                                  //Wire
  mgr->setRule(action(GEOMOp::OpSelectWire),     selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(GEOMOp::OpSelectWire),     selectOnly + " and selectionmode='WIRE'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(GEOMOp::OpSelectFace),     selectonlyId, -1);                                  //Face
  mgr->setRule(action(GEOMOp::OpSelectFace),     selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(GEOMOp::OpSelectFace),     selectOnly + " and selectionmode='FACE'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(GEOMOp::OpSelectShell),    selectonlyId, -1);                                  //Shell
  mgr->setRule(action(GEOMOp::OpSelectShell),    selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(GEOMOp::OpSelectShell),    selectOnly + " and selectionmode='SHELL'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(GEOMOp::OpSelectSolid),    selectonlyId, -1);                                  //Solid
  mgr->setRule(action(GEOMOp::OpSelectSolid),    selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(GEOMOp::OpSelectSolid),    selectOnly + " and selectionmode='SOLID'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(GEOMOp::OpSelectCompound), selectonlyId, -1);                                  //Compound
  mgr->setRule(action(GEOMOp::OpSelectCompound), selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(GEOMOp::OpSelectCompound), selectOnly + " and selectionmode='COMPOUND'", QtxPopupMgr::ToggleRule);
  mgr->insert( separator(), selectonlyId, -1);
  mgr->insert( action(GEOMOp::OpSelectAll),      selectonlyId, -1);                                  //Clear selection filter
  mgr->setRule(action(GEOMOp::OpSelectAll),      selectOnly, QtxPopupMgr::VisibleRule);
  mgr->setRule(action(GEOMOp::OpSelectAll),      selectOnly + " and selectionmode='ALL'", QtxPopupMgr::ToggleRule);
  mgr->insert( action(GEOMOp::OpShowOnly ), -1, -1 ); // display only
  mgr->setRule(action(GEOMOp::OpShowOnly ), rule.arg( types ).arg( "true" ), QtxPopupMgr::VisibleRule );
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

  // import Python module that manages GEOM plugins (need to be here because SalomePyQt API uses active module)
  PyGILState_STATE gstate = PyGILState_Ensure();
  PyObject* pluginsmanager=PyImport_ImportModule((char*)"salome_pluginsmanager");
  if(pluginsmanager==NULL)
    PyErr_Print();
  else
    {
      PyObject* result=PyObject_CallMethod( pluginsmanager, (char*)"initialize", (char*)"isss",1,"geom",tr("MEN_NEW_ENTITY").toStdString().c_str(),tr("GEOM_PLUGINS_OTHER").toStdString().c_str());
      if(result==NULL)
        PyErr_Print();
      Py_XDECREF(result);
    }
  PyGILState_Release(gstate);
  // end of GEOM plugins loading

  connect( application()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
          this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ) );

  // Reset actions accelerator keys
  action(GEOMOp::OpImport)->setEnabled( true ); // Import: CTRL + Key_I
  action(GEOMOp::OpExport)->setEnabled( true ); // Export: CTRL + Key_E
  action(GEOMOp::OpDelete)->setEnabled( true ); // Delete: Key_Delete
  action(GEOMOp::OpRename)->setEnabled( true ); // Rename: Key_F2

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

  QMenu* viewMenu = menuMgr()->findMenu( STD_Application::MenuViewId );
  if ( viewMenu )
    connect( viewMenu, SIGNAL( aboutToShow() ), this, SLOT( onViewAboutToShow() ) );

  // 0020836 (Basic vectors and origin)
  SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
  if( aResourceMgr->booleanValue( "Geometry", "auto_create_base_objects", false ) ) {
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( application()->activeStudy() );
    if( appStudy ) {
      _PTR(Study) studyDS = appStudy->studyDS();
      if( studyDS ) {
        _PTR(SComponent) aSComponent = studyDS->FindComponent("GEOM");
        if( !aSComponent ) // create objects automatically only if there is no GEOM component
          createOriginAndBaseVectors();
      }
    }
  }

  return true;
}


//=======================================================================
// function : GeometryGUI::deactivateModule()
// purpose  : Called when GEOM module is deactivated
//=======================================================================
bool GeometryGUI::deactivateModule( SUIT_Study* study )
{
  QMenu* viewMenu = menuMgr()->findMenu( STD_Application::MenuViewId );
  if ( viewMenu )
    disconnect( viewMenu, SIGNAL( aboutToShow() ), this, SLOT( onViewAboutToShow() ) );

  setMenuShown( false );
  setToolShown( false );

  disconnect( application()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
             this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ) );

  EmitSignalCloseAllDialogs();

  GUIMap::Iterator it;
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it )
    it.value()->deactivate();

  // Unset actions accelerator keys
  action(GEOMOp::OpImport)->setEnabled( false ); // Import: CTRL + Key_I
  action(GEOMOp::OpExport)->setEnabled( false ); // Export: CTRL + Key_E
  action(GEOMOp::OpDelete)->setEnabled( false ); // Delete: Key_Delete
  action(GEOMOp::OpRename)->setEnabled( false ); // Rename: Key_F2

  qDeleteAll(myOCCSelectors);
  myOCCSelectors.clear();
  getApp()->selectionMgr()->setEnabled( true, OCCViewer_Viewer::Type() );

  qDeleteAll(myVTKSelectors);
  myVTKSelectors.clear();
  getApp()->selectionMgr()->setEnabled( true, SVTK_Viewer::Type() );

  return SalomeApp_Module::deactivateModule( study );
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
//  action( GEOMOp::Op2dSketcher )->setEnabled( ViewOCC ); // SKETCHER
  action( GEOMOp::OpSuppressFaces )->setEnabled( ViewOCC ); // SuppressFace
  action( GEOMOp::OpSuppressHoles )->setEnabled( ViewOCC ); // SuppressHole
  action( GEOMOp::OpCloseContour )->setEnabled( ViewOCC ); // CloseContour
  action( GEOMOp::OpRemoveIntWires )->setEnabled( ViewOCC ); // RemoveInternalWires
  action( GEOMOp::OpAddPointOnEdge )->setEnabled( ViewOCC ); // AddPointOnEdge
//  action( GEOMOp::OpFreeBoundaries )->setEnabled( ViewOCC ); // Free boundaries

  action( GEOMOp::OpGroupCreate )->setEnabled( ViewOCC ); // Create Group
  action( GEOMOp::OpGroupEdit )->setEnabled( ViewOCC ); // Edit Group

  action( GEOMOp::OpMultiTransform )->setEnabled( ViewOCC ); // MENU BLOCKS - MULTI-TRANSFORMATION
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
  if ( vm && vm->getType() == OCCViewer_Viewer::Type() )
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

Handle(Graphic3d_HArray1OfBytes) GeometryGUI::getTexture( SalomeApp_Study* theStudy, int theId, int& theWidth, int& theHeight )
{
  theWidth = theHeight = 0;
  Handle(Graphic3d_HArray1OfBytes) aTexture;
  if ( theStudy ) {
    TextureMap aTextureMap = myTextureMap[ theStudy->studyDS()->StudyId() ];
    aTexture = aTextureMap[ theId ];
    if ( aTexture.IsNull() ) {
      GEOM::GEOM_IInsertOperations_var aInsOp = GeometryGUI::GetGeomGen()->GetIInsertOperations( theStudy->studyDS()->StudyId() );
      if ( !aInsOp->_is_nil() ) {
        CORBA::Long aWidth, aHeight;
        SALOMEDS::TMPFile_var aStream = aInsOp->GetTexture( theId, aWidth, aHeight );
        if ( aWidth > 0 && aHeight > 0 && aStream->length() > 0 ) {
          theWidth  = aWidth;
          theHeight = aHeight;
          aTexture  = new Graphic3d_HArray1OfBytes( 1, aStream->length() );
          for ( int i = 0; i < aStream->length(); i++ )
            aTexture->SetValue( i+1, (Standard_Byte)aStream[i] );
          aTextureMap[ theId ] = aTexture;
        }
      }
    }
  }
  return aTexture;
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
    } else {
      isImported = false;
    }
  }

  if (isImported) {
    menu->addAction(action(GEOMOp::OpReimport)); // Reload imported shape
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

  int defl = addPreference( tr( "PREF_DEFLECTION" ), genGroup,
                            LightApp_Preferences::DblSpin, "Geometry", "deflection_coeff" );

  // Quantities with individual precision settings
  int precGroup = addPreference( tr( "GEOM_PREF_GROUP_PRECISION" ), tabId );
  setPreferenceProperty( precGroup, "columns", 2 );

  const int nbQuantities = 8;
  int prec[nbQuantities], ii = 0;
  prec[ii++] = addPreference( tr( "GEOM_PREF_length_precision" ), precGroup,
                              LightApp_Preferences::IntSpin, "Geometry", "length_precision" );
  prec[ii++] = addPreference( tr( "GEOM_PREF_angle_precision" ), precGroup,
                              LightApp_Preferences::IntSpin, "Geometry", "angle_precision" );
  prec[ii++] = addPreference( tr( "GEOM_PREF_len_tol_precision" ), precGroup,
                              LightApp_Preferences::IntSpin, "Geometry", "len_tol_precision" );
  prec[ii++] = addPreference( tr( "GEOM_PREF_ang_tol_precision" ), precGroup,
                              LightApp_Preferences::IntSpin, "Geometry", "ang_tol_precision" );
  prec[ii++] = addPreference( tr( "GEOM_PREF_weight_precision" ), precGroup,
                              LightApp_Preferences::IntSpin, "Geometry", "weight_precision" );
  prec[ii++] = addPreference( tr( "GEOM_PREF_density_precision" ), precGroup,
                              LightApp_Preferences::IntSpin, "Geometry", "density_precision" );
  prec[ii++] = addPreference( tr( "GEOM_PREF_parametric_precision" ), precGroup,
                              LightApp_Preferences::IntSpin, "Geometry", "parametric_precision" );
  prec[ii  ] = addPreference( tr( "GEOM_PREF_param_tol_precision" ), precGroup,
                              LightApp_Preferences::IntSpin, "Geometry", "param_tol_precision" );

  // Set property for precision value for spinboxes
  for ( ii = 0; ii < nbQuantities; ii++ ){
    setPreferenceProperty( prec[ii], "min", -14 );
    setPreferenceProperty( prec[ii], "max", 14 );
    setPreferenceProperty( prec[ii], "precision", 2 );
  }

  int VertexGroup = addPreference( tr( "PREF_GROUP_VERTEX" ), tabId );
  setPreferenceProperty( VertexGroup, "columns", 2 );

  int typeOfMarker = addPreference( tr( "PREF_TYPE_OF_MARKER" ), VertexGroup,
                                    LightApp_Preferences::Selector, "Geometry", "type_of_marker" );

  int markerScale = addPreference( tr( "PREF_MARKER_SCALE" ), VertexGroup,
                                   LightApp_Preferences::Selector, "Geometry", "marker_scale" );

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

  // Set property for deflection value for spinboxes
  setPreferenceProperty( defl, "min", DEFLECTION_MIN );
  setPreferenceProperty( defl, "max", 1.0 );
  setPreferenceProperty( defl, "step", 1.0e-04 );
  setPreferenceProperty( defl, "precision", 6 );

  // Set property vertex marker type
  QList<QVariant> aMarkerTypeIndicesList;
  QList<QVariant> aMarkerTypeIconsList;

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  for ( int i = GEOM::MT_POINT; i < GEOM::MT_USER; i++ ) {
    QString icoFile = QString( "ICON_VERTEX_MARKER_%1" ).arg( i );
    QPixmap pixmap = resMgr->loadPixmap( "GEOM", tr( qPrintable( icoFile ) ) );
    aMarkerTypeIndicesList << (i-1);
    aMarkerTypeIconsList << pixmap;
  }

  setPreferenceProperty( typeOfMarker, "indexes", aMarkerTypeIndicesList );
  setPreferenceProperty( typeOfMarker, "icons",   aMarkerTypeIconsList );

  // Set property for vertex marker scale
  QList<QVariant> aMarkerScaleIndicesList;
  QStringList     aMarkerScaleValuesList;

  for ( int iii = GEOM::MS_10; iii <= GEOM::MS_70; iii++ ) {
    aMarkerScaleIndicesList << iii;
    aMarkerScaleValuesList  << QString::number( (iii-(int)GEOM::MS_10)*0.5 + 1.0 );
  }

  setPreferenceProperty( markerScale, "strings", aMarkerScaleValuesList );
  setPreferenceProperty( markerScale, "indexes", aMarkerScaleIndicesList );

  int originGroup = addPreference( tr( "PREF_GROUP_ORIGIN_AND_BASE_VECTORS" ), tabId );
  setPreferenceProperty( originGroup, "columns", 2 );

  int baseVectorsLength = addPreference( tr( "PREF_BASE_VECTORS_LENGTH" ), originGroup,
                                         LightApp_Preferences::DblSpin, "Geometry", "base_vectors_length" );
  setPreferenceProperty( baseVectorsLength, "min", 0.01 );
  setPreferenceProperty( baseVectorsLength, "max", 1000 );

  addPreference( tr( "PREF_AUTO_CREATE" ), originGroup,
                 LightApp_Preferences::Bool, "Geometry", "auto_create_base_objects" );

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

void GeometryGUI::onViewAboutToShow()
{
  SUIT_ViewWindow* window = application()->desktop()->activeWindow();
  QAction* a = action( GEOMOp::OpSwitchVectors );
  if ( window ) {
    a->setEnabled(true);
    bool vmode = window->getCustomData("VectorsMode").toBool();
    a->setText ( vmode == 1 ? tr( "MEN_VECTOR_MODE_OFF" ) : tr("MEN_VECTOR_MODE_ON") );
  } else {
    a->setText ( tr("MEN_VECTOR_MODE_ON") );
    a->setEnabled(false);
  }
}
