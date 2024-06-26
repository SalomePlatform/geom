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

// File:      ShHealOper_RemoveFace.hxx
// Created:   15.04.04 18:10:16
// Author:    Galina KULIKOVA
//
#ifndef ShHealOper_RemoveFace_HeaderFile
#define ShHealOper_RemoveFace_HeaderFile

#include <ShHealOper_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
///  Class ShHealOper_RemoveFace
//Class intended to removing faces from shape.

class ShHealOper_RemoveFace : public ShHealOper_Tool
{
 public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT ShHealOper_RemoveFace ();
  //Empty constructor.
  
  Standard_EXPORT ShHealOper_RemoveFace (const TopoDS_Shape& theShape);
  // Constructor initialized by shape from which faces will be removed.
  Standard_EXPORT virtual void Init(const TopoDS_Shape& theShape);
   //Method for initialization by whole shape.
  Standard_EXPORT Standard_Boolean Perform();
  //Removes all faces from specified shape.
  
  Standard_EXPORT Standard_Boolean Perform(const TopTools_SequenceOfShape& theRemovedFaces);
  //Removes faces specified in the sequence of faces from specified shape.

  
 private:
 
  Standard_Boolean removeFaces(const TopoDS_Shape& theShape,TopoDS_Shape& theNewShape);
  Standard_Boolean removeFaces(const TopoDS_Solid& theShape,TopoDS_Shape& theNewShape);
  Standard_Boolean removeFaces(const TopoDS_Shell& theShell,TopoDS_Shape& theNewShape);
  Standard_Boolean isReplace(const TopoDS_Shape& theOldShape,TopoDS_Shape& theNewShape);
   //Methods for removing faces from sub-shapes.

  void removePCurve(const TopoDS_Face& theFace);
  //method for removing pcurves belonging to face

  Standard_Boolean isManifold(const TopoDS_Shell& aShell);
  //denenes manifold shell.
 private:
  // ---------- PRIVATE FIELDS ----------

  TopTools_MapOfShape myMapFaces;
  TopTools_IndexedDataMapOfShapeListOfShape myMapEdgesFace;
 public:
// Declaration of CASCADE RTTI
//DEFINE_STANDARD_RTTI (ShHealOper_RemoveFace)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
//DEFINE_STANDARD_HANDLE (ShHealOper_RemoveFace, )


#endif
