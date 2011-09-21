// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:        IGESExport.cxx
// Created:     Wed May 19 14:49:45 2004
// Author:      Pavel TELKOV

#include "utilities.h"

#include <Basics_Utils.hxx>

#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <Interface_Static.hxx>

#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

#ifdef WNT
 #if defined IGESEXPORT_EXPORTS || defined IGESExport_EXPORTS
  #if defined WIN32
   #define IGESEXPORT_EXPORT __declspec( dllexport )
  #else
   #define IGESEXPORT_EXPORT
  #endif
 #else
  #if defined WIN32
   #define IGESEXPORT_EXPORT __declspec( dllimport )
  #else
   #define IGESEXPORT_EXPORT
  #endif
 #endif
#else
 #define IGESEXPORT_EXPORT
#endif

//=============================================================================
/*!
 *
 */
//=============================================================================

extern "C"
{
IGESEXPORT_EXPORT
  int Export( const TopoDS_Shape& theShape,
              const TCollection_AsciiString& theFileName,
              const TCollection_AsciiString& theFormatName )
  {
    MESSAGE("Export IGES into file " << theFileName.ToCString());
    // commented for 0021350: Please don't catch exceptions silently and send an
    // inappropriate error message instead, it is disturbing for the user and for us
    //try
    {
      // define, whether to write only faces (5.1 IGES format)
      // or shells and solids also (5.3 IGES format)
      int aBrepMode = 0;
      if (theFormatName.IsEqual("IGES_5_3"))
        aBrepMode = 1;

      // Set "C" numeric locale to save numbers correctly
      Kernel_Utils::Localizer loc;

      // initialize writer
      IGESControl_Controller::Init();
      //IGESControl_Writer ICW (Interface_Static::CVal("write.iges.unit"),
      //                        Interface_Static::IVal("write.iges.brep.mode"));
      IGESControl_Writer ICW ("M", aBrepMode); // "write.iges.unit" ->> VSR 15.09.09: export explicitly in meters
      Interface_Static::SetCVal("xstep.cascade.unit","M");

      // 09.03.2010 skl for bug 0020726
      // change default value "Average" to "Max"
      Interface_Static::SetCVal("write.precision.mode","Max");

      // perform shape writing
      ICW.AddShape( theShape );
      ICW.ComputeModel();
      bool ok = ICW.Write( theFileName.ToCString() );

      // Return previous locale
      if ( ok )
        return 1;
    }
    //catch(Standard_Failure)
    //{
    //  //THROW_SALOME_CORBA_EXCEPTION("Exception catched in IGESExport", SALOME::BAD_PARAM);
    //}
    return 0;
  }
}
