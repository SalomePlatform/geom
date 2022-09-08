// Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _GEOMImpl_ICanonicalRecognition_HXX_
#define _GEOMImpl_ICanonicalRecognition_HXX_

#include "GEOM_IOperations.hxx"

#include <GEOM_Field.hxx>

#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>

#include <vector>

class GEOM_Object;

class GEOMImpl_ICanonicalRecognition : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_ICanonicalRecognition(GEOM_Engine* theEngine);
  Standard_EXPORT ~GEOMImpl_ICanonicalRecognition();

  /*!
   * \brief Check if the shape is planar
   */
  Standard_EXPORT bool isPlane(const Handle(GEOM_Object)& theShape, double theTolerance,
    gp_Pln& thePln);

  /*!
   * \brief Check if shape is spherical
   */
  Standard_EXPORT bool isSphere(const Handle(GEOM_Object)& theShape, double theTolerance,
    gp_Sphere& theSphere);

  /*!
   * \brief Check if shape is conical
   */
  Standard_EXPORT bool isCone(const Handle(GEOM_Object)& theShape, double theTolerance,
    gp_Cone& theCone);

  /*!
   * \brief Check if shape is cylinder
   */
  Standard_EXPORT bool isCylinder(const Handle(GEOM_Object)& theShape, double theTolerance,
    gp_Cylinder& theCylinder);

  /*!
   * \brief Check if edge / wire is line
   */
  Standard_EXPORT bool isLine(const Handle(GEOM_Object)& theEdge, double theTolerance,
    gp_Lin& theLine);

  /*!
   * \brief Check if edge / wire is circle
   */
  Standard_EXPORT bool isCircle(const Handle(GEOM_Object)& theEdge, double theTolerance,
    gp_Circ& theCircle);

  /*!
   * \brief Check if edge / wire is ellipse
   */
  Standard_EXPORT bool isEllipse(const Handle(GEOM_Object)& theEdge, double theTolerance,
    gp_Elips& theElips);

};

#endif
