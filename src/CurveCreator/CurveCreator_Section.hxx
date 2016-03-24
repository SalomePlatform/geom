// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:        CurveCreator_Section.hxx
// Author:      Sergey KHROMOV

#ifndef _CurveCreator_Section_HeaderFile
#define _CurveCreator_Section_HeaderFile

#include "CurveCreator.hxx"
#include "CurveCreator_ICurve.hxx"

#include <string>

//! Structure to store sections representing the CurveCreator_Curve object
struct CURVECREATOR_EXPORT CurveCreator_Section :
  public CurveCreator_ISection
{
  //! Constructor. Initializes object with default values.
  CurveCreator_Section() : myName("Section"),myType(CurveCreator::Polyline), myIsClosed(false)
  { }

  std::string               myName; //!< section name
  CurveCreator::Coordinates myPoints;   //!< points coordinates
  CurveCreator::SectionType myType;     //!< type of the section
  bool                      myIsClosed; //!< closed or not

  //! A virtual method.
  void GetDifferentPoints(const int theDimension, Handle(TColgp_HArray1OfPnt)& thePoints) const;
};

#endif
