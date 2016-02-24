// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _AdvancedEngine_PipeTShapeDriver_HXX
#define _AdvancedEngine_PipeTShapeDriver_HXX

#include "GEOM_BaseDriver.hxx"
#include "GEOMAlgo_State.hxx"

#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Geom_Surface.hxx>

DEFINE_STANDARD_HANDLE( AdvancedEngine_PipeTShapeDriver, GEOM_BaseDriver );

class AdvancedEngine_PipeTShapeDriver : public GEOM_BaseDriver
{
public:
 // Methods PUBLIC
  // 
  AdvancedEngine_PipeTShapeDriver();
  virtual Standard_Integer Execute(LOGBOOK& log) const;
  virtual void Validate(LOGBOOK&) const {}
  Standard_Boolean MustExecute(const LOGBOOK&) const { return Standard_True; }

  static const Standard_GUID& GetID();
  ~AdvancedEngine_PipeTShapeDriver() {};
  
  virtual bool GetCreationInformation(std::string&             theOperationName,
				      std::vector<GEOM_Param>& params);
  // Type management
  //
  OCCT_DEFINE_STANDARD_RTTIEXT(AdvancedEngine_PipeTShapeDriver,GEOM_BaseDriver)
private:

  /*!
   * \brief Create a T-Shape based on pipes
   * \param r1 - the internal radius of main pipe
   * \param w1 - the thickness of main pipe
   * \param l1 - the half-length of main pipe
   * \param r2 - the internal radius of incident pipe
   * \param w2 - the thickness of incident pipe
   * \param l2 - the half-length of main pipe
   * \retval TopoDS_Shape - Resulting shape
   */
  TopoDS_Shape MakePipeTShape(double r1, double w1, double l1,
                              double r2, double w2, double l2) const;

  /*!
   * \brief Create a quarter of a T-Shape based on pipes
   * \param r1 - the internal radius of main pipe
   * \param w1 - the thickness of main pipe
   * \param l1 - the half-length of main pipe
   * \param r2 - the internal radius of incident pipe
   * \param w2 - the thickness of incident pipe
   * \param l2 - the half-length of main pipe
   * \retval TopoDS_Shape - Resulting shape
   */
  TopoDS_Shape MakeQuarterPipeTShape(double r1, double w1, double l1,
                                     double r2, double w2, double l2) const;

  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
   * \param theSurface - the surface to check state of sub-shapes against
   * \param theShape - the shape to explore
   * \param theShapeType - type of sub-shape of theShape
   * \param theState - required state
   * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Handle(TColStd_HSequenceOfInteger)
    GetShapesOnSurfaceIDs(const Handle(Geom_Surface)& theSurface,
                          const TopoDS_Shape&         theShape,
                          TopAbs_ShapeEnum            theShapeType,
                          GEOMAlgo_State              theState) const;

  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
    * \param theBox - the box to check state of sub-shapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Handle(TColStd_HSequenceOfInteger)
  GetShapesOnBoxIDs(const TopoDS_Shape& aBox,
                 const TopoDS_Shape& aShape,
                 const Standard_Integer theShapeType,
                 GEOMAlgo_State theState) const;

  //=======================================================================
  //function : getCommonShapesOnCylinders
  //purpose  : return the common edge between 2 cylindrical surfaces
  //           along OX and OZ
  //=======================================================================
  void GetCommonShapesOnCylinders(const TopoDS_Shape& theShape,
                                  TopAbs_ShapeEnum theShapeType,
                                  double r, double r2,
                                  Handle(TopTools_HSequenceOfShape)& commonShapes) const;

};

#endif // _AdvancedEngine_PipeTShapeDriver_HXX
