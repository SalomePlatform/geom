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

#ifndef _GEOMUtils_Hatcher_HXX_
#define _GEOMUtils_Hatcher_HXX_


#include <Geom2dHatch_Hatcher.hxx>
#include <GeomAbs_IsoType.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopoDS_Face.hxx>


/*!
 * This class represents a hatcher for topological faces.
 */
namespace GEOMUtils
{
  class Hatcher
  {
  public:
    
    /**
     * Constructor. Initializes the object with the face.
     */
    Standard_EXPORT Hatcher(const TopoDS_Face &theFace);
    
    /**
     * This method initializes the hatcher with hatchings.
     *
     * \param theNbIsos the number of U- and V-isolines.
     */
    Standard_EXPORT void Init(const Standard_Integer theNbIsos);
    
    /**
     * This method initializes the hatcher with hatchings.
     *
     * \param theNbIsoU the number of U-isolines.
     * \param theNbIsoV the number of V-isolines.
     */
    Standard_EXPORT void Init(const Standard_Integer theNbIsoU,
			      const Standard_Integer theNbIsoV);
    
    /**
     * This method initializes the hatcher with a hatching.
     *
     * \param theIsoType the isoline type.
     * \param theParameter the isoline parameter.
     */
    Standard_EXPORT void Init(const GeomAbs_IsoType theIsoType,
			      const Standard_Real   theParameter);

    /**
     * Compute hatching domatins.
     */
    Standard_EXPORT void Perform();

    /**
     * This method returns true if at least one hatching's domains
     * are computed successfully.
     *
     * \return Standard_True is case of success.
     */
    Standard_Boolean IsDone() const
    { return myIsDone; }

    /**
     * This method returns the initial face.
     *
     * \return the initial face.
     */
    const TopoDS_Face &GetFace() const
    { return myFace; }

    /**
     * This method returns the number of domains for a particular hatching.
     * If the operation is not done or there is no real hatching for
     * a particular index a negative value is returned.
     *
     * \param theHatchingIndex the hatching index.
     * \return the number of domains computed for the hatching.
     */
    Standard_EXPORT Standard_Integer GetNbDomains
    (const Standard_Integer theHatchingIndex) const;

    /**
     * This method returns the domputed domain range computed for a particular
     * hatching. theDomainIndex should be in the range [1..GetNbDomains].
     *
     * \param theHatchingIndex the hatching index.
     * \param theDomainIndex the domain index for the particular hatching.
     * \param theParam1 (output) the first parameter of the domain.
     * \param theParam2 (output) the last parameter of the domain.
     * \return Standard_True in case of success; Standard_False otherwise.
     */
    Standard_EXPORT Standard_Boolean GetDomain
    (const Standard_Integer  theHatchingIndex,
     const Standard_Integer  theDomainIndex,
     Standard_Real    &theParam1,
     Standard_Real    &theParam2) const;

    /**
     * This method returns Standard_True if a domain has infinite first
     * or last parameter.
     *
     * \param theHatchingIndex the hatching index.
     * \param theDomainIndex the domain index for the particular hatching.
     * \return Standard_True if a domain is infinite; Standard_False otherwise.
     */
    Standard_EXPORT Standard_Boolean IsDomainInfinite
    (const Standard_Integer  theHatchingIndex,
     const Standard_Integer  theDomainIndex) const;

    /**
     * This method returns the reference to OCCT hatcher.
     *
     * \return the reference to OCCT hatcher.
     */
    Standard_EXPORT const Geom2dHatch_Hatcher &GetHatcher() const
    { return myHatcher; }

    /**
     * This method returns the array of indices of U-isoline hatchings.
     * Can be null if the object is initialized by 0 U-isolines.
     *
     * \return the array of U-isoline hatching indices.
     */
    Standard_EXPORT const Handle(TColStd_HArray1OfInteger) &GetUIndices() const
    { return myUInd; }

    /**
     * This method returns the array of indices of V-isoline hatchings.
     * Can be null if the object is initialized by 0 V-isolines.
     *
     * \return the array of V-isoline hatching indices.
     */
    Standard_EXPORT const Handle(TColStd_HArray1OfInteger) &GetVIndices() const
    { return myVInd; }

    /**
     * This method returns the array of parameters of U-isoline hatchings.
     * Can be null if the object is initialized by 0 U-isolines.
     *
     * \return the array of U-isoline hatching parameters.
     */
    Standard_EXPORT const Handle(TColStd_HArray1OfReal) &GetUParams() const
    { return myUPrm; }

    /**
     * This method returns the array of parameters of V-isoline hatchings.
     * Can be null if the object is initialized by 0 V-isolines.
     *
     * \return the array of V-isoline hatching parameters.
     */
    Standard_EXPORT const Handle(TColStd_HArray1OfReal) &GetVParams() const
    { return myVPrm; }

    /**
     * This method returns a hatching curve by its index.
     * If the curve is not found null handle is returned.
     *
     * \param theHatchingIndex the hatching curve index.
     */
    Standard_EXPORT const Handle(Geom2d_Curve) &GetHatching
      (const Standard_Integer theHatchingIndex) const;

  protected:

    /**
     * This method clears all hatchings data.
     */
    void Clear();

  private:

    Geom2dHatch_Hatcher              myHatcher;
    TopoDS_Face                      myFace;
    Standard_Boolean                 myIsDone;
    Standard_Real                    myUMin;
    Standard_Real                    myUMax;
    Standard_Real                    myVMin;
    Standard_Real                    myVMax;
    Handle(TColStd_HArray1OfReal)    myUPrm;
    Handle(TColStd_HArray1OfReal)    myVPrm;
    Handle(TColStd_HArray1OfInteger) myUInd;
    Handle(TColStd_HArray1OfInteger) myVInd;

  };
}

#endif
