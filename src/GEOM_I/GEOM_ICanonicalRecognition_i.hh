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

#ifndef _GEOM_ICanonicalRecognition_i_HeaderFile
#define _GEOM_ICanonicalRecognition_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_ICanonicalRecognition.hxx"

class GEOM_I_EXPORT GEOM_ICanonicalRecognition_i :
  public virtual POA_GEOM::GEOM_ICanonicalRecognition,
  public virtual GEOM_IOperations_i
{
 public:
  GEOM_ICanonicalRecognition_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
                           ::GEOMImpl_ICanonicalRecognition* theImpl);
  ~GEOM_ICanonicalRecognition_i();

  /*!
   *  \brief Check if the shape is planar
   */
  CORBA::Boolean isPlane(GEOM::GEOM_Object_ptr theShape, CORBA::Double theTolerance,
    GEOM::ListOfDouble& theNormal, GEOM::ListOfDouble& theOrigin);

  /*!
   * \brief Check if shape is spherical
   */
  CORBA::Boolean isSphere(GEOM::GEOM_Object_ptr theShape, CORBA::Double theTolerance,
    GEOM::ListOfDouble& theOrigin, CORBA::Double& theRadius);

  /*!
   * \brief Check if shape is conical
   */
  CORBA::Boolean isCone(GEOM::GEOM_Object_ptr theShape, CORBA::Double theTolerance,
    GEOM::ListOfDouble& theAxis, GEOM::ListOfDouble& theApex,
    CORBA::Double& theHalfAngle);

  /*!
   * \brief Check if shape is cylinder
   */
  CORBA::Boolean isCylinder(GEOM::GEOM_Object_ptr theShape, CORBA::Double theTolerance,
    GEOM::ListOfDouble& theAxis, GEOM::ListOfDouble& theOrigin,
    CORBA::Double& theRadius);

  /*!
   * \brief Check if edge / wire is line
   */
  CORBA::Boolean isLine(GEOM::GEOM_Object_ptr theEdge, CORBA::Double theTolerance,
    GEOM::ListOfDouble& theDir, GEOM::ListOfDouble& theOrigin);

  /*!
   * \brief Check if edge / wire is circle
   */
  CORBA::Boolean isCircle(GEOM::GEOM_Object_ptr theEdge, CORBA::Double theTolerance,
    GEOM::ListOfDouble& theNormal, GEOM::ListOfDouble& theOrigin,
    CORBA::Double& theRadius);

  /*!
   * \brief Check if edge / wire is ellipse
   */
  CORBA::Boolean isEllipse(GEOM::GEOM_Object_ptr theEdge, CORBA::Double theTolerance,
    GEOM::ListOfDouble& theNormal, GEOM::ListOfDouble& theDirX,
    GEOM::ListOfDouble& theOrigin,
    CORBA::Double& theMajorRadius, CORBA::Double& theMinorRadius);

  ::GEOMImpl_ICanonicalRecognition* GetOperation()
  { return (::GEOMImpl_ICanonicalRecognition*)GetImpl(); }
};

#endif
