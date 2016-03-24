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
// File   : GEOMBase.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "GEOMBase.h"

#include <GeometryGUI.h>
#include <GEOM_Client.hxx>

////  SALOME Includes
#include <GEOM_Actor.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>
#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewWindow.h>
#include <SOCC_ViewModel.h>
#include <SOCC_Prs.h>

#include <SALOME_ListIO.hxx>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_MessageBox.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>

// // Open CASCADE Includes
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepPrimAPI_MakeCone.hxx>

#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS.hxx>

#include <Precision.hxx>

#include <vtkRenderer.h>

#include <set>

//=====================================================================================
// function : GetShapeFromIOR()
// purpose  : Get shape data by the specified IOR
//=====================================================================================
TopoDS_Shape GEOMBase::GetShapeFromIOR( const QString& IOR )
{
  GEOM::GEOM_Object_var geomObj = GEOMBase::GetObjectFromIOR( IOR );
  TopoDS_Shape shape;
  GetShape( geomObj, shape, TopAbs_SHAPE );
  return shape;
}


//=====================================================================================
// function : GetIndex()
// purpose  : Get the index of a sub-shape in a main shape : index start at 1
//=====================================================================================
int GEOMBase::GetIndex( const TopoDS_Shape& subshape, const TopoDS_Shape& shape )
{
  int idx = -1;
  if ( !shape.IsNull() && !subshape.IsNull() ) {
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes( shape, anIndices );
    if ( anIndices.Contains( subshape ) ) 
      idx = anIndices.FindIndex( subshape );
  }
  return idx;
}


//=======================================================================
// function : GetTopoFromSelection()
// purpose  : Define tds from a single selection and retuen true
//=======================================================================
TopoDS_Shape GEOMBase::GetTopoFromSelection( const SALOME_ListIO& IObjects )
{
  TopoDS_Shape shape;
  if ( IObjects.Extent() == 1 ){
    Handle(SALOME_InteractiveObject) IO = IObjects.First();
    SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
    if ( IO->hasEntry() && study ) {
      _PTR(Study) studyDS = study->studyDS();
      _PTR(SObject) obj( studyDS->FindObjectID( IO->getEntry() ) );
      _PTR(GenericAttribute) anAttr;
      if ( obj && obj->FindAttribute( anAttr, "AttributeIOR" ) ) {
        _PTR(AttributeIOR) anIOR( anAttr );
        shape = GetShapeFromIOR( anIOR->Value().c_str() );
      }
    }
  }
  return shape;
}

//=======================================================================
// function : GetNameOfSelectedIObjects()
// purpose  : Define the name geom++ or other name of mono or multi sel.
//=======================================================================
int GEOMBase::GetNameOfSelectedIObjects( const SALOME_ListIO& IObjects,
                                         QString&             name,
                                         const bool           shapesOnly )
{
  int nbSel = 0;
  name = ""; // clear output name

  if ( !shapesOnly ) {
    nbSel = IObjects.Extent();
    if ( nbSel == 1 ) {
      Handle(SALOME_InteractiveObject) anIObj = IObjects.First();
      SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
      if ( anIObj->hasEntry() && study ) {
        _PTR(Study) studyDS = study->studyDS();
        _PTR(SObject) obj( studyDS->FindObjectID( anIObj->getEntry() ) );
        _PTR(GenericAttribute) anAttr;
        if ( obj && obj->FindAttribute( anAttr, "AttributeName" ) ) {
          _PTR(AttributeName) aNameAttr ( anAttr );
          name = aNameAttr->Value().c_str();
        }
      }
    }
  }
  else {
    GEOM::ListOfGO anObjs;
    ConvertListOfIOInListOfGO( IObjects, anObjs, shapesOnly );
    nbSel = anObjs.length();
    if ( nbSel == 1 )
      name = GetName( anObjs[ 0 ] );
  }

  if ( nbSel > 1 )
    name = QObject::tr( "%1_objects" ).arg( nbSel );

  return nbSel;
}


//=================================================================================
// function : GetShapeTypeString()
// purpose  : for a single shape
//=================================================================================
QString GEOMBase::GetShapeTypeString(const TopoDS_Shape& shape)
{
  QString aTypeString;
  if ( !shape.IsNull() ) {
    switch ( shape.ShapeType() ) {
    case TopAbs_COMPOUND:
      {
        aTypeString = QObject::tr( "GEOM_COMPOUND" );
        break;
      }
    case TopAbs_COMPSOLID:
      {
        aTypeString = QObject::tr( "GEOM_COMPOUNDSOLID" );
        break;
      }
    case TopAbs_SOLID:
      {
        aTypeString = QObject::tr( "GEOM_SOLID" );
        break;
      }
    case TopAbs_SHELL:
      {
        aTypeString = QObject::tr( "GEOM_SHELL" );
        break;
      }
    case TopAbs_FACE:
      {
        BRepAdaptor_Surface surf( TopoDS::Face( shape ) );
        switch ( surf.GetType() ) {
        case GeomAbs_Plane:
          {
            aTypeString = QObject::tr( "GEOM_PLANE" );
            break;
          }
        case GeomAbs_Cylinder:
          {
            aTypeString = QObject::tr( "GEOM_SURFCYLINDER" );
            break;
          }
        case GeomAbs_Sphere:
          {
            aTypeString = QObject::tr( "GEOM_SURFSPHERE" );
            break;
          }
        case GeomAbs_Torus:
          {
            aTypeString = QObject::tr( "GEOM_SURFTORUS" );
            break;
          }
        case GeomAbs_Cone:
          {
            aTypeString = QObject::tr( "GEOM_SURFCONE" );
            break;
          }
        default:
          {
            aTypeString = QObject::tr( "GEOM_FACE" );
            break;
          }
        }
        break;
      }
    case TopAbs_WIRE:
      {
        aTypeString = QObject::tr( "GEOM_WIRE" );
        break;
      }
    case TopAbs_EDGE:
      {
        BRepAdaptor_Curve curv( TopoDS::Edge( shape ) );
        switch ( curv.GetType() ) {
        case GeomAbs_Line:
          {
            aTypeString = ( qAbs( curv.FirstParameter() ) >= 1E6 || qAbs( curv.LastParameter() ) >= 1E6 ) ?
              QObject::tr( "GEOM_LINE" ) : QObject::tr( "GEOM_EDGE" );
            break;
          }
        case GeomAbs_Circle:
          {
            aTypeString = curv.IsClosed() ? QObject::tr( "GEOM_CIRCLE" ) : QObject::tr( "GEOM_ARC" );
            break;
          }
        default:
          {
            aTypeString = QObject::tr( "GEOM_EDGE" );
            break;
          }
        }
        break;
      }
    case TopAbs_VERTEX:
      {
        aTypeString = QObject::tr( "GEOM_VERTEX" );
        break;
      }
    case TopAbs_SHAPE:
      {
        aTypeString = QObject::tr( "GEOM_SHAPE" );
        break;
      }
    default:
      {
        break;
      }
    }
  }
  return aTypeString;
}


//=======================================================================
// function : ConvertIORinGEOMAISShape()
// purpose  :
//=======================================================================
Handle(GEOM_AISShape) GEOMBase::ConvertIORinGEOMAISShape(const QString& IOR, bool onlyInActiveView)
{
  Handle(GEOM_AISShape) shape;

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( study ) {
    _PTR(Study) studyDS = study->studyDS();
    _PTR(SObject) obj( studyDS->FindObjectIOR( IOR.toLatin1().constData() ) );
    if ( obj ) {
      QList<SUIT_ViewWindow*> views;
      if ( onlyInActiveView ) 
        views.append( SUIT_Session::session()->activeApplication()->desktop()->activeWindow() );
      else 
        views = SUIT_Session::session()->activeApplication()->desktop()->windows();
      foreach ( SUIT_ViewWindow* view, views ) {
        if ( view && view->getViewManager()->getType() == OCCViewer_Viewer::Type() ) {
          Handle(AIS_InteractiveContext) ic = ((OCCViewer_Viewer*)view->getViewManager()->getViewModel())->getAISContext();

          AIS_ListOfInteractive displayed;
          ic->DisplayedObjects( displayed );
          AIS_ListIteratorOfListOfInteractive it( displayed );
          while ( it.More() && shape.IsNull() ) {
            if ( it.Value()->IsInstance( STANDARD_TYPE(GEOM_AISShape) ) ) {
              Handle(GEOM_AISShape) sh = Handle(GEOM_AISShape)::DownCast( it.Value() );
              if ( !sh.IsNull() && sh->hasIO() ) {
                Handle(SALOME_InteractiveObject) IO = Handle(SALOME_InteractiveObject)::DownCast( sh->getIO() );
                if ( !IO.IsNull() && IO->hasEntry() && obj->GetID() == IO->getEntry() )
                  shape = sh;
              }
            }
            it.Next();
          }
        }
        if ( !shape.IsNull() ) break;
      }
    }
  }
  return shape;
}


//=======================================================================
// function : ConvertIORinGEOMActor()
// purpose  :
//=======================================================================
GEOM_Actor* GEOMBase::ConvertIORinGEOMActor(const QString& IOR, bool onlyInActiveView)
{
  GEOM_Actor* actor = 0;

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( study ) {
    _PTR(Study) studyDS = study->studyDS();
    _PTR(SObject) obj( studyDS->FindObjectIOR( IOR.toLatin1().constData() ) );
    if ( obj ) {
      QList<SUIT_ViewWindow*> views;
      if ( onlyInActiveView ) 
        views.append( SUIT_Session::session()->activeApplication()->desktop()->activeWindow() );
      else 
        views = SUIT_Session::session()->activeApplication()->desktop()->windows();
      foreach ( SUIT_ViewWindow* view, views ) {
        if ( view && view->getViewManager()->getType() == SVTK_Viewer::Type() ) {
          SVTK_ViewWindow* aVTKViewWindow = dynamic_cast<SVTK_ViewWindow*>( view );
          if( !aVTKViewWindow )
            continue;
          vtkRenderer* Renderer = aVTKViewWindow->getRenderer();
          vtkActorCollection* theActors = Renderer->GetActors();
          theActors->InitTraversal();
          vtkActor* a = theActors->GetNextActor();
          while( a && !actor ) {
            if ( a->IsA( "GEOM_Actor" ) ) {
              GEOM_Actor* ga = GEOM_Actor::SafeDownCast( a );
              if ( ga && ga->hasIO() ) {
                Handle(SALOME_InteractiveObject) IO = Handle(SALOME_InteractiveObject)::DownCast( ga->getIO() );
                if ( !IO.IsNull() && IO->hasEntry() && obj->GetID() == IO->getEntry() )
                  actor = ga;
              }
            }
            a = theActors->GetNextActor();
          }
        }
        if ( actor ) break;
      }
    }
  }
  return actor;
}

//=======================================================================
// function : GetAIS()
// purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) GEOMBase::GetAIS( const Handle(SALOME_InteractiveObject)& IO,
                                                bool onlyInActiveView, bool onlyGeom )
{
  Handle(AIS_InteractiveObject) aisObject;

  if ( !IO.IsNull() && IO->hasEntry() ) {
    QList<SUIT_ViewWindow*> views;
    if ( onlyInActiveView ) 
      views.append( SUIT_Session::session()->activeApplication()->desktop()->activeWindow() );
    else 
      views = SUIT_Session::session()->activeApplication()->desktop()->windows();
    
    foreach ( SUIT_ViewWindow* view, views ) {
      if ( view && view->getViewManager()->getType() == OCCViewer_Viewer::Type() ) {
        OCCViewer_Viewer* occViewer=(OCCViewer_Viewer*)view->getViewManager()->getViewModel();
        SOCC_Viewer* soccViewer = dynamic_cast<SOCC_Viewer*>(occViewer);
        if (soccViewer) {
          SOCC_Prs* occPrs = dynamic_cast<SOCC_Prs*>( soccViewer->CreatePrs( IO->getEntry() ) );
          if ( occPrs && !occPrs->IsNull() ) {
            AIS_ListOfInteractive shapes; occPrs->GetObjects( shapes );
            if( !shapes.Extent() ) continue;
            aisObject=shapes.First();
            delete occPrs;
          }
        }
      }
      if ( !aisObject.IsNull() ) break;
    } // foreach
  }

  return aisObject;
}


//=======================================================================
// function : ConvertIOinGEOMAISShape()
// purpose  :
//=======================================================================
Handle(GEOM_AISShape) GEOMBase::ConvertIOinGEOMAISShape( const Handle(SALOME_InteractiveObject)& IO, bool onlyInActiveView )
{
  return Handle(GEOM_AISShape)::DownCast( GetAIS( IO, onlyInActiveView, true ) );
}


//=======================================================================
// function : ConvertListOfIOInListOfIOR()
// purpose  :
//=======================================================================
QStringList GEOMBase::ConvertListOfIOInListOfIOR( const SALOME_ListIO& IObjects )
{
  QStringList iors;
  SALOME_ListIteratorOfListIO it( IObjects );
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( study ) {
    _PTR(Study) studyDS = study->studyDS();
    for ( ; it.More(); it.Next() ) {
      GEOM::GEOM_Object_var geomObj = ConvertIOinGEOMObject( it.Value() );
      if ( !CORBA::is_nil( geomObj ) )
        iors.append( GetIORFromObject( geomObj ) );
    }
  }
  return iors;
}


//=======================================================================
// function : ConvertIOinGEOMObject()
// purpose  :
//=======================================================================
GEOM::GEOM_Object_ptr GEOMBase::ConvertIOinGEOMObject( const Handle(SALOME_InteractiveObject)& IO )
{
  GEOM::GEOM_Object_var object;

  if ( !IO.IsNull() && IO->hasEntry() ) {
    SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
    if ( study ) {
      _PTR(Study) studyDS = study->studyDS();
      _PTR(SObject) obj( studyDS->FindObjectID( IO->getEntry() ) );
      if ( obj ) {
        CORBA::Object_var corbaObj = GeometryGUI::ClientSObjectToObject( obj );
        if ( !CORBA::is_nil( corbaObj ) )
          object = GEOM::GEOM_Object::_narrow( corbaObj );
      }
    }
  }
  return object._retn();
}


//=======================================================================
// function : ConvertListOfIOInListOfGO()
// purpose  :
//=======================================================================
void GEOMBase::ConvertListOfIOInListOfGO( const SALOME_ListIO& IObjects,
                                          GEOM::ListOfGO&      geomObjects,
                                          bool                 shapesOnly )
{
  geomObjects.length( 0 );

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( study ) {
    _PTR(Study) studyDS = study->studyDS();

    geomObjects.length( IObjects.Extent() );
    SALOME_ListIteratorOfListIO it( IObjects );

    int i = 0;
    for ( ; it.More(); it.Next() ) {
      GEOM::GEOM_Object_var geomObj = ConvertIOinGEOMObject( it.Value() );
      if ( !CORBA::is_nil( geomObj ) && ( !shapesOnly || IsShape( geomObj ) ) )
        geomObjects[ i++ ] = geomObj;
    }
    geomObjects.length( i );
  }
}


//=================================================================================
// function : CreateArrowForLinearEdge()
// purpose  : Create a cone topology to be used to display an arrow in the middle
//          : of an edge showing its orientation. (For simulation and Viewer OCC only)
//=================================================================================
TopoDS_Shape GEOMBase::CreateArrowForLinearEdge( const TopoDS_Shape& shape )
{
  TopoDS_Shape ArrowCone;

  SUIT_ViewWindow* view = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  if ( view && view->getViewManager()->getType() == OCCViewer_Viewer::Type() && shape.ShapeType() == TopAbs_EDGE ) {
    Handle(V3d_View) view3d = ((OCCViewer_ViewWindow*)view)->getViewPort()->getView();
    Standard_Real Width, Height;
    view3d->Size( Width, Height );
    const Standard_Real aHeight = (Width + Height) / 50.0;

    try {
      Standard_Real first, last;
      Handle(Geom_Curve) curv = BRep_Tool::Curve( TopoDS::Edge( shape ), first, last );
      if ( !curv.IsNull() && curv->IsCN(1) ) {
        const Standard_Real param = ( first+last ) / 2.0;
        gp_Pnt middleParamPoint;
        gp_Vec V1;
        curv->D1( param, middleParamPoint, V1 );
        if ( V1.Magnitude() > Precision::Confusion() ) {
          /* Topology orientation not geom orientation */
          if ( shape.Orientation() == TopAbs_REVERSED )
            V1 *= -1.0;

          gp_Ax2 anAxis( middleParamPoint, gp_Dir( V1 ) );
          const Standard_Real radius1 = aHeight / 5.0;
          if ( radius1 > 10.0 * Precision::Confusion() && aHeight > 10.0 * Precision::Confusion() )
            ArrowCone = BRepPrimAPI_MakeCone( anAxis, radius1, 0.0, aHeight ).Shape();
        }
      }
    }
    catch ( Standard_Failure ) {
      // OCC failures are hard to catch in GUI.
      // This is because of the position for #include <Standard_ErrorHandler.hxx> that is very critical to find
      // in SALOME environment : compilation error !
    }
  }

  return ArrowCone;
}


//=================================================================================
// function : VertexToPoint()
// purpose  : If S can be converted in a gp_Pnt returns true and the result is P
//=================================================================================
bool GEOMBase::VertexToPoint( const TopoDS_Shape& shape, gp_Pnt& point )
{
  if ( shape.IsNull() || shape.ShapeType() != TopAbs_VERTEX )
    return false;
  point = BRep_Tool::Pnt( TopoDS::Vertex( shape ) );
  return true;
}


//=================================================================================
// function : GetBipointDxDyDz()
// purpose  :
//=================================================================================
void GEOMBase::GetBipointDxDyDz( const gp_Pnt& point1, const gp_Pnt& point2, double& dx, double& dy, double& dz )
{
  dx = point2.X() - point1.X();
  dy = point2.Y() - point1.Y();
  dz = point2.Z() - point1.Z();
}


//=================================================================================
// function : LinearEdgeExtremities()
// purpose  : If S can be converted in a linear edge and if initial an final points
//          : distance is sufficient, returns true else returns false.
//          : Resulting points are respectively P1 and P2
//=================================================================================
bool GEOMBase::LinearEdgeExtremities( const TopoDS_Shape& shape,  gp_Pnt& point1, gp_Pnt& point2 )
{
  if ( shape.IsNull() || shape.ShapeType() != TopAbs_EDGE )
    return false;

  BRepAdaptor_Curve curv( TopoDS::Edge( shape ) );
  if ( curv.GetType() != GeomAbs_Line )
    return false;
  
  gp_Pnt p1, p2;

  curv.D0( curv.FirstParameter(), p1 );
  curv.D0( curv.LastParameter(),  p2 );

  if ( p1.Distance( p2 ) <= Precision::Confusion() )
    return false;

  point1 = p1;
  point2 = p2;
  return true;
}


//=======================================================================
// function : SelectionByNameInDialogs()
// purpose  : Called when user has entered a name of object in a LineEdit.
//          : The selection is changed. Dialog box will receive the
//          : corresponding signal to manage this event.
//=======================================================================
bool GEOMBase::SelectionByNameInDialogs( QWidget* widget, const QString& objectUserName, const SALOME_ListIO& /*IObjects*/ )
{
  /* Find SObject with name in component GEOM */
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( !study ) return false;
  _PTR(Study) studyDS = study->studyDS();

  std::vector<_PTR(SObject)> listSO = studyDS->FindObjectByName( objectUserName.toStdString(), "GEOM" );

  if ( listSO.size() < 1 ) {
    SUIT_MessageBox::critical( widget, 
                               QObject::tr( "GEOM_WRN_WARNING" ),
                               QObject::tr( "GEOM_NAME_INCORRECT" ),
                               QObject::tr( "GEOM_BUT_OK" ) );
    return false;
  }

  /* More than one object with same name */
  if ( listSO.size() > 1 ) {
    SUIT_MessageBox::critical( widget,
                               QObject::tr("GEOM_WRN_WARNING"),
                               QObject::tr("GEOM_IDENTICAL_NAMES_SELECT_BY_MOUSE"),
                               QObject::tr("GEOM_BUT_OK") );
    return false;
  }

  /* Create a SALOME_InteractiveObject with a SALOME::SObject */
  Handle(SALOME_InteractiveObject) IO = new SALOME_InteractiveObject( listSO[0]->GetID().c_str(),
                                                                      "GEOM",
                                                                      objectUserName.toLatin1().constData() );

  /* Add as a selected object       */
  /* Clear any previous selection : */
  /* Warning the LineEdit is purged because of signal currentSelectionChanged ! */
  // Sel->ClearIObjects(); //mzn
  // Sel->AddIObject(SI); //mzn
  return true;
}


//=======================================================================
// function : DefineDlgPosition()
// purpose  : Define x and y the default position for a dialog box
//=======================================================================
void GEOMBase::DefineDlgPosition( QWidget* dlg, int& x, int& y )
{
  /* Here the position is on the bottom right corner - 10 */
  SUIT_Desktop* d = SUIT_Session::session()->activeApplication()->desktop();
  x = abs( d->x() + d->size().width()  - dlg->size().width()  - 10 );
  y = abs( d->y() + d->size().height() - dlg->size().height() - 10 );
}


//=======================================================================
// function : GetDefaultName()
// purpose  : Generates default names
//=======================================================================
QString GEOMBase::GetDefaultName( const QString& operation, bool extractPrefix )
{
  QString aName = "";

  // collect all object names of GEOM component
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( study ) {
    _PTR(Study) studyDS = study->studyDS();

    std::set<std::string> names;
    _PTR(SComponent) component( studyDS->FindComponent( "GEOM" ) );
    if ( component ) {
      _PTR(ChildIterator) it( studyDS->NewChildIterator( component ) );
      for ( it->InitEx( true ); it->More(); it->Next() ) {
        names.insert( it->Value()->GetName() );
      }
    }

    // build a unique name
    int aNumber = 0;
    bool isUnique = false;
    QString prefix = operation;

    if ( extractPrefix ) {
      QStringList parts = prefix.split( "_", QString::KeepEmptyParts );
      if ( parts.count() > 1 ) {
        bool ok;
        aNumber = parts.last().toLong( &ok );
        if ( ok ) {
          parts.removeLast();
          prefix = parts.join( "_" );
          aNumber--;
        }
      }
    }
    
    while ( !isUnique ) {
      aName = prefix + "_" + QString::number( ++aNumber );
      isUnique = ( names.count( aName.toStdString()) == 0 );
    }
  }
  return aName;
}


//=======================================================================
// function : ShowErrorMessage()
// purpose  : Shows message box with error code and comment
//=======================================================================
void GEOMBase::ShowErrorMessage( const QString& errorCode, const QString& comment )
{
  QStringList text;
  text << QObject::tr( "GEOM_PRP_ABORT" );
  if ( !errorCode.isEmpty() )
    text << QObject::tr( errorCode.toLatin1().constData() );
  if ( !comment.isEmpty() )
    text << QObject::tr( comment.toLatin1().constData() );

  SUIT_MessageBox::critical( SUIT_Session::session()->activeApplication()->desktop(),
                             QObject::tr( "GEOM_ERROR" ),
                             text.join( "\n" ),
                             QObject::tr( "GEOM_BUT_OK" ) );
}


//=======================================================================
// function : GetObjectFromIOR()
// purpose  : returns a GEOM_Object by given IOR (string)
//=======================================================================
GEOM::GEOM_Object_ptr GEOMBase::GetObjectFromIOR( const QString& IOR )
{
  return GeometryGUI::GetObjectFromIOR (IOR);
}

//=======================================================================
// function : GetIORFromObject()
// purpose  : returns IOR of a given GEOM_Object
//=======================================================================
QString GEOMBase::GetIORFromObject( GEOM::GEOM_Object_ptr object )
{
  return GeometryGUI::GetIORFromObject (object);
}

//=======================================================================
// function : GetShape()
// purpose  : returns a TopoDS_Shape stored in GEOM_Object
//=======================================================================
bool GEOMBase::GetShape( GEOM::GEOM_Object_ptr object, TopoDS_Shape& shape, const TopAbs_ShapeEnum type )
{
  shape = TopoDS_Shape();
  if ( !CORBA::is_nil( object ) ) {
    TopAbs_ShapeEnum stype = (TopAbs_ShapeEnum)( object->GetShapeType() );
    if ( type == TopAbs_SHAPE || type == stype )
      shape = GEOM_Client::get_client().GetShape(  GeometryGUI::GetGeomGen(), object );
  }
  return !shape.IsNull();
}

//=======================================================================
// function : GetName()
// purpose  : Get name of object
//=======================================================================
QString GEOMBase::GetName( GEOM::GEOM_Object_ptr object )
{
  QString name;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  
  if ( !CORBA::is_nil( object ) ) {
    // 1. search if object is already published in the study
    CORBA::String_var IOR = SalomeApp_Application::orb()->object_to_string( object );
    if ( study && strcmp( IOR.in(), "" ) != 0 ) {
      _PTR(SObject) aSObj( study->studyDS()->FindObjectIOR( std::string( IOR.in() ) ) );
      _PTR(GenericAttribute) anAttr;
      if ( aSObj && aSObj->FindAttribute( anAttr, "AttributeName") ) {
        _PTR(AttributeName) aNameAttr( anAttr );
        name = aNameAttr->Value().c_str();
      }
    }
    
    // 2. if object is not found in the study, try default name
    if ( name.isEmpty() ) {
      if ( object->IsMainShape() ) {
        name = GetDefaultName( "geomObj" );
      }
      else {
        GEOM::GEOM_Object_var mainShape = object->GetMainShape();
        if ( !CORBA::is_nil( mainShape  ) ) { 
          GEOM::ListOfLong_var indices = object->GetSubShapeIndices();
          if ( indices->length() > 0 ) {
            TopAbs_ShapeEnum type = (TopAbs_ShapeEnum)( object->GetShapeType() );
            name = QString( "%1:%2_%3" ).arg( GetName( mainShape.in() ) )
              .arg( TypeName( type ) ).arg( indices[0] );
          }
        }
      }
    }
  }

  return name;
}

//=======================================================================
// function : IsShape()
// purpose  : Return TRUE if object is valid and has shape
//=======================================================================
bool GEOMBase::IsShape( GEOM::GEOM_Object_ptr object )
{
  return !object->_is_nil() && object->IsShape();
}

//=======================================================================
// function : TypeName()
// purpose  : Get string representation for the shape type
//=======================================================================
QString GEOMBase::TypeName( TopAbs_ShapeEnum type, bool capitalize )
{
  QString name = "shape";
  switch( type ) {
  case TopAbs_COMPSOLID:
    name = "compsolid"; break;
  case TopAbs_COMPOUND:
    name = "compound";  break;
  case TopAbs_SOLID:
    name = "solid";     break;
  case TopAbs_SHELL:
    name = "shell";     break;
  case TopAbs_FACE:
    name = "face";      break;
  case TopAbs_WIRE:
    name = "wire";      break;
  case TopAbs_EDGE:
    name = "edge";      break;
  case TopAbs_VERTEX:
    name = "vertex";    break;
  default:
    break;
  }
  if ( capitalize && !name.isEmpty() )
    name = name.left(1).toUpper() + name.mid(1);
  return name;
}

//================================================================
// Function : GetEntry
// Purpose  : Get study entry for the given object (if it is published)
//================================================================
QString GEOMBase::GetEntry( GEOM::GEOM_Object_ptr object )
{
  QString entry;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( study && !CORBA::is_nil( object ) ) {
    QString objIOR = GetIORFromObject( object );
    if ( !objIOR.isEmpty() ) {
      _PTR(SObject) SO( study->studyDS()->FindObjectIOR( objIOR.toLatin1().constData() ) );
      if ( SO )
        entry = SO->GetID().c_str();
    }
  }
  return entry;
}

//================================================================
// Function : PublishSubObject
// Purpose  : Publish sub-shape under the main object
//================================================================
void GEOMBase::PublishSubObject( GEOM::GEOM_Object_ptr object, const QString& name )
{
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( study && !CORBA::is_nil( object ) ) {
    _PTR(Study) studyDS = study->studyDS();
    QString entry = GetEntry( object );
    GEOM::GEOM_Object_var father = object->GetMainShape();
    QString fatherEntry = GetEntry( father );
    if ( entry.isEmpty() && !CORBA::is_nil( father ) && !fatherEntry.isEmpty() ) {
      QString aName = !name.isEmpty() ? name : GetName( object );
      GeometryGUI::GetGeomGen()->AddInStudy( GeometryGUI::ClientStudyToStudy( studyDS ),
                                             object, aName.toLatin1().data(), father.in() );
    }
  }
}

//================================================================
// Function : synchronize
// Purpose  : 
//================================================================
void GEOMBase::Synchronize( QList<GEOM::GeomObjPtr>& left, QList<GEOM::GeomObjPtr>& right )
{
  // 1. remove items from the "left" list that are not in the "right" list
  QMutableListIterator<GEOM::GeomObjPtr> it1( left );
  while ( it1.hasNext() ) {
    GEOM::GeomObjPtr o1 = it1.next();
    bool found = false;
    QMutableListIterator<GEOM::GeomObjPtr> it2( right );
    while ( it2.hasNext() && !found )
      found = o1 == it2.next();
    if ( !found )
      it1.remove();
  }
  // 2. add items from the "right" list that are not in the "left" list (to keep selection order)
  it1 = right;
  while ( it1.hasNext() ) {
    GEOM::GeomObjPtr o1 = it1.next();
    bool found = false;
    QMutableListIterator<GEOM::GeomObjPtr> it2( left );
    while ( it2.hasNext() && !found )
      found = o1 == it2.next();
    if ( !found )
      left << o1;
  }
}
