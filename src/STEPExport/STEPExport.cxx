// File:	STEPExport.cxx
// Created:	Wed May 19 14:53:52 2004
// Author:	Pavel TELKOV
//		<ptv@mutex.nnov.opencascade.com>

#include "utilities.h"

#include <IFSelect_ReturnStatus.hxx>

#include <STEPControl_Writer.hxx>

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
    MESSAGE("Export STEP into file " << theFileName.ToCString());

  try 
    {
      IFSelect_ReturnStatus status ;
      //VRV: OCC 4.0 migration
      STEPControl_Writer aWriter;
//JFA: PAL6162      status = aWriter.Transfer( theShape, STEPControl_ManifoldSolidBrep );
      status = aWriter.Transfer( theShape, STEPControl_AsIs );
      //VRV: OCC 4.0 migration
      if ( status == IFSelect_RetDone ) 
	  status = aWriter.Write( theFileName.ToCString() ) ;
      if ( status == IFSelect_RetDone ) 
	return 1;
    }
  catch(Standard_Failure) 
    {
      //THROW_SALOME_CORBA_EXCEPTION("Exception catched in STEPExport", SALOME::BAD_PARAM);
    }
  return 0;
  }
}
