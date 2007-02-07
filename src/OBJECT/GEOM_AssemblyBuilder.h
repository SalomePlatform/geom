//  GEOM OBJECT : interactive object for Geometry entities visualization
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : GEOM_AssemblyBuilder.h
//  Author : Christophe ATTANASIO
//  Module : GEOM
//  $Header$

#include "GEOM_OBJECT_defs.hxx"

#include <vtkAssembly.h>
#include <vtkPropAssembly.h>
#include <vtkActorCollection.h>

// Open CASCADE Inlcudes
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

class GEOM_OBJECT_EXPORT GEOM_AssemblyBuilder {

 private:

  static void InitProperties(vtkProperty* IsoProp,
			     vtkProperty* FaceProp,
			     vtkProperty* EdgeFProp,
			     vtkProperty* EdgeSProp,
			     vtkProperty* EdgeIProp,
			     vtkProperty* VertexProp,
			     vtkProperty* IsoPVProp,
			     vtkProperty* EdgePVProp,
			     vtkProperty* VertePVProp);

  static void MeshShape(const TopoDS_Shape myShape,
			Standard_Real deflection,
			Standard_Boolean forced);


 public:


  //------------------------------------------------------------------
  // WARNING! Poor graphic performance :-(  use BuildActors instead
  //------------------------------------------------------------------
 
  static vtkAssembly* BuildAssembly(const TopoDS_Shape& myShape,
				    Standard_Real deflection,
				    Standard_Integer amode,
				    Standard_Boolean forced);

  //------------------------------------------------------------------
  // Good performance
  //------------------------------------------------------------------

  static vtkActorCollection* BuildActors(const TopoDS_Shape& myShape,
					 Standard_Real deflection,
					 Standard_Integer amode,
					 Standard_Boolean forced);


  //------------------------------------------------------------------
  // Change mode - Not implemented !!
  //------------------------------------------------------------------

  static void SwitchDisplayMode(vtkAssembly* mySALOMEAssembly);
  static void SwitchDisplayMode(vtkActorCollection* mySALOMEActors);

  //------------------------------------------------------------------
  // Erase/Display - Not implemented !!
  //------------------------------------------------------------------

  static void DisplayErase(vtkAssembly* mySALOMEAssembly);
  static void DisplayErase(vtkActorCollection* mySALOMEActors);


};
