// File:	IGESExport.cxx
// Created:	Wed May 19 14:49:45 2004
// Author:	Pavel TELKOV
//		<ptv@mutex.nnov.opencascade.com>

#include "utilities.h"

#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <Interface_Static.hxx>

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
  int Export(const TopoDS_Shape& theShape,
             const TCollection_AsciiString& theFileName,
             const TCollection_AsciiString& theFormatName)
  {
    MESSAGE("Export IGES into file " << theFileName.ToCString());
    try 
    {
      // define, whether to write only faces (5.1 IGES format)
      // or shells and solids also (5.3 IGES format)
      int aBrepMode = 0;
      if (theFormatName.IsEqual("IGES_5_3"))
        aBrepMode = 1;

      // initialize writer
      IGESControl_Controller::Init();
      //IGESControl_Writer ICW (Interface_Static::CVal("write.iges.unit"),
      //			Interface_Static::IVal("write.iges.brep.mode"));
      IGESControl_Writer ICW (Interface_Static::CVal("write.iges.unit"), aBrepMode);
	
      // perform shape writing
      ICW.AddShape( theShape );
      ICW.ComputeModel();
      if ( ICW.Write( theFileName.ToCString() ) )
        return 1;
    }
    catch(Standard_Failure) 
    {
      //THROW_SALOME_CORBA_EXCEPTION("Exception catched in IGESExport", SALOME::BAD_PARAM);
    }
    return 0;
  }
}
