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

#include <GEOM_Application.hxx>

OCCT_IMPLEMENT_STANDARD_RTTIEXT(GEOM_Application, TDocStd_Application)

//=======================================================================
//function : GEOM_Application
//purpose  : 
//=======================================================================

GEOM_Application::GEOM_Application() 
{
}

//=======================================================================
//function : ~GEOM_Application
//purpose  : 
//=======================================================================

GEOM_Application::~GEOM_Application() 
{
}

#if OCC_VERSION_LARGE <= 0x07000000
//=======================================================================
//function : Formats
//purpose  : 
//=======================================================================

void GEOM_Application::Formats(TColStd_SequenceOfExtendedString& Formats) 
{  
  Formats.Append(TCollection_ExtendedString ("SALOME_GEOM"));
#if OCC_VERSION_MAJOR > 6
  Formats.Append(TCollection_ExtendedString ("BinOcaf"));
#endif
}
#endif


//=======================================================================
//function : ResourcesName
//purpose  : 
//=======================================================================

Standard_CString GEOM_Application::ResourcesName() 
{
  return Standard_CString ("GEOMDS_Resources");
}
