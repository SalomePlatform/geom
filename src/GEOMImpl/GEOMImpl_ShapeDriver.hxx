// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  File   : GEOMImpl_ShapeDriver.ixx
//  Module : GEOMImpl

#ifndef _GEOMImpl_ShapeDriver_HeaderFile
#define _GEOMImpl_ShapeDriver_HeaderFile

#ifndef _TColStd_SequenceOfExtendedString_HeaderFile
#include <TColStd_SequenceOfExtendedString.hxx>
#endif
#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_GUID_HeaderFile
#include <Standard_GUID.hxx>
#endif 

#ifndef _TFunction_Driver_HeaderFile
#include <TFunction_Driver.hxx>
#endif
#ifndef _TFunction_Logbook_HeaderFile
#include <TFunction_Logbook.hxx>
#endif
#ifndef _Standard_CString_HeaderFile
#include <Standard_CString.hxx>
#endif

#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TColStd_HSequenceOfTransient.hxx>

class TColStd_SequenceOfExtendedString;
class TopoDS_Face;


#include "GEOM_BaseDriver.hxx"

DEFINE_STANDARD_HANDLE( GEOMImpl_ShapeDriver, GEOM_BaseDriver );

class GEOMImpl_ShapeDriver : public GEOM_BaseDriver {

public:

 // Methods PUBLIC
  // 
  Standard_EXPORT GEOMImpl_ShapeDriver();
  Standard_EXPORT ~GEOMImpl_ShapeDriver() {};

  Standard_EXPORT virtual  Standard_Integer Execute(TFunction_Logbook& log) const; 
  Standard_EXPORT virtual void Validate(TFunction_Logbook&) const {}
  Standard_EXPORT Standard_Boolean MustExecute(const TFunction_Logbook&) const { return Standard_True; }
  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT static TopoDS_Edge MakeEdgeFromWire (const TopoDS_Shape& aWire,
                                                       const Standard_Real LinTol,
                                                       const Standard_Real AngTol);

  Standard_EXPORT static TopoDS_Wire MakeWireFromEdges
                                     (const Handle(TColStd_HSequenceOfTransient)& theEdgesFuncs,
                                      const Standard_Real theTolerance);
  Standard_EXPORT virtual
  bool GetCreationInformation(std::string&             theOperationName,
                              std::vector<GEOM_Param>& params);

  // Type management
  //
DEFINE_STANDARD_RTTI( GEOMImpl_ShapeDriver )

private:

  TopoDS_Shape MakeIsoline(const TopoDS_Face &theFace,
                           const bool         IsUIso,
                           const double       theParameter) const;

  TopoDS_Shape ExtendEdge(const TopoDS_Edge   &theEdge,
                          const Standard_Real  theMin,
                          const Standard_Real  theMax) const;

  TopoDS_Shape ExtendFace(const TopoDS_Face   &theFace,
                          const Standard_Real  theUMin,
                          const Standard_Real  theUMax,
                          const Standard_Real  theVMin,
                          const Standard_Real  theVMax) const;

};

#endif
