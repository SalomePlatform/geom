// File:	BREPImport.cxx
// Created:	Wed May 19 14:29:52 2004
// Author:	Pavel TELKOV
//		<ptv@mutex.nnov.opencascade.com>

#include "utilities.h"

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>

#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

//=============================================================================
/*!
 *
 */
//=============================================================================

extern "C"
{
#ifdef WNT
  __declspec(__dllexport)
#endif
  TopoDS_Shape Import (const TCollection_AsciiString& theFileName,
                       TCollection_AsciiString&       theError)
  {
    MESSAGE("Import BREP from file " << theFileName);
    TopoDS_Shape aShape;
    BRep_Builder B;
    BRepTools::Read(aShape, theFileName.ToCString(), B);
    if (aShape.IsNull()) {
      theError = "BREP Import failed";
    }
    return aShape;
  }
}
