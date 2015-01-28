// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  File   : GEOMImpl_CopyDriver.ixx
//  Module : GEOMImpl
//
#ifndef _GEOMImpl_CopyDriver_HeaderFile
#define _GEOMImpl_CopyDriver_HeaderFile

class TopoDS_Shape;
class TopTools_IndexedDataMapOfShapeListOfShape;
class TopTools_IndexedMapOfShape;
class Standard_GUID;


#include "GEOM_BaseDriver.hxx"


DEFINE_STANDARD_HANDLE( GEOMImpl_CopyDriver, GEOM_BaseDriver );

class GEOMImpl_CopyDriver : public GEOM_BaseDriver {

public:

 // Methods PUBLIC
 // 
Standard_EXPORT GEOMImpl_CopyDriver();
Standard_EXPORT virtual  Standard_Integer Execute(TFunction_Logbook& log) const; 
Standard_EXPORT virtual void Validate(TFunction_Logbook&) const {}
Standard_EXPORT Standard_Boolean MustExecute(const TFunction_Logbook&) const { return Standard_True; }
Standard_EXPORT static const Standard_GUID& GetID();
Standard_EXPORT ~GEOMImpl_CopyDriver() {};

  Standard_EXPORT virtual
  bool GetCreationInformation(std::string&             theOperationName,
                              std::vector<GEOM_Param>& params);

DEFINE_STANDARD_RTTI( GEOMImpl_CopyDriver )

private:

  Standard_Integer transferData(TFunction_Logbook& log) const;

  Standard_Boolean getInPlace
    (const TopoDS_Shape                              &theSourceShape,
     const TopTools_IndexedMapOfShape                &theSourceIndices,
     const TopoDS_Shape                              &theDestinationShape,
           TopTools_IndexedDataMapOfShapeListOfShape &theMapSourceDest) const;

  Standard_Boolean getInPlaceOld
    (const Handle(GEOM_Function)                     &theSourceShapeRef,
     const TopTools_IndexedMapOfShape                &theSourceIndices,
     const TopoDS_Shape                              &theDestinationShape,
           TopTools_IndexedDataMapOfShapeListOfShape &theMapSourceDest) const;

  Standard_Boolean getInPlaceByHistory
    (const Handle(GEOM_Function)                     &theSourceShapeRef,
     const TopTools_IndexedMapOfShape                &theSourceIndices,
     const TopoDS_Shape                              &theDestinationShape,
     const Handle(GEOM_Function)                     &theDestinationRef,
           TopTools_IndexedDataMapOfShapeListOfShape &theMapSourceDest) const;

};

#endif
