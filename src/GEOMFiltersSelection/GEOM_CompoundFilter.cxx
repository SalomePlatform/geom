// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  GEOM GEOM_CompoundFilter : filter selector for the viewer
//  File   : GEOM_CompoundFilter.cxx
//  Author : Roman NIKOLAEV
//  Module : GEOM
//
#include "GEOM_CompoundFilter.h"

// OCCT Includes
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS_Iterator.hxx>



//=======================================================================
// function : GEOM_CompoundFilter
// purpose  : 
//=======================================================================
GEOM_CompoundFilter::GEOM_CompoundFilter(SalomeApp_Study* study)
: GEOM_SelectionFilter( study )
{
  add( TopAbs_COMPOUND );
}

//=======================================================================
// function : ~GEOM_CompoundFilter
// purpose  : 
//=======================================================================
GEOM_CompoundFilter::~GEOM_CompoundFilter()
{
}

//=======================================================================
// function : isOk
// purpose  : 
//=======================================================================
bool GEOM_CompoundFilter::isOk( const SUIT_DataOwner* sOwner) const
{
  if(GEOM_SelectionFilter::isOk(sOwner)){

    GEOM::GEOM_Object_var obj = getObject( sOwner );
    TopoDS_Shape shape;
    if ( getShape( obj, shape )){
      bool subTypes[TopAbs_SHAPE];
      getInfo(shape,subTypes);
      QList<int>::const_iterator it;
      bool result = false;
      for ( it = myKinds.constBegin(); it != myKinds.constEnd(); ++it )
        result = result || subTypes[(*it)];
      
      return result;
    }
  }
  return false;
}


//=======================================================================
// function : addSubType
// purpose  : 
//=======================================================================
void GEOM_CompoundFilter::addSubType( const int type)
{
  if(!myKinds.contains(type))
    myKinds.append(type);
}

//=======================================================================
// function : addSubTypes
// purpose  : 
//=======================================================================
void GEOM_CompoundFilter::addSubTypes(const QList<int>& kinds)
{
  myKinds = kinds;     
}

//=======================================================================
// function : getInfo()
// purpose  : 
//=======================================================================
void GEOM_CompoundFilter::getInfo(const TopoDS_Shape& aShape, bool subTypes[]) const
{
  int iType, nbTypes[TopAbs_SHAPE];
  for (iType = 0; iType < TopAbs_SHAPE; ++iType){
    nbTypes[iType] = 0;
    subTypes[iType] = false;
  }

  nbTypes[aShape.ShapeType()]++;
  TopTools_MapOfShape aMapOfShape;
  aMapOfShape.Add(aShape);
  TopTools_ListOfShape aListOfShape;
  aListOfShape.Append(aShape);
  
  TopTools_ListIteratorOfListOfShape itL (aListOfShape);
  for (; itL.More(); itL.Next()) {
    TopoDS_Iterator it (itL.Value());
    for (; it.More(); it.Next()) {
      TopoDS_Shape s = it.Value();
      if (aMapOfShape.Add(s)) {
        aListOfShape.Append(s);
        nbTypes[s.ShapeType()]++;
      }
    }
  }
  
  for(iType = TopAbs_COMPSOLID; iType < TopAbs_SHAPE; ++iType) {
    if(nbTypes[iType] > 0) {
      subTypes[iType] = true;
      break;
    }
  }
}
