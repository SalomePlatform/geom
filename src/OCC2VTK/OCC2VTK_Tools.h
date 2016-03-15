// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef OCC2VTK_TOOLS_H
#define OCC2VTK_TOOLS_H

#include "OCC2VTK.h" 

#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

class GEOM_VertexSource;
class GEOM_EdgeSource;
class GEOM_WireframeFace;
class GEOM_ShadingFace;
class vtkPolyData;

namespace GEOM
{
  /*!
   * \brief Convert shape to the VTK data sources
   * \param theShape shape
   * \param theEdgeMape map that stores face-to-edge relations
   * \param theIsVector boolen flag, when \c true causes generating additional
   * dataset for edges orientation vectors
   * \param theStandaloneVertexSource output standalone vertices data source
   * \param theIsolatedEdgeSource output standalone edges data source
   * \param theOneFaceEdgeSource output face edges data source
   * \param theSharedEdgeSource output face shared edges data source
   * \param theWireframeFaceSource output wireframe mode faces data source
   * \param theShadingFaceSource output shading mode faces data source
   */
  OCC2VTK_EXPORT void ShapeToVTK( const TopoDS_Shape& theShape,
				  const TopTools_IndexedDataMapOfShapeListOfShape& theEdgeMap,
				  bool theIsVector,
				  GEOM_VertexSource* theStandaloneVertexSource,
				  GEOM_EdgeSource* theIsolatedEdgeSource,
				  GEOM_EdgeSource* theOneFaceEdgeSource,
				  GEOM_EdgeSource* theSharedEdgeSource,
				  GEOM_WireframeFace* theWireframeFaceSource,
				  GEOM_ShadingFace* theShadingFaceSource );

  /*!
   * \brief Get VTK mesh data from the shape
   * \param theShape shape
   * \param theDeflection requested deflection coefficient
   * \return VTK data set
   */
  OCC2VTK_EXPORT vtkPolyData* GetVTKData( const TopoDS_Shape& theShape, float theDeflection );
}

#endif // OCC2VTK_TOOLS_H
