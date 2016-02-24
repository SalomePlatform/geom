// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef CURVECREATOR_DISPLAYER_H
#define CURVECREATOR_DISPLAYER_H

#include "CurveCreator_Macro.hxx"

#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>

#include <vector>

class CURVECREATOR_EXPORT CurveCreator_Displayer
{
typedef std::vector<Handle_AIS_InteractiveObject> AISObjectsList;

public:
  CurveCreator_Displayer( Handle(AIS_InteractiveContext) theContext,
                          const int theZLayer = -1 );
  ~CurveCreator_Displayer(void);

  void display( const Handle(AIS_InteractiveObject)& theObject, bool isUpdate );
  void eraseAll( bool isUpdate );
  //void highlight( const AISObjectsList& theObjects, bool isHL );

protected:
  Quantity_Color getActiveColor( bool isHL );

private:
  Handle(AIS_InteractiveContext) myContext;
  AISObjectsList                myObjects;
  int                           myZLayer;
};

#endif
