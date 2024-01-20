// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

//  File   : GEOMImpl_BlockDriver.ixx
//  Module : GEOMImpl
//
#ifndef _GEOMImpl_BlockDriver_HeaderFile
#define _GEOMImpl_BlockDriver_HeaderFile

#include <TopoDS_Shape.hxx>

#include <GEOM_BaseDriver.hxx>

DEFINE_STANDARD_HANDLE( GEOMImpl_BlockDriver, GEOM_BaseDriver )

class GEOMImpl_BlockDriver : public GEOM_BaseDriver {

public:

  Standard_EXPORT GEOMImpl_BlockDriver();
  Standard_EXPORT virtual Standard_Integer Execute(Handle(TFunction_Logbook)& log) const;
  Standard_EXPORT virtual void Validate(Handle(TFunction_Logbook)&) const {}
  Standard_EXPORT Standard_Boolean MustExecute(const Handle(TFunction_Logbook)&) const { return Standard_True; }

  Standard_EXPORT static const Standard_GUID& GetID();
  Standard_EXPORT ~GEOMImpl_BlockDriver() {};

  Standard_EXPORT virtual bool GetCreationInformation(std::string&             theOperationName,
                                                      std::vector<GEOM_Param>& params);

private:
  void MultiTransformate1D (const TopoDS_Shape&    theBlock,
                            const TopoDS_Shape&    theFace1,
                            const TopoDS_Shape&    theFace2,
                            const Standard_Integer theNbIter,
                            TopoDS_Shape&          theResult) const;

  void MultiTransformate2D (const TopoDS_Shape&    theBlock,
                            const TopoDS_Shape&    theFace1U,
                            const TopoDS_Shape&    theFace2U,
                            const Standard_Integer theNbIterU,
                            const TopoDS_Shape&    theFace1V,
                            const TopoDS_Shape&    theFace2V,
                            const Standard_Integer theNbIterV,
                            TopoDS_Shape&          theResult) const;

  DEFINE_STANDARD_RTTIEXT(GEOMImpl_BlockDriver,GEOM_BaseDriver)
};

#endif
