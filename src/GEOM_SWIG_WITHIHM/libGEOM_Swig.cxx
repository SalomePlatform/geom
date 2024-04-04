// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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
// File   : libGEOM_Swig.cxx
// Author : Nicolas REJNERI, Paul RASCLE

#include "libGEOM_Swig.h"

#include "GeometryGUI.h"
#include "GEOM_Displayer.h"
#include "GEOM_Constants.h"
#include "Material_Model.h"
#include "GEOM_Swig_LocalSelector.h"
#include "GEOMGUI_OCCSelector.h"
#include "OCCViewer_ViewManager.h"

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewModel.h>
#include <SUIT_SelectionMgr.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <OCCViewer_ViewFrame.h>
#include <SVTK_ViewWindow.h>

#include <SALOME_Event.h>
#include <utilities.h>

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

GEOM_Swig_LocalSelector* GEOM_Swig::myLocalSelector = 0;
GEOMGUI_OCCSelector* GEOM_Swig::myOCCSelector =0;

/*!
  \brief Constructor
*/
GEOM_Swig::GEOM_Swig( bool updateOB )
{
  init(updateOB);
}

/*!
  \brief Destructor
*/
GEOM_Swig::~GEOM_Swig()
{
  // Delete selector because of sigsegv in TEventInitLocalSelection::Execute()
  // when call GEOM_Swig.initLocalSelection() from Python, close the study, create a new one and
  // call it again.
  if (myOCCSelector)
  {
    delete myOCCSelector;
    myOCCSelector = nullptr;
  }
}

/*!
  \brief Internal initialization
*/
void GEOM_Swig::init( bool updateOB )
{
  class TEvent: public SALOME_Event
  {
    bool myUpdateOB;
  public:
    TEvent( bool _updateOB ):
      myUpdateOB(_updateOB)
    {}
    virtual void Execute()
    {
      // check active study
      SUIT_Application* app = SUIT_Session::session()->activeApplication();
      if (!app) return;
      
      SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
      if ( !study ) return;

      _PTR(Study) studyDS( study->studyDS() );
      _PTR(StudyBuilder) builder = studyDS->NewBuilder();

      // get/init GEOM engine
      GEOM::GEOM_Gen_var engine = GeometryGUI::GetGeomGen();
      if ( CORBA::is_nil( engine ) )
        return;

      // find GEOM component in the study
      _PTR(SComponent) component = studyDS->FindComponent( "GEOM" );
      if ( !component )
        return;

      // load GEOM data (if it is not done yet)
      std::string ior;
      if ( !component->ComponentIOR( ior ) ) {
        CORBA::String_var engineIOR = SalomeApp_Application::orb()->object_to_string( engine );
        builder->LoadWith( component, engineIOR.in() );
      }

      // update Object browser
      if (myUpdateOB && dynamic_cast<SalomeApp_Application*>( app ) )
        dynamic_cast<SalomeApp_Application*>( app )->updateObjectBrowser( true );

      //myLocalSelector = 0;
    }
  };

  ProcessVoidEvent( new TEvent(updateOB) );
}

/*!
  \brief Display the presenation in the currently active view
  \param theEntry geometry object's entry
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::createAndDisplayGO( const char* theEntry, bool theUpdateViewer )
{
  class TEvent: public SALOME_Event
  {
    std::string myEntry;
    bool        myUpdateViewer;
  public:
    TEvent( const char* _entry, bool _update ):
      myEntry( _entry ), myUpdateViewer( _update )
    {}
    virtual void Execute()
    {
      Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject( myEntry.c_str(), "GEOM", "" );

      GEOM_Displayer().Display( io, myUpdateViewer );
    }
  };

  ProcessVoidEvent( new TEvent( theEntry, theUpdateViewer ) );
}

/*!
  \brief Same as createAndDisplayGO, but also fits the active view to the contents
  \param theEntry geometry object's entry
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::createAndDisplayFitAllGO( const char* theEntry )
{
  // display object
  createAndDisplayGO( theEntry );

  // fit all the view
  class TEventFitAll: public SALOME_Event
  {
  public:
    TEventFitAll() {}
    virtual void Execute()
    {
      SUIT_Application* app = SUIT_Session::session()->activeApplication();
      if ( app ) {
        SUIT_ViewWindow* window = app->desktop()->activeWindow();
        if ( dynamic_cast<SVTK_ViewWindow*>( window ) )
          dynamic_cast<SVTK_ViewWindow*>( window )->onFitAll();
        else if ( dynamic_cast<OCCViewer_ViewFrame*>( window ) )
          dynamic_cast<OCCViewer_ViewFrame*>( window )->onViewFitAll();
      }
    }
  };
    
  ProcessVoidEvent( new TEventFitAll() );
}

/*!
  \brief Erase presentation in the currently active viewer
  \param theEntry geometry object's entry
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::eraseGO( const char* theEntry, bool theUpdateViewer )
{
  class TEvent: public SALOME_Event
  {
    std::string myEntry;
    bool        myUpdateViewer;
  public:
    TEvent( const char* _entry, bool _update ):
      myEntry( _entry ), myUpdateViewer( _update )
    {}
    virtual void Execute()
    {
      Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject( myEntry.c_str(), "GEOM", "" );

      GEOM_Displayer().Erase( io, true, myUpdateViewer );
    }
  };

  ProcessVoidEvent( new TEvent( theEntry, theUpdateViewer ) );
}

/*!
  \brief Update active viewer contents
*/
void GEOM_Swig::UpdateViewer()
{
  class TEventUpdateViewer: public SALOME_Event
  {
  public:
    TEventUpdateViewer()
    {}
    virtual void Execute()
    {
      GEOM_Displayer().UpdateViewer();
    }
  };
  
  ProcessVoidEvent(new TEventUpdateViewer());
}

/*!
  \brief Get sub-shape index inside main shape
  \param theSubIOR sub-shape geometry object's IOR
  \param theMainIOR main shape geometry object's IOR
  \return sub-shape index (-1 in case of error)
*/
int GEOM_Swig::getIndexTopology( const char* theSubIOR, const char* theMainIOR )
{
  int index = -1;

  // get geom engine
  GEOM::GEOM_Gen_var aGeomGen = GeometryGUI::GetGeomGen();

  // get main shape's geom object by IOR
  CORBA::Object_var anObject = SalomeApp_Application::orb()->string_to_object( theMainIOR );
  GEOM::GEOM_Object_var aMainShape = GEOM::GEOM_Object::_narrow( anObject.in() );
  // get sub-shape's geom object by IOR
  anObject = SalomeApp_Application::orb()->string_to_object( theSubIOR );
  GEOM::GEOM_Object_var aSubShape = GEOM::GEOM_Object::_narrow( anObject.in() );

  if ( !CORBA::is_nil( aGeomGen ) && !CORBA::is_nil( aMainShape ) && !CORBA::is_nil( aSubShape ) ) {
    // get shapes operations interface
    GEOM::GEOM_IShapesOperations_var anIShapesOperations =
      aGeomGen->GetIShapesOperations();
    if ( !CORBA::is_nil( anIShapesOperations ) )
      index = anIShapesOperations->GetTopologyIndex( aMainShape, aSubShape );
  }

  return index;
}

/*!
  \brief Get shape type name
  \param theIOR geometry object's IOR
  \return shape type name ("Shape of unknown type" in case of error)
*/
const char* GEOM_Swig::getShapeTypeString( const char* theIOR )
{
  QString aTypeName = "Shape of unknown type";

  // get geom engine
  GEOM::GEOM_Gen_var aGeomGen = GeometryGUI::GetGeomGen();

  // get shape's geom object by IOR
  CORBA::Object_var anObject = SalomeApp_Application::orb()->string_to_object( theIOR );
  GEOM::GEOM_Object_var aShape = GEOM::GEOM_Object::_narrow( anObject.in() );

  if ( !CORBA::is_nil( aGeomGen ) && !CORBA::is_nil( aShape ) ) {
    // get shapes operations interface
    GEOM::GEOM_IShapesOperations_var anIShapesOperations =
      aGeomGen->GetIShapesOperations();
    if ( !CORBA::is_nil( anIShapesOperations ) )
      aTypeName = anIShapesOperations->GetShapeTypeString( aShape );
  }

  return strdup( qPrintable( aTypeName ) );
}

/*!
  \brief Get shape's icon ID (specified by its type)
  \param theIOR geometry object's IOR
  \return icon ID ("None" in case of error)
*/
const char* GEOM_Swig::getShapeTypeIcon( const char* theIOR )
{
  static const char* icons[] = {
    "ICON_OBJBROWSER_COMPOUND",
    "ICON_OBJBROWSER_COMPSOLID",
    "ICON_OBJBROWSER_SOLID",
    "ICON_OBJBROWSER_SHELL",
    "ICON_OBJBROWSER_FACE",
    "ICON_OBJBROWSER_WIRE",
    "ICON_OBJBROWSER_EDGE",
    "ICON_OBJBROWSER_VERTEX"
  };

  const char* anIcon = "None";

  try {
    CORBA::Object_var anObject = SalomeApp_Application::orb()->string_to_object( theIOR );
    if ( !CORBA::is_nil( anObject ) ) {
      GEOM::GEOM_Object_var aShape = GEOM::GEOM_Object::_narrow( anObject.in() );
      if ( !CORBA::is_nil( aShape ) ) {
        GEOM::shape_type aType = aShape->GetShapeType();
        if ( aType >= GEOM::COMPOUND && aType < GEOM::SHAPE )
          anIcon = icons[ (int)aType ];
      }
    }
  }
  catch ( CORBA::Exception& ) {
  }

  return anIcon;
}

class TSetPropertyEvent: public SALOME_Event
{
  QString  myEntry;
  QString  myProperty;
  QVariant myValue;
  bool     myUpdateViewer;
  
public:
  TSetPropertyEvent( const QString& _entry,
                     const QString& _property,
                     const QVariant& _value,
                     bool _update = true );
  virtual void Execute();
};
  
TSetPropertyEvent::TSetPropertyEvent( const QString& _entry,
                                      const QString& _property,
                                      const QVariant& _value,
                                      bool _update ):
  myEntry( _entry ),
  myProperty( _property ),
  myValue( _value ),
  myUpdateViewer( _update )
{
}

void TSetPropertyEvent::Execute()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( !app ) return;
  
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !study ) return;
  
  GEOM_Displayer displayer;
  
  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;
  
  int mgrId = dynamic_cast<SUIT_ViewModel*>( window )->getViewManager()->getGlobalId();

  study->setObjectProperty( mgrId, myEntry, myProperty, myValue );
  
  Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject( myEntry.toUtf8().data(), "GEOM" );
  if ( window->isVisible( io ) ) displayer.Redisplay( io, myUpdateViewer );
}

/*!
  \brief Set display mode to the presentation
  \param theEntry geometry object's entry
  \param theMode display mode: 0 - wireframe, 1 - shading, 2 - shading+edges, 3-textured
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::setDisplayMode( const char* theEntry, int theMode, bool theUpdateViewer )
{
  ProcessVoidEvent( new TSetPropertyEvent( theEntry, GEOM::propertyName( GEOM::DisplayMode ), 
                                           theMode, theUpdateViewer ) );
}

/*!
  \brief Show / hide edges direction vectors for the presentation
  \param theEntry geometry object's entry
  \param theOn \c true to show edges direction vectors or \c false otherwise
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::setVectorsMode( const char* theEntry, bool theOn, bool theUpdateViewer )
{
  ProcessVoidEvent( new TSetPropertyEvent( theEntry, GEOM::propertyName( GEOM::EdgesDirection ), 
                                           theOn, theUpdateViewer ) );
}

/*!
  \brief Show / hide vertices for the presentation
  \param theEntry geometry object's entry
  \param theOn \c true to show vertices or \c false otherwise
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::setVerticesMode( const char* theEntry, bool theOn, bool theUpdateViewer )
{
  ProcessVoidEvent( new TSetPropertyEvent( theEntry, GEOM::propertyName( GEOM::Vertices ), 
                                           theOn, theUpdateViewer ) );
}

/*!
  \brief Show / hide name of shape for the presentation
  \param theEntry geometry object's entry
  \param theOn \c true to show name or \c false otherwise
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::setNameMode( const char* theEntry, bool theOn, bool theUpdateViewer )
{
  ProcessVoidEvent( new TSetPropertyEvent( theEntry, GEOM::propertyName( GEOM::ShowName ),
                                           theOn, theUpdateViewer ) );
}

/*!
  \brief Change color of the presentation
  \param theEntry geometry object's entry
  \param theRed red component of the component (0-255)
  \param theGreen green component of the component (0-255)
  \param theBlue blue component of the component (0-255)
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::setColor( const char* theEntry, int theRed, int theGreen, int theBlue, bool theUpdateViewer )
{
  ProcessVoidEvent( new TSetPropertyEvent( theEntry, GEOM::propertyName( GEOM::Color ), 
                                           QColor( theRed, theGreen, theBlue ), theUpdateViewer ) );
}

/*!
  \brief Set number of iso-lines to the presentation
  \param theEntry geometry object's entry
  \param theNbU number of iso-lines along U axis (integer value >= 0)
  \param theNbV number of iso-lines along V axis (integer value >= 0)
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::setIsos( const char* theEntry, int theNbU, int theNbV, bool theUpdateViewer )
{
  ProcessVoidEvent( new TSetPropertyEvent( theEntry, GEOM::propertyName( GEOM::NbIsos ), 
                                           QString( "%1%2%3" ).arg( theNbU ).arg( GEOM::subSectionSeparator() ).arg( theNbV ), 
                                           theUpdateViewer ) );
}

/*!
  \brief Set transparency of the presentation
  \param theEntry geometry object's entry
  \param theTransparency transparency (floating point value between 0 and 1)
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::setTransparency( const char* theEntry, float theTransparency, bool theUpdateViewer )
{
  ProcessVoidEvent( new TSetPropertyEvent( theEntry, GEOM::propertyName( GEOM::Transparency ), 
                                           theTransparency, theUpdateViewer ) );
}

/*!
  \brief Set deflection coefficient of the presentation
  \param theEntry geometry object's entry
  \param theDeflection deflection coefficient (floating point value)
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::setDeflection( const char* theEntry, float theDeflection, bool theUpdateViewer )
{
  ProcessVoidEvent( new TSetPropertyEvent( theEntry, GEOM::propertyName( GEOM::Deflection ), 
                                           theDeflection, theUpdateViewer ) );
}

/*!
  \brief Set material to the presentation
  \param theEntry geometry object's entry
  \param theMaterial material name (string)
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::setMaterial( const char* theEntry, const char* theMaterial, bool theUpdateViewer )
{
  Material_Model material;
  material.fromResources( theMaterial );
  ProcessVoidEvent( new TSetPropertyEvent( theEntry, GEOM::propertyName( GEOM::Material ), 
                                           material.toProperties(), theUpdateViewer ) );
}

/*!
  \brief Set material property to the presentation
  \param theEntry geometry object's entry
  \param theMaterial material property string
  \param theUpdateViewer \c true to update active view's contents
*/
void GEOM_Swig::setMaterialProperty( const char* theEntry, const char* theMaterial, bool theUpdateViewer )
{
  ProcessVoidEvent( new TSetPropertyEvent( theEntry, GEOM::propertyName( GEOM::Material ), 
                                           theMaterial, theUpdateViewer ) );
}

/*!
  \brief initialize local subShapes selection on a given shape
  \param theEntry geometry object's entry
  \param theMode from enum TopAbs_ShapeEnum
 */
void GEOM_Swig::initLocalSelection( const char* theEntry, int theMode)
{
  class TEventInitLocalSelection: public SALOME_Event
  {
    std::string myEntry;
    int myMode;
  public:
    TEventInitLocalSelection(const char* _entry, int _mode)
    : myEntry(_entry), myMode(_mode)
    {}
    virtual void Execute()
    {
      if (myLocalSelector)
        return;
      SUIT_Application* app = SUIT_Session::session()->activeApplication();
      if ( app )
        {
          //SUIT_ViewWindow* window = app->desktop()->activeWindow();
          SUIT_ViewWindow* wnd = 0;
          LightApp_Application* lapp = dynamic_cast<LightApp_Application*>(app);
          if ( lapp )
            {
              SUIT_ViewManager* viewMgr = lapp->activeViewManager();
              if ( viewMgr )
                {
                  wnd = viewMgr->getActiveView();
                }
              LightApp_SelectionMgr* mgr = lapp->selectionMgr();
              if (!myOCCSelector)
                {
                  myOCCSelector = new GEOMGUI_OCCSelector( ((OCCViewer_ViewManager*)viewMgr)->getOCCViewer(), mgr );
                }

              QList<SUIT_Selector*> aSelectorList;
              mgr->selectors( "OCCViewer", aSelectorList );
              for (int i=0; i< aSelectorList.size(); ++i)
                {
                  if ( LightApp_OCCSelector* aSelector = dynamic_cast<LightApp_OCCSelector*>( aSelectorList.at(i) ) )
                    {
                      aSelector->setEnabled(false);
                    }
                  if ( GEOMGUI_OCCSelector* aSelector = dynamic_cast<GEOMGUI_OCCSelector*>( aSelectorList.at(i) ) )
                    {
                      aSelector->setEnabled(true);
                    }
                }
              myOCCSelector->setEnabled(true);
            }

          myLocalSelector = new GEOM_Swig_LocalSelector(app->desktop(), wnd, myEntry, myMode);
          MESSAGE("TEventInitLocalSelection myLocalSelector: " << myLocalSelector);
        }
    }
  };

  ProcessVoidEvent(new TEventInitLocalSelection(theEntry, theMode));
}

/*!
  \brief get local subShapes selection on a given shape
  \return a list of selected subShapes indexes
 */
std::vector<int> GEOM_Swig::getLocalSelection()
{
  class TEventGetLocalSelection: public SALOME_Event
  {
  public:
    typedef std::vector<int> TResult;
    TResult myResult;

    TEventGetLocalSelection(){}

    virtual void Execute()
    {
      MESSAGE("TEventGetLocalSelection myLocalSelector: " << myLocalSelector);
      if (myLocalSelector)
        myResult = myLocalSelector->getSelection();
    }
  };

  std::vector<int> result = ProcessEvent(new TEventGetLocalSelection());
  return result;
}


/*!libGEOM_Swig
  \brief Set local subShapes selection on a given shape
  \param ids sub-shapes ids
 */
void GEOM_Swig::setLocalSelection(const std::vector<int> ids)
{
  class TEventSetLocalSelection: public SALOME_Event
  {
  public:
    typedef std::vector<int> TResult;
    TResult myIds;
    TResult myResult;

    TEventSetLocalSelection(const std::vector<int> _ids) : myIds(_ids) {}

    virtual void Execute()
    {
      MESSAGE("TEventSetLocalSelection myLocalSelector: " << myLocalSelector);
      if (myLocalSelector)
        myLocalSelector->setSelection(myIds);
    }
  };

  ProcessEvent(new TEventSetLocalSelection(ids));
}

/*!
  \brief close local subShapes selection on a given shape
 */
void GEOM_Swig::closeLocalSelection()
{
  class TEventCloseLocalSelection: public SALOME_Event
  {
  public:
    TEventCloseLocalSelection()
    {}
    virtual void Execute()
    {
      MESSAGE("TEventCloseLocalSelection myLocalSelector: " << myLocalSelector);
      if (myLocalSelector)
        {
          delete myLocalSelector;
          myLocalSelector = 0;
        }
    }
  };

  ProcessVoidEvent(new TEventCloseLocalSelection());
}

class TInitGeomGenEvent: public SALOME_Event
{
public:
  typedef bool TResult;
  TResult myResult;
  TInitGeomGenEvent() : myResult(false)
  {}
  virtual void Execute()
  {
    myResult = GeometryGUI::InitGeomGen();
  }
};

/*!
  \brief Initialize GEOM module's engine
  \return \c true if initialization succeeds or \c false otherwise
*/
bool GEOM_Swig::initGeomGen()
{
  return ProcessEvent( new TInitGeomGenEvent() );
}
