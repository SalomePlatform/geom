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

//  GEOM OBJECT : interactive object for Geometry entities visualization
//  File   : GEOM_AISDimension.hxx
//  Module : GEOM
//
#ifndef GEOM_AISDimension_HeaderFile
#define GEOM_AISDimension_HeaderFile

#include <AIS_LengthDimension.hxx>
#include <AIS_DiameterDimension.hxx>
#include <AIS_AngleDimension.hxx>

#include <Basics_OCCTVersion.hxx>

DEFINE_STANDARD_HANDLE(GEOM_AISLength,   AIS_LengthDimension)
DEFINE_STANDARD_HANDLE(GEOM_AISDiameter, AIS_DiameterDimension)
DEFINE_STANDARD_HANDLE(GEOM_AISAngle,    AIS_AngleDimension)

//=================================================================================
// class    : GEOM_AISLength
// purpose  : presentation of length dimension property
//=================================================================================
class GEOM_AISLength : public AIS_LengthDimension
{
public:

  OCCT_DEFINE_STANDARD_RTTIEXT(GEOM_AISLength,AIS_LengthDimension)

public:
  Standard_EXPORT                    GEOM_AISLength( const Standard_Integer theId );
  Standard_EXPORT  Standard_Integer  GetId() const;
  Standard_EXPORT  void              SetId( const Standard_Integer theId );

private:
  Standard_Integer                   myId;
};

//=================================================================================
// class    : GEOM_AISDiameter
// purpose  : presentation of diameter dimension property
//=================================================================================
class GEOM_AISDiameter : public AIS_DiameterDimension
{
public:

  OCCT_DEFINE_STANDARD_RTTIEXT(GEOM_AISDiameter,AIS_DiameterDimension)

public:
  Standard_EXPORT                     GEOM_AISDiameter( const Standard_Integer theId );
  Standard_EXPORT  Standard_Integer   GetId() const;
  Standard_EXPORT  void               SetId( const Standard_Integer theId );

private:
  Standard_Integer                    myId;
};

//=================================================================================
// class    : GEOM_AISAngle
// purpose  : presentation of angle dimension property
//=================================================================================
class GEOM_AISAngle : public AIS_AngleDimension
{
public:

  OCCT_DEFINE_STANDARD_RTTIEXT(GEOM_AISAngle,AIS_AngleDimension)

public:
  Standard_EXPORT                     GEOM_AISAngle( const Standard_Integer theId );
  Standard_EXPORT  Standard_Integer   GetId() const;
  Standard_EXPORT  void               SetId( const Standard_Integer theId );

private:
  Standard_Integer                    myId;
};

#endif
