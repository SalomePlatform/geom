//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
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
//  File   : GEOM_LogicalFilter.hxx
//  Author : Sergey LITONIN
//  Module : GEOM

#ifndef _GEOM_LogicalFilter_HeaderFile
#define _GEOM_LogicalFilter_HeaderFile

#include "SALOME_Filter.hxx"

#include <Standard_DefineHandle.hxx>
#include <NCollection_StdBase.hxx>
#include <NCollection_DefineList.hxx>

DEFINE_STANDARD_HANDLE( GEOM_LogicalFilter, SALOME_Filter )

DEFINE_BASECOLLECTION( GEOM_FilterColl, Handle(SALOME_Filter) )
DEFINE_LIST( GEOM_ListOfFilter, GEOM_FilterColl, Handle(SALOME_Filter) )


/*
  Class       : GEOM_LogicalFilter
  Description : Filter for combaining several filters with logical operation (OR or AND)
*/

class GEOM_LogicalFilter : public SALOME_Filter
{
public:

  enum { LO_OR, LO_AND, LO_NOT, LO_UNDEFINED };

public:

                              GEOM_LogicalFilter( const GEOM_ListOfFilter&, const int  );
                              GEOM_LogicalFilter( const Handle(SALOME_Filter)&, const int );


  virtual                     ~GEOM_LogicalFilter();
  virtual                     Standard_Boolean IsOk( const Handle(SALOME_InteractiveObject)& ) const;

  void                        SetFilters( const GEOM_ListOfFilter& );
  void                        SetLogOp( const int );
  const GEOM_ListOfFilter&    GetFilters() const;
  int                         GetLogOp() const;

private:

  GEOM_ListOfFilter           myFilters;
  int                         myLogOp;

public:
  DEFINE_STANDARD_RTTI( GEOM_LogicalFilter )
};

#endif


























