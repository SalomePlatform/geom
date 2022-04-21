// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <GEOMImpl_ITestOperations.hxx>

#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <Bnd_Box.hxx>
#include <utilities.h>

#ifndef MAX2
#define MAX2(X, Y)    (Abs(X) > Abs(Y) ? Abs(X) : Abs(Y))
#endif
#ifndef MAX3
#define MAX3(X, Y, Z) (MAX2(MAX2(X, Y), Z))
#endif

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOMImpl_ITestOperations::GEOMImpl_ITestOperations(GEOM_Engine* theEngine)
: GEOM_IOperations(theEngine)
{
  MESSAGE("GEOMImpl_ITestOperations::GEOMImpl_ITestOperations");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_ITestOperations::~GEOMImpl_ITestOperations()
{
  MESSAGE("GEOMImpl_ITestOperations::~GEOMImpl_ITestOperations");
}


//=============================================================================
/*!
 *  \brief Build a mesh on (a copy of ) the given shape.
 *
 *  This test function is aimed for checking performance of OCCT tesselation
 *  algorithm on particlar geometrical shapes.
 *
 *  \param theShape is a source object
 *  \param theLinearDeflection is a value of deflection coefficient
 *  \param theIsRelative says if given value of deflection is relative to shape's bounding box
 *  \param theAngularDeflection is a angular deflection for edges in radians
 *  \return \c true in case of success; otherwise \c false.
 */
//=============================================================================
bool GEOMImpl_ITestOperations::Tesselate(Handle(GEOM_Object) theShape,
					 double theLinearDeflection,
					 bool theIsRelative,
					 double theAngularDeflection)
{
  // ATTENTION!
  // We don't need results of this method to be present in the data tree;
  // so we don't need a driver for it.

  // reset error code
  SetErrorCode(KO);
  // create a copy of the source shape
  TopoDS_Shape aShape = BRepBuilderAPI_Copy(theShape->GetValue()).Shape();
  // use default deflection if necessary
  if (theLinearDeflection <= 0)
    theLinearDeflection = 0.001;
  // compute absolute deflection if necessary: 0.001
  if (theIsRelative) {
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    Bnd_Box bndBox;
    BRepBndLib::Add(aShape, bndBox);
    bndBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    theLinearDeflection = MAX3(aXmax-aXmin, aYmax-aYmin, aZmax-aZmin) * theLinearDeflection * 4;
  }
  // use default deviation angle if necessary: 20 degrees
  if (theAngularDeflection <= 0)
    theAngularDeflection = 20. * M_PI / 180.;
  // compute triangulation
  BRepTools::Clean(aShape);
  BRepMesh_IncrementalMesh aMesh(aShape, theLinearDeflection, Standard_False, theAngularDeflection);
  // set OK status and return
  SetErrorCode(OK);
  return true;
}
