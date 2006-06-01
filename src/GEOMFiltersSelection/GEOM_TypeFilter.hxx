//  GEOM GEOMGUI : Implementation of selection filters for GEOM module
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : GEOM_TypeFilter.hxx
//  Author : Sergey LITONIN
//  Module : GEOM

#ifndef _GEOM_TypeFilter_HeaderFile
#define _GEOM_TypeFilter_HeaderFile

#include "SALOME_TypeFilter.hxx"

#include <Standard_DefineHandle.hxx>

DEFINE_STANDARD_HANDLE( GEOM_TypeFilter, SALOME_TypeFilter )

/*
  Class       : GEOM_TypeFilter
  Description : Filter for verifyimg type of geom object ( GEOM_POINT, GEOM_MARKER  and so on )
*/

class GEOM_TypeFilter : public SALOME_TypeFilter
{

public:

                              GEOM_TypeFilter( const int  );


  virtual                     ~GEOM_TypeFilter();
  virtual                     Standard_Boolean IsOk( const Handle(SALOME_InteractiveObject)& ) const;

  void                        SetType( const int );
  int                         GetType() const;

private:

  int                         myType;

public:
  DEFINE_STANDARD_RTTI( GEOM_TypeFilter )
};

#endif


























