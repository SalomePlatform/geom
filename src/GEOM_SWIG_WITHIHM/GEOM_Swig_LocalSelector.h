// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

#ifndef _GEOM_SWIG_LOCALSELECTOR_H_
#define _GEOM_SWIG_LOCALSELECTOR_H_

#include <string>
#include <vector>

#include <QObject>

#include "GEOMBase_Helper.h"

class GEOM_Swig_LocalSelector : public GEOMBase_Helper
{
public:
  GEOM_Swig_LocalSelector(QWidget* parent, SUIT_ViewWindow* wnd, const std::string& shapeEntry, int mode);
  ~GEOM_Swig_LocalSelector();
  
  std::vector<int> getSelection();

protected:
  GEOM::GEOM_Object_var              myObject;
  GEOM::short_array_var              mySubShapesInd;

};

#endif /* _GEOM_SWIG_LOCALSELECTOR_H_ */
