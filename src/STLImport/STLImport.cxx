// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File:        STLImport.cxx
// Author:      Sergey KHROMOV


#include "utilities.h"
#include <StlAPI_Reader.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_Label.hxx>


#ifdef WIN32
  #if defined STLIMPORT_EXPORTS || defined STLImport_EXPORTS
    #define STLIMPORT_EXPORT __declspec( dllexport )
  #else
    #define STLIMPORT_EXPORT __declspec( dllimport )
  #endif
#else
  #define STLIMPORT_EXPORT
#endif


//=============================================================================
/*!
 *
 */
//=============================================================================

extern "C"
{
STLIMPORT_EXPORT
  TopoDS_Shape Import (const TCollection_AsciiString& theFileName,
                       const TCollection_AsciiString& /*theFormatName*/,
                       TCollection_AsciiString&       theError,
                       const TDF_Label&)
  {
    MESSAGE("Import STL from file " << theFileName.ToCString());

    StlAPI_Reader aReader;
    TopoDS_Shape aShape;

    aReader.Read(aShape, theFileName.ToCString());

    if (aShape.IsNull()) {
      theError = "STL Import failed";
    }

    return aShape;
  }
}
