// File:	IGESImport.cxx
// Created:	Wed May 19 14:36:35 2004
// Author:	Pavel TELKOV
//		<ptv@mutex.nnov.opencascade.com>

#include "utilities.h"

#include <IFSelect_ReturnStatus.hxx>
#include <IGESControl_Reader.hxx>

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
    IGESControl_Reader aReader;
    TopoDS_Shape aResShape;
    try {
      IFSelect_ReturnStatus status = aReader.ReadFile(theFileName.ToCString());

      if (status == IFSelect_RetDone) {
        MESSAGE("ImportIGES : all Geometry Transfer");
        //OCC 5.1.2 porting
        //     aReader.Clear();
        //     aReader.TransferRoots(false);
        aReader.ClearShapes();
        aReader.TransferRoots();

        MESSAGE("ImportIGES : count of shapes produced = " << aReader.NbShapes());
        aResShape = aReader.OneShape();

      } else {
//        switch (status) {
//        case IFSelect_RetVoid:
//          theError = "Nothing created or No data to process";
//          break;
//        case IFSelect_RetError:
//          theError = "Error in command or input data";
//          break;
//        case IFSelect_RetFail:
//          theError = "Execution was run, but has failed";
//          break;
//        case IFSelect_RetStop:
//          theError = "Execution has been stopped. Quite possible, an exception was raised";
//          break;
//        default:
//          break;
//        }
        theError = "Wrong format of the imported file. Can't import file.";
	aResShape.Nullify();
      }
    }
    catch(Standard_Failure) {
      Handle(Standard_Failure) aFail = Standard_Failure::Caught();
      theError = aFail->GetMessageString();
      aResShape.Nullify();
    }
    return aResShape;
  }
}
