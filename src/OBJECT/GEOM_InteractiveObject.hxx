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

#ifndef _GEOM_InteractiveObject_HeaderFile
#define _GEOM_InteractiveObject_HeaderFile

#include <SALOME_InteractiveObject.hxx>

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>

class GEOM_InteractiveObject : public SALOME_InteractiveObject
{
public:
  Standard_EXPORT GEOM_InteractiveObject();
  Standard_EXPORT GEOM_InteractiveObject(const char* anIOR,
					 const char* aFatherIOR,
					 const char* aComponentDataType,
					 const char* anEntry = "");
  Standard_EXPORT ~GEOM_InteractiveObject();

  Standard_EXPORT void setIOR(const char* anEntry);
  Standard_EXPORT const char* getIOR();

  Standard_EXPORT void setFatherIOR(const char* anEntry);
  Standard_EXPORT const char* getFatherIOR();

  Standard_EXPORT virtual Standard_Boolean isSame(const Handle(SALOME_InteractiveObject)& anIO);

private: 
  std::string myIOR;
  std::string myFatherIOR;

public:
  OCCT_DEFINE_STANDARD_RTTIEXT(GEOM_InteractiveObject,SALOME_InteractiveObject)
};

DEFINE_STANDARD_HANDLE(GEOM_InteractiveObject, SALOME_InteractiveObject)

#endif
