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

#ifndef _GEOM_IBasicOperations_i_HeaderFile
#define _GEOM_IBasicOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IBasicOperations.hxx"

class GEOM_I_EXPORT GEOM_IBasicOperations_i :
    public virtual POA_GEOM::GEOM_IBasicOperations,
    public virtual GEOM_IOperations_i
{
 public:
   GEOM_IBasicOperations_i (PortableServer::POA_ptr thePOA,
			    GEOM::GEOM_Gen_ptr theEngine,
			    ::GEOMImpl_IBasicOperations* theImpl);
   ~GEOM_IBasicOperations_i();

   GEOM::GEOM_Object_ptr MakePointXYZ (CORBA::Double theX,
				       CORBA::Double theY,
				       CORBA::Double theZ);

   GEOM::GEOM_Object_ptr MakePointWithReference (GEOM::GEOM_Object_ptr theReference,
						 CORBA::Double theX,
						 CORBA::Double theY,
						 CORBA::Double theZ);

   GEOM::GEOM_Object_ptr MakePointOnCurve
                      (GEOM::GEOM_Object_ptr theCurve,
					   CORBA::Double         theParameter,
                       CORBA::Boolean        takeOrientationIntoAccount);

   GEOM::GEOM_Object_ptr MakePointOnCurveByLength (GEOM::GEOM_Object_ptr theCurve,
						   CORBA::Double         theLength,
						   GEOM::GEOM_Object_ptr theStartPoint);

   GEOM::GEOM_Object_ptr MakePointOnCurveByCoord (GEOM::GEOM_Object_ptr theCurve,
                                                  CORBA::Double theXParameter,
                                                  CORBA::Double theYParameter,
                                                  CORBA::Double theZParameter);

   GEOM::GEOM_Object_ptr MakePointOnSurface (GEOM::GEOM_Object_ptr theSurface,
					     CORBA::Double theUParameter,
					     CORBA::Double theVParameter);

   GEOM::GEOM_Object_ptr MakePointOnSurfaceByCoord (GEOM::GEOM_Object_ptr theSurface,
                                                    CORBA::Double theXParameter,
                                                    CORBA::Double theYParameter,
                                                    CORBA::Double theZParameter);

   GEOM::GEOM_Object_ptr MakePointOnFace (GEOM::GEOM_Object_ptr theFace);

   GEOM::GEOM_Object_ptr MakePointOnLinesIntersection (GEOM::GEOM_Object_ptr theLine1,
						       GEOM::GEOM_Object_ptr theLine2);

   GEOM::GEOM_Object_ptr MakeTangentOnCurve (GEOM::GEOM_Object_ptr theRefCurve,
					     CORBA::Double theParameter);

   GEOM::GEOM_Object_ptr MakeVectorDXDYDZ (CORBA::Double theDX,
					   CORBA::Double theDY,
					   CORBA::Double theDZ);

   GEOM::GEOM_Object_ptr MakeVectorTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
					   GEOM::GEOM_Object_ptr thePnt2);

   GEOM::GEOM_Object_ptr MakeLine (GEOM::GEOM_Object_ptr thePnt,
				   GEOM::GEOM_Object_ptr theDir);

   GEOM::GEOM_Object_ptr MakeLineTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
					 GEOM::GEOM_Object_ptr thePnt2);

   GEOM::GEOM_Object_ptr MakeLineTwoFaces (GEOM::GEOM_Object_ptr theFace1,
					   GEOM::GEOM_Object_ptr theFace2);

   GEOM::GEOM_Object_ptr MakePlaneThreePnt (GEOM::GEOM_Object_ptr thePnt1,
					    GEOM::GEOM_Object_ptr thePnt2,
					    GEOM::GEOM_Object_ptr thePnt3,
					    CORBA::Double theTrimSize);

   GEOM::GEOM_Object_ptr MakePlanePntVec (GEOM::GEOM_Object_ptr thePnt,
					  GEOM::GEOM_Object_ptr theVec,
					  CORBA::Double theTrimSize);

   GEOM::GEOM_Object_ptr MakePlaneFace (GEOM::GEOM_Object_ptr theFace,
					CORBA::Double theTrimSize);
					
   GEOM::GEOM_Object_ptr MakePlane2Vec (GEOM::GEOM_Object_ptr theVec1,
					  GEOM::GEOM_Object_ptr theVec2,
					  CORBA::Double theTrimSize);
					  
   GEOM::GEOM_Object_ptr MakePlaneLCS (GEOM::GEOM_Object_ptr theLCS,
				       CORBA::Double theTrimSize,
				       CORBA::Double theOrientation );

   GEOM::GEOM_Object_ptr MakeMarker (CORBA::Double theOX , CORBA::Double theOY , CORBA::Double theOZ,
				     CORBA::Double theXDX, CORBA::Double theXDY, CORBA::Double theXDZ,
				     CORBA::Double theYDX, CORBA::Double theYDY, CORBA::Double theYDZ);

   GEOM::GEOM_Object_ptr MakeMarkerFromShape (GEOM::GEOM_Object_ptr theShape);
   
   GEOM::GEOM_Object_ptr MakeMarkerPntTwoVec (GEOM::GEOM_Object_ptr theOrigin, GEOM::GEOM_Object_ptr theXVec, GEOM::GEOM_Object_ptr theYVec);

   GEOM::GEOM_Object_ptr MakeTangentPlaneOnFace (GEOM::GEOM_Object_ptr theFace, 
						 CORBA::Double theParameterU,
						 CORBA::Double theParameterV,
						 CORBA::Double theTrimSize);

   ::GEOMImpl_IBasicOperations* GetOperations() { return (::GEOMImpl_IBasicOperations*)GetImpl(); }
};

#endif
