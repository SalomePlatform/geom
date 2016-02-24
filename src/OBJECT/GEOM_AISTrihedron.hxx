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

//  GEOM_AISTrihedron : Filter for VTK viewer
//  File   : GEOM_AISTrihedron.hxx
//  Author : Sergey LITONIN
//  Module : SALOME
//
#ifndef GEOM_AISTrihedron_HeaderFile
#define GEOM_AISTrihedron_HeaderFile

#include <SALOME_InteractiveObject.hxx>

#include <AIS_Trihedron.hxx>
#include <Standard_DefineHandle.hxx>
#include <TCollection_AsciiString.hxx>

class Geom_Axis2Placement;

DEFINE_STANDARD_HANDLE(GEOM_AISTrihedron, AIS_Trihedron)

/*
  Class       : GEOM_AISTrihedron
  Description : Interactive object for displaying local CS
*/

class GEOM_AISTrihedron : public AIS_Trihedron
{
public:

  Standard_EXPORT                                  GEOM_AISTrihedron( const Handle(Geom_Axis2Placement)& );

  Standard_EXPORT Handle(SALOME_InteractiveObject) getIO();
  Standard_EXPORT void                             setIO( const Handle(SALOME_InteractiveObject)& );
  Standard_EXPORT Standard_Boolean                 hasIO();
  Standard_EXPORT Standard_CString                 getName();
  Standard_EXPORT void                             setName( Standard_CString aName );

private:

  TCollection_AsciiString                          myName;
  Handle(SALOME_InteractiveObject)                 myObject;

public:
  OCCT_DEFINE_STANDARD_RTTIEXT(GEOM_AISTrihedron,AIS_Trihedron)
};

#endif
