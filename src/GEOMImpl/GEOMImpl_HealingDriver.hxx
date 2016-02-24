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
//  File   : GEOMImpl_HealingDriver.hxx
//  Module : GEOMImpl

#ifndef _GEOMImpl_HealingDriver_HeaderFile
#define _GEOMImpl_HealingDriver_HeaderFile

#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TColStd_HSequenceOfTransient.hxx>

#include <GEOM_BaseDriver.hxx>

class GEOMImpl_IHealing;
class ShHealOper_Tool;


DEFINE_STANDARD_HANDLE( GEOMImpl_HealingDriver, GEOM_BaseDriver );

class GEOMImpl_HealingDriver : public GEOM_BaseDriver {

public:

  Standard_EXPORT GEOMImpl_HealingDriver();
  Standard_EXPORT ~GEOMImpl_HealingDriver() {};

  Standard_EXPORT static const Standard_GUID& GetID();

  Standard_EXPORT virtual Standard_Integer Execute(LOGBOOK& log) const;
  Standard_EXPORT virtual void Validate(LOGBOOK&) const {}
  Standard_EXPORT Standard_Boolean MustExecute(const LOGBOOK&) const { return Standard_True; }

  Standard_EXPORT static Standard_Boolean AreEdgesC1 (const TopoDS_Edge& E1, const TopoDS_Edge& E2);
  Standard_EXPORT static void FuseCollinearEdges (const TopoDS_Shape&,
                                                  const Handle(TColStd_HSequenceOfTransient)&,
                                                  TopoDS_Shape&);
  Standard_EXPORT virtual
  bool GetCreationInformation(std::string&             theOperationName,
                              std::vector<GEOM_Param>& params);

  OCCT_DEFINE_STANDARD_RTTIEXT(GEOMImpl_HealingDriver,GEOM_BaseDriver)

private:
  Standard_Boolean ShapeProcess  ( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
  Standard_Boolean SuppressFaces ( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
  void             SuppressFacesRec (const TopTools_SequenceOfShape& theShapesFaces,
                                     const TopoDS_Shape&             theOriginalShape,
                                     TopoDS_Shape&                   theOutShape) const;
  Standard_Boolean CloseContour  ( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
  Standard_Boolean RemoveIntWires( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
  Standard_Boolean RemoveHoles   ( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
  Standard_Boolean Sew           ( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape&, Standard_Boolean ) const;
  Standard_Boolean RemoveInternalFaces ( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
  Standard_Boolean AddPointOnEdge( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
  Standard_Boolean ChangeOrientation( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;
  void             LimitTolerance( GEOMImpl_IHealing*, const TopoDS_Shape&, TopoDS_Shape& ) const;

  void             SaveStatistics( const ShHealOper_Tool& healer, bool add=false ) const;
};

#endif
