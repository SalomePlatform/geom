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

// File   : GEOMGUI_Selection.cxx
// Author : Alexander SOLOVYOV, Open CASCADE S.A.S. (alexander.solovyov@opencascade.com)

#include "GEOMGUI_Selection.h"
#include <GEOMGUI_DimensionProperty.h>

#include "GeometryGUI.h"
#include "GEOM_Displayer.h"

#include "Material_Model.h"

#include <GEOM_Constants.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>

#include "LightApp_DataOwner.h"

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ResourceMgr.h>

#include <SALOME_Prs.h>
#include <SALOME_InteractiveObject.hxx>

#include <SOCC_Prs.h>
#include <SVTK_Prs.h>
#include <SALOME_Actor.h>
#include <GEOM_Actor.h>

#include <OCCViewer_ViewModel.h>
#include <SVTK_ViewModel.h>

#include <GEOMImpl_Types.hxx>

#include <GEOM_AISShape.hxx>
#include <GEOM_VTKPropertyMaterial.hxx>

// OCCT Includes
#include <AIS.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_GraphicTool.hxx>
#include <Aspect_TypeOfFacingModel.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include<Graphic3d_MaterialAspect.hxx>

// VTK Includes
#include <vtkActorCollection.h>

#define OCC_DISPLAY_MODE_TO_STRING( str, dm ) { \
    if ( dm == AIS_WireFrame ) \
      str = QString( "Wireframe" ); \
    else if ( dm == AIS_Shaded )    \
      str = QString( "Shading" ); \
    else if ( dm == GEOM_AISShape::ShadingWithEdges )    \
      str = QString( "ShadingWithEdges" ); \
    else if ( dm == GEOM_AISShape::TexturedShape )    \
      str = QString( "Texture" ); \
    else \
      str = QString(); }

#define USE_VISUAL_PROP_MAP

#ifdef USE_VISUAL_PROP_MAP
  #define VTK_DISPLAY_MODE_TO_STRING( str, dm ) { \
      if ( dm == 0 ) \
        str = QString( "Wireframe" ); \
      else if ( dm == 1 )    \
        str = QString( "Shading" ); \
      else if ( dm == 2 )    \
        str = QString( "ShadingWithEdges" ); \
      else \
        str = QString(); }
#else
  #define VTK_DISPLAY_MODE_TO_STRING( str, dm ) { \
      if ( dm == 0 ) \
        str = QString( "Wireframe" ); \
      else if ( dm == 1 )    \
        str = QString( "Shading" ); \
      else if ( dm == 3 )    \
        str = QString( "ShadingWithEdges" ); \
      else \
        str = QString(); }
#endif

GEOMGUI_Selection::GEOMGUI_Selection()
: LightApp_Selection()
{
}

GEOMGUI_Selection::~GEOMGUI_Selection()
{
}

void GEOMGUI_Selection::init( const QString& context, LightApp_SelectionMgr* selMgr )
{
  LightApp_Selection::init( context, selMgr );

  myObjects.resize( count() );

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( study() );
  if ( appStudy ) {
    _PTR(Study) study = appStudy->studyDS();
    for ( int idx = 0; idx < count(); idx++ ) {
      QString anEntry = entry( idx );
      if ( study && !anEntry.isEmpty() ) {
        _PTR(SObject) aSO( study->FindObjectID( anEntry.toStdString() ) );
        if ( aSO ) {
          CORBA::Object_var varObj = GeometryGUI::ClientSObjectToObject( aSO );
          myObjects[idx] = GEOM::GEOM_BaseObject::_narrow( varObj );
        }
      }
    }
  }
}

//QVariant GEOMGUI_Selection::contextParameter( const QString& p ) const
QVariant GEOMGUI_Selection::parameter( const QString& p ) const
{
  QVariant v;
  if ( p == "isOCC" )
    v = activeViewType() == OCCViewer_Viewer::Type();
  else if ( p == "selectionmode" )
    v = selectionMode();
  else if ( p == "hasImported" )
    v = hasImported();
  else if ( p == "allImported" )
    v = allImported();
  else
    v = LightApp_Selection::parameter( p );
  return v;
}

//QVariant GEOMGUI_Selection::objectParameter( const int idx, const QString& p ) const
QVariant GEOMGUI_Selection::parameter( const int idx, const QString& p ) const
{
  QVariant v;
  if ( p == "type" )
    v = typeName( idx );
  else if ( p == "typeid" )
    v = typeId( idx );
  else if ( p == "displaymode" )
    v = displayMode( idx );
  else if ( p == "isAutoColor" )
    v = isAutoColor( idx );
  else if ( p == "isVectorsMode" )
    v = isVectorsMode( idx );
  else if ( p == "isVerticesMode" )
    v = isVerticesMode( idx );
  else if ( p == "isNameMode" )
    v = isNameMode( idx );
  else if ( p == "topLevel" )
    v = topLevel( idx );
  else if ( p == "autoBringToFront" )
    v = autoBringToFront( idx );
  else if ( p == "hasChildren" )
    v = hasChildren( idx );
  else if ( p == "nbChildren" )
    v = nbChildren(idx);
  else if ( p == "hasConcealedChildren" )
    v = hasConcealedChildren( idx );
  else if ( p == "hasDisclosedChildren" )
    v = hasDisclosedChildren( idx );
  else if ( p == "compoundOfVertices" )
    v = compoundOfVertices( idx );
  else if ( p == "imported" )
    v = isImported( idx );
  else if ( p == "isPhysicalMaterial" )
    v = isPhysicalMaterial(idx);
  else if ( p == "isFolder" )
    v = isFolder(idx);
  else if ( p == "hasHiddenDimensions" )
    v = hasHiddenDimensions(idx);
  else if ( p == "hasVisibleDimensions" )
    v = hasVisibleDimensions(idx);
  else
    v = LightApp_Selection::parameter( idx, p );

  return v;
}

// the method to skip temporary objects from selection (called from LightApp)
bool GEOMGUI_Selection::processOwner( const LightApp_DataOwner* theOwner )
{
  return !theOwner->entry().contains("_");
}

QString GEOMGUI_Selection::typeName( const int index ) const
{
  if ( isComponent( index ) )
    return "Component";
  if ( isFolder( index ) )
    return "Folder";

  static QString aGroup( "Group" );
  static QString aShape( "Shape" );
  static QString aField( "Field" );
  static QString aFieldStep( "FieldStep" );
  static QString anUnknown( "Unknown" );

  GEOM::GEOM_BaseObject_var anObj = getBaseObject( index );
  if ( !CORBA::is_nil( anObj ) ) {
    const int aGeomType = anObj->GetType();
    switch ( aGeomType ) {
    case GEOM_GROUP     : return aGroup;
    case GEOM_FIELD     : return aField;
    case GEOM_FIELD_STEP: return aFieldStep;
    default             : return aShape;
    }
  }
  return anUnknown;
}

int GEOMGUI_Selection::typeId( const int index ) const
{
  int aType = -1;
  GEOM::GEOM_Object_var anObj = getObject( index );
  if ( !CORBA::is_nil( anObj ) )
    aType = (int)anObj->GetShapeType();
  return aType;
}

bool GEOMGUI_Selection::isVisible( const int index ) const
{
#ifdef USE_VISUAL_PROP_MAP
  QVariant v = visibleProperty( entry( index ), GEOM::propertyName( GEOM::Visibility ) );
  if ( v.canConvert( QVariant::Bool ) )
    return v.toBool();
#endif

  bool res = false;

  GEOM::GEOM_Object_var obj = getObject( index );
  SALOME_View* view = GEOM_Displayer::GetActiveView();
  if ( !CORBA::is_nil( obj ) && view ) {
    Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject( entry( index ).toLatin1().constData(), "GEOM", "TEMP_IO" );
    res = view->isVisible( io );
  }

  return res;
}

bool GEOMGUI_Selection::isAutoColor( const int index ) const
{
  GEOM::GEOM_Object_var obj = getObject( index );
  if ( !CORBA::is_nil( obj ) )
    return obj->GetAutoColor();
  return false;
}

bool GEOMGUI_Selection::isImported( const int index ) const
{
  GEOM::GEOM_Object_var obj = getObject( index );
  if ( !CORBA::is_nil( obj ) )
    return obj->GetType() == GEOM_IMPORT;
  return false;
}

bool GEOMGUI_Selection::hasImported() const
{
  bool res = false;
  for ( int i = 0; i < count() && !res; i++ )
    res = isImported( i );
  return res;
}

bool GEOMGUI_Selection::allImported() const
{
  bool res = true;
  for ( int i = 0; i < count() && res; i++ )
    res = isImported( i );
  return res;
}

QVariant GEOMGUI_Selection::visibleProperty( const QString& entry, const QString& propName ) const
{
  QVariant v;
  LightApp_Study* aStudy = study();
  if ( aStudy ) {
    LightApp_Application* anApp = ::qobject_cast<LightApp_Application*>( aStudy->application() );
    if ( anApp && anApp->activeViewManager() ) {
      int id = anApp->activeViewManager()->getGlobalId();
      v = aStudy->getObjectProperty( id, entry, propName, QVariant() );
    }
  }
  return v;
}

QString GEOMGUI_Selection::displayMode( const int index ) const
{
  QString res;
  QString viewType = activeViewType();
  
#ifdef USE_VISUAL_PROP_MAP
  QVariant v = visibleProperty( entry( index ), GEOM::propertyName( GEOM::DisplayMode ) );
  if ( v.canConvert( QVariant::Int ) ) {
    int dm = v.toInt();
    if ( viewType == OCCViewer_Viewer::Type() ) {
      OCC_DISPLAY_MODE_TO_STRING( res, dm );
    } else if ( viewType == SVTK_Viewer::Type() ) {
      VTK_DISPLAY_MODE_TO_STRING( res, dm );
    }
    return res;
  }
#endif

  SALOME_View* view = GEOM_Displayer::GetActiveView();
  if ( view /*fix for 9320==>*/&& ( viewType == OCCViewer_Viewer::Type() || viewType == SVTK_Viewer::Type() ) ) {
    SALOME_Prs* prs = view->CreatePrs( entry( index ).toLatin1().constData() );
    if ( prs ) {
      if ( viewType == OCCViewer_Viewer::Type() ) { // assuming OCC
        SOCC_Prs* occPrs = (SOCC_Prs*) prs;
        AIS_ListOfInteractive lst;
        occPrs->GetObjects( lst );
        if ( lst.Extent() ) {
          Handle(AIS_InteractiveObject) io = lst.First();
          if ( !io.IsNull() ) {
            int dm;
            Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(io);
            if(!aSh.IsNull()) {
              dm = aSh->isTopLevel() ? aSh->prevDisplayMode() : aSh->DisplayMode();
            } else {
              dm = io->DisplayMode();
            }
            OCC_DISPLAY_MODE_TO_STRING( res, dm );
            if ( res.isEmpty() ) { // return default display mode of AIS_InteractiveContext
              OCCViewer_Viewer* occViewer = (OCCViewer_Viewer*)SUIT_Session::session()->activeApplication()->
                desktop()->activeWindow()->getViewManager()->getViewModel();
              Handle(AIS_InteractiveContext) ic = occViewer->getAISContext();
              dm = ic->DisplayMode();
              OCC_DISPLAY_MODE_TO_STRING( res, dm );
            }
          }
        }
      }
      else if ( viewType == SVTK_Viewer::Type() ) { // assuming VTK
        SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( prs );
        vtkActorCollection* lst = vtkPrs ? vtkPrs->GetObjects() : 0;
        if ( lst ) {
          lst->InitTraversal();
          vtkActor* actor = lst->GetNextActor();
          if ( actor ) {
            SALOME_Actor* salActor = dynamic_cast<SALOME_Actor*>( actor );
            if ( salActor ) {
              int dm = salActor->getDisplayMode();
              VTK_DISPLAY_MODE_TO_STRING( res, dm );
            } // if ( salome actor )
          } // if ( actor )
        } // if ( lst == vtkPrs->GetObjects() )
      } // if VTK
    }
  }

  return res;
}

bool GEOMGUI_Selection::autoBringToFront( const int /*index*/ ) const
{
  return SUIT_Session::session()->resourceMgr()->booleanValue( "Geometry", "auto_bring_to_front", "false" );
}

bool GEOMGUI_Selection::isVectorsMode( const int index ) const
{
#ifdef USE_VISUAL_PROP_MAP
  QVariant v = visibleProperty( entry( index ), GEOM::propertyName( GEOM::EdgesDirection ) );
  if ( v.canConvert( QVariant::Bool ) )
    return v.toBool();
#endif

  bool res = false;
  
  SALOME_View* view = GEOM_Displayer::GetActiveView();
  QString viewType = activeViewType();
  if ( view && ( viewType == OCCViewer_Viewer::Type() || viewType == SVTK_Viewer::Type() ) ) {
    SALOME_Prs* prs = view->CreatePrs( entry( index ).toLatin1().constData() );
    if ( prs ) {
      if ( viewType == OCCViewer_Viewer::Type() ) { // assuming OCC
        SOCC_Prs* occPrs = (SOCC_Prs*) prs;
        AIS_ListOfInteractive lst;
        occPrs->GetObjects( lst );
        if ( lst.Extent() ) {
          Handle(AIS_InteractiveObject) io = lst.First();
          if ( !io.IsNull() ) {
            Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(io);
            if ( !aSh.IsNull() )
              res = aSh->isShowVectors();
          }
        }
      }
      else if ( viewType == SVTK_Viewer::Type() ) { // assuming VTK
        SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( prs );
        vtkActorCollection* lst = vtkPrs ? vtkPrs->GetObjects() : 0;
        if ( lst ) {
          lst->InitTraversal();
          vtkActor* actor = lst->GetNextActor();
          if ( actor ) {
            GEOM_Actor* aGeomActor = GEOM_Actor::SafeDownCast(actor);
            if ( aGeomActor )
              res = aGeomActor->GetVectorMode();
            }
        }
      }
    }
  }

  return res;
}

bool GEOMGUI_Selection::isVerticesMode( const int index ) const
{
#ifdef USE_VISUAL_PROP_MAP
  QVariant v = visibleProperty( entry( index ), GEOM::propertyName( GEOM::Vertices ) );
  if ( v.canConvert( QVariant::Bool ) )
    return v.toBool();
#endif

  bool res = false;
  
  SALOME_View* view = GEOM_Displayer::GetActiveView();
  QString viewType = activeViewType();
  if ( view && ( viewType == OCCViewer_Viewer::Type() || viewType == SVTK_Viewer::Type() ) ) {
    SALOME_Prs* prs = view->CreatePrs( entry( index ).toLatin1().constData() );
    if ( prs ) {
      if ( viewType == OCCViewer_Viewer::Type() ) { // assuming OCC
        SOCC_Prs* occPrs = (SOCC_Prs*) prs;
        AIS_ListOfInteractive lst;
        occPrs->GetObjects( lst );
        if ( lst.Extent() ) {
          Handle(AIS_InteractiveObject) io = lst.First();
          if ( !io.IsNull() ) {
            Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(io);
            if ( !aSh.IsNull() )
              res = aSh->isShowVertices();
          }
        }
      }
      else if ( viewType == SVTK_Viewer::Type() ) { // assuming VTK
        SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( prs );
        vtkActorCollection* lst = vtkPrs ? vtkPrs->GetObjects() : 0;
        if ( lst ) {
          lst->InitTraversal();
          vtkActor* actor = lst->GetNextActor();
          if ( actor ) {
            GEOM_Actor* aGeomActor = GEOM_Actor::SafeDownCast(actor);
            if ( aGeomActor )
              res = aGeomActor->GetVerticesMode();
            }
        }
      }
    }
  }

  return res;
}

bool GEOMGUI_Selection::isNameMode( const int index ) const
{
#ifdef USE_VISUAL_PROP_MAP
  QVariant v = visibleProperty( entry( index ), GEOM::propertyName( GEOM::ShowName ) );
  if ( v.canConvert( QVariant::Bool ) )
    return v.toBool();
#endif

  bool res = false;

  SALOME_View* view = GEOM_Displayer::GetActiveView();
  QString viewType = activeViewType();
  if ( view && ( viewType == OCCViewer_Viewer::Type() || viewType == SVTK_Viewer::Type() ) ) {
    SALOME_Prs* prs = view->CreatePrs( entry( index ).toLatin1().constData() );
    if ( prs ) {
      if ( viewType == OCCViewer_Viewer::Type() ) { // assuming OCC
        SOCC_Prs* occPrs = (SOCC_Prs*) prs;
        AIS_ListOfInteractive lst;
        occPrs->GetObjects( lst );
        if ( lst.Extent() ) {
          Handle(AIS_InteractiveObject) io = lst.First();
          if ( !io.IsNull() ) {
            Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(io);
            if ( !aSh.IsNull() )
              res = aSh->isShowName();
          }
        }
      }
      else if ( viewType == SVTK_Viewer::Type() ) { // assuming VTK
        SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( prs );
        vtkActorCollection* lst = vtkPrs ? vtkPrs->GetObjects() : 0;
        if ( lst ) {
          lst->InitTraversal();
          vtkActor* actor = lst->GetNextActor();
          if ( actor ) {
            GEOM_Actor* aGeomActor = GEOM_Actor::SafeDownCast(actor);
            if ( aGeomActor )
              res = aGeomActor->GetNameMode();
            }
        }
      }
    }
  }

  return res;
}

bool GEOMGUI_Selection::hasChildren( const _PTR(SObject)& obj )
{
  if ( obj ) {
    // as soon as Use Case browser data tree was added
    return obj->GetStudy()->GetUseCaseBuilder()->HasChildren( obj );
  }
}

bool GEOMGUI_Selection::expandable( const _PTR(SObject)& obj )
{
  bool exp = true;
  _PTR(GenericAttribute) anAttr;
  if ( obj && obj->FindAttribute( anAttr, "AttributeExpandable" ) ) {
    _PTR(AttributeExpandable) aAttrExp = anAttr;
    exp = aAttrExp->IsExpandable();
  }
  return exp;
}

bool GEOMGUI_Selection::isCompoundOfVertices( GEOM::GEOM_Object_ptr obj )
{
  bool ret = false;
  /*
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
  (SUIT_Session::session()->activeApplication()->activeStudy());*/
  if ( /*appStudy && */!CORBA::is_nil( obj ) )
    ret = obj->GetShapeType() == GEOM::COMPOUND && obj->GetMaxShapeType() == GEOM::VERTEX;
  return ret;
}

bool GEOMGUI_Selection::isFolder( const _PTR(SObject)& obj )
{
  bool ret = false;
  _PTR(GenericAttribute) anAttr;
  if ( obj && obj->FindAttribute(anAttr, "AttributeLocalID") ) {
    _PTR(AttributeLocalID) aLocalID( anAttr );
    ret = aLocalID->Value() == 999;
  }
  return ret;
}

bool GEOMGUI_Selection::hasChildren( const int index ) const
{
  bool ok = false;
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( study() );

  if ( appStudy ) {
    QString anEntry = entry( index );
    _PTR(Study) study = appStudy->studyDS();
    if ( study && !anEntry.isEmpty() ) {
      _PTR(SObject) aSO( study->FindObjectID( anEntry.toStdString() ) );
      ok = hasChildren( aSO );
    }
  }
  return ok;
}

int GEOMGUI_Selection::nbChildren( const int index ) const
{
  int nb = 0;
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( study() );

  if ( appStudy ) {
    QString anEntry = entry( index );
    _PTR(Study) study = appStudy->studyDS();
    if ( study && !anEntry.isEmpty() ) {
      _PTR(SObject) aSO( study->FindObjectID( anEntry.toStdString() ) );
      if ( aSO && aSO->GetStudy()->GetUseCaseBuilder()->IsUseCaseNode(aSO) ) {
        _PTR(UseCaseIterator) it = aSO->GetStudy()->GetUseCaseBuilder()->GetUseCaseIterator( aSO ); 
        for (it->Init(false); it->More(); it->Next()) nb++;
      }
    }
  }
  return nb;
}

bool GEOMGUI_Selection::hasConcealedChildren( const int index ) const
{
  bool OK = false;
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( study() );

  if ( appStudy ) {
    QString anEntry = entry( index );
    _PTR(Study) study = appStudy->studyDS();
    if ( study && !anEntry.isEmpty() ) {
      _PTR(SObject) aSO( study->FindObjectID( anEntry.toStdString() ) );
      OK = !expandable( aSO ) && hasChildren( aSO );
    }
  }
  return OK;
}

bool GEOMGUI_Selection::hasDisclosedChildren( const int index ) const
{
  bool OK = false;
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( study() );

  if ( appStudy )  {
    QString anEntry = entry( index );
    _PTR(Study) study = appStudy->studyDS();
    if ( study && !anEntry.isEmpty() ) {
      _PTR(SObject) aSO( study->FindObjectID( anEntry.toStdString() ) );
      OK = expandable( aSO ) && hasChildren( aSO );
    }
  }
  return OK;
}

bool GEOMGUI_Selection::compoundOfVertices( const int index ) const
{
  GEOM::GEOM_Object_var obj = getObject( index );
  return isCompoundOfVertices( obj );
}

bool GEOMGUI_Selection::isComponent( const int index ) const
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( study() );

  if ( appStudy ) {
    QString anEntry = entry( index );
    _PTR(Study) study = appStudy->studyDS();
    if ( study && !anEntry.isNull() ) {
      _PTR(SObject) aSO( study->FindObjectID( anEntry.toStdString() ) );
      if ( aSO && aSO->GetFatherComponent() )
        return aSO->GetFatherComponent()->GetIOR() == aSO->GetIOR();
    }
  }
  return false;
}

GEOM::GEOM_Object_ptr GEOMGUI_Selection::getObject( const int index ) const
{
  GEOM::GEOM_Object_var o;
  if ( 0 <= index && index < myObjects.size() )
    o = GEOM::GEOM_Object::_narrow( myObjects[index] );
  return o._retn();
}

GEOM::GEOM_BaseObject_ptr GEOMGUI_Selection::getBaseObject( const int index ) const
{
  GEOM::GEOM_BaseObject_var o;
  if ( 0 <= index && index < myObjects.size() )
    o = GEOM::GEOM_BaseObject::_duplicate( myObjects[index] );
  return o._retn();
}

QString GEOMGUI_Selection::selectionMode() const
{
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( study()->application() );
  if ( app ) {
    GeometryGUI* aGeomGUI = dynamic_cast<GeometryGUI*>( app->module( "Geometry" ) );
    if ( aGeomGUI ) {
      switch ( aGeomGUI->getLocalSelectionMode() )
      {
        case GEOM_POINT      : return "VERTEX";
        case GEOM_EDGE       : return "EDGE";
        case GEOM_WIRE       : return "WIRE";
        case GEOM_FACE       : return "FACE";
        case GEOM_SHELL      : return "SHELL";
        case GEOM_SOLID      : return "SOLID";
        case GEOM_COMPOUND   : return "COMPOUND";
        case GEOM_ALLOBJECTS : return "ALL";
        default: return "";
      }
    }
  }
  return "";
}

bool GEOMGUI_Selection::topLevel( const int index ) const
{
#ifdef USE_VISUAL_PROP_MAP
  QVariant v = visibleProperty( entry( index ), GEOM::propertyName( GEOM::TopLevel ) );
  if ( v.canConvert<bool>() )
    return v.toBool();
#endif

  bool res = false;

  SALOME_View* view = GEOM_Displayer::GetActiveView();
  QString viewType = activeViewType();
  if ( view && viewType == OCCViewer_Viewer::Type() ) {
    SALOME_Prs* prs = view->CreatePrs( entry( index ).toLatin1().constData() );
    if ( prs ) {
      if ( viewType == OCCViewer_Viewer::Type() ) { // assuming OCC
        SOCC_Prs* occPrs = (SOCC_Prs*) prs;
        AIS_ListOfInteractive lst;
        occPrs->GetObjects( lst );
        if ( lst.Extent() ) {
          Handle(AIS_InteractiveObject) io = lst.First();
          if ( !io.IsNull() ) {
            Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(io);
            if ( !aSh.IsNull() )
              res = (bool)aSh->isTopLevel();
          }
        }
      }
    }
  }
  return res;
}

bool GEOMGUI_Selection::isPhysicalMaterial( const int idx ) const
{
#ifdef USE_VISUAL_PROP_MAP
  QVariant v = visibleProperty( entry( idx ), GEOM::propertyName( GEOM::Material ) );
  if ( v.canConvert<QString>() ) {
    Material_Model material;
    material.fromProperties( v.toString() );
    return material.isPhysical();
  }
#endif

  bool res = false;
  
  SALOME_View* view = GEOM_Displayer::GetActiveView();
  QString viewType = activeViewType();
  if ( view ) {
    SALOME_Prs* prs = view->CreatePrs( entry( idx ).toLatin1().constData() );
    if ( prs ) {
      if ( viewType == OCCViewer_Viewer::Type() ) { // assuming OCC
        SOCC_Prs* occPrs = (SOCC_Prs*) prs;
        AIS_ListOfInteractive lst;
        occPrs->GetObjects( lst );
        if ( lst.Extent() ) {
          Handle(AIS_InteractiveObject) io = lst.First();
          if ( !io.IsNull() ) {
            Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(io);
            if ( !aSh.IsNull() )
              res = (bool) aSh->Attributes()->ShadingAspect()->
                Material(Aspect_TOFM_BOTH_SIDE).MaterialType( Graphic3d_MATERIAL_PHYSIC );
          }
        }
      }
      else if ( viewType == SVTK_Viewer::Type() ) { // assuming VTK
        SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( prs );
        vtkActorCollection* lst = vtkPrs ? vtkPrs->GetObjects() : 0;
        if ( lst ) {
          lst->InitTraversal();
          vtkActor* actor = lst->GetNextActor();
          if ( actor ) {
            GEOM_Actor* aGeomGActor = GEOM_Actor::SafeDownCast( actor );
            if ( aGeomGActor ) {
              GEOM_VTKPropertyMaterial* mat  = GEOM_VTKPropertyMaterial::SafeDownCast(aGeomGActor->GetProperty());
              if ( mat )
                res = mat->GetPhysical();
            } // if ( salome actor )
          } // if ( actor )
        } // if ( lst == vtkPrs->GetObjects() )
      }
    }
  }

  return res;
}

bool GEOMGUI_Selection::isFolder( const int index ) const
{
  bool res = false;
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( study() );

  if ( appStudy ) {
    QString anEntry = entry( index );
    _PTR(Study) study = appStudy->studyDS();
    if ( study && !anEntry.isNull() ) {
      _PTR(SObject) aSO( study->FindObjectID( anEntry.toStdString() ) );
      if ( aSO ) res = isFolder( aSO );
    }
  }
  return res;
}

bool GEOMGUI_Selection::hasDimensions( const int theIndex, bool& theHidden, bool& theVisible ) const
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( study() );

  if ( !appStudy )
  {
    return false;
  }

  QString anEntry = entry( theIndex );
  _PTR(Study) aStudy = appStudy->studyDS();
  if ( !aStudy || anEntry.isNull() )
  {
    return false;
  }

  GEOMGUI_DimensionProperty aDimensions( appStudy, anEntry.toStdString() );

  theHidden  = false;
  theVisible = false;

  for ( int it = 0; it < aDimensions.GetNumber(); ++it )
  {
    if ( aDimensions.IsVisible( it ) )
      theVisible = true;
    else
      theHidden = true;
  }

  return aDimensions.GetNumber() > 0;
}

bool GEOMGUI_Selection::hasHiddenDimensions( const int theIndex ) const
{
  bool isAnyVisible = false;
  bool isAnyHidden = false;
  if ( !hasDimensions( theIndex, isAnyHidden, isAnyVisible ) )
  {
    return false;
  }

  return isAnyHidden;
}

bool GEOMGUI_Selection::hasVisibleDimensions( const int theIndex ) const
{
  bool isAnyVisible = false;
  bool isAnyHidden = false;
  if ( !hasDimensions( theIndex, isAnyHidden, isAnyVisible ) )
  {
    return false;
  }

  return isAnyVisible;
}

