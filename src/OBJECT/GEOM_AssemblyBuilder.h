//  File      : GEOM_AssemblyBuilder.h
//  Created   : Wed Feb 20 17:24:59 2002
//  Author    : Christophe ATTANASIO
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE 2002
//  $Header$

// VTK
#include <vtkAssembly.h>
#include <vtkPropAssembly.h>
#include <vtkActorCollection.h>

// Open CASCADE Inlcudes
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

class GEOM_AssemblyBuilder {

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
