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

// SALOME Includes
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"

#include "QAD_Desktop.h"
#include "QAD_Application.h"
#include "QAD_RightFrame.h"
#include "QAD_Config.h"
#include "QAD_Tools.h"
#include "QAD_MessageBox.h"
#include "QAD_Resource.h"
#include "SALOMEGUI_Desktop.h"

#include "OCCViewer_Viewer3d.h"
#include "OCCViewer_ViewFrame.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_Prs.h"

#include "VTKViewer_ViewFrame.h"
#include "VTKViewer_RenderWindowInteractor.h"
#include "VTKViewer_InteractorStyleSALOME.h"
#include "VTKViewer_Prs.h"
#include "SALOME_Actor.h"

#include "SALOME_Selection.h"
#include "SALOME_ListIteratorOfListIO.hxx"
#include "GEOM_AISShape.hxx"
#include "GEOM_Displayer.h"

#include "GEOMImpl_Types.hxx"

// External includes
#include <qfileinfo.h>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <OSD_SharedLibrary.hxx>

#include "utilities.h"

#include <vtkCamera.h>
#include <vtkRenderer.h>

using namespace std;

extern "C" {
  Standard_EXPORT SALOMEGUI* GetComponentGUI() {
    return GeometryGUI::GetGeomGUI();
  }
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
    if ( !act ) {
      p->fillRect( x, y+1, w, h-2, cg.mid() );
      p->drawRect( x, y+1, w, h-2 );
    }
    else {
      p->fillRect( x, y+1, w, h-2, cg.midlight() );
      QPen oldPen = p->pen();
      p->setPen( cg.mid() );
      p->drawRect( x, y+1, w, h-2 );
      p->setPen( oldPen );
    }
    p->setFont( myFont );
    p->drawText( x, y, w, h, AlignHCenter | AlignVCenter | ShowPrefix | DontClip, myString );
  }

  QSize sizeHint()
  {
    return QFontMetrics( myFont ).size( AlignHCenter | AlignVCenter | ShowPrefix | DontClip, myString );
  }

private:
  QString myString;
  QFont   myFont;

};

//=======================================================================
// function : GeometryGUI::GetGeomGUI()
// purpose  : Gets the only object of GeometryGUI [ static ]
//=======================================================================
GeometryGUI* GeometryGUI::GetGeomGUI()
{
  static GeometryGUI myContext;
  return &myContext;
}

//=======================================================================
// function : GetORB
// purpose  : Returns a reference to ORB [ static ]
//=======================================================================
CORBA::ORB_var GeometryGUI::GetORB()
{
  ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
  return init.orb();
}

//=======================================================================
// function : GeometryGUI::GeometryGUI()
// purpose  : Constructor
//=======================================================================
GeometryGUI::GeometryGUI() :
  SALOMEGUI()
{
  QAD_Desktop* desktop = QAD_Application::getDesktop();
  Engines::Component_var comp = desktop->getEngine( "FactoryServer", "GEOM" );

  myComponentGeom   = GEOM::GEOM_Gen::_narrow( comp );
  myState           = -1;
  myActiveDialogBox = 0;
  myFatherior       = "";

  gp_Pnt origin = gp_Pnt(0., 0., 0.);
  gp_Dir direction = gp_Dir(0., 0., 1.);
  myWorkingPlane = gp_Ax3(origin, direction);
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
typedef GEOMGUI* (*LibraryGUI)();
GEOMGUI* GeometryGUI::getLibrary( const QString& libraryName )
{
  if ( !myGUIMap.contains( libraryName ) ) {
    // try to load library if it is not loaded yet
    QCString libs;
    if( ( libs = getenv( "LD_LIBRARY_PATH" ) ) ) {
      QStringList dirList = QStringList::split( ":", libs, false ); // skip empty entries
      for( int i = dirList.count()-1; i >= 0; i-- ) {
	QString dir = dirList[ i ];
	QFileInfo fi( QAD_Tools::addSlash( dirList[ i ] ) + libraryName );
	if( fi.exists() ) {
	  OSD_SharedLibrary aSharedLibrary( (char*)fi.fileName().latin1() );
	  bool res = aSharedLibrary.DlOpen( OSD_RTLD_LAZY );
	  if( !res ) {
	    MESSAGE( "Can't open library : " << aSharedLibrary.DlError() );
	    continue; // continue search further
	  }
	  OSD_Function osdF = aSharedLibrary.DlSymb( "GetLibGUI" );
	  if ( osdF != NULL ) {
	    LibraryGUI func = (GEOMGUI* (*) ())osdF;
	    GEOMGUI* libGUI = (*func)();
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

  if( QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    OCCViewer_ViewPort* vp = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewPort();
    Handle(V3d_View) view3d = ((OCCViewer_ViewPort3d*)vp)->getView();

    view3d->SetProj(DZ.X(), DZ.Y(), DZ.Z());
    view3d->SetUp(DY.X(), DY.Y(), DY.Z());

    ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->onViewFitAll();
  }
  else if( QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
    vtkRenderer* myRenderer = ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRenderer();

    vtkCamera* camera = myRenderer->GetActiveCamera();
    camera->SetPosition(DZ.X(), DZ.Y(), DZ.Z());
    camera->SetViewUp(DY.X(), DY.Y(), DY.Z());
    camera->SetFocalPoint(0,0,0);

    ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->onViewFitAll();
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
// purpose  : manage all events on GUI [static]
//=======================================================================
bool GeometryGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  GeometryGUI* geomGUI = GeometryGUI::GetGeomGUI();

  // get main menu
  QMenuBar* Mb = parent->getMainMenuBar();
  // check if current viewframe is of OCC type
  bool ViewOCC = parent->getActiveStudy()->getActiveStudyFrame()->getTypeView()  == VIEW_OCC;
  // if current viewframe is not of OCC and not of VTK type - return immediately
  if( !ViewOCC && parent->getActiveStudy()->getActiveStudyFrame()->getTypeView() != VIEW_VTK)
    return false;

  // disable non-OCC viewframe menu commands
  //Mb->setItemEnabled( 404, ViewOCC ); // SKETCHER
  Mb->setItemEnabled( 603, ViewOCC ); // SuppressFace
  Mb->setItemEnabled( 604, ViewOCC ); // SuppressHole
  Mb->setItemEnabled( 606, ViewOCC ); // CloseContour
  Mb->setItemEnabled( 607, ViewOCC ); // RemoveInternalWires
  Mb->setItemEnabled( 608, ViewOCC ); // AddPointOnEdge
  //Mb->setItemEnabled( 609, ViewOCC ); // Free boundaries
  Mb->setItemEnabled( 413, ViewOCC ); // Isos Settings

  Mb->setItemEnabled( 800, ViewOCC ); // Create Group
  Mb->setItemEnabled( 801, ViewOCC ); // Edit Group

  Mb->setItemEnabled(9998, ViewOCC ); // MENU BLOCKS - MULTI-TRANSFORMATION

  GEOMGUI* library = 0;
  // try to get-or-load corresponding GUI library
  if( theCommandID == 111  ||  // MENU FILE - IMPORT BREP
      theCommandID == 112  ||  // MENU FILE - IMPORT IGES
      theCommandID == 113  ||  // MENU FILE - IMPORT STEP
      theCommandID == 121  ||  // MENU FILE - EXPORT BREP
      theCommandID == 122  ||  // MENU FILE - EXPORT IGES
      theCommandID == 123  ||  // MENU FILE - EXPORT STEP
      theCommandID == 31   ||  // MENU EDIT - COPY
      theCommandID == 33   ||  // MENU EDIT - DELETE
      theCommandID == 411  ||  // MENU SETTINGS - ADD IN STUDY
      theCommandID == 412  ||  // MENU SETTINGS - SHADING COLOR
      theCommandID == 413  ||  // MENU SETTINGS - ISOS
      theCommandID == 414  ||  // MENU SETTINGS - STEP VALUE FOR SPIN BOXES
      theCommandID == 5103 ||  // MENU TOOLS - CHECK GEOMETRY
      theCommandID == 5104 ||  // MENU TOOLS - LOAD SCRIPT
      theCommandID == 8032 ||  // POPUP VIEWER - COLOR
      theCommandID == 8033 ||  // POPUP VIEWER - TRANSPARENCY
      theCommandID == 8034 ||  // POPUP VIEWER - ISOS
      theCommandID == 804  ||  // POPUP VIEWER - ADD IN STUDY
      theCommandID == 901  ||  // OBJECT BROWSER - RENAME
      theCommandID == 9024 ) { // OBJECT BROWSER - OPEN
    library = geomGUI->getLibrary( "libGEOMToolsGUI.so" );
  }
  else if( theCommandID == 211  ||  // MENU VIEW - WIREFRAME/SHADING
	   theCommandID == 212  ||  // MENU VIEW - DISPLAY ALL
	   theCommandID == 213  ||  // MENU VIEW - DISPLAY ONLY
	   theCommandID == 214  ||  // MENU VIEW - ERASE ALL
	   theCommandID == 215  ||  // MENU VIEW - ERASE
	   theCommandID == 216  ||  // MENU VIEW - DISPLAY
	   theCommandID == 80311 ||  // POPUP VIEWER - WIREFRAME
	   theCommandID == 80312 ) { // POPUP VIEWER - SHADING
    library = geomGUI->getLibrary( "libDisplayGUI.so" );
  }
  else if( theCommandID == 4011 ||  // MENU BASIC - POINT
	   theCommandID == 4012 ||  // MENU BASIC - LINE
	   theCommandID == 4013 ||  // MENU BASIC - CIRCLE
	   theCommandID == 4014 ||  // MENU BASIC - ELLIPSE
	   theCommandID == 4015 ||  // MENU BASIC - ARC
	   theCommandID == 4016 ||  // MENU BASIC - VECTOR
	   theCommandID == 4017 ||  // MENU BASIC - PLANE
	   theCommandID == 4018 ||  // MENU BASIC - WPLANE
	   theCommandID == 4019 ||  // MENU BASIC - CURVE
	   theCommandID == 4020 ) { // MENU BASIC - REPAIR
    library = geomGUI->getLibrary( "libBasicGUI.so" );
  }
  else if( theCommandID == 4021 ||  // MENU PRIMITIVE - BOX
	   theCommandID == 4022 ||  // MENU PRIMITIVE - CYLINDER
	   theCommandID == 4023 ||  // MENU PRIMITIVE - SPHERE
	   theCommandID == 4024 ||  // MENU PRIMITIVE - TORUS
	   theCommandID == 4025 ) { // MENU PRIMITIVE - CONE
    library = geomGUI->getLibrary( "libPrimitiveGUI.so" );
  }
  else if( theCommandID == 4031 ||  // MENU GENERATION - PRISM
	   theCommandID == 4032 ||  // MENU GENERATION - REVOLUTION
	   theCommandID == 4033 ||  // MENU GENERATION - FILLING
	   theCommandID == 4034 ) { // MENU GENERATION - PIPE
    library = geomGUI->getLibrary( "libGenerationGUI.so" );
  }
  else if( theCommandID == 404 ||   // MENU ENTITY - SKETCHER
	   theCommandID == 407 ) {  // MENU ENTITY - EXPLODE
    library = geomGUI->getLibrary( "libEntityGUI.so" );
  }
  else if( theCommandID == 4081 ||  // MENU BUILD - EDGE
	   theCommandID == 4082 ||  // MENU BUILD - WIRE
	   theCommandID == 4083 ||  // MENU BUILD - FACE
	   theCommandID == 4084 ||  // MENU BUILD - SHELL
	   theCommandID == 4085 ||  // MENU BUILD - SOLID
	   theCommandID == 4086 ) { // MENU BUILD - COMPUND
    library = geomGUI->getLibrary( "libBuildGUI.so" );
  }
  else if( theCommandID == 5011 ||  // MENU BOOLEAN - FUSE
	   theCommandID == 5012 ||  // MENU BOOLEAN - COMMON
	   theCommandID == 5013 ||  // MENU BOOLEAN - CUT
	   theCommandID == 5014 ) { // MENU BOOLEAN - SECTION
    library = geomGUI->getLibrary( "libBooleanGUI.so" );
  }
  else if( theCommandID == 5021 ||  // MENU TRANSFORMATION - TRANSLATION
	   theCommandID == 5022 ||  // MENU TRANSFORMATION - ROTATION
	   theCommandID == 5023 ||  // MENU TRANSFORMATION - LOCATION
	   theCommandID == 5024 ||  // MENU TRANSFORMATION - MIRROR
	   theCommandID == 5025 ||  // MENU TRANSFORMATION - SCALE
	   theCommandID == 5026 ||  // MENU TRANSFORMATION - OFFSET
	   theCommandID == 5027 ||  // MENU TRANSFORMATION - MULTI-TRANSLATION
	   theCommandID == 5028 ) { // MENU TRANSFORMATION - MULTI-ROTATION
    library = geomGUI->getLibrary( "libTransformationGUI.so" );
  }
  else if( theCommandID == 503 ||   // MENU OPERATION - PARTITION
	   theCommandID == 504 ||   // MENU OPERATION - ARCHIMEDE
	   theCommandID == 505 ||   // MENU OPERATION - FILLET
	   theCommandID == 506 ||   // MENU OPERATION - CHAMFER  
	   theCommandID == 507 ) {  // MENU OPERATION - CLIPPING RANGE
    library = geomGUI->getLibrary( "libOperationGUI.so" );
  }
  else if( theCommandID == 601 ||   // MENU REPAIR - SEWING
	   theCommandID == 603 ||   // MENU REPAIR - SUPPRESS FACES
	   theCommandID == 604 ||   // MENU REPAIR - SUPPRESS HOLE
           theCommandID == 605 ||   // MENU REPAIR - SHAPE PROCESSING
           theCommandID == 606 ||   // MENU REPAIR - CLOSE CONTOUR
           theCommandID == 607 ||   // MENU REPAIR - REMOVE INTERNAL WIRES
           theCommandID == 608 ||   // MENU REPAIR - ADD POINT ON EDGE
           theCommandID == 609 ||   // MENU REPAIR - FREE BOUNDARIES
	   theCommandID == 602 ) {  // MENU REPAIR - GLUE FACES
    library = geomGUI->getLibrary( "libRepairGUI.so" );
  }
  else if( theCommandID == 701  ||  // MENU MEASURE - PROPERTIES
	   theCommandID == 702  ||  // MENU MEASURE - CDG
	   theCommandID == 703  ||  // MENU MEASURE - INERTIA
	   theCommandID == 7041 ||  // MENU MEASURE - BOUNDING BOX
	   theCommandID == 7042 ||  // MENU MEASURE - MIN DISTANCE
	   theCommandID == 705  ||  // MENU MEASURE - TOLERANCE
	   theCommandID == 706  ||  // MENU MEASURE - WHATIS
	   theCommandID == 707  ||  // MENU MEASURE - CHECK
	   theCommandID == 708 ) {  // MENU MEASURE - POINT COORDINATES
    library = geomGUI->getLibrary( "libMeasureGUI.so" );
  }
  else if( theCommandID == 800  ||  // MENU GROUP - CREATE
	   theCommandID == 801 ) {  // MENU GROUP - EDIT
    library = geomGUI->getLibrary( "libGroupGUI.so" );
  }
  else if( theCommandID == 9999 ||  // MENU BLOCKS - HEXAHEDRAL SOLID
           theCommandID == 9998 ||  // MENU BLOCKS - MULTI-TRANSFORMATION
           theCommandID == 9997 ||  // MENU BLOCKS - QUADRANGLE FACE
           theCommandID == 9995 ) { // MENU BLOCKS - EXPLODE ON BLOCKS
    library = geomGUI->getLibrary( "libBlocksGUI.so" );
  }

  // call method of corresponding GUI library
  if ( library ) {
    return library->OnGUIEvent( theCommandID, parent );
  }
  else {
    QAD_MessageBox::error1( parent, tr( "GEOM_ERROR" ), tr( "GEOM_ERR_LIB_NOT_FOUND" ), tr( "GEOM_BUT_OK" ) );
  }
  return false;
}


//=================================================================================
// function : GeometryGUI::OnKeyPress()
// purpose  : Called when any key is pressed by user [static]
//=================================================================================
bool GeometryGUI::OnKeyPress(QKeyEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame)
{
  GeometryGUI* geomGUI = GeometryGUI::GetGeomGUI();
  GUIMap::Iterator it;
  bool bOk = true;
  for ( it = geomGUI->myGUIMap.begin(); it != geomGUI->myGUIMap.end(); ++it )
    bOk = bOk && it.data()->OnKeyPress( pe, parent, studyFrame );
  return bOk;
}


//=================================================================================
// function : GeometryGUI::OnMouseMove()
// purpose  : Manages mouse move events [static]
//=================================================================================
bool GeometryGUI::OnMouseMove(QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame)
{
  GeometryGUI* geomGUI = GeometryGUI::GetGeomGUI();
  GUIMap::Iterator it;
  bool bOk = true;
  for ( it = geomGUI->myGUIMap.begin(); it != geomGUI->myGUIMap.end(); ++it )
    bOk = bOk && it.data()->OnMouseMove( pe, parent, studyFrame );
  return bOk;
}


//=================================================================================
// function : GeometryGUI::0nMousePress()
// purpose  : Manage mouse press events [static]
//=================================================================================
bool GeometryGUI::OnMousePress(QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame)
{
  GeometryGUI* geomGUI = GeometryGUI::GetGeomGUI();
  GUIMap::Iterator it;
  // OnMousePress() should return false if this event should be processed further
  // (see OCCViewer_Viewer3d::onMousePress() for explanation)
  bool processed = false;
  for ( it = geomGUI->myGUIMap.begin(); it != geomGUI->myGUIMap.end(); ++it )
    processed = processed || it.data()->OnMousePress( pe, parent, studyFrame );
  return processed;
}

static void UpdateVtkSelection(QAD_Desktop* parent)
{
  if (!parent->getActiveStudy()) return;

  QList<QAD_StudyFrame> aFrameList = parent->getActiveStudy()->getStudyFrames();

  for (QAD_StudyFrame* aStudyFrame = aFrameList.first(); aStudyFrame; aStudyFrame = aFrameList.next()) {
    if (aStudyFrame->getTypeView() == VIEW_VTK) {
      QAD_ViewFrame* aViewFrame = aStudyFrame->getRightFrame()->getViewFrame();
      VTKViewer_ViewFrame* aVtkViewFrame = dynamic_cast<VTKViewer_ViewFrame*>(aViewFrame);
      if (!aVtkViewFrame) continue;
      VTKViewer_RenderWindowInteractor* anInteractor = aVtkViewFrame->getRWInteractor();
      if (anInteractor) {
	anInteractor->SetSelectionProp();
	anInteractor->SetSelectionTolerance();
	VTKViewer_InteractorStyleSALOME* aStyle = anInteractor->GetInteractorStyleSALOME();
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
bool GeometryGUI::SetSettings(QAD_Desktop* parent)
{
  GeometryGUI* geomGUI = GetGeomGUI();
  QMenuBar*    Mb = parent->getMainMenuBar();
  QAD_Study*   ActiveStudy = parent->getActiveStudy();
    
  

  /* Wireframe or Shading */
  int DisplayMode = 0;
  bool ViewOCC = false;
  if ( ActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)ActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
    DisplayMode = ic->DisplayMode();
    ViewOCC = true;
  }
  else if (ActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
    VTKViewer_RenderWindowInteractor* myRenderInter = ((VTKViewer_ViewFrame*)ActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
    DisplayMode = myRenderInter->GetDisplayMode();
  }

  if( DisplayMode == 1 )
    Mb->changeItem( 211, tr( "GEOM_MEN_WIREFRAME" ) );
  else
    Mb->changeItem( 211, tr( "GEOM_MEN_SHADING" ) );


  /* Add in Study  - !!!ALWAYS TRUE!!! */ /////// VSR : TO BE REMOVED
  QString AddInStudy = QAD_CONFIG->getSetting("Geometry:SettingsAddInStudy");
  int Settings_AddInStudy;
  /*if(!AddInStudy.isEmpty())
    Settings_AddInStudy = AddInStudy.toInt();
  else
  */
  Settings_AddInStudy = 1;
  Mb->setItemChecked(411, Settings_AddInStudy);

  /* step value */
  QString S = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  if(S.isEmpty())
    QAD_CONFIG->addSetting("Geometry:SettingsGeomStep", "100");

  /* isos */
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

  //Mb->setItemEnabled(404, ViewOCC); // SKETCHER
  Mb->setItemEnabled(603, ViewOCC); // SuppressFace
  Mb->setItemEnabled(604, ViewOCC); // SuppressHole
  Mb->setItemEnabled(606, ViewOCC); // CloseContour
  Mb->setItemEnabled(607, ViewOCC); // RemoveInternalWires
  Mb->setItemEnabled(608, ViewOCC); // AddPointOnEdge
//  Mb->setItemEnabled(609, ViewOCC); // Free boundaries
  Mb->setItemEnabled(413, ViewOCC); // Isos Settings

  Mb->setItemEnabled( 800, ViewOCC ); // Create Group
  Mb->setItemEnabled( 801, ViewOCC ); // Edit Group

  Mb->setItemEnabled(9998, ViewOCC); // MENU BLOCKS - MULTI-TRANSFORMATION

      
  // PAL5356: update VTK selection
  ::UpdateVtkSelection(parent);
  bool bOk = true;
  GUIMap::Iterator it;
  for ( it = geomGUI->myGUIMap.begin(); it != geomGUI->myGUIMap.end(); ++it )
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

//=======================================================================
// function : GeometryGUI::Deactivate()
// purpose  : Called when GEOM module is deactivated [ static ]
//=======================================================================
void GeometryGUI::Deactivate()
{
  GeometryGUI* geomGUI = GetGeomGUI();
  GetGeomGUI()->EmitSignalCloseAllDialogs();
  GUIMap::Iterator it;
  for ( it = geomGUI->myGUIMap.begin(); it != geomGUI->myGUIMap.end(); ++it )
    it.data()->Deactivate();
}

//=================================================================================
// function : GeometryGUI::DefinePopup()
// purpose  : Called from desktop to define popup menu [static]
//=================================================================================
void GeometryGUI::DefinePopup(QString& theContext, QString& theParent, QString& theObject)
{
  GeometryGUI* geomGUI   = GetGeomGUI();
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
    aDisplayMode = ((VTKViewer_ViewFrame*)aViewFrame)->getRWInteractor()->GetDisplayMode();

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
	popup->removeItem( QAD_DisplayOnly_Popup_ID );
	if ( !isOCCViewer && !isVTKViewer ) {
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
	      VTKViewer_Prs* vtkPrs = dynamic_cast<VTKViewer_Prs*>( prs );
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
      QFont f = QApplication::font(); f.setBold( TRUE );
      popup->removeItem( QAD_TopLabel_Popup_ID );
      popup->insertItem( new CustomItem( objectName, f ), QAD_TopLabel_Popup_ID, topItem );
    }
  }

  return false;
}

//=================================================================================
// function : GeometryGUI::ActiveStudyChanged()
// purpose  : static
//=================================================================================
bool GeometryGUI::ActiveStudyChanged(QAD_Desktop* parent)
{
  GeometryGUI* geomGUI     = GetGeomGUI();
  QAD_Study*   ActiveStudy = parent->getActiveStudy();
  QMenuBar* Mb = QAD_Application::getDesktop()->getMainMenuBar();
  int DisplayMode = 0;
  bool ViewOCC = false;

  if( ActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)ActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
    DisplayMode = ic->DisplayMode();
    ViewOCC = true;
  }
  else if( ActiveStudy->getActiveStudyFrame()->getTypeView() == VIEW_VTK ) {
    VTKViewer_RenderWindowInteractor* myRenderInter = ((VTKViewer_ViewFrame*)ActiveStudy->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
    DisplayMode = myRenderInter->GetDisplayMode();
  }

  if( DisplayMode == 1 )
    Mb->changeItem( 211, tr( "GEOM_MEN_WIREFRAME" ) );
  else
    Mb->changeItem( 211, tr( "GEOM_MEN_SHADING" ) );

  //Mb->setItemEnabled( 404, ViewOCC ); // SKETCHER
  Mb->setItemEnabled( 603, ViewOCC ); // SuppressFace
  Mb->setItemEnabled( 604, ViewOCC ); // SuppressHole
  Mb->setItemEnabled( 606, ViewOCC ); // CloseContour
  Mb->setItemEnabled( 413, ViewOCC ); // Isos Settings
  Mb->setItemEnabled(9998, ViewOCC ); // MENU BLOCKS - MULTI-TRANSFORMATION

  geomGUI->EmitSignalCloseAllDialogs();

  // PAL5356: update VTK selection
  ::UpdateVtkSelection( parent );

  bool bOk = true;
  GUIMap::Iterator it;
  for ( it = geomGUI->myGUIMap.begin(); it != geomGUI->myGUIMap.end(); ++it )
    bOk = bOk && it.data()->ActiveStudyChanged( parent );
  return bOk;
}

//=======================================================================
// function : GeometryGUI::BuildPresentation()
// purpose  : static
//=======================================================================
void GeometryGUI::BuildPresentation( const Handle(SALOME_InteractiveObject)& theIO,
                                     QAD_ViewFrame* theViewFrame )
{
  GEOM_Displayer().Display( theIO, false, theViewFrame );
}

void GeometryGUI::SupportedViewType(int* buffer, int bufferSize)
{
  if(!buffer || !bufferSize) return;
  buffer[0] = (int)VIEW_OCC;
  if (--bufferSize) buffer[1] = (int)VIEW_VTK;
}
