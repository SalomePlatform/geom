// File:	BREPExport.cxx
// Created:	Wed May 19 13:10:05 2004
// Author:	Pavel TELKOV
//		<ptv@mutex.nnov.opencascade.com>

#include "utilities.h"

#include <BRepTools.hxx>

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
  int Export(const TopoDS_Shape& theShape, const TCollection_AsciiString& theFileName)
  {
    MESSAGE("Export BREP into file " << theFileName.ToCString());

    if ( !BRepTools::Write( theShape, theFileName.ToCString() ) )
      return 0;

    return 1;
  }
}
