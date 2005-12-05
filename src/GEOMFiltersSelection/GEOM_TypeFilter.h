// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
#ifndef GEOM_TYPEFILTER_H
#define GEOM_TYPEFILTER_H

#include "GEOM_SelectionFilter.h"

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

class Standard_EXPORT GEOM_TypeFilter : public GEOM_SelectionFilter
{
public:
  GEOM_TypeFilter(SalomeApp_Study* study, const int type, const bool isShapeType = false );
  ~GEOM_TypeFilter();

  virtual bool isOk( const SUIT_DataOwner* ) const;
  
  int          type() const;

private:
  int                   myType;
  bool                  myIsShapeType;    
};

#endif
