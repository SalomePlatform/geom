// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

#ifndef _GEOM_IMeasureOperations_i_HeaderFile
#define _GEOM_IMeasureOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IMeasureOperations.hxx"

class GEOM_I_EXPORT GEOM_IMeasureOperations_i :
    public virtual POA_GEOM::GEOM_IMeasureOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_IMeasureOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
			    ::GEOMImpl_IMeasureOperations* theImpl);
  ~GEOM_IMeasureOperations_i();

  void GetPosition (GEOM::GEOM_Object_ptr theShape,
		    CORBA::Double& Ox, CORBA::Double& Oy, CORBA::Double& Oz,
		    CORBA::Double& Zx, CORBA::Double& Zy, CORBA::Double& Zz,
		    CORBA::Double& Xx, CORBA::Double& Xy, CORBA::Double& Xz);

  void GetBasicProperties (GEOM::GEOM_Object_ptr theShape,
			   CORBA::Double& theLength,
			   CORBA::Double& theSurfArea,
			   CORBA::Double& theVolume);

  GEOM::GEOM_Object_ptr GetCentreOfMass (GEOM::GEOM_Object_ptr theShape);

  void GetInertia (GEOM::GEOM_Object_ptr theShape,
		   CORBA::Double& I11, CORBA::Double& I12, CORBA::Double& I13,
		   CORBA::Double& I21, CORBA::Double& I22, CORBA::Double& I23,
		   CORBA::Double& I31, CORBA::Double& I32, CORBA::Double& I33,
		   CORBA::Double& Ix , CORBA::Double& Iy , CORBA::Double& Iz);

  void GetBoundingBox (GEOM::GEOM_Object_ptr theShape,
		       CORBA::Double& Xmin, CORBA::Double& Xmax,
		       CORBA::Double& Ymin, CORBA::Double& Ymax,
		       CORBA::Double& Zmin, CORBA::Double& Zmax);

  void GetTolerance (GEOM::GEOM_Object_ptr theShape,
		     CORBA::Double& FaceMin, CORBA::Double& FaceMax,
		     CORBA::Double& EdgeMin, CORBA::Double& EdgeMax,
		     CORBA::Double& VertMin, CORBA::Double& VertMax);

  CORBA::Boolean CheckShape (GEOM::GEOM_Object_ptr theShape,
			     CORBA::String_out     theDescription);

  CORBA::Boolean CheckShapeWithGeometry (GEOM::GEOM_Object_ptr theShape,
					 CORBA::String_out     theDescription);

  char* WhatIs (GEOM::GEOM_Object_ptr theShape);

  CORBA::Double GetMinDistance (GEOM::GEOM_Object_ptr theShape1,
				GEOM::GEOM_Object_ptr theShape2,
                                CORBA::Double& X1, CORBA::Double& Y1, CORBA::Double& Z1,
                                CORBA::Double& X2, CORBA::Double& Y2, CORBA::Double& Z2);


  void PointCoordinates( GEOM::GEOM_Object_ptr theShape,
    CORBA::Double& X, CORBA::Double& Y, CORBA::Double& Z );

  ::GEOMImpl_IMeasureOperations* GetOperations()
  { return (::GEOMImpl_IMeasureOperations*)GetImpl(); }
};

#endif
