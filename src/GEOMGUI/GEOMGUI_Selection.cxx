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

// File   : GEOMGUI_Selection.cxx
// Author : Alexander SOLOVYOV, Open CASCADE S.A.S. (alexander.solovyov@opencascade.com)
//
#include "GEOMGUI_Selection.h"

#include "GeometryGUI.h"
#include "GEOM_Displayer.h"

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>

#include "LightApp_DataOwner.h"

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>

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

// OCCT Includes
#include <AIS.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListOfInteractive.hxx>

// VTK Includes
#include <vtkActorCollection.h>

GEOMGUI_Selection::GEOMGUI_Selection()
: LightApp_Selection()
{
}

GEOMGUI_Selection::~GEOMGUI_Selection()
{
}

QVariant GEOMGUI_Selection::parameter( const QString& p ) const
{
  if ( p == "isOCC" ) return QVariant( activeViewType() == OCCViewer_Viewer::Type() );
  if ( p == "selectionmode" ){ 
    return QVariant(selectionMode()); 
  }
  return LightApp_Selection::parameter( p );
}

QVariant GEOMGUI_Selection::parameter( const int ind, const QString& p ) const
{
//  if      ( p == "isVisible"   )    return QVariant( isVisible( ind ) );
// parameter isVisible is calculated in base SalomeApp_Selection
//  else
  if( p == "type" )
    return QVariant( typeName( ind ) );
  if( p == "typeid" )
    return QVariant( typeId( ind ) );
  else if ( p == "displaymode" )
    return QVariant( displayMode( ind ) );
  else if ( p == "isAutoColor" )
    return QVariant( isAutoColor( ind ) );
  else if ( p == "isVectorsMode" )
    return QVariant( isVectorsMode( ind ) );
  else if ( p == "hasHiddenChildren" )
    return QVariant( hasHiddenChildren( ind ) );
  else if ( p == "hasShownChildren" )
    return QVariant( hasShownChildren( ind ) );
  else if ( p == "compoundOfVertices" )
    return QVariant( compoundOfVertices( ind ) );
  else
    return LightApp_Selection::parameter( ind, p );
}

// the method to skip temporary objects from selection (called from LightApp)

bool GEOMGUI_Selection::processOwner( const LightApp_DataOwner* theOwner)
{
  return !theOwner->entry().contains("_");
}

QString GEOMGUI_Selection::typeName( const int index ) const
{
  if ( isComponent( index ) )
    return "Component";
  GEOM::GEOM_Object_var anObj = getObject( index );
  if ( !CORBA::is_nil( anObj ) ) {
    const int aGeomType = anObj->GetType();
    if ( aGeomType == GEOM_GROUP )
      return "Group";
    else
      return "Shape";
  }
  return "Unknown";
}

int GEOMGUI_Selection::typeId( const int index ) const
{
  int aType = -1;
  GEOM::GEOM_Object_var anObj = getObject( index );
  if ( !CORBA::is_nil( anObj ) )
    //aType = anObj->GetType();
    aType = (int)anObj->GetShapeType();
  return aType;
}

bool GEOMGUI_Selection::isVisible( const int index ) const
{
  GEOM::GEOM_Object_var obj = getObject( index );
  SALOME_View* view = GEOM_Displayer::GetActiveView();
  if ( !CORBA::is_nil( obj ) && view ) {
    Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject( entry( index ).toLatin1().constData(), "GEOM", "TEMP_IO" );
    return view->isVisible( io );
  }
  return false;
}

bool GEOMGUI_Selection::isAutoColor( const int index ) const
{
  GEOM::GEOM_Object_var obj = getObject( index );
  if ( !CORBA::is_nil( obj ) )
    return obj->GetAutoColor();
  return false;
}

QString GEOMGUI_Selection::displayMode( const int index ) const
{
  SALOME_View* view = GEOM_Displayer::GetActiveView();
  QString viewType = activeViewType();
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
            int dm = io->DisplayMode();
            if ( dm == AIS_WireFrame )
              return "Wireframe";
            else if ( dm == AIS_Shaded )
              return "Shading";
            else { // return default display mode of AIS_InteractiveContext
              OCCViewer_Viewer* occViewer = (OCCViewer_Viewer*) SUIT_Session::session()->activeApplication()->desktop(
                                            )->activeWindow()->getViewManager()->getViewModel();
              Handle(AIS_InteractiveContext) ic = occViewer->getAISContext();
              dm = ic->DisplayMode();
              if ( dm == AIS_WireFrame )
                return "Wireframe";
              else if ( dm == AIS_Shaded )
                return "Shading";
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
              if ( dm == 0 )
                return "Wireframe";
              else if ( dm == 1 )
                return "Shading";
            } // if ( salome actor )
          } // if ( actor )
        } // if ( lst == vtkPrs->GetObjects() )
      } // if VTK
    }
  }
  return "";
}

bool GEOMGUI_Selection::isVectorsMode( const int index ) const
{
  bool ret = false;
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
              ret = aSh->isShowVectors();
          }
        }
      } else if ( viewType == SVTK_Viewer::Type() ) { // assuming VTK
        SVTK_Prs* vtkPrs = dynamic_cast<SVTK_Prs*>( prs );
        vtkActorCollection* lst = vtkPrs ? vtkPrs->GetObjects() : 0;
        if ( lst ) {
          lst->InitTraversal();
          vtkActor* actor = lst->GetNextActor();
          if ( actor ) {
            GEOM_Actor* aGeomActor = GEOM_Actor::SafeDownCast(actor);
            if ( aGeomActor )
              ret = aGeomActor->GetVectorMode();
          }
        }
      }
    }
  }
  return ret;
}

bool GEOMGUI_Selection::hasChildren( const _PTR(SObject)& obj )
{
  bool ok = false;
  if ( obj ) {
    _PTR(ChildIterator) it ( obj->GetStudy()->NewChildIterator( obj ) );
    for ( ; it->More() && !ok; it->Next() ) {
      _PTR(SObject) child = it->Value();
      if ( child ) {
        _PTR(SObject) refObj;
        if ( child->ReferencedObject( refObj ) ) continue; // omit references
        if ( child->GetName() != "" ) ok = true;
      }
    }
  }
  return ok;
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
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
    (SUIT_Session::session()->activeApplication()->activeStudy());
  if ( appStudy && !CORBA::is_nil( obj ) && obj->GetShapeType() == GEOM::COMPOUND ) {
    GEOM::GEOM_IMeasureOperations_var anOper = GeometryGUI::GetGeomGen()->GetIMeasureOperations( appStudy->id() );
    QString whatIs = anOper->WhatIs( obj );
    QStringList data = whatIs.split( "\n", QString::SkipEmptyParts );
    int nbVertices = 0, nbCompounds = 0, nbOther = 0;
    foreach ( QString s, data ) {
      QString type = s.section( ":", 0, 0 ).trimmed().toLower();
      int cnt = s.section( ":", 1, 1 ).trimmed().toInt();
      if ( type == "vertex" ) nbVertices += cnt;
      else if ( type == "compound" ) nbCompounds += cnt;
      else if ( type != "shape" ) nbOther += cnt;
    }
    ret = nbVertices > 0 && nbCompounds == 1 && nbOther == 0;
    anOper->Destroy();
  }
  return ret;
}

bool GEOMGUI_Selection::hasHiddenChildren( const int index ) const
{
  bool OK = false;
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
    (SUIT_Session::session()->activeApplication()->activeStudy());

  if ( appStudy && index >= 0 && index < count() )  {
    _PTR(Study) study = appStudy->studyDS();
    QString anEntry = entry( index );

    if ( study && !anEntry.isEmpty() ) {
      _PTR(SObject) aSO( study->FindObjectID( anEntry.toStdString() ) );
      OK = !expandable( aSO ) && hasChildren( aSO );
    }
  }
  return OK;
}

bool GEOMGUI_Selection::hasShownChildren( const int index ) const
{
  bool OK = false;
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
    (SUIT_Session::session()->activeApplication()->activeStudy());

  if ( appStudy && index >= 0 && index < count() )  {
    _PTR(Study) study = appStudy->studyDS();
    QString anEntry = entry( index );

    if ( study && !anEntry.isEmpty() ) {
      _PTR(SObject) aSO( study->FindObjectID( anEntry.toStdString() ) );
      OK = expandable( aSO ) && hasChildren( aSO );
    }
  }
  return OK;
}

bool GEOMGUI_Selection::compoundOfVertices( const int index ) const
{
  return isCompoundOfVertices( getObject( index ) );
}

bool GEOMGUI_Selection::isComponent( const int index ) const
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
    (SUIT_Session::session()->activeApplication()->activeStudy());

  if ( appStudy && index >= 0 && index < count() )  {
    _PTR(Study) study = appStudy->studyDS();
    QString anEntry = entry( index );

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
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
    (SUIT_Session::session()->activeApplication()->activeStudy());

  if (appStudy && index >= 0 && index < count()) {
    _PTR(Study) study = appStudy->studyDS();
    QString anEntry = entry(index);

    if (study && !anEntry.isNull()) {
      _PTR(SObject) aSO (study->FindObjectID(anEntry.toStdString()));
      if (aSO) {
        CORBA::Object_var anObj = GeometryGUI::ClientSObjectToObject(aSO);
        return GEOM::GEOM_Object::_narrow(anObj);
      }
    }
  }
  return GEOM::GEOM_Object::_nil();
}

QString GEOMGUI_Selection::selectionMode() const
{
  SalomeApp_Application* app = (SalomeApp_Application*)(SUIT_Session::session()->activeApplication());
  if (app) {
    GeometryGUI* aGeomGUI = dynamic_cast<GeometryGUI*>( app->module( "Geometry" ) );
    if (aGeomGUI) {
      switch (aGeomGUI->getLocalSelectionMode())
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
