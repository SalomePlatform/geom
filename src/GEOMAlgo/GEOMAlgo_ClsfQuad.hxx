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

// File:        GEOMAlgo_ClsfQuad.hxx
// Created:     Fri Feb 13 16:03:19 2015
// Author:      Sergey KHROMOV
//
//
#ifndef _GEOMAlgo_ClsfQuad_HeaderFile
#define _GEOMAlgo_ClsfQuad_HeaderFile


#include <GEOMAlgo_Clsf.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <Standard_DefineHandle.hxx>

#include <vector>


DEFINE_STANDARD_HANDLE(GEOMAlgo_ClsfQuad, GEOMAlgo_Clsf)

//=======================================================================
// class   : GEOMAlgo_ClsfQuad
//purpose  :
//=======================================================================
class GEOMAlgo_ClsfQuad : public GEOMAlgo_Clsf
{

public:

  Standard_EXPORT
    GEOMAlgo_ClsfQuad();

  Standard_EXPORT
    virtual ~GEOMAlgo_ClsfQuad();

  Standard_EXPORT
    void SetCorners(const gp_Pnt &theTopLeftPoint,
                    const gp_Pnt &theTopRigthPoint,
                    const gp_Pnt &theBottomLeftPoint,
                    const gp_Pnt &theBottomRigthPoint);

  Standard_EXPORT
    void GetCorners(gp_Pnt &theTopLeftPoint,
                    gp_Pnt &theTopRigthPoint,
                    gp_Pnt &theBottomLeftPoint,
                    gp_Pnt &theBottomRigthPoint) const;

  Standard_EXPORT
    virtual  void Perform();

  Standard_EXPORT
    virtual  void CheckData();

  Standard_EXPORT
    virtual  Standard_Boolean CanBeON(const Handle(Geom_Curve)& aC) const;

  Standard_EXPORT
    virtual  Standard_Boolean CanBeON(const Handle(Geom_Surface)& aST) const;

  OCCT_DEFINE_STANDARD_RTTIEXT(GEOMAlgo_ClsfQuad,GEOMAlgo_Clsf)

protected:

  bool                              myConcaveQuad;
  std::vector<bool>                 myConcaveSide;
  std::vector<gp_Pnt>               myPoints;
  std::vector<GeomAdaptor_Surface>  myPlanes;
  gp_Vec                            myQuadNormal;

};
#endif
