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

//  File   : GEOMImpl_PrismDriver.ixx
//  Module : GEOMImpl

#ifndef _GEOMImpl_PrismDriver_HeaderFile
#define _GEOMImpl_PrismDriver_HeaderFile

#include <TopoDS_Shape.hxx>
#include <gp_Vec.hxx>
#include <Standard_Real.hxx>

#include <GEOM_BaseDriver.hxx>

DEFINE_STANDARD_HANDLE( GEOMImpl_PrismDriver, GEOM_BaseDriver );

class GEOMImpl_PrismDriver : public GEOM_BaseDriver {

public:

  Standard_EXPORT GEOMImpl_PrismDriver();
  Standard_EXPORT ~GEOMImpl_PrismDriver() {};

  Standard_EXPORT virtual Standard_Integer Execute(LOGBOOK& log) const;
  Standard_EXPORT virtual void Validate (LOGBOOK&) const {}
  Standard_EXPORT Standard_Boolean MustExecute (const LOGBOOK&) const { return Standard_True; }

  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT static TopoDS_Shape MakeScaledPrism (const TopoDS_Shape& theShapeBase,
                                                       const gp_Vec&       theVector,
                                                       const Standard_Real theScaleFactor,
                                                       const gp_Pnt&       theCDG = gp::Origin(),
                                                       bool                isCDG = false);
  
  Standard_EXPORT static TopoDS_Shape MakeDraftPrism (const TopoDS_Shape& theInitShape,
                                                      const TopoDS_Shape& theBaseShape,
                                                      const Standard_Real theHeight,
                                                      const Standard_Real theAngle,
                                                      bool                isProtrusion,
                                                      const TopoDS_Shape& theSupport,
                                                      bool                isInvert = false);

  Standard_EXPORT virtual
  bool GetCreationInformation(std::string&             theOperationName,
                              std::vector<GEOM_Param>& params);

  OCCT_DEFINE_STANDARD_RTTIEXT(GEOMImpl_PrismDriver,GEOM_BaseDriver)
};

#endif
