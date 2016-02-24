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

//  File   : GEOMImpl_PolylineDumper.h
//  Author : Sergey KHROMOV


#ifndef _GEOMImpl_PolylineDumper_HXX_
#define _GEOMImpl_PolylineDumper_HXX_


#include "GEOM_GEOMImpl.hxx"

#include <GEOM_Object.hxx>

#include <TColStd_HArray1OfExtendedString.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <TColStd_HArray1OfReal.hxx>

#include <list>


/**
 * This is a helper class to form a dump of a polyline 2d curves creation
 * algorithm.
 */
class GEOMIMPL_EXPORT GEOMImpl_PolylineDumper
{

public:

  /**
   * This construcor initializes the object with 2D polyline creation
   * parameters.
   *
   *  \param theCoords the list of coordinates list. theCoordsList[0]
   *         is the coordinates list of the first section. theCoordsList[1]
   *         is for the second section etc.
   *  \param theNames the list of names. The order corresponds to theCoords.
   *  \param theTypes the list of curve types. The order corresponds to
   *         theCoords.
   *  \param theCloseds the list of Closed flags. The order corresponds to
   *         theCoords.
   *  \param thePlnCoords 9 double values, defining origin,
   *         OZ and OX directions of the working plane.
   */
  GEOMImpl_PolylineDumper
            (const std::list <std::list <double> >        &theCoords,
             const Handle(TColStd_HArray1OfExtendedString) &theNames,
             const Handle(TColStd_HArray1OfByte)           &theTypes,
             const Handle(TColStd_HArray1OfByte)           &theCloseds,
             const Handle(TColStd_HArray1OfReal)           &thePlnCoords);

  /**
   * This construcor initializes the object with 2D polyline creation
   * parameters.
   *
   *  \param theCoords the list of coordinates list. theCoordsList[0]
   *         is the coordinates list of the first section. theCoordsList[1]
   *         is for the second section etc.
   *  \param theNames the list of names. The order corresponds to theCoords.
   *  \param theTypes the list of curve types. The order corresponds to
   *         theCoords.
   *  \param theCloseds the list of Closed flags. The order corresponds to
   *         theCoords.
   *  \param theWorkingPlane planar Face or LCS(Marker) of the working plane.
   */
  GEOMImpl_PolylineDumper
            (const std::list <std::list <double> >        &theCoords,
             const Handle(TColStd_HArray1OfExtendedString) &theNames,
             const Handle(TColStd_HArray1OfByte)           &theTypes,
             const Handle(TColStd_HArray1OfByte)           &theCloseds,
             const Handle(GEOM_Object)                     &theWorkingPlane);

  /**
   * This method returns Standard_True if the dump description is created
   * successfully.
   *
   * \return Standard_True in case of success; Standard_False otherwise.
   */
  Standard_Boolean IsDone() const
  { return myIsDone; }

  /**
   * This method performs dump of the polyline.
   *
   * \param theObject the newly created object.
   * \return Standard_True in case of success; Standard_False otherwise.
   */
  Standard_Boolean Dump(const Handle(GEOM_Object) &theObject);

protected:

  /**
   * This method generates the description required for python dump.
   * It is called from constructor.
   */
  void init();

private:

  const std::list <std::list <double> >  &myCoords;
  Handle(TColStd_HArray1OfExtendedString)  myNames;
  Handle(TColStd_HArray1OfByte)            myTypes;
  Handle(TColStd_HArray1OfByte)            myCloseds;
  Handle(TColStd_HArray1OfReal)            myPlnCoords;
  Handle(GEOM_Object)                      myWorkingPlane;
  Standard_Boolean                        myIsDone;
  TCollection_ExtendedString              myDescr;

};

#endif
