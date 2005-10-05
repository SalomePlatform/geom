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
//  File   : GEOMBase.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "GEOMBase.h"
#include "GeometryGUI.h"
#include "GEOMBase_aParameterDlg.h"

#include "GEOM_Client.hxx"

////  SALOME Includes
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"

#include "SALOME_LifeCycleCORBA.hxx"

#include "GEOM_AssemblyBuilder.h"
#include "GEOM_Actor.h"
#include "SVTK_RenderWindowInteractor.h"
#include "SVTK_ViewModel.h"
#include "SVTK_ViewWindow.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewWindow.h"

#include "SALOME_ListIO.hxx"
#include "SALOME_ListIteratorOfListIO.hxx"
#include "GEOM_AISTrihedron.hxx"

#include "SUIT_Session.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_MessageBox.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"

// // Open CASCADE Includes
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepPrimAPI_MakeCone.hxx>

#include <Geom_Circle.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <Precision.hxx>

#include <vtkRenderer.h>
#include <qvaluelist.h>
#include <qstringlist.h>

#include "GEOMImpl_Types.hxx"

using namespace std;

#include "SALOMEDSClient.hxx"


//=======================================================================
// function : GEOMBase()
// purpose  : Constructor
//=======================================================================
GEOMBase::GEOMBase()
{
}


//=======================================================================
// function : ~GEOMBase()
// purpose  : Destructor
//=======================================================================
GEOMBase::~GEOMBase()
{
}

//=====================================================================================
// function : GetShapeFromIOR()
// purpose  : exist also as static method !
//=====================================================================================
TopoDS_Shape GEOMBase::GetShapeFromIOR(QString IOR)
{
  TopoDS_Shape result;
  if(IOR.stripWhiteSpace().isEmpty())
    return result;

  CORBA::Object_var obj = SalomeApp_Application::orb()->string_to_object((char*)(IOR.latin1()));
  if(CORBA::is_nil(obj))
    return result;
  GEOM::GEOM_Object_var GeomObject = GEOM::GEOM_Object::_narrow( obj );
  if (GeomObject->_is_nil())
    return result;

  result = GEOM_Client().GetShape(GeometryGUI::GetGeomGen(), GeomObject);
  return result;
}


//=====================================================================================
// function : GetIndex()
// purpose  : Get the index of a sub shape in a main shape : index start at 1
//=====================================================================================
int GEOMBase::GetIndex(const TopoDS_Shape& subshape, const TopoDS_Shape& shape, int /*ShapeType*/)
{
  if(shape.IsNull() || subshape.IsNull())
    return -1;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(shape, anIndices);
  if(anIndices.Contains(subshape)) return anIndices.FindIndex(subshape);

  return -1;
}


//=======================================================================
// function : GetTopoFromSelection()
// purpose  : Define tds from a single selection and retuen true
//=======================================================================
bool GEOMBase::GetTopoFromSelection(const SALOME_ListIO& aList, TopoDS_Shape& tds)
{
  if(aList.Extent() != 1)
    return false;

  Handle(SALOME_InteractiveObject) IO = aList.First();
  /* case SObject */
  if(IO->hasEntry()) {
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
    if ( !appStudy ) return false;
    _PTR(Study) aStudy = appStudy->studyDS();

    _PTR(SObject) obj ( aStudy->FindObjectID(IO->getEntry()) );
    _PTR(GenericAttribute) anAttr;
    if( obj ) {
      if(obj->FindAttribute(anAttr, "AttributeIOR")) {
	_PTR(AttributeIOR) anIOR ( anAttr );
	tds = GetShapeFromIOR(anIOR->Value().c_str());
	if(tds.IsNull())
	  return false;
	else
	  return true;
      }
    }
  }

  return false;
}

//=======================================================================
// function : GetNameOfSelectedIObjects()
// purpose  : Define the name geom++ or other name of mono or multi sel.
//=======================================================================
int GEOMBase::GetNameOfSelectedIObjects( const SALOME_ListIO& aList,
                                         QString&          theName,
                                         const bool        theShapesOnly )
{
  if ( !theShapesOnly )
    {
      int nbSel = aList.Extent();
      if ( nbSel == 1 )
	{
	  Handle(SALOME_InteractiveObject) anIObj = aList.First();
	  if(anIObj->hasEntry()) {
	    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
	    if ( !appStudy ) return nbSel;
	    _PTR(Study) aStudy = appStudy->studyDS();

	    _PTR(SObject) obj ( aStudy->FindObjectID(anIObj->getEntry()) );

	    _PTR(GenericAttribute) anAttr;

	    if ( obj && obj->FindAttribute( anAttr, "AttributeName") )
	      {
		_PTR(AttributeName) aNameAttr ( anAttr );
		theName = aNameAttr->Value().c_str();
	      }
	  }
	}
      else
	theName = QObject::tr("%1_objects").arg(nbSel);

      return nbSel;
    }
  else
    {
      GEOM::ListOfGO anObjs;
      ConvertListOfIOInListOfGO( aList, anObjs, theShapesOnly );
      if ( anObjs.length() == 1 ) {
	theName = GetName( anObjs[ 0 ] );
      }
      else
	theName = QString( "%1_objects" ).arg( anObjs.length() );

      return anObjs.length();
    }
}


//=================================================================================
// function : GetShapeTypeString()
// purpose  : for a single shape
//=================================================================================
bool GEOMBase::GetShapeTypeString(const TopoDS_Shape& aShape, Standard_CString& aTypeString)
{
  if(aShape.IsNull()) {
    aTypeString = "aNullShape";
    return false;
  }
  switch(aShape.ShapeType())
    {
    case TopAbs_COMPOUND:
      {
	aTypeString = CORBA::string_dup(QObject::tr("GEOM_COMPOUND"));
	return true;
      }
    case  TopAbs_COMPSOLID:
      {
	aTypeString = CORBA::string_dup(QObject::tr("GEOM_COMPOUNDSOLID")) ;
	return true ;
      }
    case TopAbs_SOLID:
      {
	aTypeString = CORBA::string_dup(QObject::tr("GEOM_SOLID")) ;
	return true ;
      }
    case TopAbs_SHELL:
      {
	aTypeString = CORBA::string_dup(QObject::tr("GEOM_SHELL")) ;
	return true ;
      }
    case TopAbs_FACE:
      {
	BRepAdaptor_Surface surf(TopoDS::Face(aShape));
	if(surf.GetType() == GeomAbs_Plane) {
	  aTypeString = CORBA::string_dup(QObject::tr("GEOM_PLANE"));
	  return true;
	}
	else if(surf.GetType() == GeomAbs_Cylinder) {
	  aTypeString = CORBA::string_dup(QObject::tr("GEOM_SURFCYLINDER"));
	  return true;
	}
	else if(surf.GetType() == GeomAbs_Sphere) {
	  aTypeString = CORBA::string_dup(QObject::tr("GEOM_SURFSPHERE"));
	  return true ;
	}
	else if(surf.GetType() == GeomAbs_Torus) {
	  aTypeString = CORBA::string_dup(QObject::tr("GEOM_SURFTORUS"));
	  return true ;
	}
	else if(surf.GetType() == GeomAbs_Cone) {
	  aTypeString = CORBA::string_dup(QObject::tr("GEOM_SURFCONE"));
	  return true ;
	}
	else {
	  aTypeString = CORBA::string_dup(QObject::tr("GEOM_FACE"));
	  return true;
	}
      }
    case TopAbs_WIRE:
      {
	aTypeString = CORBA::string_dup(QObject::tr("GEOM_WIRE"));
	return true;
      }
    case TopAbs_EDGE:
      {
	BRepAdaptor_Curve curv(TopoDS::Edge(aShape));
	if(curv.GetType() == GeomAbs_Line) {
	  if((Abs(curv.FirstParameter()) >= 1E6) || (Abs(curv.LastParameter()) >= 1E6))
	    aTypeString = CORBA::string_dup(QObject::tr("GEOM_LINE"));
	  else
	    aTypeString = CORBA::string_dup(QObject::tr("GEOM_EDGE"));
	  return true;
	}
	else if(curv.GetType() == GeomAbs_Circle) {
	  if(curv.IsClosed())
	    aTypeString = CORBA::string_dup(QObject::tr("GEOM_CIRCLE"));
	  else
	    aTypeString = CORBA::string_dup(QObject::tr("GEOM_ARC"));
	return true;
      }
	else {
	  aTypeString = CORBA::string_dup(QObject::tr("GEOM_EDGE"));
	  return true;
	}
      }
    case TopAbs_VERTEX:
      {
	aTypeString = CORBA::string_dup(QObject::tr("GEOM_VERTEX"));
	return true;
      }
    case TopAbs_SHAPE:
      {
	aTypeString = CORBA::string_dup(QObject::tr("GEOM_SHAPE"));
	return true;
      }
    }
  return false;
}


//=======================================================================
// function : ConvertIORinGEOMAISShape()
// purpose  :
//=======================================================================
Handle(GEOM_AISShape) GEOMBase::ConvertIORinGEOMAISShape(const char * IOR, Standard_Boolean& testResult, bool onlyInActiveView)
{
  Handle(GEOM_AISShape) resultShape;
  testResult = false;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( !appStudy ) return resultShape;
  _PTR(Study) aStudy = appStudy->studyDS();

  _PTR(SObject) anObj ( aStudy->FindObjectIOR( IOR ) );
  if ( !anObj )
    return resultShape;

  QPtrList<SUIT_ViewWindow> aViewWindowsList = SUIT_Session::session()->activeApplication()->desktop()->windows();

  for ( QPtrListIterator<SUIT_ViewWindow> it( aViewWindowsList ); it.current(); ++it ) {
    if (it.current()->getViewManager()->getType() == OCCViewer_Viewer::Type()) {
      Handle (AIS_InteractiveContext) ic = ((OCCViewer_Viewer*)it.current()->getViewManager()->getViewModel())->getAISContext();

      AIS_ListOfInteractive List;
      ic->DisplayedObjects(List);
      AIS_ListOfInteractive List1;
      ic->ObjectsInCollector(List1);
      List.Append(List1);

      AIS_ListIteratorOfListOfInteractive ite(List);
      while(ite.More()) {
	if(ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
	  Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
	  if(aSh->hasIO()) {
	    Handle(SALOME_InteractiveObject) GIO = Handle(SALOME_InteractiveObject)::DownCast(aSh->getIO());
	    if(GIO->hasEntry() && strcmp(GIO->getEntry(), anObj->GetID().c_str()) == 0) {
	      if(!onlyInActiveView ||
		 it.current() == SUIT_Session::session()->activeApplication()->desktop()->activeWindow()) {
		testResult = true;
		resultShape = aSh;
		return resultShape;
	      }
	    }
	  }
	}
	ite.Next();
      }
    }
  }
  return  resultShape;
}


//=======================================================================
// function : ConvertIORinGEOMActor()
// purpose  :
//=======================================================================
GEOM_Actor* GEOMBase::ConvertIORinGEOMActor(const char* IOR, Standard_Boolean& testResult, bool onlyInActiveView)
{
  testResult = false;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( !appStudy ) return GEOM_Actor::New();
  _PTR(Study) aStudy = appStudy->studyDS();

  _PTR(SObject) anObj ( aStudy->FindObjectIOR( IOR ) );
  if ( !anObj )
    return GEOM_Actor::New();

  QPtrList<SUIT_ViewWindow> aViewWindowsList = SUIT_Session::session()->activeApplication()->desktop()->windows();

  for ( QPtrListIterator<SUIT_ViewWindow> it( aViewWindowsList ); it.current(); ++it ) {
    if (it.current()->getViewManager()->getType() == SVTK_Viewer::Type()) {
      SVTK_ViewWindow* aVTKViewWindow = dynamic_cast<SVTK_ViewWindow*>( it.current() );
      if( !aVTKViewWindow )
	continue;
      vtkRenderer* Renderer = aVTKViewWindow->getRenderer();
      vtkActorCollection* theActors = Renderer->GetActors();
      theActors->InitTraversal();
      vtkActor *ac = theActors->GetNextActor();
      while(!(ac==NULL)) {
	if( ac->IsA("GEOM_Actor")) {
	  GEOM_Actor* anActor = GEOM_Actor::SafeDownCast(ac);
	  if(anActor->hasIO()) {
	    Handle(SALOME_InteractiveObject) GIO = Handle(SALOME_InteractiveObject)::DownCast(anActor->getIO());
	    if(GIO->hasEntry() && strcmp(GIO->getEntry(), anObj->GetID().c_str()) == 0) {
	      if(!onlyInActiveView ||
		 it.current() == SUIT_Session::session()->activeApplication()->desktop()->activeWindow()) {
		testResult = true;
		return anActor;
	      }
	    }
	  }
	}
	ac = theActors->GetNextActor();
      }
    }
  }
  testResult = false;
  return GEOM_Actor::New();
}

//=======================================================================
// function : GetAIS()
// purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) GEOMBase::GetAIS( const Handle(SALOME_InteractiveObject)& theIO,
                                                const bool                              isOnlyInActiveView )
{
  if ( theIO.IsNull() || !theIO->hasEntry() )
    return Handle(AIS_InteractiveObject)();

  QPtrList<SUIT_ViewWindow> aViewWindowsList = SUIT_Session::session()->activeApplication()->desktop()->windows();

  for ( QPtrListIterator<SUIT_ViewWindow> it( aViewWindowsList ); it.current(); ++it ) {
    if (it.current()->getViewManager()->getType() != OCCViewer_Viewer::Type())
      continue;
    Handle (AIS_InteractiveContext) anIC = ((OCCViewer_Viewer*)it.current()->getViewManager()->getViewModel())->getAISContext();

    AIS_ListOfInteractive aList;
    anIC->DisplayedObjects( aList );
    anIC->ObjectsInCollector( aList );

    AIS_ListIteratorOfListOfInteractive anIter( aList );
    for ( ; anIter.More(); anIter.Next() )
    {
      Handle(SALOME_InteractiveObject) anObj =
        Handle(SALOME_InteractiveObject)::DownCast( anIter.Value()->GetOwner() );

      if( !anObj.IsNull() && strcmp( anObj->getEntry(), theIO->getEntry() ) == 0 )
      {
        if( isOnlyInActiveView )
        {
          if ( it.current() == SUIT_Session::session()->activeApplication()->desktop()->activeWindow() )
            return anIter.Value();
        }
        else
          return anIter.Value();
      }
    }
  }

  return Handle(AIS_InteractiveObject)();
}


//=======================================================================
// function : ConvertIOinGEOMAISShape()
// purpose  :
//=======================================================================
Handle(GEOM_AISShape) GEOMBase::ConvertIOinGEOMAISShape(const Handle(SALOME_InteractiveObject)& IO, Standard_Boolean& testResult, bool onlyInActiveView)
{
  Handle(GEOM_AISShape) res;

  if ( !IO->hasEntry() )
  {
    testResult = false;
    return res;
  }

  QPtrList<SUIT_ViewWindow> aViewWindowsList = SUIT_Session::session()->activeApplication()->desktop()->windows();

  for ( QPtrListIterator<SUIT_ViewWindow> it( aViewWindowsList ); it.current(); ++it ) {
    if (it.current()->getViewManager()->getType() == OCCViewer_Viewer::Type()) {
      Handle (AIS_InteractiveContext) ic = ((OCCViewer_Viewer*)it.current()->getViewManager()->getViewModel())->getAISContext();

      AIS_ListOfInteractive List;
      ic->DisplayedObjects(List);
      AIS_ListOfInteractive List1;
      ic->ObjectsInCollector(List1);
      List.Append(List1);

      AIS_ListIteratorOfListOfInteractive ite(List);
      while(ite.More())
      {
        if(ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape)))
        {
          Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
          if( aSh->hasIO() )
          {
            if( strcmp( aSh->getIO()->getEntry(), IO->getEntry() ) == 0 )
            {
              if(onlyInActiveView)
              {
                if(it.current() == SUIT_Session::session()->activeApplication()->desktop()->activeWindow())
                {
                  testResult = true;
                  return aSh;
                }
              }
              else
              {
                testResult = true;
                return aSh;
              }
            }
          }
        }
        ite.Next();
      }
    }
  }
  testResult = false;
  return res;
}


//=======================================================================
// function : ConvertIOinGEOMShape()
// purpose  :
//=======================================================================
GEOM::GEOM_Object_ptr GEOMBase::ConvertIOinGEOMShape(const Handle(SALOME_InteractiveObject)& IO, Standard_Boolean& testResult)
{
  GEOM::GEOM_Object_var aShape;
  testResult = false;

  /* case SObject */
  if(IO->hasEntry()) {
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
    if ( !appStudy ) return GEOM::GEOM_Object::_nil();
    _PTR(Study) aStudy = appStudy->studyDS();

    _PTR(SObject) obj ( aStudy->FindObjectID(IO->getEntry()) );
    _PTR(GenericAttribute) anAttr;
    if(obj) {
      if(obj->FindAttribute(anAttr, "AttributeIOR")) {
	_PTR(AttributeIOR) anIOR ( anAttr );
	aShape = GeometryGUI::GetGeomGen()->GetIORFromString(anIOR->Value().c_str());
	if(!CORBA::is_nil(aShape))
	  testResult = true;
	return aShape._retn();
      }
    }
  }
  return GEOM::GEOM_Object::_nil();
}


//=======================================================================
// function : ConvertListOfIOInListOfIOR()
// purpose  :
//=======================================================================
void GEOMBase::ConvertListOfIOInListOfIOR(const SALOME_ListIO& aList, GEOM::string_array& listIOR)
{
  int nbSel = aList.Extent();
  listIOR.length(nbSel);
  int j=0;
  SALOME_ListIteratorOfListIO It(aList);
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
    ( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( !appStudy ) return;
  _PTR(Study) aStudy = appStudy->studyDS();

  for (int i=0; It.More(); It.Next(), i++) {
    Handle(SALOME_InteractiveObject) IObject = It.Value();
    if (IObject->hasEntry()) {
      _PTR(SObject) obj ( aStudy->FindObjectID(IObject->getEntry()) );
      _PTR(GenericAttribute) anAttr;
      if (obj && obj->FindAttribute(anAttr, "AttributeIOR")) {
	_PTR(AttributeIOR) anIOR (anAttr);
        //CORBA::Object_var theObj = dynamic_cast<SALOMEDS_Study*>
        //  (aStudy.get())->ConvertIORToObject(anIOR->Value());
        CORBA::Object_var theObj = GeometryGUI::ClientSObjectToObject(obj);
	if (!CORBA::is_nil(theObj) && theObj->_is_a("IDL:GEOM/GEOM_Object:1.0")) {
	  listIOR[j] = CORBA::string_dup(anIOR->Value().c_str());
	  j++;
	}
      }
    }
  }
  listIOR.length(j);
}


//=======================================================================
// function : ConvertIOinGEOMObject()
// purpose  :
//=======================================================================
GEOM::GEOM_Object_ptr GEOMBase::ConvertIOinGEOMObject( const Handle(SALOME_InteractiveObject)& theIO,
						       Standard_Boolean& theResult )
{
  theResult = Standard_False;
  GEOM::GEOM_Object_var aReturnObject;
  if ( !theIO.IsNull() )
  {
    const char* anEntry = theIO->getEntry();

    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
      ( SUIT_Session::session()->activeApplication()->activeStudy() );
    if ( !appStudy ) return  GEOM::GEOM_Object::_nil();
    _PTR(Study) aStudy = appStudy->studyDS();

    _PTR(SObject) aSObj ( aStudy->FindObjectID( anEntry ) );

    if (aSObj)
    {
      aReturnObject = GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aSObj));
      theResult = !CORBA::is_nil( aReturnObject );
    }
  }
  return aReturnObject._retn();
}


//=======================================================================
// function : ConvertListOfIOInListOfGO()
// purpose  :
//=======================================================================
void GEOMBase::ConvertListOfIOInListOfGO( const SALOME_ListIO& theList,
                                          GEOM::ListOfGO&      theListGO,
                                          const bool           theShapesOnly )
{
  int nbSel = theList.Extent();
  theListGO.length( nbSel );
  SALOME_ListIteratorOfListIO anIter( theList );

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( !appStudy ) return;
  _PTR(Study) aStudy = appStudy->studyDS();

  int j = 0;
  for ( int i=0; anIter.More(); anIter.Next(), i++ )
  {
    Handle(SALOME_InteractiveObject) anIObj = anIter.Value();
    _PTR(SObject) aSObj ( aStudy->FindObjectID( anIObj->getEntry() ) );

    if ( aSObj )
    {
      GEOM::GEOM_Object_var aGeomObj =
	GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aSObj));
      if ( !CORBA::is_nil( aGeomObj ) && ( !theShapesOnly || IsShape( aGeomObj ) ) )
        theListGO[ j++ ] = aGeomObj;
    }
  }

  theListGO.length( j );
}

//=================================================================================
// function : CreateArrowForLinearEdge()
// purpose  : Create a cone topology to be used to display an arrow in the middle
//          : of an edge showing its orientation. (For simulation and Viewer OCC only)
//=================================================================================
bool GEOMBase::CreateArrowForLinearEdge(const TopoDS_Shape& tds, TopoDS_Shape& ArrowCone)
{
  if(SUIT_Session::session()->activeApplication()->desktop()->activeWindow()->getViewManager()->getType()
     != OCCViewer_Viewer::Type() || tds.ShapeType() != TopAbs_EDGE)
    return false;

  OCCViewer_ViewPort3d* vp3d = ((OCCViewer_ViewWindow*)SUIT_Session::session()->activeApplication()->desktop()->activeWindow())->getViewPort();
  Handle( V3d_View) view3d = vp3d->getView();
  Standard_Real Width, Height;
  view3d->Size(Width, Height);
  const Standard_Real aHeight = (Width + Height) / 50.0;

  try {
    Standard_Real first, last;
    Handle(Geom_Curve) curv = BRep_Tool::Curve(TopoDS::Edge(tds), first, last);
    if(!curv->IsCN(1))
      return false;

    const Standard_Real param = (first+last) / 2.0;
    gp_Pnt middleParamPoint;
    gp_Vec V1;
    curv->D1( param, middleParamPoint, V1);
    if(V1.Magnitude() < Precision::Confusion())
      return false;

    /* Topology orientation not geom orientation */
    if(tds.Orientation() == TopAbs_REVERSED)
      V1 *= -1.0;

    gp_Ax2 anAxis( middleParamPoint, gp_Dir(V1));
    const Standard_Real radius1 = aHeight / 5.0;
    if(radius1 > 10.0 * Precision::Confusion() && aHeight > 10.0 * Precision::Confusion()) {
      ArrowCone = BRepPrimAPI_MakeCone( anAxis, radius1, 0.0, aHeight ).Shape();
      return true;
    }
  }
  catch(Standard_Failure) {
    // OCC failures are hard to catch in GUI.
    // This  because of the position for  #include <Standard_ErrorHandler.hxx> that is very critic to find
    // in SALOME environment : compilation error !
  }
  return false;
}


//=================================================================================
// function : VertexToPoint()
// purpose  : If S can be converted in a gp_Pnt returns true and the result is P
//=================================================================================
bool GEOMBase::VertexToPoint(const TopoDS_Shape& S, gp_Pnt& P)
{
  if(S.IsNull() || S.ShapeType() != TopAbs_VERTEX)
    return false;
  P = BRep_Tool::Pnt(TopoDS::Vertex(S));
  return true;
}


//=================================================================================
// function : GetBipointDxDyDz()
// purpose  :
//=================================================================================
void GEOMBase::GetBipointDxDyDz(gp_Pnt P1, gp_Pnt P2, double& dx, double& dy, double& dz)
{
  dx = P2.X() - P1.X();
  dy = P2.Y() - P1.Y();
  dz = P2.Z() - P1.Z();
  return;
}


//=================================================================================
// function : LinearEdgeExtremities()
// purpose  : If S can be converted in a linear edge and if initial an final points
//          : distance is sufficient, returns true else returns false.
//          : Resulting points are respectively P1 and P2
//=================================================================================
bool GEOMBase::LinearEdgeExtremities(const TopoDS_Shape& S,  gp_Pnt& P1, gp_Pnt& P2)
{
  if(S.IsNull() || S.ShapeType() != TopAbs_EDGE)
    return false;
  BRepAdaptor_Curve curv(TopoDS::Edge(S));
  if(curv.GetType() != GeomAbs_Line)
    return false;

  curv.D0(curv.FirstParameter(), P1);
  curv.D0(curv.LastParameter(), P2);

  if(P1.Distance(P2) <= Precision::Confusion())
    return false;

  return true;
}


//=======================================================================
// function : Parameter()
// purpose  : return a parameter (float) from a dialog box
//
//  avalue1    : is a float or integer used as a default value displayed
//  aTitle1    : is the title for aValue1
//  aTitle     : is the main title
//  bottom     : maximum value to be entered
//  top	       : minimum value to be entered
//  decimals   : number of decimals
//=======================================================================
double GEOMBase::Parameter(Standard_Boolean& res, const char* aValue1, const char* aTitle1, const char* aTitle, const double bottom, const double top, const int decimals)
{
  GEOMBase_aParameterDlg * Dialog = new GEOMBase_aParameterDlg(aValue1, aTitle1, SUIT_Session::session()->activeApplication()->desktop(),
							       aTitle, TRUE, 0, bottom, top, decimals);
  int r = Dialog->exec();
  float X = 0.0;
  if(r == QDialog::Accepted) {
    res = Standard_True;
    X = Dialog->getValue();
  }
  else
    res = Standard_False;
  delete Dialog;
  return X;
}


//=======================================================================
// function : SelectionByNameInDialogs()
// purpose  : Called when user has entered a name of object in a LineEdit.
//          : The selection is changed. Dialog box will receive the
//          : corresponding signal to manage this event.
//=======================================================================
bool GEOMBase::SelectionByNameInDialogs(QWidget* aWidget, const QString& objectUserName, const SALOME_ListIO& aList)
{
  /* Find SObject with name in component GEOM */
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( !appStudy ) return false;
  _PTR(Study) ST = appStudy->studyDS();

  std::vector<_PTR(SObject)> listSO;
  listSO = ST->FindObjectByName(objectUserName.latin1(), "GEOM");

  if(listSO.size() < 1) {
    const QString caption  = QObject::tr("GEOM_WRN_WARNING");
    const QString text = QObject::tr("GEOM_NAME_INCORRECT");
    const QString button0  = QObject::tr("GEOM_BUT_OK");
    SUIT_MessageBox::error1(aWidget, caption, text, button0);
    return false;
  }
  /* More than one object with same name */
  if(listSO.size() > 1) {
    const QString caption  = QObject::tr("GEOM_WRN_WARNING");
    const QString text = QObject::tr("GEOM_IDENTICAL_NAMES_SELECT_BY_MOUSE");
    const QString button0  = QObject::tr("GEOM_BUT_OK") ;
    SUIT_MessageBox::error1(aWidget, caption, text, button0) ;
    listSO.clear();
    return false;
  }

  _PTR(SObject) theObj ( listSO[0] );
  /* Create a SALOME_InteractiveObject with a SALOME::SObject */
  char* aCopyobjectUserName = CORBA::string_dup(objectUserName);
  Handle(SALOME_InteractiveObject) SI = new SALOME_InteractiveObject(theObj->GetID().c_str(), "GEOM", aCopyobjectUserName);
  delete(aCopyobjectUserName);

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
bool GEOMBase::DefineDlgPosition(QWidget* aDlg, int& x, int& y)
{
  /* Here the position is on the bottom right corner - 10 */
  SUIT_Desktop* PP = SUIT_Session::session()->activeApplication()->desktop();
  x = abs(PP->x() + PP->size().width() - aDlg->size().width() - 10);
  y = abs(PP->y() + PP->size().height() - aDlg->size().height() - 10);
  return true;
}


//=======================================================================
// function : GetDefaultName()
// purpose  : Generates default names
//=======================================================================
QString GEOMBase::GetDefaultName(const QString& theOperation)
{
  QString aName = "";

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( !appStudy ) return aName;
  _PTR(Study) aStudy = appStudy->studyDS();

  int aNumber = 0;
  _PTR(SObject) obj;
  do
    {
      aName = theOperation+"_"+QString::number(++aNumber);
      obj = aStudy->FindObject(aName.latin1());
    }
  while (obj);

  return aName;
}


//=======================================================================
// function : ShowErrorMessage()
// purpose  : Shows message box with error code and comment
//=======================================================================
void GEOMBase::ShowErrorMessage(const char* theErrorCode, const char* theComment)
{
  QString anErrorCode(theErrorCode);
  QString aComment(theComment);

  QString aText = "";
  if (!anErrorCode.isEmpty())
    aText.append("\n" + QObject::tr(anErrorCode));
  if (!aComment.isEmpty())
    aText.append("\n" + QString(theComment));

  SUIT_MessageBox::error1( SUIT_Session::session()->activeApplication()->desktop(), QObject::tr( "GEOM_ERROR" ),
			   QObject::tr("GEOM_PRP_ABORT") + aText, "OK" );
}


//=======================================================================
// function : GetObjectFromIOR()
// purpose  : returns a GEOM_Object by given IOR (string)
//=======================================================================
GEOM::GEOM_Object_ptr GEOMBase::GetObjectFromIOR( const char* theIOR )
{
  GEOM::GEOM_Object_var anObject;
  if ( theIOR == NULL || strlen( theIOR ) == 0 )
    return anObject._retn(); // returning nil object

  anObject = GEOM::GEOM_Object::_narrow( SalomeApp_Application::orb()->string_to_object( theIOR ) );
  return anObject._retn();
}

//=======================================================================
// function : GetIORFromObject()
// purpose  : returns IOR of a given GEOM_Object
//=======================================================================
char* GEOMBase::GetIORFromObject( const GEOM::GEOM_Object_ptr& theObject )
{
  if ( CORBA::is_nil( theObject ) )
    return NULL;

  return SalomeApp_Application::orb()->object_to_string( theObject );
}

//=======================================================================
// function : GetShape()
// purpose  : returns a TopoDS_Shape stored in GEOM_Object
//=======================================================================
bool GEOMBase::GetShape( const GEOM::GEOM_Object_ptr& theObject, TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType )
{
  if ( !CORBA::is_nil( theObject ) )
  {
    TopoDS_Shape aTopoDSShape = GEOM_Client().GetShape(  GeometryGUI::GetGeomGen(), theObject );
    if ( !aTopoDSShape.IsNull() && ( theType == TopAbs_SHAPE || theType == aTopoDSShape.ShapeType() ) )
    {
       theShape = aTopoDSShape;
       return true;
    }
  }
  return false;
}

//=======================================================================
// function : GetName()
// purpose  : Get name of object
//=======================================================================
QString GEOMBase::GetName( GEOM::GEOM_Object_ptr theObj )
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );

  if ( appStudy )
  {
    string anIOR = SalomeApp_Application::orb()->object_to_string( theObj );
    if ( anIOR != "" )
    {
      _PTR(SObject) aSObj ( appStudy->studyDS()->FindObjectIOR( anIOR ) );

      _PTR(GenericAttribute) anAttr;

      if ( aSObj && aSObj->FindAttribute( anAttr, "AttributeName") )
      {
        _PTR(AttributeName) aNameAttr ( anAttr );
	return QString( aNameAttr->Value().c_str() );
      }
    }
  }

  return QString("");
}

bool GEOMBase::IsShape( GEOM::GEOM_Object_ptr theObj )
{
  return !theObj->_is_nil() && theObj->IsShape();
}
