
#ifndef _GEOM_IMeasureOperations_i_HeaderFile
#define _GEOM_IMeasureOperations_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IMeasureOperations.hxx"

class GEOM_IMeasureOperations_i :
    public virtual POA_GEOM::GEOM_IMeasureOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_IMeasureOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
			    ::GEOMImpl_IMeasureOperations* theImpl);
  ~GEOM_IMeasureOperations_i();

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
