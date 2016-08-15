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
//  File   : GeometryGUI.cxx
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include <Standard_math.hxx>  // E.A. must be included before Python.h to fix compilation on windows
#ifdef HAVE_FINITE
#undef HAVE_FINITE            // VSR: avoid compilation warning on Linux : "HAVE_FINITE" redefined
#endif
#include "Python.h"
#include "GeometryGUI.h"
#include "GeometryGUI_Operations.h"
#include "GEOMPluginGUI.h"
#include "GEOMGUI_OCCSelector.h"
#include "GEOMGUI_Selection.h"
#include "GEOMGUI_CreationInfoWdg.h"
#include "GEOMGUI_TextTreeWdg.h"
#include "GEOMGUI_DimensionProperty.h"
#include "GEOM_Constants.h"
#include "GEOM_Displayer.h"
#include "GEOM_AISShape.hxx"
#include "GEOMUtils_XmlHandler.hxx"

#include "GEOM_Actor.h"

#include <Material_ResourceMgr.h>
#include <Material_Model.h>

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

#ifndef DISABLE_GRAPHICSVIEW
#include <GraphicsView_Viewer.h>
#endif

#include <SalomeApp_Application.h>
#include <SalomeApp_DataObject.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Tools.h>

#include <LightApp_SelectionMgr.h>
#include <LightApp_VTKSelector.h>
#include <LightApp_DataObject.h>
#include <LightApp_Preferences.h>

#include <SALOME_LifeCycleCORBA.hxx>
#include <SALOME_ListIO.hxx>

#include <SALOMEDSClient_ClientFactory.hxx>
#include <SALOMEDSClient_IParameters.hxx>

#include <SALOMEDS_SObject.hxx>

#include <Basics_OCCTVersion.hxx>
#include <QtxFontEdit.h>

// External includes
#include <QDir>
#include <QSet>
#include <QMenu>
#include <QTime>
#include <QAction>
#include <QFileInfo>
#include <QString>
#include <QPainter>
#include <QSignalMapper>
#include <QFontDatabase>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <OSD_SharedLibrary.hxx>
#include <NCollection_DataMap.hxx>

#include <TColStd_HArray1OfByte.hxx>
#include <TColStd_SequenceOfHAsciiString.hxx>

#include <utilities.h>

#include <vtkCamera.h>
#include <vtkRenderer.h>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

#include <Font_SystemFont.hxx>
#include <Font_FontMgr.hxx>
#include <TCollection_HAsciiString.hxx>

#include "GEOM_version.h"
#include "GEOMImpl_Types.hxx" // dangerous hxx (defines short-name macros) - include after all

extern "C" {
  Standard_EXPORT CAM_Module* createModule() {
    return new GeometryGUI();
  }

  Standard_EXPORT char* getModuleVersion() {
    return (char*)GEOM_VERSION_STR;
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
  if ( CORBA::is_nil( myComponentGeom ) ) return false;
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

void GeometryGUI::Modified (bool theIsUpdateActions)
{
  if ( SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() ) ) {
    if ( SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() ) ) {
      appStudy->Modified();
      if ( theIsUpdateActions )
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
  myTopLevelIOList()
{
  if ( CORBA::is_nil( myComponentGeom ) )
  {
    Engines::EngineComponent_var comp =
      SalomeApp_Application::lcc()->FindOrLoad_Component( "FactoryServer", "GEOM" );
    myComponentGeom = GEOM::GEOM_Gen::_narrow( comp );
  }

  myActiveDialogBox = 0;

  gp_Pnt origin = gp_Pnt(0., 0., 0.);
  gp_Dir direction = gp_Dir(0., 0., 1.);
  myWorkingPlane = gp_Ax3(origin, direction);

  myDisplayer = 0;
  myLocalSelectionMode = GEOM_ALLOBJECTS;

  myCreationInfoWdg = 0;
  myTextTreeWdg = 0;

  connect( Material_ResourceMgr::resourceMgr(), SIGNAL( changed() ), this, SLOT( updateMaterials() ) );

  Q_INIT_RESOURCE( GEOMGUI );
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
#ifndef WIN32
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
// function : GeometryGUI::getPluginLibrary()
// purpose  : get or load GUI Plugin library by name [ internal ]
//=======================================================================
typedef GEOMPluginGUI* (*PluginLibraryGUI)( GeometryGUI* );
GEOMPluginGUI* GeometryGUI::getPluginLibrary( const QString& libraryName )
{
  if ( !myGUIMap.contains( libraryName ) ) {
    // try to load library if it is not loaded yet
#ifndef WIN32
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
            PluginLibraryGUI func = (GEOMPluginGUI* (*) (GeometryGUI*))osdF;
            GEOMPluginGUI* libGUI = (*func)( this );
            if ( libGUI ) {
              myGUIMap[ libraryName ] = libGUI;
              break; // found and loaded!
            }
          }
        }
      }
    }
  }
  return myGUIMap.contains( libraryName ) ? (GEOMPluginGUI*)myGUIMap[ libraryName ] : 0;
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

  if ( ViewOCC ) {
    OCCViewer_ViewWindow* vw = dynamic_cast<OCCViewer_ViewWindow*>( window );
    if ( vw ) {
      Handle(V3d_View) view3d =  vw->getViewPort()->getView();

      view3d->SetProj(DZ.X(), DZ.Y(), DZ.Z());
      view3d->SetUp(DY.X(), DY.Y(), DY.Z());
      vw->onViewFitAll();
    }
  }
  else if ( ViewVTK ) {
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
void GeometryGUI::OnGUIEvent( int id, const QVariant& theParam )
{
  SUIT_Application* anApp = application();
  if (!anApp) return;
  SUIT_Desktop* desk = anApp->desktop();

  // check type of the active viewframe
  SUIT_ViewWindow* window = desk->activeWindow();
  bool ViewOCC = ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );
  bool ViewVTK = ( window && window->getViewManager()->getType() == SVTK_Viewer::Type() );
#ifndef DISABLE_GRAPHICSVIEW
  bool ViewDep = ( window && window->getViewManager()->getType() == GraphicsView_Viewer::Type() );
#else
  bool ViewDep = 0;
#endif
  // if current viewframe is not of OCC and not of VTK type - return immediately
  // fix for IPAL8958 - allow some commands to execute even when NO viewer is active (rename for example)
  QList<int> NotViewerDependentCommands;
  NotViewerDependentCommands << GEOMOp::OpDelete
                             << GEOMOp::OpShow
                             << GEOMOp::OpShowOnly
                             << GEOMOp::OpShowOnlyChildren
                             << GEOMOp::OpDiscloseChildren
                             << GEOMOp::OpConcealChildren
                             << GEOMOp::OpUnpublishObject
                             << GEOMOp::OpPublishObject
                             << GEOMOp::OpPointMarker
                             << GEOMOp::OpCreateFolder
                             << GEOMOp::OpSortChildren;
  if ( !ViewOCC && !ViewVTK && !ViewDep && !NotViewerDependentCommands.contains( id ) ) {
    // activate OCC viewer
    getApp()->getViewManager(OCCViewer_Viewer::Type(), /*create=*/true);
  }

  // fix for IPAL9103, point 2
  if ( CORBA::is_nil( GetGeomGen() ) ) {
    SUIT_MessageBox::critical( desk, tr( "GEOM_ERROR" ), tr( "GEOM_ERR_GET_ENGINE" ), tr( "GEOM_BUT_OK" ) );
    return;
  }

  QString libName;
  // find corresponding GUI library
  switch ( id ) {
  case GEOMOp::OpOriginAndVectors:   // MENU BASIC - ORIGIN AND BASE VECTORS
    createOriginAndBaseVectors(); // internal operation
    return;
  case GEOMOp::OpSelectVertex:       // POPUP MENU - SELECT ONLY - VERTEX
  case GEOMOp::OpSelectEdge:         // POPUP MENU - SELECT ONLY - EDGE
  case GEOMOp::OpSelectWire:         // POPUP MENU - SELECT ONLY - WIRE
  case GEOMOp::OpSelectFace:         // POPUP MENU - SELECT ONLY - FACE
  case GEOMOp::OpSelectShell:        // POPUP MENU - SELECT ONLY - SHELL
  case GEOMOp::OpSelectSolid:        // POPUP MENU - SELECT ONLY - SOLID
  case GEOMOp::OpSelectCompound:     // POPUP MENU - SELECT ONLY - COMPOUND
  case GEOMOp::OpSelectAll:          // POPUP MENU - SELECT ONLY - SELECT ALL
  case GEOMOp::OpDelete:             // MENU EDIT - DELETE
#ifndef DISABLE_PYCONSOLE
  case GEOMOp::OpCheckGeom:          // MENU TOOLS - CHECK GEOMETRY
#endif
  case GEOMOp::OpMaterialsLibrary:   // MENU TOOLS - MATERIALS LIBRARY
  case GEOMOp::OpDeflection:         // POPUP MENU - DEFLECTION COEFFICIENT
  case GEOMOp::OpColor:              // POPUP MENU - COLOR
  case GEOMOp::OpSetTexture:         // POPUP MENU - SETTEXTURE
  case GEOMOp::OpTransparency:       // POPUP MENU - TRANSPARENCY
  case GEOMOp::OpIncrTransparency:   // SHORTCUT   - INCREASE TRANSPARENCY
  case GEOMOp::OpDecrTransparency:   // SHORTCUT   - DECREASE TRANSPARENCY
  case GEOMOp::OpIsos:               // POPUP MENU - ISOS
  case GEOMOp::OpIncrNbIsos:         // SHORTCUT   - INCREASE NB ISOS
  case GEOMOp::OpDecrNbIsos:         // SHORTCUT   - DECREASE NB ISOS
  case GEOMOp::OpAutoColor:          // POPUP MENU - AUTO COLOR
  case GEOMOp::OpNoAutoColor:        // POPUP MENU - DISABLE AUTO COLOR
  case GEOMOp::OpDiscloseChildren:   // POPUP MENU - DISCLOSE CHILD ITEMS
  case GEOMOp::OpConcealChildren:    // POPUP MENU - CONCEAL CHILD ITEMS
  case GEOMOp::OpUnpublishObject:    // POPUP MENU - UNPUBLISH
  case GEOMOp::OpPublishObject:      // ROOT GEOM OBJECT - POPUP MENU - PUBLISH
  case GEOMOp::OpPointMarker:        // POPUP MENU - POINT MARKER
  case GEOMOp::OpMaterialProperties: // POPUP MENU - MATERIAL PROPERTIES
  case GEOMOp::OpPredefMaterial:     // POPUP MENU - <SOME MATERIAL>
  case GEOMOp::OpPredefMaterCustom:  // POPUP MENU - MATERIAL PROPERTIES - CUSTOM...
  case GEOMOp::OpEdgeWidth:          // POPUP MENU - LINE WIDTH - EDGE WIDTH
  case GEOMOp::OpIsosWidth:          // POPUP MENU - LINE WIDTH - ISOS WIDTH
  case GEOMOp::OpBringToFront:       // POPUP MENU - BRING TO FRONT
  case GEOMOp::OpClsBringToFront:    //
  case GEOMOp::OpCreateFolder:       // POPUP MENU - CREATE FOLDER
  case GEOMOp::OpSortChildren:       // POPUP MENU - SORT CHILD ITEMS
#ifndef DISABLE_GRAPHICSVIEW
  case GEOMOp::OpShowDependencyTree: // POPUP MENU - SHOW DEPENDENCY TREE
#endif
  case GEOMOp::OpReduceStudy:        // POPUP MENU - REDUCE STUDY
    libName = "GEOMToolsGUI";
    break;
  case GEOMOp::OpDMWireframe:        // MENU VIEW - WIREFRAME
  case GEOMOp::OpDMShading:          // MENU VIEW - SHADING
  case GEOMOp::OpDMShadingWithEdges: // MENU VIEW - SHADING
  case GEOMOp::OpDMTexture:          // MENU VIEW - TEXTURE
  case GEOMOp::OpShowAll:            // MENU VIEW - SHOW ALL
  case GEOMOp::OpShowOnly:           // MENU VIEW - DISPLAY ONLY
  case GEOMOp::OpShowOnlyChildren:   // MENU VIEW - SHOW ONLY CHILDREN
  case GEOMOp::OpHideAll:            // MENU VIEW - ERASE ALL
  case GEOMOp::OpHide:               // MENU VIEW - ERASE
  case GEOMOp::OpShow:               // MENU VIEW - DISPLAY
  case GEOMOp::OpSwitchVectors:      // MENU VIEW - VECTOR MODE
  case GEOMOp::OpSwitchVertices:     // MENU VIEW - VERTICES MODE
  case GEOMOp::OpSwitchName:         // MENU VIEW - VERTICES MODE
  case GEOMOp::OpWireframe:          // POPUP MENU - WIREFRAME
  case GEOMOp::OpShading:            // POPUP MENU - SHADING
  case GEOMOp::OpShadingWithEdges:   // POPUP MENU - SHADING WITH EDGES
  case GEOMOp::OpTexture:            // POPUP MENU - TEXTURE
  case GEOMOp::OpVectors:            // POPUP MENU - VECTORS
  case GEOMOp::OpVertices:           // POPUP MENU - VERTICES
  case GEOMOp::OpShowName:           // POPUP MENU - SHOW NAME
    libName = "DisplayGUI";
    break;
  case GEOMOp::OpPoint:              // MENU BASIC - POINT
  case GEOMOp::OpLine:               // MENU BASIC - LINE
  case GEOMOp::OpCircle:             // MENU BASIC - CIRCLE
  case GEOMOp::OpEllipse:            // MENU BASIC - ELLIPSE
  case GEOMOp::OpArc:                // MENU BASIC - ARC
  case GEOMOp::OpVector:             // MENU BASIC - VECTOR
  case GEOMOp::OpPlane:              // MENU BASIC - PLANE
  case GEOMOp::OpCurve:              // MENU BASIC - CURVE
  case GEOMOp::OpLCS:                // MENU BASIC - LOCAL COORDINATE SYSTEM
    libName = "BasicGUI";
    break;
  case GEOMOp::OpBox:                // MENU PRIMITIVE - BOX
  case GEOMOp::OpCylinder:           // MENU PRIMITIVE - CYLINDER
  case GEOMOp::OpSphere:             // MENU PRIMITIVE - SPHERE
  case GEOMOp::OpTorus:              // MENU PRIMITIVE - TORUS
  case GEOMOp::OpCone:               // MENU PRIMITIVE - CONE
  case GEOMOp::OpRectangle:          // MENU PRIMITIVE - FACE
  case GEOMOp::OpDisk:               // MENU PRIMITIVE - DISK
    libName = "PrimitiveGUI";
    break;
  case GEOMOp::OpPrism:              // MENU GENERATION - PRISM
  case GEOMOp::OpRevolution:         // MENU GENERATION - REVOLUTION
  case GEOMOp::OpFilling:            // MENU GENERATION - FILLING
  case GEOMOp::OpPipe:               // MENU GENERATION - PIPE
  case GEOMOp::OpPipePath:           // MENU GENERATION - RESTORE PATH
  case GEOMOp::OpThickness:          // MENU GENERATION - THICKNESS
    libName = "GenerationGUI";
    break;
  case GEOMOp::Op2dSketcher:         // MENU ENTITY - SKETCHER
  case GEOMOp::Op3dSketcher:         // MENU ENTITY - 3D SKETCHER
  case GEOMOp::OpIsoline:            // MENU BASIC  - ISOLINE
  case GEOMOp::OpExplode:            // MENU ENTITY - EXPLODE
  case GEOMOp::OpSurfaceFromFace:    // MENU ENTITY - SURFACE FROM FACE
#ifdef WITH_OPENCV
  case GEOMOp::OpFeatureDetect:      // MENU ENTITY - FEATURE DETECTION
#endif
  case GEOMOp::OpPictureImport:      // MENU ENTITY - IMPORT PICTURE IN VIEWER
  case GEOMOp::OpCreateField:        // MENU FIELD - CREATE FIELD
  case GEOMOp::OpEditField:          // MENU FIELD - EDIT FIELD
  case GEOMOp::OpEditFieldPopup:     // POPUP MENU - EDIT FIELD
  case GEOMOp::Op2dPolylineEditor:   // MENU BASIC - POLYLINE EDITOR
    libName = "EntityGUI";
    break;
  case GEOMOp::OpEdge:               // MENU BUILD - EDGE
  case GEOMOp::OpWire:               // MENU BUILD - WIRE
  case GEOMOp::OpFace:               // MENU BUILD - FACE
  case GEOMOp::OpShell:              // MENU BUILD - SHELL
  case GEOMOp::OpSolid:              // MENU BUILD - SOLID
  case GEOMOp::OpCompound:           // MENU BUILD - COMPUND
    libName = "BuildGUI";
    break;
  case GEOMOp::OpFuse:               // MENU BOOLEAN - FUSE
  case GEOMOp::OpCommon:             // MENU BOOLEAN - COMMON
  case GEOMOp::OpCut:                // MENU BOOLEAN - CUT
  case GEOMOp::OpSection:            // MENU BOOLEAN - SECTION
    libName = "BooleanGUI";
    break;
  case GEOMOp::OpTranslate:          // MENU TRANSFORMATION - TRANSLATION
  case GEOMOp::OpRotate:             // MENU TRANSFORMATION - ROTATION
  case GEOMOp::OpChangeLoc:          // MENU TRANSFORMATION - LOCATION
  case GEOMOp::OpMirror:             // MENU TRANSFORMATION - MIRROR
  case GEOMOp::OpScale:              // MENU TRANSFORMATION - SCALE
  case GEOMOp::OpOffset:             // MENU TRANSFORMATION - OFFSET
  case GEOMOp::OpProjection:         // MENU TRANSFORMATION - PROJECTION
  case GEOMOp::OpProjOnCyl:          // MENU TRANSFORMATION - PROJECTION ON CYLINDER
  case GEOMOp::OpMultiTranslate:     // MENU TRANSFORMATION - MULTI-TRANSLATION
  case GEOMOp::OpMultiRotate:        // MENU TRANSFORMATION - MULTI-ROTATION
  case GEOMOp::OpReimport:           // CONTEXT(POPUP) MENU - RELOAD_IMPORTED
  case GEOMOp::OpExtension:          // MENU TRANSFORMATION - EXTENSION
    libName = "TransformationGUI";
    break;
  case GEOMOp::OpPartition:          // MENU OPERATION - PARTITION
  case GEOMOp::OpArchimede:          // MENU OPERATION - ARCHIMEDE
  case GEOMOp::OpFillet3d:           // MENU OPERATION - FILLET
  case GEOMOp::OpChamfer:            // MENU OPERATION - CHAMFER
  case GEOMOp::OpShapesOnShape:      // MENU OPERATION - GET SHAPES ON SHAPE
  case GEOMOp::OpFillet2d:           // MENU OPERATION - FILLET 2D
  case GEOMOp::OpFillet1d:           // MENU OPERATION - FILLET 1D
  case GEOMOp::OpSharedShapes:       // MENU OPERATION - GET SHARED SHAPES
  case GEOMOp::OpExtrudedBoss:       // MENU OPERATION - EXTRUDED BOSS
  case GEOMOp::OpExtrudedCut:        // MENU OPERATION - EXTRUDED CUT
  case GEOMOp::OpTransferData:       // MENU OPERATION - TRANSFER DATA
  case GEOMOp::OpExtraction:         // MENU OPERATION - EXTRACT AND REBUILD
    libName = "OperationGUI";
    break;
  case GEOMOp::OpSewing:             // MENU REPAIR - SEWING
  case GEOMOp::OpSuppressFaces:      // MENU REPAIR - SUPPRESS FACES
  case GEOMOp::OpSuppressHoles:      // MENU REPAIR - SUPPRESS HOLE
  case GEOMOp::OpShapeProcess:       // MENU REPAIR - SHAPE PROCESSING
  case GEOMOp::OpCloseContour:       // MENU REPAIR - CLOSE CONTOUR
  case GEOMOp::OpRemoveIntWires:     // MENU REPAIR - REMOVE INTERNAL WIRES
  case GEOMOp::OpAddPointOnEdge:     // MENU REPAIR - ADD POINT ON EDGE
  case GEOMOp::OpFreeBoundaries:     // MENU MEASURE - FREE BOUNDARIES
  case GEOMOp::OpFreeFaces:          // MENU MEASURE - FREE FACES
  case GEOMOp::OpOrientation:        // MENU REPAIR - CHANGE ORIENTATION
  case GEOMOp::OpGlueFaces:          // MENU REPAIR - GLUE FACES
  case GEOMOp::OpGlueEdges:          // MENU REPAIR - GLUE EDGES
  case GEOMOp::OpLimitTolerance:     // MENU REPAIR - LIMIT TOLERANCE
  case GEOMOp::OpRemoveWebs:         // MENU REPAIR - REMOVE INTERNAL FACES
  case GEOMOp::OpRemoveExtraEdges:   // MENU REPAIR - REMOVE EXTRA EDGES
  case GEOMOp::OpFuseEdges:          // MENU REPAIR - FUSE COLLINEAR EDGES
  case GEOMOp::OpUnionFaces:         // MENU REPAIR - UNION FACES
  case GEOMOp::OpInspectObj:         // MENU REPAIR - INSPECT OBJECT
    libName = "RepairGUI";
    break;
  case GEOMOp::OpProperties:         // MENU MEASURE - PROPERTIES
  case GEOMOp::OpCenterMass:         // MENU MEASURE - CDG
  case GEOMOp::OpInertia:            // MENU MEASURE - INERTIA
  case GEOMOp::OpNormale:            // MENU MEASURE - NORMALE
  case GEOMOp::OpBoundingBox:        // MENU MEASURE - BOUNDING BOX
  case GEOMOp::OpMinDistance:        // MENU MEASURE - MIN DISTANCE
  case GEOMOp::OpAngle:              // MENU MEASURE - ANGLE
  case GEOMOp::OpTolerance:          // MENU MEASURE - TOLERANCE
  case GEOMOp::OpWhatIs:             // MENU MEASURE - WHATIS
  case GEOMOp::OpCheckShape:         // MENU MEASURE - CHECK
  case GEOMOp::OpCheckCompound:      // MENU MEASURE - CHECK COMPOUND OF BLOCKS
  case GEOMOp::OpGetNonBlocks:       // MENU MEASURE - Get NON BLOCKS
  case GEOMOp::OpPointCoordinates:   // MENU MEASURE - POINT COORDINATES
  case GEOMOp::OpCheckSelfInters:    // MENU MEASURE - CHECK SELF INTERSECTIONS
  case GEOMOp::OpFastCheckInters:    // MENU MEASURE - FAST CHECK INTERSECTIONS
  case GEOMOp::OpManageDimensions:   // MENU MEASURE - MANAGE DIMENSIONS
#ifndef DISABLE_PLOT2DVIEWER
  case GEOMOp::OpShapeStatistics:    // MENU MEASURE - SHAPE STATISTICS
#endif
  case GEOMOp::OpShowAllDimensions:  // POPUP MENU - SHOW ALL DIMENSIONS
  case GEOMOp::OpHideAllDimensions:  // POPUP MENU - HIDE ALL DIMENSIONS
    libName = "MeasureGUI";
    break;
  case GEOMOp::OpGroupCreate:        // MENU GROUP - CREATE
  case GEOMOp::OpGroupCreatePopup:   // POPUP MENU - CREATE GROUP
  case GEOMOp::OpGroupEdit:          // MENU GROUP - EDIT
  case GEOMOp::OpGroupUnion:         // MENU GROUP - UNION
  case GEOMOp::OpGroupIntersect:     // MENU GROUP - INTERSECT
  case GEOMOp::OpGroupCut:           // MENU GROUP - CUT
    libName = "GroupGUI";
    break;
  case GEOMOp::OpHexaSolid:          // MENU BLOCKS - HEXAHEDRAL SOLID
  case GEOMOp::OpMultiTransform:     // MENU BLOCKS - MULTI-TRANSFORMATION
  case GEOMOp::OpQuadFace:           // MENU BLOCKS - QUADRANGLE FACE
  case GEOMOp::OpPropagate:          // MENU BLOCKS - PROPAGATE
  case GEOMOp::OpExplodeBlock:       // MENU BLOCKS - EXPLODE ON BLOCKS
    libName = "BlocksGUI";
    break;
  //case GEOMOp::OpAdvancedNoOp:       // NO OPERATION (advanced operations base)
  //case GEOMOp::OpPipeTShape:         // MENU NEW ENTITY - ADVANCED - PIPE TSHAPE
  //case GEOMOp::OpPipeTShapeGroups:     // MENU NEW ENTITY - ADVANCED - PIPE TSHAPE GROUPS
  //case GEOMOp::OpDividedDisk:           // MENU NEW ENTITY - ADVANCED - DIVIDEDDISK
  //case GEOMOp::OpDividedCylinder:           // MENU NEW ENTITY - ADVANCED - DIVIDEDCYLINDER
  //case GEOMOp::OpSmoothingSurface:           // MENU NEW ENTITY - ADVANCED - SMOOTHINGSURFACE
    //@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@//
    //libName = "AdvancedGUI";
    //break;
  default:
    if (myPluginActions.contains(id)) {
      libName = myPluginActions[id].first;

      GEOMPluginGUI* library = 0;
      if ( !libName.isEmpty() ) {
#ifndef WIN32
        libName = QString( "lib" ) + libName + ".so";
#else
        libName = libName + ".dll";
#endif
        library = getPluginLibrary( libName );
      }

      // call method of corresponding GUI library
      if ( library ) {
        //QString action ("%1");
        //action = action.arg(id);

        //if( !theParam.isValid() )
          library->OnGUIEvent( myPluginActions[id].second, desk );
        //else
        //  library->OnGUIEvent( id, desk, theParam);
      }
      else
        SUIT_MessageBox::critical( desk, tr( "GEOM_ERROR" ), tr( "GEOM_ERR_LIB_NOT_FOUND" ), tr( "GEOM_BUT_OK" ) );

      updateCreationInfo();
      return;
    }
    break;
  }

  GEOMGUI* library = 0;
  if ( !libName.isEmpty() ) {
#ifndef WIN32
    libName = QString( "lib" ) + libName + ".so";
#else
    libName = libName + ".dll";
#endif
    library = getLibrary( libName );
  }

  // call method of corresponding GUI library
  if ( library ) {
    if( !theParam.isValid() )
      library->OnGUIEvent( id, desk );
    else
      library->OnGUIEvent( id, desk, theParam);
  }
  else
    SUIT_MessageBox::critical( desk, tr( "GEOM_ERROR" ), tr( "GEOM_ERR_LIB_NOT_FOUND" ), tr( "GEOM_BUT_OK" ) );

  updateCreationInfo();
}

//=================================================================================
// function : GeometryGUI::activateOperation()
// purpose  :
//=================================================================================
bool GeometryGUI::activateOperation( int actionId )
{
  OnGUIEvent(actionId);
  return true;
}

//=================================================================================
// function : GeometryGUI::activateOperation()
// purpose  :
//=================================================================================
bool GeometryGUI::activateOperation( const QString& actionId )
{
  bool isOk = false;

  int id = actionId.toInt(&isOk);
  if (isOk)
    OnGUIEvent(id);

  return isOk;
}

//=================================================================================
// function : GeometryGUI::activateOperation()
// purpose  :
//=================================================================================
bool GeometryGUI::activateOperation( const QString& actionId, const QString& plugin )
{
  bool isOk = false;

  QString pluginLib = plugin;
  // TODO: if <plugin> is a plugin name, find plugin library name
  if (myPluginLibs.contains(plugin))
    pluginLib = myPluginLibs[plugin];

  QMap<int, PluginAction>::iterator actionsIter = myPluginActions.begin();
  for (; actionsIter != myPluginActions.end(); ++actionsIter) {
    const PluginAction& anAction = actionsIter.value();
    if (anAction.first == pluginLib && anAction.second == actionId) {
      // activate operation
      OnGUIEvent(actionsIter.key());
      isOk = true;
    }
  }

  return isOk;
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
// function : GeometryGUI::OnMouseRelease()
// purpose  : Manages mouse release events [static]
//=================================================================================
void GeometryGUI::OnMouseRelease( SUIT_ViewWindow* w, QMouseEvent* e )
{
  if ( !application() )
    return;
  foreach ( GEOMGUI* lib, myGUIMap )
    lib->OnMouseRelease( e, application()->desktop(), w );
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
  bool aLocked = (_PTR(AttributeStudyProperties)(appStudy->studyDS()->GetProperties()))->IsLocked();
  if ( aLocked ) {
    SUIT_MessageBox::warning ( application()->desktop(), QObject::tr("WRN_WARNING"), QObject::tr("WRN_STUDY_LOCKED") );
    return;
  }
  if ( appStudy ) {
    _PTR(Study) studyDS = appStudy->studyDS();
    if ( studyDS && !CORBA::is_nil( GetGeomGen() ) ) {
      GEOM::GEOM_IBasicOperations_var aBasicOperations = GetGeomGen()->GetIBasicOperations( studyDS->StudyId() );
      if ( !aBasicOperations->_is_nil() ) {
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
        anOrigin->UnRegister();
        anOX->UnRegister();
        anOY->UnRegister();
        anOZ->UnRegister();
        aBasicOperations->UnRegister();

        getApp()->updateObjectBrowser( true );
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

  createGeomAction( GEOMOp::OpDelete,     "DELETE", "", Qt::Key_Delete );

  createGeomAction( GEOMOp::OpPoint,      "POINT" );
  createGeomAction( GEOMOp::OpLine,       "LINE" );
  createGeomAction( GEOMOp::OpCircle,     "CIRCLE" );
  createGeomAction( GEOMOp::OpEllipse,    "ELLIPSE" );
  createGeomAction( GEOMOp::OpArc,        "ARC" );
  createGeomAction( GEOMOp::OpCurve,      "CURVE" );
  createGeomAction( GEOMOp::OpIsoline,    "ISOLINE" );
  createGeomAction( GEOMOp::OpVector,     "VECTOR" );
  createGeomAction( GEOMOp::OpPlane,      "PLANE" );
  createGeomAction( GEOMOp::OpLCS,        "LOCAL_CS" );
  createGeomAction( GEOMOp::OpOriginAndVectors, "ORIGIN_AND_VECTORS" );
  createGeomAction( GEOMOp::OpSurfaceFromFace,  "SURFACE_FROM_FACE" );

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
  createGeomAction( GEOMOp::OpPipePath,    "PIPE_PATH" );
  createGeomAction( GEOMOp::OpThickness,   "THICKNESS" );

  createGeomAction( GEOMOp::OpGroupCreate, "GROUP_CREATE" );
  createGeomAction( GEOMOp::OpGroupEdit,   "GROUP_EDIT" );
  createGeomAction( GEOMOp::OpGroupUnion,  "GROUP_UNION" );
  createGeomAction( GEOMOp::OpGroupIntersect, "GROUP_INTERSECT" );
  createGeomAction( GEOMOp::OpGroupCut,    "GROUP_CUT" );

  createGeomAction( GEOMOp::OpCreateField, "FIELD_CREATE" );
  createGeomAction( GEOMOp::OpEditField,   "FIELD_EDIT" );

  createGeomAction( GEOMOp::OpReimport,    "RELOAD_IMPORTED" );

  createGeomAction( GEOMOp::OpQuadFace,    "Q_FACE" );
  createGeomAction( GEOMOp::OpHexaSolid,   "HEX_SOLID" );

  createGeomAction( GEOMOp::Op2dSketcher,  "SKETCH" );
  createGeomAction( GEOMOp::Op3dSketcher,  "3DSKETCH" );
  createGeomAction( GEOMOp::OpExplode,     "EXPLODE" );
#ifdef WITH_OPENCV
  createGeomAction( GEOMOp::OpFeatureDetect,"FEATURE_DETECTION" );
#endif
  createGeomAction( GEOMOp::OpPictureImport,"PICTURE_IMPORT" );
  createGeomAction( GEOMOp::Op2dPolylineEditor, "CURVE_CREATOR" );

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
  createGeomAction( GEOMOp::OpProjection,     "PROJECTION" );
  createGeomAction( GEOMOp::OpProjOnCyl,      "PROJ_ON_CYL" );
  createGeomAction( GEOMOp::OpMultiTranslate, "MUL_TRANSLATION" );
  createGeomAction( GEOMOp::OpMultiRotate,    "MUL_ROTATION" );
  createGeomAction( GEOMOp::OpExtension,      "EXTENSION" );

  createGeomAction( GEOMOp::OpPartition,      "PARTITION" );
  createGeomAction( GEOMOp::OpArchimede,      "ARCHIMEDE" );
  createGeomAction( GEOMOp::OpFillet3d,       "FILLET" );
  createGeomAction( GEOMOp::OpChamfer,        "CHAMFER" );
  createGeomAction( GEOMOp::OpShapesOnShape,  "GET_SHAPES_ON_SHAPE" );
  createGeomAction( GEOMOp::OpSharedShapes,   "GET_SHARED_SHAPES" );
  createGeomAction( GEOMOp::OpTransferData,   "TRANSFER_DATA" );
  createGeomAction( GEOMOp::OpExtraction,     "EXTRACTION" );
  createGeomAction( GEOMOp::OpExtrudedCut,    "EXTRUDED_CUT" );
  createGeomAction( GEOMOp::OpExtrudedBoss,   "EXTRUDED_BOSS" );
  createGeomAction( GEOMOp::OpFillet1d,       "FILLET_1D" );
  createGeomAction( GEOMOp::OpFillet2d,       "FILLET_2D" );

  createGeomAction( GEOMOp::OpMultiTransform, "MUL_TRANSFORM" );
  createGeomAction( GEOMOp::OpExplodeBlock,   "EXPLODE_BLOCKS" );
  createGeomAction( GEOMOp::OpPropagate,      "PROPAGATE" );

  createGeomAction( GEOMOp::OpSewing,           "SEWING" );
  createGeomAction( GEOMOp::OpGlueFaces,        "GLUE_FACES" );
  createGeomAction( GEOMOp::OpGlueEdges,        "GLUE_EDGES" );
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
  createGeomAction( GEOMOp::OpRemoveWebs,       "REMOVE_WEBS" );
  createGeomAction( GEOMOp::OpRemoveExtraEdges, "REMOVE_EXTRA_EDGES" );
  createGeomAction( GEOMOp::OpFuseEdges,        "FUSE_EDGES" );
  createGeomAction( GEOMOp::OpUnionFaces,       "UNION_FACES" );
  createGeomAction( GEOMOp::OpInspectObj,       "INSPECT_OBJECT" );

  createGeomAction( GEOMOp::OpPointCoordinates, "POINT_COORDS" );
  createGeomAction( GEOMOp::OpProperties,       "BASIC_PROPS" );
  createGeomAction( GEOMOp::OpCenterMass,       "MASS_CENTER" );
  createGeomAction( GEOMOp::OpInertia,          "INERTIA" );
  createGeomAction( GEOMOp::OpNormale,          "NORMALE" );
  createGeomAction( GEOMOp::OpBoundingBox,      "BND_BOX" );
  createGeomAction( GEOMOp::OpMinDistance,      "MIN_DIST" );
  createGeomAction( GEOMOp::OpAngle,            "MEASURE_ANGLE" );
  createGeomAction( GEOMOp::OpManageDimensions, "MANAGE_DIMENSIONS" );

  createGeomAction( GEOMOp::OpTolerance,        "TOLERANCE" );
  createGeomAction( GEOMOp::OpWhatIs,           "WHAT_IS" );
  createGeomAction( GEOMOp::OpCheckShape,       "CHECK" );
  createGeomAction( GEOMOp::OpCheckCompound,    "CHECK_COMPOUND" );
  createGeomAction( GEOMOp::OpGetNonBlocks,     "GET_NON_BLOCKS" );
  createGeomAction( GEOMOp::OpCheckSelfInters,  "CHECK_SELF_INTERSECTIONS" );
  createGeomAction( GEOMOp::OpFastCheckInters,  "FAST_CHECK_INTERSECTIONS" );
#ifndef DISABLE_PLOT2DVIEWER
  createGeomAction( GEOMOp::OpShapeStatistics,  "SHAPE_STATISTICS" );
#endif

#ifndef DISABLE_PYCONSOLE
#ifdef _DEBUG_ // PAL16821
  createGeomAction( GEOMOp::OpCheckGeom,        "CHECK_GEOMETRY" );
#endif
#endif

  createGeomAction( GEOMOp::OpMaterialsLibrary,   "MATERIALS_LIBRARY" );
  createGeomAction( GEOMOp::OpDMWireframe,        "WIREFRAME" );
  createGeomAction( GEOMOp::OpDMShading,          "SHADING" );
  createGeomAction( GEOMOp::OpDMShadingWithEdges, "SHADING_WITH_EDGES" );
  createGeomAction( GEOMOp::OpDMTexture,          "TEXTURE" );
  createGeomAction( GEOMOp::OpShowAll,          "DISPLAY_ALL" );
  createGeomAction( GEOMOp::OpHideAll,          "ERASE_ALL" );
  createGeomAction( GEOMOp::OpShow,             "DISPLAY" );
  createGeomAction( GEOMOp::OpSwitchVectors,    "VECTOR_MODE");
  createGeomAction( GEOMOp::OpSwitchVertices,   "VERTICES_MODE");
  createGeomAction( GEOMOp::OpSwitchName,       "NAME_MODE");
  createGeomAction( GEOMOp::OpSelectVertex,     "VERTEX_SEL_ONLY" ,"", 0, true );
  createGeomAction( GEOMOp::OpSelectEdge,       "EDGE_SEL_ONLY", "", 0, true );
  createGeomAction( GEOMOp::OpSelectWire,       "WIRE_SEL_ONLY", "",  0, true );
  createGeomAction( GEOMOp::OpSelectFace,       "FACE_SEL_ONLY", "", 0, true );
  createGeomAction( GEOMOp::OpSelectShell,      "SHELL_SEL_ONLY", "",  0, true );
  createGeomAction( GEOMOp::OpSelectSolid,      "SOLID_SEL_ONLY", "", 0, true );
  createGeomAction( GEOMOp::OpSelectCompound,   "COMPOUND_SEL_ONLY", "",  0, true );
  createGeomAction( GEOMOp::OpSelectAll,        "ALL_SEL_ONLY", "",  0, true );
  createGeomAction( GEOMOp::OpShowOnly,         "DISPLAY_ONLY" );
  createGeomAction( GEOMOp::OpShowOnlyChildren, "SHOW_ONLY_CHILDREN" );
  createGeomAction( GEOMOp::OpBringToFront,     "BRING_TO_FRONT", "", 0, true );
  createGeomAction( GEOMOp::OpClsBringToFront,  "CLS_BRING_TO_FRONT" );
  createGeomAction( GEOMOp::OpHide,             "ERASE" );

  createGeomAction( GEOMOp::OpWireframe,        "POP_WIREFRAME", "", 0, true );
  createGeomAction( GEOMOp::OpShading,          "POP_SHADING", "", 0, true );
  createGeomAction( GEOMOp::OpShadingWithEdges, "POP_SHADING_WITH_EDGES", "", 0, true );
  createGeomAction( GEOMOp::OpTexture,          "POP_TEXTURE", "", 0, true );
  createGeomAction( GEOMOp::OpEdgeWidth,        "EDGE_WIDTH");
  createGeomAction( GEOMOp::OpIsosWidth,        "ISOS_WIDTH");
  createGeomAction( GEOMOp::OpVectors,          "POP_VECTORS", "", 0, true );
  createGeomAction( GEOMOp::OpVertices,         "POP_VERTICES", "", 0, true );
  createGeomAction( GEOMOp::OpShowName,         "POP_SHOW_NAME", "", 0, true );
  createGeomAction( GEOMOp::OpDeflection,       "POP_DEFLECTION" );
  createGeomAction( GEOMOp::OpColor,            "POP_COLOR" );
  createGeomAction( GEOMOp::OpSetTexture,       "POP_SETTEXTURE" );
  createGeomAction( GEOMOp::OpTransparency,     "POP_TRANSPARENCY" );
  createGeomAction( GEOMOp::OpIsos,             "POP_ISOS" );
  createGeomAction( GEOMOp::OpAutoColor,        "POP_AUTO_COLOR" );
  createGeomAction( GEOMOp::OpNoAutoColor,      "POP_DISABLE_AUTO_COLOR" );
  createGeomAction( GEOMOp::OpGroupCreatePopup, "POP_CREATE_GROUP" );
  createGeomAction( GEOMOp::OpEditFieldPopup,   "POP_EDIT_FIELD" );
  createGeomAction( GEOMOp::OpDiscloseChildren, "POP_DISCLOSE_CHILDREN" );
  createGeomAction( GEOMOp::OpConcealChildren,  "POP_CONCEAL_CHILDREN" );
  createGeomAction( GEOMOp::OpUnpublishObject,  "POP_UNPUBLISH_OBJ" );
  createGeomAction( GEOMOp::OpPublishObject,    "POP_PUBLISH_OBJ" );
  createGeomAction( GEOMOp::OpPointMarker,      "POP_POINT_MARKER" );
  createGeomAction( GEOMOp::OpMaterialProperties,   "POP_MATERIAL_PROPERTIES" );
  createGeomAction( GEOMOp::OpPredefMaterCustom,    "POP_PREDEF_MATER_CUSTOM" );
  createGeomAction( GEOMOp::OpCreateFolder, "POP_CREATE_FOLDER" );
  createGeomAction( GEOMOp::OpSortChildren, "POP_SORT_CHILD_ITEMS" );
#ifndef DISABLE_GRAPHICSVIEW
  createGeomAction( GEOMOp::OpShowDependencyTree, "POP_SHOW_DEPENDENCY_TREE" );
#endif
  createGeomAction( GEOMOp::OpReduceStudy,       "POP_REDUCE_STUDY" );
  createGeomAction( GEOMOp::OpShowAllDimensions, "POP_SHOW_ALL_DIMENSIONS" );
  createGeomAction( GEOMOp::OpHideAllDimensions, "POP_HIDE_ALL_DIMENSIONS" );

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

  //createGeomAction( GEOMOp::OpPipeTShape, "PIPETSHAPE" );
  //createGeomAction( GEOMOp::OpDividedDisk, "DIVIDEDDISK" );
  //createGeomAction( GEOMOp::OpDividedCylinder, "DIVIDEDCYLINDER" );
  //createGeomAction( GEOMOp::OpSmoothingSurface, "SMOOTHINGSURFACE" );
  //@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@//

  // ---- create menus --------------------------

  int fileId = createMenu( tr( "MEN_FILE" ), -1, -1 );

  int editId = createMenu( tr( "MEN_EDIT" ), -1, -1 );
  createMenu( GEOMOp::OpDelete, editId, -1 );

  int newEntId = createMenu( tr( "MEN_NEW_ENTITY" ), -1, -1, 10 );

  int basicId = createMenu( tr( "MEN_BASIC" ), newEntId, -1 );
  createMenu( GEOMOp::OpPoint,            basicId, -1 );
  createMenu( GEOMOp::OpLine,             basicId, -1 );
  createMenu( GEOMOp::OpCircle,           basicId, -1 );
  createMenu( GEOMOp::OpEllipse,          basicId, -1 );
  createMenu( GEOMOp::OpArc,              basicId, -1 );
  createMenu( GEOMOp::OpCurve,            basicId, -1 );
  createMenu( GEOMOp::Op2dSketcher,       basicId, -1 );
  createMenu( GEOMOp::Op2dPolylineEditor, basicId, -1 );
  createMenu( GEOMOp::Op3dSketcher,       basicId, -1 );
  createMenu( GEOMOp::OpIsoline,          basicId, -1 );
  createMenu( GEOMOp::OpSurfaceFromFace, basicId, -1 );
  createMenu( separator(),                basicId, -1 );
  createMenu( GEOMOp::OpVector,           basicId, -1 );
  createMenu( GEOMOp::OpPlane,            basicId, -1 );
  createMenu( GEOMOp::OpLCS,              basicId, -1 );
  createMenu( GEOMOp::OpOriginAndVectors, basicId, -1 );

  int primId = createMenu( tr( "MEN_PRIMITIVES" ), newEntId, -1 );
  createMenu( GEOMOp::OpBox,       primId, -1 );
  createMenu( GEOMOp::OpCylinder,  primId, -1 );
  createMenu( GEOMOp::OpSphere,    primId, -1 );
  createMenu( GEOMOp::OpTorus,     primId, -1 );
  createMenu( GEOMOp::OpCone,      primId, -1 );
  createMenu( GEOMOp::OpRectangle, primId, -1 );
  createMenu( GEOMOp::OpDisk,      primId, -1 );
  //createMenu( GEOMOp::OpPipeTShape,primId, -1 );

  int genId = createMenu( tr( "MEN_GENERATION" ), newEntId, -1 );
  createMenu( GEOMOp::OpPrism,      genId, -1 );
  createMenu( GEOMOp::OpRevolution, genId, -1 );
  createMenu( GEOMOp::OpFilling,    genId, -1 );
  createMenu( GEOMOp::OpPipe,       genId, -1 );
  createMenu( GEOMOp::OpPipePath,   genId, -1 );
  createMenu( GEOMOp::OpThickness,  genId, -1 );

  //int advId = createMenu( tr( "MEN_ADVANCED" ), newEntId, -1 );
  //createMenu( GEOMOp::OpSmoothingSurface, advId, -1 );
  //@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@//

  createMenu( separator(), newEntId, -1 );

  int groupId = createMenu( tr( "MEN_GROUP" ), newEntId, -1 );
  createMenu( GEOMOp::OpGroupCreate,    groupId, -1 );
  createMenu( GEOMOp::OpGroupEdit,      groupId, -1 );
  createMenu( GEOMOp::OpGroupUnion,     groupId, -1 );
  createMenu( GEOMOp::OpGroupIntersect, groupId, -1 );
  createMenu( GEOMOp::OpGroupCut,       groupId, -1 );

  createMenu( separator(), newEntId, -1 );

  int fieldId = createMenu( tr( "MEN_FIELD" ), newEntId, -1 );
  createMenu( GEOMOp::OpCreateField,    fieldId, -1 );
  createMenu( GEOMOp::OpEditField,      fieldId, -1 );

  createMenu( separator(), newEntId, -1 );

  int blocksId = createMenu( tr( "MEN_BLOCKS" ), newEntId, -1 );
  createMenu( GEOMOp::OpQuadFace,        blocksId, -1 );
  createMenu( GEOMOp::OpHexaSolid,       blocksId, -1 );
  //createMenu( GEOMOp::OpDividedDisk,     blocksId, -1 );
  //createMenu( GEOMOp::OpDividedCylinder, blocksId, -1 );

  createMenu( separator(),          newEntId, -1 );

  createMenu( GEOMOp::OpExplode,    newEntId, -1 );

  int buildId = createMenu( tr( "MEN_BUILD" ), newEntId, -1 );
  createMenu( GEOMOp::OpEdge,     buildId, -1 );
  createMenu( GEOMOp::OpWire,     buildId, -1 );
  createMenu( GEOMOp::OpFace,     buildId, -1 );
  createMenu( GEOMOp::OpShell,    buildId, -1 );
  createMenu( GEOMOp::OpSolid,    buildId, -1 );
  createMenu( GEOMOp::OpCompound, buildId, -1 );

  createMenu( separator(),          newEntId, -1 );

  createMenu( GEOMOp::OpPictureImport, newEntId, -1 );
#ifdef WITH_OPENCV
  createMenu( GEOMOp::OpFeatureDetect, newEntId, -1 );
#endif

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
  createMenu( GEOMOp::OpProjection,     transId, -1 );
  createMenu( GEOMOp::OpExtension,      transId, -1 );
  createMenu( GEOMOp::OpProjOnCyl,      transId, -1 );
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
  createMenu( GEOMOp::OpSharedShapes,  operId, -1 );
  createMenu( GEOMOp::OpTransferData,  operId, -1 );
  createMenu( GEOMOp::OpExtraction,    operId, -1 );

  createMenu( separator(), operId, -1 );

  createMenu( GEOMOp::OpFillet1d,      operId, -1 );
  createMenu( GEOMOp::OpFillet2d,      operId, -1 );
  createMenu( GEOMOp::OpFillet3d,      operId, -1 );
  createMenu( GEOMOp::OpChamfer,       operId, -1 );
  createMenu( GEOMOp::OpExtrudedBoss,  operId, -1 );
  createMenu( GEOMOp::OpExtrudedCut,   operId, -1 );

  int repairId = createMenu( tr( "MEN_REPAIR" ), -1, -1, 10 );
  createMenu( GEOMOp::OpShapeProcess,    repairId, -1 );
  createMenu( GEOMOp::OpSuppressFaces,   repairId, -1 );
  createMenu( GEOMOp::OpCloseContour,    repairId, -1 );
  createMenu( GEOMOp::OpRemoveIntWires,  repairId, -1 );
  createMenu( GEOMOp::OpSuppressHoles,   repairId, -1 );
  createMenu( GEOMOp::OpSewing,          repairId, -1 );
  createMenu( GEOMOp::OpGlueFaces,       repairId, -1 );
  createMenu( GEOMOp::OpGlueEdges,       repairId, -1 );
  createMenu( GEOMOp::OpLimitTolerance,  repairId, -1 );
  createMenu( GEOMOp::OpAddPointOnEdge,  repairId, -1 );
  //createMenu( GEOMOp::OpFreeBoundaries,  repairId, -1 );
  //createMenu( GEOMOp::OpFreeFaces,       repairId, -1 );
  createMenu( GEOMOp::OpOrientation,      repairId, -1 );
  createMenu( GEOMOp::OpRemoveWebs,       repairId, -1 );
  createMenu( GEOMOp::OpRemoveExtraEdges, repairId, -1 );
  createMenu( GEOMOp::OpFuseEdges,        repairId, -1 );
  createMenu( GEOMOp::OpUnionFaces,       repairId, -1 );

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
  createMenu( GEOMOp::OpBoundingBox,      dimId, -1 );
  createMenu( GEOMOp::OpMinDistance,      dimId, -1 );
  createMenu( GEOMOp::OpAngle,            dimId, -1 );
  createMenu( GEOMOp::OpManageDimensions, dimId, -1 );

  createMenu( separator(),               measurId, -1 );
  createMenu( GEOMOp::OpTolerance,       measurId, -1 );
  createMenu( separator(),               measurId, -1 );
  createMenu( GEOMOp::OpWhatIs,          measurId, -1 );
  createMenu( GEOMOp::OpCheckShape,      measurId, -1 );
  createMenu( GEOMOp::OpCheckCompound,   measurId, -1 );
  createMenu( GEOMOp::OpGetNonBlocks,    measurId, -1 );
  createMenu( GEOMOp::OpCheckSelfInters, measurId, -1 );
  createMenu( GEOMOp::OpFastCheckInters, measurId, -1 );
  createMenu( GEOMOp::OpInspectObj,      measurId, -1 );
#ifndef DISABLE_PLOT2DVIEWER
  createMenu( GEOMOp::OpShapeStatistics, measurId, -1 );
#endif

  int toolsId = createMenu( tr( "MEN_TOOLS" ), -1, -1, 50 );
#ifndef DISABLE_PYCONSOLE
#if defined(_DEBUG_) || defined(_DEBUG) // PAL16821
  createMenu( separator(),         toolsId, -1 );
  createMenu( GEOMOp::OpCheckGeom, toolsId, -1 );
#endif
#endif

  createMenu( separator(),         toolsId, -1 );
  createMenu( GEOMOp::OpMaterialsLibrary, toolsId, -1 );
  createMenu( separator(),         toolsId, -1 );

  int viewId = createMenu( tr( "MEN_VIEW" ), -1, -1 );
  createMenu( separator(),       viewId, -1 );

  int dispmodeId = createMenu( tr( "MEN_DISPLAY_MODE" ), viewId, -1 );
  createMenu( GEOMOp::OpDMWireframe,        dispmodeId, -1 );
  createMenu( GEOMOp::OpDMShading,          dispmodeId, -1 );
  createMenu( GEOMOp::OpDMShadingWithEdges, dispmodeId, -1 );
  createMenu( GEOMOp::OpDMTexture,          dispmodeId, -1 );
  createMenu( separator(),                  dispmodeId, -1 );
  createMenu( GEOMOp::OpSwitchVectors,      dispmodeId, -1 );
  createMenu( GEOMOp::OpSwitchVertices,     dispmodeId, -1 );
  createMenu( GEOMOp::OpSwitchName,         dispmodeId, -1 );

  createMenu( separator(),       viewId, -1 );
  createMenu( GEOMOp::OpShowAll, viewId, -1 );
  createMenu( GEOMOp::OpHideAll, viewId, -1 );
  createMenu( separator(),       viewId, -1 );
  createMenu( GEOMOp::OpPublishObject, viewId, -1 );
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

  int basicTbId = createTool( tr( "TOOL_BASIC" ), QString( "GEOMBasic" ) );
  createTool( GEOMOp::OpPoint,            basicTbId );
  createTool( GEOMOp::OpLine,             basicTbId );
  createTool( GEOMOp::OpCircle,           basicTbId );
  createTool( GEOMOp::OpEllipse,          basicTbId );
  createTool( GEOMOp::OpArc,              basicTbId );
  createTool( GEOMOp::OpCurve,            basicTbId );
  createTool( GEOMOp::OpVector,           basicTbId );
  createTool( GEOMOp::Op2dSketcher,       basicTbId ); //rnc
  createTool( GEOMOp::Op2dPolylineEditor, basicTbId ); 
  createTool( GEOMOp::Op3dSketcher,       basicTbId ); //rnc
  createTool( GEOMOp::OpIsoline,          basicTbId );
  createTool( GEOMOp::OpSurfaceFromFace,  basicTbId );
  createTool( GEOMOp::OpPlane,            basicTbId );
  createTool( GEOMOp::OpLCS,              basicTbId );
  createTool( GEOMOp::OpOriginAndVectors, basicTbId );

//   int sketchTbId = createTool( tr( "TOOL_SKETCH" ), QString( "GEOMSketch" ) );
//   createTool( GEOMOp::Op2dSketcher,  sketchTbId );
//   createTool( GEOMOp::Op3dSketcher,  sketchTbId );

  int primTbId = createTool( tr( "TOOL_PRIMITIVES" ), QString( "GEOMPrimitives" ) );
  createTool( GEOMOp::OpBox,        primTbId );
  createTool( GEOMOp::OpCylinder,   primTbId );
  createTool( GEOMOp::OpSphere,     primTbId );
  createTool( GEOMOp::OpTorus,      primTbId );
  createTool( GEOMOp::OpCone,       primTbId );
  createTool( GEOMOp::OpRectangle,  primTbId );
  createTool( GEOMOp::OpDisk,       primTbId );
  //createTool( GEOMOp::OpPipeTShape, primTbId ); //rnc

  //int blocksTbId = createTool( tr( "TOOL_BLOCKS" ), QString( "GEOMBlocks" ) );
  //createTool( GEOMOp::OpDividedDisk, blocksTbId );
  //createTool( GEOMOp::OpDividedCylinder, blocksTbId );

  int boolTbId = createTool( tr( "TOOL_BOOLEAN" ), QString( "GEOMBooleanOperations" ) );
  createTool( GEOMOp::OpFuse,       boolTbId );
  createTool( GEOMOp::OpCommon,     boolTbId );
  createTool( GEOMOp::OpCut,        boolTbId );
  createTool( GEOMOp::OpSection,    boolTbId );

  int genTbId = createTool( tr( "TOOL_GENERATION" ), QString( "GEOMGeneration" ) );
  createTool( GEOMOp::OpPrism,      genTbId );
  createTool( GEOMOp::OpRevolution, genTbId );
  createTool( GEOMOp::OpFilling,    genTbId );
  createTool( GEOMOp::OpPipe,       genTbId );
  createTool( GEOMOp::OpPipePath,   genTbId );
  createTool( GEOMOp::OpThickness,  genTbId );

  int transTbId = createTool( tr( "TOOL_TRANSFORMATION" ), QString( "GEOMTransformation" ) );
  createTool( GEOMOp::OpTranslate,      transTbId );
  createTool( GEOMOp::OpRotate,         transTbId );
  createTool( GEOMOp::OpChangeLoc,      transTbId );
  createTool( GEOMOp::OpMirror,         transTbId );
  createTool( GEOMOp::OpScale,          transTbId );
  createTool( GEOMOp::OpOffset,         transTbId );
  createTool( GEOMOp::OpProjection,     transTbId );
  createTool( GEOMOp::OpExtension,      transTbId );
  createTool( GEOMOp::OpProjOnCyl,      transTbId );
  createTool( separator(),              transTbId );
  createTool( GEOMOp::OpMultiTranslate, transTbId );
  createTool( GEOMOp::OpMultiRotate,    transTbId );

  int operTbId = createTool( tr( "TOOL_OPERATIONS" ), QString( "GEOMOperations" ) );
  createTool( GEOMOp::OpExplode,         operTbId );
  createTool( GEOMOp::OpPartition,       operTbId );
  createTool( GEOMOp::OpArchimede,       operTbId );
  createTool( GEOMOp::OpShapesOnShape,   operTbId );
  createTool( GEOMOp::OpSharedShapes,    operTbId );
  createTool( GEOMOp::OpTransferData,    operTbId );
  createTool( GEOMOp::OpExtraction,      operTbId );

  int featTbId = createTool( tr( "TOOL_FEATURES" ), QString( "GEOMModification" ) );
  createTool( GEOMOp::OpFillet1d,        featTbId );
  createTool( GEOMOp::OpFillet2d,        featTbId );
  createTool( GEOMOp::OpFillet3d,        featTbId );
  createTool( GEOMOp::OpChamfer,         featTbId );
  createTool( GEOMOp::OpExtrudedBoss,    featTbId );
  createTool( GEOMOp::OpExtrudedCut,     featTbId );

  int buildTbId = createTool( tr( "TOOL_BUILD" ), QString( "GEOMBuild" ) );
  createTool( GEOMOp::OpEdge,     buildTbId );
  createTool( GEOMOp::OpWire,     buildTbId );
  createTool( GEOMOp::OpFace,     buildTbId );
  createTool( GEOMOp::OpShell,    buildTbId );
  createTool( GEOMOp::OpSolid,    buildTbId );
  createTool( GEOMOp::OpCompound, buildTbId );

  int measureTbId = createTool( tr( "TOOL_MEASURES" ), QString( "GEOMMeasures" ) );
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
  createTool( GEOMOp::OpGetNonBlocks,     measureTbId );
  createTool( GEOMOp::OpCheckSelfInters,  measureTbId );
  createTool( GEOMOp::OpFastCheckInters,  measureTbId );

  int picturesTbId = createTool( tr( "TOOL_PICTURES" ), QString( "GEOMPictures" ) );
  createTool( GEOMOp::OpPictureImport,    picturesTbId );
#ifdef WITH_OPENCV
  createTool( GEOMOp::OpFeatureDetect,  picturesTbId );
#endif

  //int advancedTbId = createTool( tr( "TOOL_ADVANCED" ) );
  //createTool( GEOMOp::OpSmoothingSurface, advancedTbId );
  //@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@ do not remove this line @@//

  // ---- create popup menus --------------------------

  QString clientOCCorVTK = "(client='OCCViewer' or client='VTKViewer')";
  QString clientOCC = "(client='OCCViewer')";
  QString clientOCCorVTK_AndSomeVisible = clientOCCorVTK + " and selcount>0 and isVisible";
  QString clientOCC_AndSomeVisible = clientOCC + " and selcount>0 and isVisible";

  QString clientOCCorOB = "(client='ObjectBrowser' or client='OCCViewer')";
  QString clientOCCorVTKorOB = "(client='ObjectBrowser' or client='OCCViewer' or client='VTKViewer')";
  QString clientOCCorVTKorOB_AndSomeVisible = clientOCCorVTKorOB + " and selcount>0 and isVisible";
  QString clientOCCorOB_AndSomeVisible = clientOCCorOB + " and selcount>0 and isVisible";

  QString autoColorPrefix =
    "(client='ObjectBrowser' or client='OCCViewer' or client='VTKViewer') and type='Shape' and selcount=1";

  QtxPopupMgr* mgr = popupMgr();

  mgr->insert( action(  GEOMOp::OpDelete ), -1, -1 );  // delete
  mgr->setRule( action( GEOMOp::OpDelete ), QString("$type in {'Shape' 'Group' 'Folder' 'Field' 'FieldStep'} and selcount>0"), QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpGroupCreatePopup ), -1, -1 ); // create group
  mgr->setRule( action( GEOMOp::OpGroupCreatePopup ), QString("type='Shape' and selcount=1 and isOCC=true"), QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpEditFieldPopup ), -1, -1 ); // edit field
  mgr->setRule( action( GEOMOp::OpEditFieldPopup ), QString("(type='Field' or type='FieldStep') and isOCC=true"), QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpDiscloseChildren ), -1, -1 ); // disclose child items
  mgr->setRule( action( GEOMOp::OpDiscloseChildren ), QString("client='ObjectBrowser' and type='Shape' and selcount=1 and hasConcealedChildren=true"), QtxPopupMgr::VisibleRule );

  mgr->insert( action(  GEOMOp::OpConcealChildren ), -1, -1 ); // conceal child items
  mgr->setRule( action( GEOMOp::OpConcealChildren ), QString("client='ObjectBrowser' and type='Shape' and selcount=1 and hasDisclosedChildren=true"), QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpGroupEdit ), -1, -1 );  // edit group
  mgr->setRule( action( GEOMOp::OpGroupEdit ),  QString("client='ObjectBrowser' and type='Group' and selcount=1 and isOCC=true"), QtxPopupMgr::VisibleRule );
  mgr->insert( separator(), -1, -1 );     // -----------

  //QString bringRule = clientOCCorOB + " and ($component={'GEOM'}) and (selcount>0) and isOCC=true and topLevel=false";
  QString bringRule = clientOCCorOB + " and ($component={'GEOM'}) and isFolder=false and (selcount>0) and isOCC=true";
  mgr->insert( action(GEOMOp::OpBringToFront ), -1, -1 ); // bring to front
  mgr->setRule(action(GEOMOp::OpBringToFront), bringRule + " and autoBringToFront = false", QtxPopupMgr::VisibleRule );
  mgr->setRule(action(GEOMOp::OpBringToFront), "topLevel=true", QtxPopupMgr::ToggleRule );
  mgr->insert( action(GEOMOp::OpClsBringToFront ), -1, -1 ); // clear bring to front
  mgr->setRule( action(GEOMOp::OpClsBringToFront ), clientOCC + " and autoBringToFront = false", QtxPopupMgr::VisibleRule );
  mgr->insert( separator(), -1, -1 );     // -----------
  dispmodeId = mgr->insert(  tr( "MEN_DISPLAY_MODE" ), -1, -1 ); // display mode menu
  mgr->insert( action(  GEOMOp::OpWireframe ), dispmodeId, -1 ); // wireframe
  mgr->setRule( action( GEOMOp::OpWireframe ), clientOCCorVTK_AndSomeVisible + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );
  mgr->setRule( action( GEOMOp::OpWireframe ), clientOCCorVTK + " and displaymode='Wireframe'", QtxPopupMgr::ToggleRule );
  mgr->insert( action(  GEOMOp::OpShading ), dispmodeId, -1 ); // shading
  mgr->setRule( action( GEOMOp::OpShading ), clientOCCorVTK_AndSomeVisible + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );
  mgr->setRule( action( GEOMOp::OpShading ), clientOCCorVTK + " and displaymode='Shading'", QtxPopupMgr::ToggleRule );
  mgr->insert( action(  GEOMOp::OpShadingWithEdges ), dispmodeId, -1 ); // shading with edges
  mgr->setRule( action( GEOMOp::OpShadingWithEdges ), clientOCCorVTK_AndSomeVisible + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );
  mgr->setRule( action( GEOMOp::OpShadingWithEdges ), clientOCCorVTK + " and displaymode='ShadingWithEdges'", QtxPopupMgr::ToggleRule );
  mgr->insert( action(  GEOMOp::OpTexture ), dispmodeId, -1 ); // wireframe
  mgr->setRule( action( GEOMOp::OpTexture ), clientOCC_AndSomeVisible, QtxPopupMgr::VisibleRule );
  mgr->setRule( action( GEOMOp::OpTexture), clientOCC + " and displaymode='Texture'", QtxPopupMgr::ToggleRule );
  mgr->insert( separator(), dispmodeId, -1 );
  mgr->insert( action(  GEOMOp::OpVectors ), dispmodeId, -1 ); // vectors
  mgr->setRule( action( GEOMOp::OpVectors ), clientOCCorVTK_AndSomeVisible  + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );
  mgr->setRule( action( GEOMOp::OpVectors ), clientOCCorVTK + " and isVectorsMode", QtxPopupMgr::ToggleRule );
  mgr->insert( action(  GEOMOp::OpVertices ), dispmodeId, -1 ); // vertices
  mgr->setRule( action( GEOMOp::OpVertices ), clientOCCorVTK_AndSomeVisible  + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );
  mgr->setRule( action( GEOMOp::OpVertices ), clientOCCorVTK + " and isVerticesMode", QtxPopupMgr::ToggleRule );
  mgr->insert( action(  GEOMOp::OpShowName ), dispmodeId, -1 ); // show name
  mgr->setRule( action( GEOMOp::OpShowName ), clientOCCorVTK_AndSomeVisible  + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );
  mgr->setRule( action( GEOMOp::OpShowName ), clientOCCorVTK + " and isNameMode", QtxPopupMgr::ToggleRule );
  mgr->insert( separator(), -1, -1 );     // -----------

  mgr->insert( action(  GEOMOp::OpColor ), -1, -1 ); // color
  mgr->setRule( action( GEOMOp::OpColor ), clientOCCorVTKorOB_AndSomeVisible + " and ($component={'GEOM'})" + "and isPhysicalMaterial=false", QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpTransparency ), -1, -1 ); // transparency
  mgr->setRule( action( GEOMOp::OpTransparency ), clientOCCorVTK_AndSomeVisible + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpIsos ), -1, -1 ); // isos
  mgr->setRule( action( GEOMOp::OpIsos ), clientOCCorVTK_AndSomeVisible + " and selcount>0 and isVisible" + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpDeflection ), -1, -1 ); // deflection
  mgr->setRule( action( GEOMOp::OpDeflection ), clientOCCorVTK_AndSomeVisible + " and selcount>0 and isVisible" + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpPointMarker ), -1, -1 ); // point marker
  mgr->setRule( action( GEOMOp::OpPointMarker ), clientOCCorOB + " and $type in {'Shape' 'Group' 'Field' 'FieldStep'} and selcount>0 and isOCC=true", QtxPopupMgr::VisibleRule );

  // material properties
  mgr->insert( action(  GEOMOp::OpMaterialProperties ), -1, -1 );
  mgr->setRule( action( GEOMOp::OpMaterialProperties ), clientOCCorVTK_AndSomeVisible + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );

 // texture
  mgr->insert( action(  GEOMOp::OpSetTexture ), -1, -1 );
  mgr->setRule( action( GEOMOp::OpSetTexture ), clientOCCorOB_AndSomeVisible + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );

  int lineW = mgr->insert(  tr( "MEN_LINE_WIDTH" ), -1, -1 ); // line width menu
  mgr->insert( action(  GEOMOp::OpEdgeWidth ), lineW, -1 ); // edge width
  mgr->setRule( action( GEOMOp::OpEdgeWidth ), clientOCCorVTK_AndSomeVisible + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );

  mgr->insert( action(  GEOMOp::OpIsosWidth ), lineW, -1 ); // isos width
  mgr->setRule( action( GEOMOp::OpIsosWidth ), clientOCCorVTK_AndSomeVisible + " and ($component={'GEOM'})", QtxPopupMgr::VisibleRule );

  mgr->insert( separator(), -1, -1 );     // -----------
  mgr->insert( action(  GEOMOp::OpAutoColor ), -1, -1 ); // auto color
  mgr->setRule( action( GEOMOp::OpAutoColor ), autoColorPrefix + " and isAutoColor=false", QtxPopupMgr::VisibleRule );
  mgr->insert( action(  GEOMOp::OpNoAutoColor ), -1, -1 ); // disable auto color
  mgr->setRule( action( GEOMOp::OpNoAutoColor ), autoColorPrefix + " and isAutoColor=true", QtxPopupMgr::VisibleRule );
  mgr->insert( separator(), -1, -1 );     // -----------

  QString canDisplay = "($component={'GEOM'}) and (selcount>0) and ({true} in $canBeDisplayed) ",
          onlyComponent = "((type='Component') and selcount=1)",
          rule = canDisplay + "and ((($type in {%1}) and( %2 )) or " + onlyComponent + ")",
          types = "'Shape' 'Group' 'FieldStep'";

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
  mgr->insert( action(GEOMOp::OpShowOnlyChildren ), -1, -1 ); // display only children
  mgr->setRule(action(GEOMOp::OpShowOnlyChildren ), (canDisplay + "and ($type in {%1}) and client='ObjectBrowser' and hasChildren=true").arg( types ), QtxPopupMgr::VisibleRule );

  QString aDimensionRule = "($component={'GEOM'}) and selcount=1 and isVisible and type='Shape' and %1";

  mgr->insert( separator(), -1, -1 ); // -----------
  mgr->insert( action( GEOMOp::OpShowAllDimensions ), -1, -1 ); // show all dimensions
  mgr->setRule( action( GEOMOp::OpShowAllDimensions ), aDimensionRule.arg( "hasHiddenDimensions" ), QtxPopupMgr::VisibleRule );
  mgr->insert( action( GEOMOp::OpHideAllDimensions ), -1, -1 ); // hide all dimensions
  mgr->setRule( action( GEOMOp::OpHideAllDimensions ), aDimensionRule.arg( "hasVisibleDimensions" ), QtxPopupMgr::VisibleRule );

  mgr->insert( separator(), -1, -1 );     // -----------
  mgr->insert( action(  GEOMOp::OpUnpublishObject ), -1, -1 ); // Unpublish object
  mgr->setRule( action( GEOMOp::OpUnpublishObject ), QString("client='ObjectBrowser' and $type in {'Shape' 'Group' 'Field' 'FieldStep'} and selcount>0"), QtxPopupMgr::VisibleRule );

  mgr->insert( action(  GEOMOp::OpPublishObject ), -1, -1 ); // Publish object
  mgr->setRule( action( GEOMOp::OpPublishObject ), QString("client='ObjectBrowser' and isComponent=true"), QtxPopupMgr::VisibleRule );

  mgr->insert( action(  GEOMOp::OpReimport ), -1, -1 );  // delete
  mgr->setRule( action( GEOMOp::OpReimport ), QString("$imported in {'true'} and selcount>0"), QtxPopupMgr::VisibleRule );

  mgr->insert( separator(), -1, -1 );     // -----------
  mgr->insert( action(  GEOMOp::OpCreateFolder ), -1, -1 ); // Create Folder
  mgr->setRule( action( GEOMOp::OpCreateFolder ), QString("client='ObjectBrowser' and $component={'GEOM'} and (isComponent=true or isFolder=true)"), QtxPopupMgr::VisibleRule );

  mgr->insert( separator(), -1, -1 );     // -----------
  mgr->insert( action(  GEOMOp::OpSortChildren ), -1, -1 ); // Sort child items
  mgr->setRule( action( GEOMOp::OpSortChildren ), QString("client='ObjectBrowser' and $component={'GEOM'} and nbChildren>1"), QtxPopupMgr::VisibleRule );

#ifndef DISABLE_GRAPHICSVIEW
  mgr->insert( separator(), -1, -1 );     // -----------
  mgr->insert( action(  GEOMOp::OpShowDependencyTree ), -1, -1 ); // Show dependency tree
  mgr->setRule( action( GEOMOp::OpShowDependencyTree ), clientOCCorVTKorOB + " and selcount>0 and ($component={'GEOM'}) and type='Shape'", QtxPopupMgr::VisibleRule );
#endif

  mgr->insert( separator(), -1, -1 );     // -----------
  mgr->insert( action(  GEOMOp::OpReduceStudy ), -1, -1 ); // Reduce Study
  mgr->setRule( action( GEOMOp::OpReduceStudy ), clientOCCorVTKorOB + " and selcount>0 and ($component={'GEOM'}) and type='Shape'", QtxPopupMgr::VisibleRule );

  mgr->hide( mgr->actionId( action( myEraseAll ) ) );

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if (resMgr) {
    GEOM_AISShape::setTopLevelDisplayMode((GEOM_AISShape::TopLevelDispMode)resMgr->integerValue("Geometry", "toplevel_dm", 0));
    QColor c = resMgr->colorValue( "Geometry", "toplevel_color", QColor( 170, 85, 0 ) );
    GEOM_AISShape::setTopLevelColor(SalomeApp_Tools::color(c));
  }

  // create plugin actions and menus
  addPluginActions();
}

//=======================================================================
// function : GeometryGUI::addPluginActions()
// purpose  :
//=======================================================================
void GeometryGUI::addPluginActions()
{
  // Resource manager
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if (!resMgr) return;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( application()->activeStudy() );
  if (!appStudy) return;

  // Find names of a resource XML files ("AdvancedGEOM.xml" and others);

  GEOMUtils::PluginInfo plugins = GEOMUtils::ReadPluginInfo();

  int id = GEOMOp::OpLastOperationID; // TODO?

  // loop on plugins
  GEOMUtils::PluginInfo::const_iterator it;
  for ( it = plugins.begin(); it != plugins.end(); ++it ) {
    // bind action lib and label to its ID for activateOperation() method proper work
    GEOMUtils::PluginData pdata = (*it);
    myPluginLibs[pdata.name.c_str()] = pdata.clientLib.c_str();
    std::list<GEOMUtils::ActionData> actions = (*it).actions;
    std::list<GEOMUtils::ActionData>::const_iterator ait;
    for ( ait = actions.begin(); ait != actions.end(); ++ait ) {
      GEOMUtils::ActionData adata = (*ait);
      // icon
      QPixmap icon;
      if ( !adata.icon.empty() )
        icon = resMgr->loadPixmap( pdata.name.c_str(), adata.icon.c_str() );
      // menu text (path)
      QStringList smenus = QString( adata.menuText.c_str() ).split( "/" );
      QString actionName = smenus.last();
      actionName = actionName.toUpper().prepend( "MEN_" );
      smenus.removeLast();

      // path to action in toolbar
      QStringList stools = QString( adata.toolTip.c_str() ).split( "/" );
      QString actionTool = stools.last();
      actionTool = actionTool.toUpper().prepend( "TOP_" );
      stools.removeLast();
      
      QString actionStat = adata.statusText.c_str();
      actionStat = actionStat.toUpper().prepend( "STB_" );

      createAction( id, // ~ adata.label
                    tr( actionTool.toLatin1().constData() ),
                    icon,
                    tr( actionName.toLatin1().constData() ),
                    tr( actionStat.toLatin1().constData() ),
                    QKeySequence( tr( adata.accel.c_str() ) ),
                    application()->desktop(),
                    false /*toggle*/,
                    this, SLOT( OnGUIEvent() ),
                    QString() /*shortcutAction*/ );
      
      int menuId = -1;
      foreach ( QString subMenu, smenus ) {
        QStringList subMenuList = subMenu.split( ":" );
        QString subMenuName = subMenuList[0].toUpper().prepend( "MEN_" );
        int subMenuGroup = subMenuList.size() > 1 ? subMenuList[1].toInt() : -1;
        menuId = createMenu( tr( subMenuName.toLatin1().constData() ), menuId, -1, subMenuGroup );
      }
      createMenu( id, menuId, -1 );
      
      if ( !stools.isEmpty() ) {
        QString subTool = stools[0];
        subTool = subTool.toUpper().prepend( "TOOL_" );
        int toolId = createTool( tr( subTool.toLatin1().constData() ) );
        createTool(id, toolId);
      }

      // add action id to map
      PluginAction anAction( pdata.clientLib.c_str(), adata.label.c_str() );
      myPluginActions[id] = anAction;
      
      id++;
    }
  }
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
  PyObject* pluginsmanager = PyImport_ImportModuleNoBlock((char*)"salome_pluginsmanager");
  if ( !pluginsmanager ) {
    PyErr_Print();
  }
  else {
    PyObject* result =
      PyObject_CallMethod(pluginsmanager, (char*)"initialize", (char*)"isss", 1, "geom",
                          tr("MEN_NEW_ENTITY").toUtf8().data(),
                          tr("GEOM_PLUGINS_OTHER").toUtf8().data());
    if ( !result )
      PyErr_Print();
    Py_XDECREF(result);
  }
  PyGILState_Release(gstate);
  // end of GEOM plugins loading

  connect( application()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
          this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ) );

  // Reset actions accelerator keys
  action(GEOMOp::OpDelete)->setEnabled( true ); // Delete: Key_Delete

  GUIMap::Iterator it;
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it )
    it.value()->activate( application()->desktop() );

  LightApp_SelectionMgr* sm = getApp()->selectionMgr();

  connect( sm, SIGNAL( currentSelectionChanged() ), this, SLOT( updateCreationInfo() ));
  connect( sm, SIGNAL( currentSelectionChanged() ), this, SLOT( onAutoBringToFront() ));
  connect( sm, SIGNAL( currentSelectionChanged() ), this, SLOT( updateFieldColorScale() ));

  if ( !myCreationInfoWdg )
    myCreationInfoWdg = new GEOMGUI_CreationInfoWdg( getApp() );
  getApp()->insertDockWindow( myCreationInfoWdg->getWinID(), myCreationInfoWdg );
  getApp()->placeDockWindow( myCreationInfoWdg->getWinID(), Qt::LeftDockWidgetArea );

  if ( !myTextTreeWdg )
    myTextTreeWdg = new GEOMGUI_TextTreeWdg( getApp() );
  getApp()->insertDockWindow( myTextTreeWdg->getWinID(), myTextTreeWdg );
  getApp()->placeDockWindow( myTextTreeWdg->getWinID(), Qt::LeftDockWidgetArea );

  //NPAL 19674
  SALOME_ListIO selected;
  sm->selectedObjects( selected );
  sm->clearSelected();

  SUIT_ViewManager* vm;
  ViewManagerList OCCViewManagers, VTKViewManagers;

  application()->viewManagers( OCCViewer_Viewer::Type(), OCCViewManagers );
  QListIterator<SUIT_ViewManager*> itOCC( OCCViewManagers );
  while ( itOCC.hasNext() && (vm = itOCC.next()) )
    onViewManagerAdded(vm);

  application()->viewManagers( SVTK_Viewer::Type(), VTKViewManagers );
  QListIterator<SUIT_ViewManager*> itVTK( VTKViewManagers );
  while ( itVTK.hasNext() && (vm = itVTK.next()) )
    onViewManagerAdded(vm);

  sm->setSelectedObjects( selected, true );   //NPAL 19674

  QMenu* viewMenu = menuMgr()->findMenu( STD_Application::MenuViewId );
  if ( viewMenu )
    connect( viewMenu, SIGNAL( aboutToShow() ), this, SLOT( onViewAboutToShow() ) );

  // 0020836 (Basic vectors and origin)
  SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
  if ( aResourceMgr->booleanValue( "Geometry", "auto_create_base_objects", false ) ) {
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( application()->activeStudy() );
    if ( appStudy ) {
      _PTR(Study) studyDS = appStudy->studyDS();
      if ( studyDS ) {
        _PTR(SComponent) aSComponent = studyDS->FindComponent("GEOM");
        if ( !aSComponent ) // create objects automatically only if there is no GEOM component
          createOriginAndBaseVectors();
      }
    }
  }

  Py_XDECREF(pluginsmanager);
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

  LightApp_SelectionMgr* selMrg = getApp()->selectionMgr();

  disconnect( selMrg, SIGNAL( currentSelectionChanged() ), this, SLOT( updateCreationInfo() ));
  disconnect( selMrg, SIGNAL( currentSelectionChanged() ), this, SLOT( updateFieldColorScale() ));
  if ( myCreationInfoWdg ) {
    getApp()->removeDockWindow( myCreationInfoWdg->getWinID() );
    myCreationInfoWdg = 0;
  }
  if ( myTextTreeWdg ) {
    getApp()->removeDockWindow( myTextTreeWdg->getWinID() );
    disconnect( application(), 0, myTextTreeWdg, 0 );
    myTextTreeWdg = 0;
  }

  EmitSignalCloseAllDialogs();

  GUIMap::Iterator it;
  for ( it = myGUIMap.begin(); it != myGUIMap.end(); ++it )
    it.value()->deactivate();

  // Unset actions accelerator keys
  action(GEOMOp::OpDelete)->setEnabled( false ); // Delete: Key_Delete

  qDeleteAll(myOCCSelectors);
  myOCCSelectors.clear();
  selMrg->setEnabled( true, OCCViewer_Viewer::Type() );

  qDeleteAll(myVTKSelectors);
  myVTKSelectors.clear();
  selMrg->setEnabled( true, SVTK_Viewer::Type() );

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

  const bool ViewOCC = ( win->getViewManager() ? win->getViewManager()->getType() == OCCViewer_Viewer::Type() : false );
  //const bool ViewVTK = ( win->getViewManager() ? win->getViewManager()->getType() == SVTK_Viewer::Type() : false );

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
  action( GEOMOp::OpCreateField )->setEnabled( ViewOCC ); // Create Field
  action( GEOMOp::OpEditField )->setEnabled( ViewOCC ); // Edit Field

  action( GEOMOp::OpMultiTransform )->setEnabled( ViewOCC ); // MENU BLOCKS - MULTI-TRANSFORMATION
}

void GeometryGUI::windows( QMap<int, int>& mappa ) const
{
  mappa.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
  mappa.insert( SalomeApp_Application::WT_NoteBook, Qt::LeftDockWidgetArea );
#ifndef DISABLE_PYCONSOLE
  mappa.insert( SalomeApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea );
#endif
  if ( myCreationInfoWdg )
    mappa.insert( myCreationInfoWdg->getWinID(), Qt::LeftDockWidgetArea );
  if ( myTextTreeWdg )
    mappa.insert( myTextTreeWdg->getWinID(), Qt::LeftDockWidgetArea );
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
    connect( vm, SIGNAL( mouseRelease ( SUIT_ViewWindow*, QMouseEvent* ) ),
             this, SLOT( OnMouseRelease( SUIT_ViewWindow*, QMouseEvent* ) ) );

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
          /*delete*/ myOCCSelectors.takeAt( myOCCSelectors.indexOf( sr ) );
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
          /*delete*/ myVTKSelectors.takeAt( myVTKSelectors.indexOf( sr ) );
          break;
        }
  }
}

//================================================================================
/*!
 * \brief Slot called when selection changed. Shows creation info of a selected object
 */
//================================================================================

void GeometryGUI::updateCreationInfo()
{
  if ( myCreationInfoWdg )
    myCreationInfoWdg->clear();

  // Code below is commented to have myCreationInfoWdg filled as soon as it is shown again
  // if ( !myCreationInfoWdg->isVisible() )
  //   return;

  // look for a sole selected GEOM_Object
  GEOM::GEOM_BaseObject_var geomObj;

  SALOME_ListIO selected;
  getApp()->selectionMgr()->selectedObjects( selected );

  _PTR(Study) study = dynamic_cast<SalomeApp_Study*>( getApp()->activeStudy() )->studyDS();
  SALOME_ListIteratorOfListIO selIt( selected );
  for ( ; selIt.More(); selIt.Next() )
  {
    Handle(SALOME_InteractiveObject) io = selIt.Value();
    if ( !io->hasEntry() ) continue;
    _PTR(SObject) sobj = study->FindObjectID( io->getEntry() );
    if ( !sobj ) continue;
    CORBA::Object_var          obj = GeometryGUI::ClientSObjectToObject( sobj );
    GEOM::GEOM_BaseObject_var gobj = GEOM::GEOM_BaseObject::_narrow( obj );
    if ( !gobj->_is_nil() )
    {
      if ( !geomObj->_is_nil() )
        return; // several GEOM objects selected
      geomObj = gobj;
    }
  }
  if ( geomObj->_is_nil() ) return;

  // pass creation info of geomObj to myCreationInfoWdg

  if ( myCreationInfoWdg ) {

    GEOM::CreationInformationSeq_var info;
    try {
      OCC_CATCH_SIGNALS;
      info = geomObj->GetCreationInformation();
    }
    catch (...) {
    }
    myCreationInfoWdg->setInfo( info );
  }
}

void GeometryGUI::onAutoBringToFront()
{
  bool isAutoBringToFront = SUIT_Session::session()->resourceMgr()->booleanValue( "Geometry", "auto_bring_to_front", "false" );
  if( !isAutoBringToFront )
    return;
  
  SUIT_ViewWindow* SUIT_window = application()->desktop()->activeWindow();
  if ( !SUIT_window || SUIT_window->getViewManager()->getType() != OCCViewer_Viewer::Type() )
        return;

  SalomeApp_Study* appStudy = dynamic_cast< SalomeApp_Study* >( getApp()->activeStudy() );
  if (!appStudy) return;

  GEOM_Displayer displayer( appStudy );
  
  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;
  
  int aMgrId = dynamic_cast< SUIT_ViewModel* >( window )->getViewManager()->getGlobalId();
  
  SALOME_ListIO selected;
  getApp()->selectionMgr()->selectedObjects( selected );
  if (!myTopLevelIOList.IsEmpty())
  {
    for( SALOME_ListIteratorOfListIO It( myTopLevelIOList ); It.More(); It.Next() )
    {
      Handle( SALOME_InteractiveObject ) io = It.Value();
      bool isSelected = false;
      for( SALOME_ListIteratorOfListIO It_sel( selected ); It_sel.More(); It_sel.Next() )
      {
        Handle( SALOME_InteractiveObject ) sel_io = It_sel.Value();
        if( io->isSame( sel_io ) )
          isSelected = true;
      }
      if (!isSelected && appStudy->findObjectByEntry(io->getEntry()))
      {
        appStudy->setObjectProperty( aMgrId, io->getEntry(), GEOM::propertyName( GEOM::TopLevel ), false );
        if ( window->isVisible( io ) ) displayer.Redisplay( io, false );     
      }
    }
  }
  
  myTopLevelIOList.Assign(selected);
  for( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() )
  {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    appStudy->setObjectProperty( aMgrId, io->getEntry(), GEOM::propertyName( GEOM::TopLevel ), true );
    if ( window->isVisible( io ) ) displayer.Redisplay( io, false );     
  }
    
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}

void GeometryGUI::updateFieldColorScale()
{
  if( SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( getApp()->activeStudy() ) )
  {
    GEOM_Displayer aDisplayer( aStudy );
    aDisplayer.UpdateColorScale();
  }
}

QString GeometryGUI::engineIOR() const
{
  if ( !CORBA::is_nil( GetGeomGen() ) )
    return QString( getApp()->orb()->object_to_string( GetGeomGen() ) );
  return "";
}

Handle(TColStd_HArray1OfByte) GeometryGUI::getTexture
      (SalomeApp_Study* theStudy, int theId, int& theWidth, int& theHeight)
{
  theWidth = theHeight = 0;

  Handle(TColStd_HArray1OfByte) aTexture;

  if (theStudy) {
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

          aTexture  = new TColStd_HArray1OfByte (1, aStream->length());

          for (int i = 0; i < aStream->length(); i++)
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
  SalomeApp_Module::contextMenuPopup( client, menu, title );
  SALOME_ListIO lst;
  getApp()->selectionMgr()->selectedObjects( lst );

  //Add submenu for predefined materials
  bool isPredefMat = SUIT_Session::session()->resourceMgr()->booleanValue( "Geometry", "predef_materials" );
  if ( ( client == "OCCViewer" || client == "VTKViewer" ) && lst.Extent() > 0 ) {
    QtxPopupMgr* mgr = popupMgr();
    //get parrent for submenu
    QAction* act = mgr->action( mgr->actionId( action(  GEOMOp::OpMaterialProperties ) ) );
    //Clear old  menu
    QMenu* oldMenu = act->menu() ;
    if( oldMenu ) {
      delete oldMenu;
    }
    if( isPredefMat ){
      QMenu* matMenu = new QMenu();
      QSignalMapper* signalMapper = new QSignalMapper( matMenu );

      //Get current material model for the object
      QVariant v;
      LightApp_Application* anApp = dynamic_cast<LightApp_Application*>( getApp() );
      if ( anApp && anApp->activeViewManager() ) {
        LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( anApp->activeStudy() );
        if( aStudy ) {
          v = aStudy->getObjectProperty( anApp->activeViewManager()->getGlobalId(), lst.Last()->getEntry(), GEOM::propertyName( GEOM::Material ), QVariant() );
        }
      }
      QString curModel = "";
      if ( v.canConvert<QString>() ) curModel = v.toString();
      // get list of all predefined materials
      QStringList materials = Material_ResourceMgr::resourceMgr()->materials();
      bool found = false;
      foreach ( QString material, materials )
      {
        QAction* menAct = matMenu->addAction( material );
        connect(menAct, SIGNAL( toggled( bool ) ), signalMapper, SLOT( map() ) );
        signalMapper->setMapping( menAct, material );
        menAct->setCheckable( true );
        // Set checked if this material is current
        Material_Model aModel;
        aModel.fromResources( material );
        if ( !found && aModel.toProperties() == curModel ) {
          menAct->setChecked( true );
          found = true;
        }
      }
      matMenu->insertAction( matMenu->addSeparator(), action(  GEOMOp::OpPredefMaterCustom ) );
      matMenu->insertSeparator( action(  GEOMOp::OpPredefMaterCustom ) );
      connect( signalMapper, SIGNAL( mapped( const QString & ) ),
                 this, SLOT( OnSetMaterial( const QString & ) ) );
      act->setMenu( matMenu );
    }
  }
  //Set name
  if ( ( client == "OCCViewer" || client == "VTKViewer" ) && lst.Extent() == 1 ) {
    Handle(SALOME_InteractiveObject) io = lst.First();
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( application()->activeStudy() );
    _PTR(Study) study = appStudy->studyDS();
    _PTR(SObject) obj = study->FindObjectID( io->getEntry() );
    if ( obj ) {
      QString aName = QString( obj->GetName().c_str() );
      aName.remove( QRegExp("\\s+$") );
      title = aName;
    }
  }
}

void GeometryGUI::OnSetMaterial(const QString& theName)
{
  OnGUIEvent( GEOMOp::OpPredefMaterial, QVariant( theName ) );
}


void GeometryGUI::createPreferences()
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  int tabId = addPreference( tr( "PREF_TAB_SETTINGS" ) );

  int genGroup = addPreference( tr( "PREF_GROUP_GENERAL" ), tabId );
  setPreferenceProperty( genGroup, "columns", 2 );

  int dispmode = addPreference( tr( "PREF_DISPLAY_MODE" ), genGroup,
                                LightApp_Preferences::Selector,
                                "Geometry", "display_mode" );

  addPreference( tr( "PREF_SHADING_COLOR" ), genGroup,
                 LightApp_Preferences::Color, "Geometry", "shading_color" );

  addPreference( tr( "PREF_EDGES_IN_SHADING" ), genGroup,
                 LightApp_Preferences::Color, "Geometry", "edges_in_shading_color" );

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

  addPreference( tr( "PREF_LABEL_COLOR" ), genGroup,
                 LightApp_Preferences::Color, "Geometry", "label_color" );

  addPreference( "", genGroup, LightApp_Preferences::Space );

  addPreference( tr( "PREF_TOPLEVEL_COLOR" ), genGroup,
                 LightApp_Preferences::Color, "Geometry", "toplevel_color" );

  int top_lev_dm = addPreference( tr( "PREF_TOPLEVEL_DM" ), genGroup,
                      LightApp_Preferences::Selector, "Geometry", "toplevel_dm" );

  int transparency = addPreference( tr( "PREF_TRANSPARENCY" ), genGroup,
                                    LightApp_Preferences::IntSpin, "Geometry", "transparency" );

  int defl = addPreference( tr( "PREF_DEFLECTION" ), genGroup,
                            LightApp_Preferences::DblSpin, "Geometry", "deflection_coeff" );

  addPreference( tr( "PREF_PREDEF_MATERIALS" ), genGroup,
                 LightApp_Preferences::Bool, "Geometry", "predef_materials" );

  int material = addPreference( tr( "PREF_MATERIAL" ), genGroup,
                                LightApp_Preferences::Selector,
                                "Geometry", "material" );

  addPreference( tr( "PREF_EDITGROUP_COLOR" ), genGroup,
                 LightApp_Preferences::Color, "Geometry", "editgroup_color" );

  const int nb = 4;
  int wd[nb];
  int iter=0;

  wd[iter++] = addPreference( tr( "PREF_EDGE_WIDTH" ), genGroup,
                              LightApp_Preferences::IntSpin, "Geometry", "edge_width" );

  wd[iter++] = addPreference( tr( "PREF_ISOLINES_WIDTH" ), genGroup,
                              LightApp_Preferences::IntSpin, "Geometry", "isolines_width" );

  wd[iter++] = addPreference( tr( "PREF_PREVIEW_EDGE_WIDTH" ), genGroup,
                                     LightApp_Preferences::IntSpin, "Geometry", "preview_edge_width" );

  wd[iter++] = addPreference( tr( "PREF_MEASURES_LINE_WIDTH" ), genGroup,
                              LightApp_Preferences::IntSpin, "Geometry", "measures_line_width" );

  for (int i = 0; i < nb; i++) {
    setPreferenceProperty( wd[i], "min", 1 );
    setPreferenceProperty( wd[i], "max", 5 );
  }

  int step = addPreference( tr( "PREF_STEP_VALUE" ), genGroup,
                            LightApp_Preferences::IntSpin, "Geometry", "SettingsGeomStep" );

  addPreference( tr( "PREF_AUTO_BRING_TO_FRONT" ), genGroup,
                 LightApp_Preferences::Bool, "Geometry", "auto_bring_to_front" );

  int aDimGroupId = addPreference( tr( "PREF_DIMENSIONS" ), tabId );
  setPreferenceProperty( aDimGroupId, "columns", 2 );

  addPreference( tr( "PREF_DIMENSIONS_COLOR" ), aDimGroupId,
                 LightApp_Preferences::Color, "Geometry", "dimensions_color" );

  int aDimLineWidthId = addPreference( tr( "PREF_DIMENSIONS_LINE_WIDTH" ), aDimGroupId, 
                                       LightApp_Preferences::IntSpin, "Geometry", "dimensions_line_width" );

  setPreferenceProperty( aDimLineWidthId, "min", 1 );
  setPreferenceProperty( aDimLineWidthId, "max", 5 );

  int aDimFontId = addPreference( tr( "PREF_DIMENSIONS_FONT" ), aDimGroupId, LightApp_Preferences::Font, "Geometry", "dimensions_font" );

  int f = QtxFontEdit::Family | QtxFontEdit::Size;
  setPreferenceProperty( aDimFontId, "features", f );
  setPreferenceProperty( aDimFontId, "mode", QtxFontEdit::Custom );

  Handle(Font_FontMgr) fmgr = Font_FontMgr::GetInstance();
  QString aFontFile = "";
  resMgr->value("resources", "GEOM", aFontFile);
  aFontFile = aFontFile + QDir::separator() + "Y14.5M-2009.ttf";
  // add enginier font into combobox
  int fontID = QFontDatabase::addApplicationFont( aFontFile );
  Handle(Font_SystemFont) sf = new Font_SystemFont( 
    new TCollection_HAsciiString("Y14.5M-2009"), 
    Font_FA_Regular, 
    new TCollection_HAsciiString(aFontFile.toLatin1().data()) );
  // register font in OCC font manager
  fmgr->RegisterFont( sf, Standard_False );

  // get list of supported fonts by OCC
  QStringList anOCCFonts;
  TColStd_SequenceOfHAsciiString theFontsNames;
  fmgr->GetAvailableFontsNames( theFontsNames );
  for(Standard_Integer i=1; i<=theFontsNames.Length(); i++) {
    Handle(TCollection_HAsciiString) str = theFontsNames(i);
    anOCCFonts << str->ToCString();
  }
  anOCCFonts.removeDuplicates();
  // set the supported fonts into combobox to use its only
  setPreferenceProperty( aDimFontId, "fonts", anOCCFonts );

  int aDimArrLengthId = addPreference( tr( "PREF_DIMENSIONS_ARROW_LENGTH" ), aDimGroupId,
                                       LightApp_Preferences::DblSpin, "Geometry", "dimensions_arrow_length" );

  setPreferenceProperty( aDimArrLengthId, "min", 1e-9 );
  setPreferenceProperty( aDimArrLengthId, "max", 1e+9 );
  setPreferenceProperty( aDimArrLengthId, "precision", 9 );

  int aLengthUnitsId = addPreference( tr( "PREF_DIMENSIONS_LENGTH_UNITS" ), aDimGroupId,
                                      LightApp_Preferences::Selector, "Geometry", "dimensions_length_units" );

  int anAngUnitsId = addPreference( tr( "PREF_DIMENSIONS_ANGLE_UNITS" ), aDimGroupId,
                                   LightApp_Preferences::Selector, "Geometry", "dimensions_angle_units" );

  QStringList aListOfLengthUnits;
  aListOfLengthUnits << "m";
  aListOfLengthUnits << "cm";
  aListOfLengthUnits << "mm";
  aListOfLengthUnits << "in.";
  aListOfLengthUnits << "ft.";

  QStringList aListOfAngUnits;
  aListOfAngUnits << "rad";
  aListOfAngUnits << "deg";

  setPreferenceProperty( aLengthUnitsId, "strings", aListOfLengthUnits );
  setPreferenceProperty( anAngUnitsId,   "strings", aListOfAngUnits );

  addPreference( tr( "PREF_DIMENSIONS_SHOW_UNITS" ), aDimGroupId,
                 LightApp_Preferences::Bool, "Geometry", "dimensions_show_units" );

  int aDimDefFlyout = addPreference( tr( "PREF_DIMENSIONS_DEFAULT_FLYOUT" ), aDimGroupId,
                                     LightApp_Preferences::DblSpin, "Geometry", "dimensions_default_flyout" );

  setPreferenceProperty( aDimDefFlyout, "min", 1e-9 );
  setPreferenceProperty( aDimDefFlyout, "max", 1e+9 );
  setPreferenceProperty( aDimDefFlyout, "precision", 9 );

  addPreference( tr( "PREF_DIMENSIONS_USE_TEXT3D" ), aDimGroupId,
                 LightApp_Preferences::Bool, "Geometry", "dimensions_use_text3d" );

  int isoGroup = addPreference( tr( "PREF_ISOS" ), tabId );
  setPreferenceProperty( isoGroup, "columns", 2 );
  int isoU = addPreference( tr( "PREF_ISOS_U" ), isoGroup,
                            LightApp_Preferences::IntSpin, "Geometry", "iso_number_u" );
  setPreferenceProperty( isoU, "min", 0 );
  setPreferenceProperty( isoU, "max", 100000 );
  int isoV = addPreference( tr( "PREF_ISOS_V" ), isoGroup,
                            LightApp_Preferences::IntSpin, "Geometry", "iso_number_v" );
  setPreferenceProperty( isoV, "min", 0 );
  setPreferenceProperty( isoV, "max", 100000 );

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
  aModesList.append( tr("MEN_SHADING_WITH_EDGES") );
  aModesList.append( tr("MEN_TEXTURE") );

  QList<QVariant> anIndexesList;
  anIndexesList.append(0);
  anIndexesList.append(1);
  anIndexesList.append(2);
  anIndexesList.append(3);

  setPreferenceProperty( dispmode, "strings", aModesList );
  setPreferenceProperty( dispmode, "indexes", anIndexesList );

  // Set property for top level display mode
  QStringList aTopModesList;
  aTopModesList.append( tr("MEN_SHOW_ADD_WACTOR") );
  aTopModesList.append( tr("MEN_KEEP_CURRENT_DM") );
  aTopModesList.append( tr("MEN_WIREFRAME") );
  aTopModesList.append( tr("MEN_SHADING") );
  aTopModesList.append( tr("MEN_SHADING_WITH_EDGES") );

  QList<QVariant> aTopIndexesList;
  aTopIndexesList.append(0);
  aTopIndexesList.append(1);
  aTopIndexesList.append(2);
  aTopIndexesList.append(3);
  aTopIndexesList.append(4);

  setPreferenceProperty( top_lev_dm, "strings", aTopModesList );
  setPreferenceProperty( top_lev_dm, "indexes", aTopIndexesList );

  // Set property for step value for spinboxes
  setPreferenceProperty( step, "min", 1 );
  setPreferenceProperty( step, "max", 10000 );
  setPreferenceProperty( step, "precision", 3 );

  // Set property for trandparency value for spinboxes
  setPreferenceProperty( transparency, "min", 0 );
  setPreferenceProperty( transparency, "max", 100 );

  // Set property for deflection value for spinboxes
  setPreferenceProperty( defl, "min", GEOM::minDeflection() );
  setPreferenceProperty( defl, "max", 1.0 );
  setPreferenceProperty( defl, "step", 1.0e-04 );
  setPreferenceProperty( defl, "precision", 6 );

  // Set property for default material
  setPreferenceProperty( material, "strings", Material_ResourceMgr::resourceMgr()->materials() );

  // Set property vertex marker type
  QList<QVariant> aMarkerTypeIndicesList;
  QList<QVariant> aMarkerTypeIconsList;

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

  // Scalar bar for field step presentation
  int scalarBarGroup = addPreference( tr( "PREF_GROUP_SCALAR_BAR" ), tabId );
  setPreferenceProperty( scalarBarGroup, "columns", 2 );

  int sbXPosition = addPreference( tr( "PREF_SCALAR_BAR_X_POSITION" ), scalarBarGroup,
                                   LightApp_Preferences::DblSpin, "Geometry", "scalar_bar_x_position" );
  setPreferenceProperty( sbXPosition, "min", 0 );
  setPreferenceProperty( sbXPosition, "max", 1 );
  setPreferenceProperty( sbXPosition, "step", 0.05 );

  int sbYPosition = addPreference( tr( "PREF_SCALAR_BAR_Y_POSITION" ), scalarBarGroup,
                                   LightApp_Preferences::DblSpin, "Geometry", "scalar_bar_y_position" );
  setPreferenceProperty( sbYPosition, "min", 0 );
  setPreferenceProperty( sbYPosition, "max", 1 );
  setPreferenceProperty( sbYPosition, "step", 0.05 );

  int sbWidth = addPreference( tr( "PREF_SCALAR_BAR_WIDTH" ), scalarBarGroup,
                               LightApp_Preferences::DblSpin, "Geometry", "scalar_bar_width" );
  setPreferenceProperty( sbWidth, "min", 0 );
  setPreferenceProperty( sbWidth, "max", 1 );
  setPreferenceProperty( sbWidth, "step", 0.05 );

  int sbHeight = addPreference( tr( "PREF_SCALAR_BAR_HEIGHT" ), scalarBarGroup,
                                LightApp_Preferences::DblSpin, "Geometry", "scalar_bar_height" );
  setPreferenceProperty( sbHeight, "min", 0 );
  setPreferenceProperty( sbHeight, "max", 1 );
  setPreferenceProperty( sbHeight, "step", 0.05 );

  int sbTextHeight = addPreference( tr( "PREF_SCALAR_BAR_TEXT_HEIGHT" ), scalarBarGroup,
                                    LightApp_Preferences::IntSpin, "Geometry", "scalar_bar_text_height" );
  setPreferenceProperty( sbTextHeight, "min", 6 );
  setPreferenceProperty( sbTextHeight, "max", 24 );
  setPreferenceProperty( sbTextHeight, "step", 1 );

  int sbNbIntervals = addPreference( tr( "PREF_SCALAR_BAR_NUMBER_OF_INTERVALS" ), scalarBarGroup,
                                     LightApp_Preferences::IntSpin, "Geometry", "scalar_bar_nb_intervals" );
  setPreferenceProperty( sbNbIntervals, "min", 2 );
  setPreferenceProperty( sbNbIntervals, "max", 64 );
  setPreferenceProperty( sbNbIntervals, "step", 1 );

  int originGroup = addPreference( tr( "PREF_GROUP_ORIGIN_AND_BASE_VECTORS" ), tabId );
  setPreferenceProperty( originGroup, "columns", 2 );

  int baseVectorsLength = addPreference( tr( "PREF_BASE_VECTORS_LENGTH" ), originGroup,
                                         LightApp_Preferences::DblSpin, "Geometry", "base_vectors_length" );
  setPreferenceProperty( baseVectorsLength, "min", 0.01 );
  setPreferenceProperty( baseVectorsLength, "max", 1000 );

  addPreference( tr( "PREF_AUTO_CREATE" ), originGroup,
                 LightApp_Preferences::Bool, "Geometry", "auto_create_base_objects" );

  int operationsGroup = addPreference( tr( "PREF_GROUP_OPERATIONS" ), tabId );
  setPreferenceProperty( operationsGroup, "columns", 2 );

  addPreference( tr( "GEOM_PREVIEW" ), operationsGroup,
                 LightApp_Preferences::Bool, "Geometry", "geom_preview" );

  addPreference( tr( "PREF_HIDE_INPUT_OBJECT" ), operationsGroup,
                 LightApp_Preferences::Bool, "Geometry", "hide_input_object" );

  int DependencyViewId = addPreference( tr( "PREF_TAB_DEPENDENCY_VIEW" ) );

  int treeGeneralGroup = addPreference( tr( "PREF_GROUP_GENERAL" ), DependencyViewId );

  int hierarchy_type = addPreference( tr( "PREF_HIERARCHY_TYPE" ), treeGeneralGroup,
                      LightApp_Preferences::Selector, "Geometry", "dependency_tree_hierarchy_type" );

  QStringList aHierarchyTypeList;
  aHierarchyTypeList.append( tr("MEN_BOTH_ASCENDANTS_DESCENDANTS") );
  aHierarchyTypeList.append( tr("MEN_ONLY_ASCENDANTS") );
  aHierarchyTypeList.append( tr("MEN_ONLY_DESCENDANTS") );

  QList<QVariant> aHierarchyTypeIndexesList;
  aHierarchyTypeIndexesList.append(0);
  aHierarchyTypeIndexesList.append(1);
  aHierarchyTypeIndexesList.append(2);

  setPreferenceProperty( hierarchy_type, "strings", aHierarchyTypeList );
  setPreferenceProperty( hierarchy_type, "indexes", aHierarchyTypeIndexesList );

  addPreference( tr( "GEOM_MOVE_POSSIBILITY" ), treeGeneralGroup,
                 LightApp_Preferences::Bool, "Geometry", "dependency_tree_move_nodes" );

  int treeColorGroup = addPreference( tr( "PREF_GROUP_DEPENDENCY_VIEW_COLOR" ), DependencyViewId );

  addPreference( tr( "PREF_DEPENDENCY_VIEW_BACKGROUND_COLOR"), treeColorGroup,
                 LightApp_Preferences::Color, "Geometry", "dependency_tree_background_color" );

  addPreference( tr( "PREF_DEPENDENCY_VIEW_NODE_COLOR"), treeColorGroup,
                 LightApp_Preferences::Color, "Geometry", "dependency_tree_node_color" );
  addPreference( tr( "PREF_DEPENDENCY_VIEW_MAIN_NODE_COLOR"), treeColorGroup,
                 LightApp_Preferences::Color, "Geometry", "dependency_tree_main_node_color" );
  addPreference( tr( "PREF_DEPENDENCY_VIEW_UNPUBLISH_NODE_COLOR"), treeColorGroup,
                 LightApp_Preferences::Color, "Geometry", "dependency_tree_unpublish_node_color" );
  addPreference( tr( "PREF_DEPENDENCY_VIEW_SELECT_NODE_COLOR"), treeColorGroup,
                 LightApp_Preferences::Color, "Geometry", "dependency_tree_select_node_color" );

  addPreference( tr( "PREF_DEPENDENCY_VIEW_ARROW_COLOR"), treeColorGroup,
                 LightApp_Preferences::Color, "Geometry", "dependency_tree_arrow_color" );
  addPreference( tr( "PREF_DEPENDENCY_VIEW_HIGHLIGHT_ARROW_COLOR"), treeColorGroup,
                 LightApp_Preferences::Color, "Geometry", "dependency_tree_highlight_arrow_color" );
  addPreference( tr( "PREF_DEPENDENCY_VIEW_SELECT_ARROW_COLOR"), treeColorGroup,
                 LightApp_Preferences::Color, "Geometry", "dependency_tree_select_arrow_color" );




}

void GeometryGUI::preferencesChanged( const QString& section, const QString& param )
{
  if (section == "Geometry") {
    SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
    if (param == QString("SettingsGeomStep")) {
      double spin_step = aResourceMgr->doubleValue(section, param, 100.);
      EmitSignalDefaultStepValueChanged(spin_step);
    }
    else if (param == QString("toplevel_color")) {
      QColor c = aResourceMgr->colorValue( "Geometry", "toplevel_color", QColor( 170, 85, 0 ) );
      GEOM_AISShape::setTopLevelColor(SalomeApp_Tools::color(c));
    }
    else if (param == QString("toplevel_dm")) {
      GEOM_AISShape::setTopLevelDisplayMode((GEOM_AISShape::TopLevelDispMode)aResourceMgr->integerValue("Geometry", "toplevel_dm", 0));
    }
    else if (param == QString("scalar_bar_x_position") ||
             param == QString("scalar_bar_y_position") ||
             param == QString("scalar_bar_width") ||
             param == QString("scalar_bar_height") ||
             param == QString("scalar_bar_text_height") ||
             param == QString("scalar_bar_nb_intervals")) {
      if( SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( getApp()->activeStudy() ) )
      {
        GEOM_Displayer aDisplayer( aStudy );
        bool anIsRedisplayFieldSteps = param == QString("scalar_bar_nb_intervals");
        aDisplayer.UpdateColorScale( anIsRedisplayFieldSteps, true );
      }
    }
    else if ( param == QString("dimensions_color")        ||
              param == QString("dimensions_line_width")   ||
              param == QString("dimensions_font")         ||
              param == QString("dimensions_arrow_length") ||
              param == QString("dimensions_show_units")   ||
              param == QString("dimensions_length_units") ||
              param == QString("dimensions_angle_units")  ||
              param == QString("dimensions_use_text3d")  ||
              param == QString("label_color") )
    {
      SalomeApp_Application* anApp = getApp();
      if ( !anApp )
      {
        return;
      }

      SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
      if ( !aStudy )
      {
        return;
      }

      GEOM_Displayer aDisplayer( aStudy );

      ViewManagerList aVMs;
      anApp->viewManagers( OCCViewer_Viewer::Type(), aVMs );
      ViewManagerList::Iterator anIt = aVMs.begin();
      for ( ; anIt != aVMs.end(); ++anIt )
      {
        SOCC_Viewer* aViewer = dynamic_cast<SOCC_Viewer*>( (*anIt)->getViewModel() );
        if ( !aViewer )
        {
          continue;
        }

        SALOME_ListIO aVisible;
        aViewer->GetVisible( aVisible );
        aDisplayer.Redisplay( aVisible, false, aViewer );
      }
      if ( param == QString( "label_color" ) ) {
        ViewManagerList aVMsVTK;
        anApp->viewManagers( SVTK_Viewer::Type(), aVMsVTK );
        ViewManagerList::Iterator anIt = aVMsVTK.begin();
        for ( ; anIt != aVMsVTK.end(); ++anIt )
        {
            SVTK_Viewer* aViewer = dynamic_cast<SVTK_Viewer*>( (*anIt)->getViewModel() );
            if ( !aViewer )
            {
              continue;
            }
            SALOME_ListIO aVisible;
            aViewer->GetVisible( aVisible );
            aDisplayer.Redisplay( aVisible, false, aViewer );
          }
      }
      aDisplayer.UpdateViewer();
    }
    else if ( param.startsWith( "dependency_tree") )
      emit SignalDependencyTreeParamChanged( section, param );
  }
}

LightApp_Displayer* GeometryGUI::displayer()
{
  if ( !myDisplayer )
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
 * Store visual parameters in AttributeParameter attribute(s)
 */
void GeometryGUI::storeVisualParameters (int savePoint)
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(application()->activeStudy());
  if ( !appStudy || !appStudy->studyDS() )
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

  QSet<QString> anEntriesToStoreShared;
  QList<SUIT_ViewManager*> lst;
  QList<SUIT_ViewManager*>::Iterator it;

  // main cycle to store parameters of displayed objects
  lst.clear();
  getApp()->viewManagers(lst);
  for (it = lst.begin(); it != lst.end(); it++) {
    SUIT_ViewManager* vman = *it;
    QString vType = vman->getType();
    int aMgrId = vman->getGlobalId();
    // saving VTK actors properties
    QVector<SUIT_ViewWindow*> views = vman->getViews();
    for (int i = 0, iEnd = vman->getViewsCount(); i < iEnd; i++) {
      const ObjMap& anObjects = appStudy->getObjectProperties(aMgrId);
      ObjMap::ConstIterator o_it = anObjects.begin();
      for (; o_it != anObjects.end(); o_it++) {
        const PropMap& aProps = o_it.value();

        //Check that object exists in the study
        _PTR(SObject) obj( studyDS->FindObjectID( o_it.key().toLatin1().data() ) );
        if ( !obj || !(aProps.count() > 0))
          continue;
        // entry is "encoded" = it does NOT contain component adress, since it is a
        // subject to change on next component loading

        std::string entry = ip->encodeEntry(o_it.key().toLatin1().data(), componentName);

        _PTR(GenericAttribute) anAttr;
        if (!obj->FindAttribute(anAttr, "AttributeIOR"))
          continue;

        // remember entry of object to store shared GEOM properties
        // (e.g. dimension properties).
        if ( vType == OCCViewer_Viewer::Type() )
        {
          anEntriesToStoreShared.insert( o_it.key() );
        }

        QString param, occParam = vType;
        occParam += GEOM::sectionSeparator();
        occParam += QString::number(aMgrId);
        occParam += GEOM::sectionSeparator();

        if (aProps.contains(GEOM::propertyName( GEOM::Visibility ))) {
          param = occParam + GEOM::propertyName( GEOM::Visibility );
          ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::Visibility )).toBool() ? "On" : "Off");
        }

        if (aProps.contains(GEOM::propertyName( GEOM::DisplayMode ))) {
          param = occParam + GEOM::propertyName( GEOM::DisplayMode );
          ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::DisplayMode )).toString().toStdString());
        }

        if (aProps.contains(GEOM::propertyName( GEOM::Color ))) {
          QColor c = aProps.value(GEOM::propertyName( GEOM::Color )).value<QColor>();
          QStringList val;
          val << QString::number(c.redF());
          val << QString::number(c.greenF());
          val << QString::number(c.blueF());
          param = occParam + GEOM::propertyName( GEOM::Color );
          ip->setParameter(entry, param.toStdString(), val.join( GEOM::subSectionSeparator()).toStdString());
        }
        
        if (aProps.contains(GEOM::propertyName( GEOM::Texture ))) {
          param = occParam + GEOM::propertyName( GEOM::Texture );
          ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::Texture )).toString().toStdString());
        }

        if (vType == SVTK_Viewer::Type()) {
          if (aProps.contains(GEOM::propertyName( GEOM::Opacity ))) {
            param = occParam + GEOM::propertyName( GEOM::Opacity );
            ip->setParameter(entry, param.toStdString(), QString::number(1. - aProps.value(GEOM::propertyName( GEOM::Transparency )).toDouble()).toStdString());
          }
        } else if (vType == SOCC_Viewer::Type()) {
          if (aProps.contains(GEOM::propertyName( GEOM::Transparency ))) {
            param = occParam + GEOM::propertyName( GEOM::Transparency );
            ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::Transparency )).toString().toStdString());
          }

          if (aProps.contains(GEOM::propertyName( GEOM::TopLevel ))) {
            param = occParam + GEOM::propertyName( GEOM::TopLevel );
            ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::TopLevel )).toString().toStdString());
          }
        }

        if (aProps.contains(GEOM::propertyName( GEOM::NbIsos ))) {
          param = occParam + GEOM::propertyName( GEOM::NbIsos );
          ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::NbIsos )).toString().toStdString());
        }

        if (aProps.contains(GEOM::propertyName( GEOM::EdgesDirection ))) {
          param = occParam + GEOM::propertyName( GEOM::EdgesDirection );
          ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::EdgesDirection )).toString().toStdString());
        }

        if (aProps.contains(GEOM::propertyName( GEOM::Vertices ))) {
          param = occParam + GEOM::propertyName( GEOM::Vertices );
          ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::Vertices )).toString().toStdString());
        }

        if (aProps.contains(GEOM::propertyName( GEOM::ShowName ))) {
          param = occParam + GEOM::propertyName( GEOM::ShowName );
          ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::ShowName )).toString().toStdString());
        }

        if (aProps.contains(GEOM::propertyName( GEOM::Deflection ))) {
          param = occParam + GEOM::propertyName( GEOM::Deflection );
          ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::Deflection )).toString().toStdString());
        }

        //Marker type of the vertex - ONLY for the "Vertex" and "Compound of the Vertex"
        if (aProps.contains(GEOM::propertyName( GEOM::PointMarker ))) {
          param = occParam + GEOM::propertyName( GEOM::PointMarker );
          ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::PointMarker )).toString().toStdString());
        }

        if (aProps.contains(GEOM::propertyName( GEOM::Material ))) {
          param = occParam + GEOM::propertyName( GEOM::Material );
          ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::Material )).toString().toStdString());
        }

        if (aProps.contains(GEOM::propertyName( GEOM::LineWidth ))) {
             param = occParam + GEOM::propertyName( GEOM::LineWidth );
           ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::LineWidth )).toString().toStdString());
        }

        if (aProps.contains(GEOM::propertyName( GEOM::IsosWidth ))) {
          param = occParam + GEOM::propertyName( GEOM::IsosWidth );
          ip->setParameter(entry, param.toStdString(), aProps.value(GEOM::propertyName( GEOM::IsosWidth )).toString().toStdString());
        }
      } // object iterator
    } // for (views)
  } // for (viewManagers)

  // store dimension attributes of objects:
  // since the displayed object always persists in property map, we remember the object entries
  // on the passes when we store viewer related properties - to avoid extra iterations on GEOM component tree.
  QString aDimensionParam = OCCViewer_Viewer::Type() + GEOM::sectionSeparator() + GEOM::propertyName( GEOM::Dimensions );
  QSet<QString>::ConstIterator aEntryIt = anEntriesToStoreShared.constBegin();
  for ( ; aEntryIt != anEntriesToStoreShared.constEnd(); ++aEntryIt )
  {
    std::string aStudyEntry = (*aEntryIt).toLatin1().data();
    std::string aStoreEntry = ip->encodeEntry( aStudyEntry, componentName);

    GEOMGUI_DimensionProperty aDimensions( appStudy, aStudyEntry );

    if ( aDimensions.GetNumber() == 0 )
    {
      continue;
    }

    ip->setParameter( aStoreEntry, aDimensionParam.toStdString(), ((QString)aDimensions).toLatin1().data() );
  }
}

/*!
 * \brief Restore visual parameters
 *
 * This method is called after the study document is opened.
 * Restore visual parameters from AttributeParameter attribute(s)
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

    QString viewerTypStr;
    QString viewIndexStr;
    int viewIndex;
    QVector<PropMap> aListOfMap;

    for (; namesIt != paramNames.end(); ++namesIt, ++valuesIt)
    {
      // visual parameters are stored in strings as follows: 
      //   1) ViewerType_ViewIndex_ParamName
      //   2) ViewerType_ParamName (shared for GEOM module)
      // '_' is used as separator and should not be used in viewer type or parameter names.
      QStringList lst = QString((*namesIt).c_str()).split( GEOM::sectionSeparator(), QString::SkipEmptyParts);

      bool isShared = lst.size() == 2;
      bool isViewer = lst.size() == 3;
      if ( !isShared && !isViewer )
      {
        continue;
      }

      // shared visual parameters
      if ( isShared )
      {
        QString aParamNameStr( lst[1] );
        QString aValuesStr( (*valuesIt).c_str() );

        // shared dimension properties are stored as attribute
        if ( aParamNameStr == GEOM::propertyName( GEOM::Dimensions ) )
        {
          GEOMGUI_DimensionProperty aDimensionProp( aValuesStr );
          aDimensionProp.SaveToAttribute( appStudy, entry.toLatin1().data() );
        }

        continue;
      }

      // per view visual parameters
      viewerTypStr = lst[0];
      viewIndexStr = lst[1];
      QString paramNameStr = lst[2];

      bool ok;
      viewIndex = viewIndexStr.toUInt(&ok);
      if (!ok) // bad conversion of view index to integer
        continue;

      if ((viewIndex + 1) > aListOfMap.count()) {
        aListOfMap.resize(viewIndex + 1);
      }

      QString val((*valuesIt).c_str());
      if (paramNameStr == GEOM::propertyName( GEOM::Visibility )) {
        aListOfMap[viewIndex].insert(GEOM::propertyName( GEOM::Visibility ), val == "On");
      } else if (paramNameStr == GEOM::propertyName( GEOM::Opacity )) {
        aListOfMap[viewIndex].insert(GEOM::propertyName( GEOM::Transparency ), 1. - val.toDouble());
      } else if (paramNameStr == GEOM::propertyName( GEOM::Transparency )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::Transparency ), val.toDouble() );
      } else if (paramNameStr == GEOM::propertyName( GEOM::TopLevel )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::TopLevel ), val == "true" || val == "1");
      } else if (paramNameStr == GEOM::propertyName( GEOM::DisplayMode )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::DisplayMode ), val.toInt());
      } else if (paramNameStr == GEOM::propertyName( GEOM::NbIsos )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::NbIsos ), val);
      } else if (paramNameStr == GEOM::propertyName( GEOM::Color )) {
        QStringList rgb = val.split(GEOM::subSectionSeparator());
        if (rgb.count() == 3) {
          QColor c = QColor::fromRgbF(rgb[0].toDouble(), rgb[1].toDouble(), rgb[2].toDouble());
          aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::Color ), c);
        }
      } else if (paramNameStr == GEOM::propertyName( GEOM::Texture )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::Texture ), val );
      } else if (paramNameStr == GEOM::propertyName( GEOM::EdgesDirection )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::EdgesDirection ), val == "true" || val == "1");
      } else if (paramNameStr == GEOM::propertyName( GEOM::Vertices )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::Vertices ), val == "true" || val == "1");
      } else if (paramNameStr == GEOM::propertyName( GEOM::ShowName )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::ShowName ), val == "true" || val == "1");
      } else if (paramNameStr == GEOM::propertyName( GEOM::Deflection )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::Deflection ), val.toDouble());
      } else if (paramNameStr == GEOM::propertyName( GEOM::PointMarker )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::PointMarker ), val);
      } else if (paramNameStr == GEOM::propertyName( GEOM::Material )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::Material ), val);
      } else if (paramNameStr == GEOM::propertyName( GEOM::LineWidth )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::LineWidth ), val.toInt());
      } else if (paramNameStr == GEOM::propertyName( GEOM::IsosWidth )) {
        aListOfMap[viewIndex].insert( GEOM::propertyName( GEOM::IsosWidth ), val.toInt());
      }
    } // for names/parameters iterator

    QList<SUIT_ViewManager*> lst = getApp()->viewManagers();

    for (int index = 0; index < aListOfMap.count(); index++) {
      appStudy->setObjectProperties(index, entry, aListOfMap[index]);

      //Get Visibility property of the current PropMap
      if (aListOfMap[index].value(GEOM::propertyName( GEOM::Visibility )) == 1) {
        SUIT_ViewManager* vman = lst.at(index);
        SUIT_ViewModel* vmodel = vman->getViewModel();
        displayer()->Display(entry, true, dynamic_cast<SALOME_View*>(vmodel));
      }
    }
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

// Compute current name mode of the viewer
void UpdateNameMode( SalomeApp_Application* app )
{
  bool isMode = false;
  SalomeApp_Study* aStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  SUIT_ViewWindow* viewWindow = app->desktop()->activeWindow();
  GEOM_Displayer displayer( aStudy );
  int aMgrId = viewWindow->getViewManager()->getGlobalId();

  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  SALOME_ListIO anIOlst;
  window->GetVisible( anIOlst );

  for ( SALOME_ListIteratorOfListIO It( anIOlst ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    QVariant v = aStudy->getObjectProperty( aMgrId, io->getEntry(), GEOM::propertyName( GEOM::ShowName ), QVariant() );
    bool isIONameMode =  v.isValid() ? v.toBool() : false;
    if( isIONameMode )
      isMode = true;
  }
  viewWindow->setProperty( "NameMode", isMode );
}

void GeometryGUI::onViewAboutToShow()
{
  SUIT_ViewWindow* window = application()->desktop()->activeWindow();
  QAction* a = action( GEOMOp::OpSwitchVectors );
  QAction* aVerticesAction = action( GEOMOp::OpSwitchVertices );
  QAction* aNameAction = action( GEOMOp::OpSwitchName );
  if ( window ) {
    a->setEnabled(true);
    bool vmode = window->property("VectorsMode").toBool();
    a->setText ( vmode == 1 ? tr( "MEN_VECTOR_MODE_OFF" ) : tr("MEN_VECTOR_MODE_ON") );
    aVerticesAction->setEnabled(true);
    vmode = window->property("VerticesMode").toBool();
    aVerticesAction->setText ( vmode == 1 ? tr( "MEN_VERTICES_MODE_OFF" ) : tr("MEN_VERTICES_MODE_ON") );
    UpdateNameMode( getApp() );
    aNameAction->setEnabled(true);
    vmode = window->property("NameMode").toBool();
    aNameAction->setText ( vmode == 1 ? tr( "MEN_NAME_MODE_OFF" ) : tr("MEN_NAME_MODE_ON") );
  } else {
    a->setText ( tr("MEN_VECTOR_MODE_ON") );
    a->setEnabled(false);
    aVerticesAction->setText ( tr("MEN_VERTICES_MODE_ON") );
    aVerticesAction->setEnabled(false);
    aNameAction->setText ( tr("MEN_NAME_MODE_ON") );
    aNameAction->setEnabled(false);
  }
}

/*!
  \brief Return action by id
  \param id identifier of the action
  \return action
*/
QAction* GeometryGUI::getAction(const int id) {
  return action(id);
}

/*!
  \brief GEOM module message handler

  This method can be re-implemented in the subclasses.
  This is a GEOM module message handler.

  \param msg the message received.
*/
void GeometryGUI::message(const QString& msg)
{
  // dispatch message
  QStringList data = msg.split("/");
  const int nbStrings = data.count();

  if (nbStrings > 0) {
    if (data[0] == "modified") {
      // get mesh entry
      QString anIOR = nbStrings > 1 ? data[1] : QString();

      if ( anIOR.isEmpty() ) {
        return;
      }

      // Get the geom object.
      GEOM::GEOM_Object_ptr anObj = GeometryGUI::GetObjectFromIOR (anIOR);

      // Clear the shape buffer
      GeometryGUI::ClearShapeBuffer (anObj);
    }
  }
}

/*!
  \brief Clears the shape buffer.

  This is a static method. It clears the shape buffer.

  \param theObj the object
*/
void GeometryGUI::ClearShapeBuffer( GEOM::GEOM_Object_ptr theObj )
{
  if ( CORBA::is_nil( theObj ) )
    return;

  CORBA::String_var IOR = SalomeApp_Application::orb()->object_to_string( theObj );
  TCollection_AsciiString asciiIOR( (char *)IOR.in() );
  GEOM_Client::get_client().RemoveShapeFromBuffer( asciiIOR );

  SALOMEDSClient_StudyManager *aManager = SalomeApp_Application::studyMgr();

  if (!aManager)
    return;

  _PTR(Study) aStudy = aManager->GetStudyByID(theObj->GetStudyID());

  if ( !aStudy )
    return;

  _PTR(SObject) aSObj ( aStudy->FindObjectIOR( std::string( IOR ) ) );
  if ( !aSObj )
    return;

  _PTR(ChildIterator) anIt ( aStudy->NewChildIterator( aSObj ) );
  for ( anIt->InitEx( true ); anIt->More(); anIt->Next() ) {
    _PTR(GenericAttribute) anAttr;
    if ( anIt->Value()->FindAttribute(anAttr, "AttributeIOR") ) {
      _PTR(AttributeIOR) anIOR ( anAttr );
      TCollection_AsciiString asciiIOR( (char*)anIOR->Value().c_str() );
      GEOM_Client::get_client().RemoveShapeFromBuffer( asciiIOR );
    }
  }
}

/*!
  \brief Returns the object from IOR.

  This is a static method. It returns the object from its IOR.

  \param IOR object IOR
  \return GEOM object.
*/
GEOM::GEOM_Object_ptr GeometryGUI::GetObjectFromIOR( const QString& IOR )
{
  GEOM::GEOM_Object_var geomObj;
  if ( !IOR.isEmpty() ) {
    CORBA::Object_var corbaObj = SalomeApp_Application::orb()->string_to_object
      ( IOR.toLatin1().constData() );
    if ( !CORBA::is_nil( corbaObj ) )
      geomObj = GEOM::GEOM_Object::_narrow( corbaObj );
  }
  return geomObj._retn();
}

/*!
  \brief Returns IOR of the object.

  This is a static method. It returns the object's IOR.

  \param object the GEOM object.
  \return object's IOR.
*/
QString GeometryGUI::GetIORFromObject( GEOM::GEOM_Object_ptr object )
{
  QString IOR;
  if ( !CORBA::is_nil( object ) ) {
    CORBA::String_var anIOR =
      SalomeApp_Application::orb()->object_to_string( object );
    IOR = anIOR.in();
  }
  return IOR;
}

/*!
  \brief Check if this object is can't be renamed in place

  This method can be re-implemented in the subclasses.
  Return true in case if object isn't reference or component (module root).

  \param entry column id
  \return \c true if the item can be renamed by the user in place (e.g. in the Object browser)
*/
bool GeometryGUI::renameAllowed( const QString& entry) const {

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  SalomeApp_Study* appStudy = app ? dynamic_cast<SalomeApp_Study*>( app->activeStudy() ) : 0;
  SalomeApp_DataObject* obj = appStudy ? dynamic_cast<SalomeApp_DataObject*>(appStudy->findObjectByEntry(entry)) : 0;

  return (app && appStudy && obj && !appStudy->isComponent(entry) && !obj->isReference());
}

/*!
  Rename object by entry.
  \param entry entry of the object
  \param name new name of the object
  \brief Return \c true if rename operation finished successfully, \c false otherwise.
*/
bool GeometryGUI::renameObject( const QString& entry, const QString& name)
{
  bool result = false;

  SalomeApp_Application* app =
    dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication());
  SalomeApp_Study* appStudy = app ? dynamic_cast<SalomeApp_Study*>( app->activeStudy() ) : 0;

  if (!appStudy)
    return result;

  _PTR(Study) aStudy = appStudy->studyDS();

  if (!aStudy)
    return result;

  bool aLocked = (_PTR(AttributeStudyProperties)(appStudy->studyDS()->GetProperties()))->IsLocked();
  if ( aLocked ) {
    SUIT_MessageBox::warning ( app->desktop(), QObject::tr("WRN_WARNING"), QObject::tr("WRN_STUDY_LOCKED") );
    return result;
  }

  _PTR(SObject) obj ( aStudy->FindObjectID(qPrintable(entry)) );
  _PTR(GenericAttribute) anAttr;
  if ( obj ) {
    if ( obj->FindAttribute(anAttr, "AttributeName") ) {
      _PTR(AttributeName) aName (anAttr);

      aName->SetValue( name.toLatin1().data() ); // rename the SObject
      GEOM::GEOM_Object_var anObj = GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(obj));
      if (!CORBA::is_nil(anObj)) {
        anObj->SetName( name.toLatin1().data() );  // Rename the corresponding GEOM_Object
        emit SignalDependencyTreeRenameObject( anObj->GetEntry() );
      }
      result = true;
    }
  }
  return result;
}

void GeometryGUI::updateMaterials()
{
  LightApp_Preferences* pref = preferences();
  if ( pref ) {
    QStringList materials = Material_ResourceMgr::resourceMgr()->materials();
    QString currentMaterial = SUIT_Session::session()->resourceMgr()->stringValue( "Geometry", "material" );
    if ( !materials.contains( currentMaterial ) )
      // user material set as default in the preferences, might be removed
      SUIT_Session::session()->resourceMgr()->setValue( "Geometry", "material", QString( "Plastic" ) );

    QtxPreferenceItem* prefItem = pref->rootItem()->findItem( tr( "PREF_MATERIAL" ), true );
    if ( prefItem ) {
      setPreferenceProperty( prefItem->id(),
                             "strings", materials );
      prefItem->retrieve();
    }
  }
}

/*!
  \brief Check if the module allows "drag" operation of its objects.

  Overloaded from LightApp_Module class.
  
  This function is a part of the general drag-n-drop mechanism.
  The goal of this function is to check data object passed as a parameter
  and decide if it can be dragged or no.

  \param what data object being tested for drag operation
  \return \c true if module allows dragging of the specified object
  \sa isDropAccepted(), dropObjects()
*/
bool GeometryGUI::isDraggable( const SUIT_DataObject* what ) const
{
  // we allow dragging object under root and object from folder
  int aLevel = what->level();
  bool anObjectInFolder = false;
  if ( aLevel > 2 ) {
    const SalomeApp_DataObject* dataObj = dynamic_cast<const SalomeApp_DataObject*>( what );
    if ( dataObj ) {
      _PTR(SObject) aSO = dataObj->object();
      if ( aSO ) {
        _PTR(GenericAttribute) anAttr;
        _PTR(SObject) aFatherSO = aSO->GetStudy()->GetUseCaseBuilder()->GetFather( aSO );
        if ( aFatherSO && aFatherSO->FindAttribute(anAttr, "AttributeLocalID") ) {
          _PTR(AttributeLocalID) aLocalID( anAttr );
          anObjectInFolder = aLocalID->Value() == 999;
        }
      }
    }
  }
  return aLevel == 2 || anObjectInFolder;
}

/*!
  \brief Check if the module allows "drop" operation on the given object.

  Overloaded from LightApp_Module class.

  This function is a part of the general drag-n-drop mechanism.
  The goal of this function is to check data object passed as a parameter
  and decide if it can be used as a target for the "drop" operation.
  The processing of the drop operation itself is done in the dropObjects() function.

  \param where target data object
  \return \c true if module supports dropping on the \a where data object
  \sa isDraggable(), dropObjects()
*/
bool GeometryGUI::isDropAccepted( const SUIT_DataObject* where ) const
{
  // we allow dropping into folder and top-level GEOM object
  int aLevel = where->level();
  bool isFolder = false;
  if ( aLevel > 1 ) {
    const SalomeApp_DataObject* dataObj = dynamic_cast<const SalomeApp_DataObject*>( where );
    if ( dataObj ) {
      _PTR(SObject) aSO = dataObj->object();
      if ( aSO ) {
        _PTR(GenericAttribute) anAttr;
        if ( aSO->FindAttribute(anAttr, "AttributeLocalID") ) {
          _PTR(AttributeLocalID) aLocalID( anAttr );
          isFolder = aLocalID->Value() == 999;
        }
      }
    }
  }
  return aLevel == 1 || isFolder;
}

/*!
  \brief Complete drag-n-drop operation.
  
  Overloaded from LightApp_Module class.

  This function is a part of the general drag-n-drop mechanism.
  Its goal is to handle dropping of the objects being dragged according
  to the chosen operation (move). The dropping is performed in the
  context of the parent data object \a where and the \a row (position in the 
  children index) at which the data should be dropped. If \a row is equal to -1,
  this means that objects are added to the end of the children list.

  \param what objects being dropped
  \param where target data object
  \param row child index at which the drop operation is performed
  \param action drag-n-drop operation (Qt::DropAction) - move

  \sa isDraggable(), isDropAccepted()
*/
void GeometryGUI::dropObjects( const DataObjectList& what, SUIT_DataObject* where,
                               const int row, Qt::DropAction action )
{
  if (action != Qt::CopyAction && action != Qt::MoveAction)
    return; // unsupported action

  // get parent object
  SalomeApp_DataObject* dataObj = dynamic_cast<SalomeApp_DataObject*>( where );
  if ( !dataObj ) return; // wrong parent
  _PTR(SObject) parentObj = dataObj->object();

  // Find the current Study and StudyBuilder
  _PTR(Study) aStudy = parentObj->GetStudy();
  _PTR(UseCaseBuilder) aUseCaseBuilder = aStudy->GetUseCaseBuilder();
  // collect all parents of the target node
  QStringList parentIDs;
  _PTR(SObject) parent = parentObj;
  while( !parent->IsNull() ) {
    parentIDs << parent->GetID().c_str();
    parent = aUseCaseBuilder->GetFather(parent);
  }

  // collect objects being dropped
  GEOM::object_list_var objects = new GEOM::object_list();
  objects->length( what.count() );
  int count = 0;
  for ( int i = 0; i < what.count(); i++ ) {
    dataObj = dynamic_cast<SalomeApp_DataObject*>( what[i] );
    if ( !dataObj ) continue;  // skip wrong objects
    _PTR(SObject) sobj = dataObj->object();
    // check that dropped object is not a parent of target object
    if ( parentIDs.contains( sobj->GetID().c_str() ) ) {
      return; // it's not allowed to move node into it's child 
    }
    objects[i] = _CAST(SObject, sobj)->GetSObject();
    count++;
  }
  objects->length( count );

  // call engine function
  GetGeomGen()->Move( objects.in(),                              // what
                      _CAST(SObject, parentObj)->GetSObject(),   // where
                      row );                                     // row

  // update Object browser
  getApp()->updateObjectBrowser( false );
}

void GeometryGUI::emitDimensionsUpdated( QString entry )
{
  emit DimensionsUpdated( entry );
}
