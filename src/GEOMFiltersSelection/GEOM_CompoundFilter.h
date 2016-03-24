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

#ifndef GEOM_COMPOUNDFILTER_H
#define GEOM_COMPOUNDFILTER_H
#include "GEOM_SelectionFilter.h"

#include <QList>

class Standard_EXPORT GEOM_CompoundFilter : public GEOM_SelectionFilter
{
 public:
  GEOM_CompoundFilter( SalomeApp_Study* study );
  ~GEOM_CompoundFilter();

  virtual bool isOk( const SUIT_DataOwner* ) const;
  void addSubType(const int );
  void addSubTypes(const QList<int>& );

 private:
  void getInfo(const TopoDS_Shape&, bool subTypes []) const;
  
  
 private:
  QList<int>              myKinds;
};


#endif

