#include "GEOM_SelectionFilter.h"

#include "GEOM_Client.hxx"

#include <SalomeApp_DataOwner.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>

#include <SALOME_LifeCycleCORBA.hxx>

#include <SUIT_Session.h>

#include <SALOMEDSClient.hxx>


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
  return false;
}

//=======================================================================
// function : getObject
// purpose  :
//=======================================================================
GEOM::GEOM_Object_ptr GEOM_SelectionFilter::getObject( const SUIT_DataOwner* sOwner, const bool extractReference ) const
{
  GEOM::GEOM_Object_var anObj;

  const SalomeApp_DataOwner* owner = dynamic_cast<const SalomeApp_DataOwner*>(sOwner);
  SalomeApp_Study* appStudy = getStudy();
  if (owner && appStudy)
  {
    _PTR(Study) study = appStudy->studyDS();
    QString entry = owner->entry();

    _PTR(SObject) aSO (study->FindObjectID(entry.latin1())), aRefSO;
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
      Engines::Component_var comp = ls->FindOrLoad_Component( "FactoryServer", "GEOM" );
      GEOM::GEOM_Gen_var geomGen = GEOM::GEOM_Gen::_narrow( comp );
      if ( !CORBA::is_nil( geomGen ) )
      {
	TopoDS_Shape aTopoDSShape = GEOM_Client().GetShape( geomGen, theObject );

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
    myTypes.remove( type );
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
