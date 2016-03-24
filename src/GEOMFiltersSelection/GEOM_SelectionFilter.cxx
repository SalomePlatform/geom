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

#include "GEOM_SelectionFilter.h"

#include "GEOM_Client.hxx"

#include <LightApp_DataOwner.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>

#include <SALOME_LifeCycleCORBA.hxx>

#include <SUIT_Session.h>

//=======================================================================
// function : GEOM_SelectionFilter
// purpose  :
//=======================================================================
GEOM_SelectionFilter::GEOM_SelectionFilter( SalomeApp_Study* study, const bool theAll )
  : SalomeApp_Filter(study)
{
  myAll = theAll;
}

//=======================================================================
// function : ~GEOM_SelectionFilter
// purpose  :
//=======================================================================
GEOM_SelectionFilter::~GEOM_SelectionFilter()
{
}

//=======================================================================
// function : isOk
// purpose  :
//=======================================================================
bool GEOM_SelectionFilter::isOk( const SUIT_DataOwner* sOwner ) const
{
  GEOM::GEOM_Object_var obj = getObject( sOwner );
  if ( !CORBA::is_nil( obj ) && obj->IsShape() )
  {
    if ( isAll() )
      return true;

    TopoDS_Shape shape;
    if ( getShape( obj, shape ) )
      return contains( shape.ShapeType() ) && isShapeOk( shape );
  }

  // IMP 0020435: fine local selection
  {
    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>(sOwner);
    if (owner) {
      QString entry = owner->entry();
      int index = entry.lastIndexOf("_");
      if (index > 0) {
        return true;
      }
    }
  }

  return false;
}

//=======================================================================
// function : getObject
// purpose  :
//=======================================================================
GEOM::GEOM_Object_ptr GEOM_SelectionFilter::getObject (const SUIT_DataOwner* sOwner,
                                                       const bool extractReference) const
{
  GEOM::GEOM_Object_var anObj;

  const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>(sOwner);
  SalomeApp_Study* appStudy = getStudy();
  if (owner && appStudy)
  {
    _PTR(Study) study = appStudy->studyDS();
    QString entry = owner->entry();

    _PTR(SObject) aSO (study->FindObjectID(entry.toStdString())), aRefSO;
    if( extractReference && aSO && aSO->ReferencedObject( aRefSO ) )
      aSO = aRefSO;

    if (aSO) {
      std::string aValue = aSO->GetIOR();
      if (strcmp(aValue.c_str(), "") != 0) {
        CORBA::ORB_ptr anORB = SalomeApp_Application::orb();
        CORBA::Object_var aCorbaObj = anORB->string_to_object(aValue.c_str());
        anObj = GEOM::GEOM_Object::_narrow(aCorbaObj);
      }
    }
  }

  if (!CORBA::is_nil(anObj))
    return anObj._retn();

  return GEOM::GEOM_Object::_nil();
}

//=======================================================================
// function : getShape
// purpose  :
//=======================================================================
bool GEOM_SelectionFilter::getShape (const GEOM::GEOM_Object_ptr& theObject,
                                     TopoDS_Shape&                theShape) const
{
  if ( !CORBA::is_nil( theObject ) )
  {
    SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>
      ( SUIT_Session::session()->activeApplication() );
    if ( app )
    {
      SALOME_LifeCycleCORBA* ls = new SALOME_LifeCycleCORBA( app->namingService() );
      static GEOM::GEOM_Gen_var geomGen;
      if(CORBA::is_nil( geomGen )) {
        Engines::EngineComponent_var comp = ls->FindOrLoad_Component( "FactoryServer", "GEOM" );
        geomGen = GEOM::GEOM_Gen::_narrow( comp );
      }
      if ( !CORBA::is_nil( geomGen ) )
      {
        TopoDS_Shape aTopoDSShape = GEOM_Client::get_client().GetShape( geomGen, theObject );

        if ( !aTopoDSShape.IsNull() )
        {
          theShape = aTopoDSShape;
          return true;
        }
      }
    }
  }
  return false;
}

//=======================================================================
// function : contains
// purpose  :
//=======================================================================
bool GEOM_SelectionFilter::contains( const int type ) const
{
  return myTypes.contains( type );
}

//=======================================================================
// function : add
// purpose  :
//=======================================================================
void GEOM_SelectionFilter::add( const int type )
{
  if ( !contains( type ) )
    myTypes.append( type );
}

//=======================================================================
// function : remove
// purpose  :
//=======================================================================
void GEOM_SelectionFilter::remove( const int type )
{
  if ( contains( type ) )
    myTypes.removeAll( type );
}

//=======================================================================
// function : setAll
// purpose  :
//=======================================================================
void GEOM_SelectionFilter::setAll( const bool all )
{
  myAll = all;
}

//=======================================================================
// function : isAll
// purpose  :
//=======================================================================
bool GEOM_SelectionFilter::isAll() const
{
  return myAll;
}

//=======================================================================
// function : isShapeOk
// purpose  :
//=======================================================================
bool GEOM_SelectionFilter::isShapeOk( const TopoDS_Shape& ) const
{
  return true;
}
