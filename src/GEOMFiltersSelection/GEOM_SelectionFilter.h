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

#ifndef GEOM_SELECTIONFILTER_H
#define GEOM_SELECTIONFILTER_H

#include <SalomeApp_Filter.h>

#include <TopoDS_Shape.hxx>

#include <QList>

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

class Standard_EXPORT GEOM_SelectionFilter : public SalomeApp_Filter
{
public:
  GEOM_SelectionFilter( SalomeApp_Study* study, const bool theAll = false );
  ~GEOM_SelectionFilter();

  virtual bool isOk( const SUIT_DataOwner* ) const;

protected:
  GEOM::GEOM_Object_ptr getObject( const SUIT_DataOwner*, const bool = true ) const;
  bool                  getShape( const GEOM::GEOM_Object_ptr&, TopoDS_Shape& ) const;
  
  bool                  contains( const int ) const;
  void                  add( const int );
  void                  remove( const int );

  void                  setAll( const bool );
  bool                  isAll() const;

  bool                  isShapeOk( const TopoDS_Shape& ) const;

private:
  QList<int>            myTypes;
  bool                  myAll;

};

#endif
