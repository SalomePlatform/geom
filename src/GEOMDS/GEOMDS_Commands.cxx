//  GEOM GEOMDS : implementation of Geometry component data structure and Geometry documents management
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
//  File   : GeomDS_Commands.cxx
//  Author : Yves FRICAUD/Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "utilities.h"
#include "GEOMDS_Commands.ixx"

#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_Reference.hxx>
#include <TNaming_Tool.hxx>
#include <TDF_ChildIterator.hxx>


//=======================================================================
//function : GEOMDS_Commands
//purpose  : 
//=======================================================================
GEOMDS_Commands::GEOMDS_Commands(const TDF_Label& Main)
  : myLab(Main)
{
}


//=======================================================================
// function : Generated()
// purpose  :
//=======================================================================
TDF_Label GEOMDS_Commands::Generated(const TopoDS_Shape& S,
				     const TCollection_ExtendedString& Name)
{
  TDF_Label NewLab = myLab.NewChild();
  TNaming_Builder B(NewLab);
  B.Generated(S);
  TDataStd_Name::Set(NewLab,Name);
  return NewLab;
}



//=======================================================================
// function : Generated()
// purpose  : 
//=======================================================================
TDF_Label GEOMDS_Commands::Generated(const TopoDS_Shape& S1,
				     const TopoDS_Shape& S2,
				     const TCollection_ExtendedString& Name)
{
  TDF_Label NewLab = myLab.NewChild();
  TNaming_Builder B(NewLab);
  B.Generated(S1,S2);
  TDataStd_Name::Set(NewLab,Name);
  return NewLab;
}



//=======================================================================
// function : AddShape()
// purpose  :
//=======================================================================
TDF_Label GEOMDS_Commands::AddShape(const TopoDS_Shape& S,
				    const TCollection_ExtendedString& Name)
{
  TDF_Label NewLab = myLab.NewChild();
  TNaming_Builder B(NewLab);
  B.Select(S,S);
  TDataStd_Name::Set(NewLab,Name);
  return NewLab;
}


//=======================================================================
// function : AddIndependentShape()
// purpose  : SAME than AddShape() : will be renamed later
//=======================================================================
TDF_Label GEOMDS_Commands::AddIndependentShape(const TopoDS_Shape& S, 
					       const TCollection_AsciiString& nameIOR)
{
  TDF_Label NewLab = myLab.NewChild();
  TNaming_Builder B(NewLab);
  B.Select(S,S);
  TDataStd_Name::Set(NewLab, nameIOR);
  return NewLab;
}


//=======================================================================
// function : AddDependentShape()
// purpose  :
//=======================================================================
TDF_Label GEOMDS_Commands::AddDependentShape(const TopoDS_Shape& S,
					     const TCollection_AsciiString& nameIOR,
					     const TDF_Label& mainLab)
{
  TDF_Label NewLab = myLab.NewChild();
  TNaming_Builder B(NewLab);
  B.Select(S,S);
  TDataStd_Name::Set(NewLab, nameIOR);
  /* NewLab has a reference attribute to mainLab (the main shape in fact) */
  TDF_Reference::Set(NewLab, mainLab) ;
  return NewLab;
}



//=======================================================================
// function : AddConstructiveElement()
// purpose  : 
//=======================================================================
TDF_Label GEOMDS_Commands::AddConstructiveElement(const TopoDS_Shape& S,
						  const TCollection_ExtendedString& nameIOR,
						  const GEOMDS_ConstructiveType& aType) 
{
  TDF_Label NewLab = myLab.NewChild();
  TNaming_Builder B(NewLab);
  B.Select(S,S);
  TDataStd_Name::Set(NewLab, nameIOR);
  /* Add the Attribute Constructive Element coded with a TDataStd_Integer from an enum */
  TDataStd_Integer::Set(NewLab, Standard_Integer(aType));
  return NewLab;
}


//=======================================================================
// function : AddIORNameAttribute()       
// purpose  : Add attribute TDataStd_Name to a label
//          : this attribute represents the name/IOR of object
//          : Return false if attribute exist before
//=======================================================================
Standard_Boolean GEOMDS_Commands::AddIORNameAttribute(const TDF_Label& aLabel,
						      const TCollection_ExtendedString& nameIOR)
{
  if( this->HasIOR(aLabel) )
    return false ;
  TDataStd_Name::Set(aLabel, nameIOR);
  return true ; 
}



//=======================================================================
// function : IsConstructiveElement() 1/2
// purpose  : Return true if 'aLabel' is a constructive element
//=======================================================================
Standard_Boolean GEOMDS_Commands::IsConstructiveElement(const TDF_Label& aLabel)
{
  Handle(TDataStd_Integer) anAttType ;
  if( aLabel.FindAttribute(TDataStd_Integer::GetID(), anAttType ) )
    return true ;
  return false;
}


//=======================================================================
// function : IsConstructiveElement() 2/2
// purpose  : Return true if 'aLabel' is a constructive element and return the
//          : topology ' returnTopo' and type 'returnType'
//=======================================================================
Standard_Boolean GEOMDS_Commands::IsConstructiveElement(const TDF_Label& aLabel,
							TopoDS_Shape& returnTopo,
							GEOMDS_ConstructiveType& returnType)
{
  Handle(TDataStd_Integer) anAttType ;
  Handle(TNaming_NamedShape) anAttTopo ;

  if( aLabel.FindAttribute(TDataStd_Integer::GetID(), anAttType) && aLabel.FindAttribute(TNaming_NamedShape::GetID(), anAttTopo)) {

    returnTopo = TNaming_Tool::GetShape(anAttTopo) ;
    returnType = GEOMDS_ConstructiveType( anAttType->Get() ) ;
    return true ;
  }
  return false;
}


//=======================================================================
// function : GetShape()
// purpose  : return true and 'returnTopo' if a topology is found on 'aLabel'
//=======================================================================
Standard_Boolean GEOMDS_Commands::GetShape(const TDF_Label& aLabel,
					   TopoDS_Shape& returnTopo)
{
  Handle(TNaming_NamedShape) anAttTopo ;
  if( aLabel.FindAttribute(TNaming_NamedShape::GetID(), anAttTopo)) {
    returnTopo = TNaming_Tool::GetShape(anAttTopo) ;
    return true ;
  }
  return false;
}


//=======================================================================
// function : IsDependentShape()
// purpose  : return true if the shape in the label is dependant (a sub shape)
//=======================================================================
Standard_Boolean GEOMDS_Commands::IsDependentShape(const TDF_Label& aLabel)
{
  Handle(TDF_Reference) anAttRef ;
  if( aLabel.FindAttribute(TDF_Reference::GetID(), anAttRef))
    return true ;
  return false;
}



//=======================================================================
// function : GetMainShapeLabel()
// purpose  : return true if an attribute Reference is found for 'aLabel'
//          : so 'returnMainLabel' is defined. 'aLabel' is supposed to be
//          : a dependent object, otherwise return false.
//=======================================================================
Standard_Boolean GEOMDS_Commands::GetMainShapeLabel(const TDF_Label& aLabel,
						    TDF_Label& returnMainLabel)
{
  Handle(TDF_Reference) anAttRef ;
  if( aLabel.FindAttribute(TDF_Reference::GetID(), anAttRef)) {
    returnMainLabel = anAttRef->Get() ;
    return true ;
  }
  return false;
}


//=======================================================================
// function : ClearAllIOR()
// purpose  : Clear all IOR from aLabel usually the main label.
//          : Useful before reconstruction after a load of a document.
//          : IOR is the attribute often called 'name' or 'nameIOR'
//=======================================================================
Standard_Boolean GEOMDS_Commands::ClearAllIOR(const TDF_Label& aLabel)
{
  TDF_ChildIterator it;
  Handle(TDataStd_Name) anAttName ;
  bool notTested = false ;
  for( it.Initialize(aLabel, Standard_False); it.More(); it.Next() ) {
    TDF_Label L = it.Value() ;
    if( L.FindAttribute(TDataStd_Name::GetID(), anAttName) ) {
      notTested = L.ForgetAttribute(TDataStd_Name::GetID()) ;
      if(notTested)
	MESSAGE("in GEOMDS_Commands::ClearAllIOR : IOR CLEARED" )
    ClearAllIOR(L);
    }
  }
  return true ;
}


//=======================================================================
// function : HasIOR()
// purpose  : Return true is 'aLabel' has an attribute IOR (nameIOR)
//=======================================================================
Standard_Boolean GEOMDS_Commands::HasIOR(const TDF_Label& aLabel)
{
  Handle(TDataStd_Name) anAttName ;
  if( !aLabel.FindAttribute(TDataStd_Name::GetID(), anAttName) )
    return false ;
  return true ;
}

//=======================================================================
// function : ReturnNameIOR()
// purpose  : Return true is 'aLabel' has an attribute IOR (nameIOR)
//          : and define 'returnNameIOR'
//=======================================================================
Standard_Boolean GEOMDS_Commands::ReturnNameIOR(const TDF_Label& aLabel,
						TCollection_ExtendedString& returnNameIOR)
{
  Handle(TDataStd_Name) anAttName ;
  if( !aLabel.FindAttribute(TDataStd_Name::GetID(), anAttName) )
    return false ;
  else {
    returnNameIOR = anAttName->Get() ;
    return true ;
  }
}
