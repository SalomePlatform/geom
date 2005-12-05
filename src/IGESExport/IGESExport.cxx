// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
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

#ifdef WNT
#include <SALOME_WNT.hxx>
#else
#define SALOME_WNT_EXPORT
#endif

//=============================================================================
/*!
 *
 */
//=============================================================================

extern "C"
{
SALOME_WNT_EXPORT
  int Export(const TopoDS_Shape& theShape, const TCollection_AsciiString& theFileName)
  {
    MESSAGE("Export IGES into file " << theFileName.ToCString());
    try 
      {
	//VRV: OCC 4.0 migration
	IGESControl_Controller::Init();
	IGESControl_Writer ICW (Interface_Static::CVal("XSTEP.iges.unit"),
				Interface_Static::IVal("XSTEP.iges.writebrep.mode"));
	//VRV: OCC 4.0 migration
	
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
