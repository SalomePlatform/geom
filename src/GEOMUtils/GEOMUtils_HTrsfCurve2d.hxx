// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _GEOMUtils_HTrsfCurve2d_HXX_
#define _GEOMUtils_HTrsfCurve2d_HXX_


#include <GEOMUtils_TrsfCurve2d.hxx>

#include <Adaptor2d_HCurve2d.hxx>


namespace GEOMUtils
{

  class HTrsfCurve2d;

  DEFINE_STANDARD_HANDLE(HTrsfCurve2d, Adaptor2d_HCurve2d);

  /*!
   *  This class represents an adaptor curve that represents an original curve
   *  transformed by an anisotropic transformation. This is a class manipulated
   *  by handle.
   */
  class HTrsfCurve2d : public Adaptor2d_HCurve2d
  {

  public:
    
    /**
     * Constructor. Initializes the object with the transformation parameters.
     * Input parameters are not checked for validity. It is under responsibility
     * of the caller.
     */
    Standard_EXPORT HTrsfCurve2d(const Handle(Geom2d_Curve) &theCurve,
                                 const Trsf2d               &theTrsf);
    
    /**
     * Constructor. Initializes the object with the curve, first and last
     * parameters and transformation. Input parameters are not checked
     * for validity. It is under responsibility of the caller.
     */
    Standard_EXPORT HTrsfCurve2d(const Handle(Geom2d_Curve) &theCurve,
                                 const Standard_Real         theUFirst,
                                 const Standard_Real         theULast,
                                 const Trsf2d               &theTrsf);

    /**
     * Redefined method from the base class.
     */
    const Adaptor2d_Curve2d &Curve2d() const
    { return myCurve; }

  private:

    TrsfCurve2d myCurve;

  public:

  OCCT_DEFINE_STANDARD_RTTIEXT(HTrsfCurve2d,Adaptor2d_HCurve2d)
  };
}

#endif
