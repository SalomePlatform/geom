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
//  File   : AdvancedEngine_IOperations.hxx
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef _AdvancedEngine_IOperations_HXX_
#define _AdvancedEngine_IOperations_HXX_

#include "AdvancedEngine.hxx"

#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#include <TopTools_ListOfShape.hxx>
#include <Geom_Surface.hxx>

#include <list>
#include <gp_Ax2.hxx>

class GEOMImpl_IBasicOperations;
class GEOMImpl_IBooleanOperations;
class GEOMImpl_IShapesOperations;
class GEOMImpl_ITransformOperations;
class GEOMImpl_IBlocksOperations;
class GEOMImpl_I3DPrimOperations;
class GEOMImpl_ILocalOperations;
class GEOMImpl_IHealingOperations;
class GEOMImpl_IGroupOperations;

class ADVANCEDENGINE_EXPORT AdvancedEngine_IOperations: public GEOM_IOperations
{
private:
  bool MakePipeTShapePartition(Handle(GEOM_Object) theShape,
                               double theR1, double theW1, double theL1,
                               double theR2, double theW2, double theL2,
                               double theH = 0, double theW = 0,
                               double theRF = 0, bool isNormal = true);

  bool MakePipeTShapeMirrorAndGlue(Handle(GEOM_Object) theShape,
                                   double theR1, double theW1, double theL1,
                                   double theR2, double theW2, double theL2);

  bool MakePipeTShapeThicknessReduction (Handle(GEOM_Object) theShape,
                                         double theR1, double theW1, double theL1,
                                         double theR2, double theW2, double theL2,
                                         double theRL, double theWL, double theLtransL, double theLthinL,
                                         double theRR, double theWR, double theLtransR, double theLthinR,
                                         double theRI, double theWI, double theLtransI, double theLthinI);

  bool MakeGroups(Handle(GEOM_Object) theShape, int shapType,
                  double theR1, double theW1, double theL1,
                  double theR2, double theW2, double theL2,
                  double theH, double theW, double theRF,
                  Handle(TColStd_HSequenceOfTransient) theSeq,
                  gp_Trsf aTrsf);

  bool GetFacesOnSurf(const TopoDS_Shape &theShape,
                      const Handle(Geom_Surface)& theSurface,
                      const Standard_Real theTolerance,
                      TopTools_ListOfShape &theFaces);

  TopoDS_Shape MakeConicalFace(const gp_Ax2 &theAxis,
                               const double theRadius,
                               const double theRadiusThin,
                               const double theHeight,
                               const gp_Trsf &theTrsf);

  bool MakeInternalGroup(const Handle(GEOM_Object) &theShape,
                         const double theR1, const double theLen1,
                         const double theR2, const double theLen2,
                         const double theRL, const double theTransLenL,
                         const double theRR, const double theTransLenR,
                         const double theRI, const double theTransLenI,
                         const Handle(TColStd_HSequenceOfTransient) &theSeq,
                         const gp_Trsf &theTrsf);

  gp_Trsf GetPositionTrsf(double theL1, double theL2,
                          Handle(GEOM_Object) P1 = 0,
                          Handle(GEOM_Object) P2 = 0,
                          Handle(GEOM_Object) P3 = 0);

  bool CheckCompatiblePosition(double& theL1, double& theL2, 
                               Handle(GEOM_Object) theP1, 
                               Handle(GEOM_Object) theP2,
                               Handle(GEOM_Object) theP3,
                               double theTolerance);

private:
  GEOMImpl_IBasicOperations*     myBasicOperations;
  GEOMImpl_IBooleanOperations*   myBooleanOperations;
  GEOMImpl_IShapesOperations*    myShapesOperations;
  GEOMImpl_ITransformOperations* myTransformOperations;
  GEOMImpl_IBlocksOperations*    myBlocksOperations;
  GEOMImpl_I3DPrimOperations*    my3DPrimOperations;
  GEOMImpl_ILocalOperations*     myLocalOperations;
  GEOMImpl_IHealingOperations*   myHealingOperations;
  GEOMImpl_IGroupOperations*     myGroupOperations;

public:

  /*!
   * \brief Add three thickness reductions at the open ends of the pipe T-Shape
   *
   * \param theShape - the pipe T-Shape
   * \param r1 - the internal radius of main pipe
   * \param w1 - the thickness of main pipe
   * \param l1 - the half-length of main pipe
   * \param r2 - the internal radius of incident pipe
   * \param w2 - the thickness of incident pipe
   * \param l2 - the half-length of main pipe
   * \param r*, w*, ltrans* and lthin* - internal radius, thickness, length of transition part
   *                                     and length of thin part of left(L), right(R) and
   *                                     incident(I) thickness reduction correspondingly
   * \param fuseReductions - boolean flag (use true to generate single solid,
   *                         false to obtain parts, useful for hexameshing)
   * \retval TopoDS_Shape - Resulting shape
   */
  static TopoDS_Shape MakePipeTShapeThicknessReduction
                                     (TopoDS_Shape theShape,
                                      double r1, double w1, double l1,
                                      double r2, double w2, double l2,
                                      double rL, double wL, double ltransL, double lthinL,
                                      double rR, double wR, double ltransR, double lthinR,
                                      double rI, double wI, double ltransI, double lthinI,
                                      bool fuseReductions);

  /*!
   * \brief Create one thickness reduction element
   *
   * This method is called three times from MakePipeTShapeThicknessReduction
   * to create three thickness reductions (one per each open end of a pipe T-Shape)
   *
   * \param theAxes - the position
   * \param R - the internal radius of main pipe
   * \param W - the thickness of main pipe
   * \param Rthin - the internal radius of thin part
   * \param Wthin - the thickness of thin part
   * \param Ltrans - the length of transition part
   * \param Lthin - the length of thin part
   * \param fuse - boolean flag (use true to generate single solid,
   *               false to obtain parts, useful for hexameshing)
   * \retval TopoDS_Shape - Resulting shape
   */
  static TopoDS_Shape MakeThicknessReduction (gp_Ax2 theAxes,
					      const double R, const double W,
					      const double Rthin, const double Wthin,
					      const double Ltrans, const double Lthin,
					      bool fuse);
  
public:
  AdvancedEngine_IOperations(GEOM_Engine* theEngine, int theDocID);
  ~AdvancedEngine_IOperations();

  Handle(TColStd_HSequenceOfTransient) 
                  MakePipeTShape(double theR1, double theW1, double theL1,
                                 double theR2, double theW2, double theL2,
                                 double theRL, double theWL, double theLtransL, double theLthinL,
                                 double theRR, double theWR, double theLtransR, double theLthinR,
                                 double theRI, double theWI, double theLtransI, double theLthinI,
                                 bool theHexMesh = true);

  Handle(TColStd_HSequenceOfTransient)
                  MakePipeTShapeWithPosition(double theR1, double theW1, double theL1,
                                             double theR2, double theW2, double theL2,
                                             double theRL, double theWL, double theLtransL, double theLthinL,
                                             double theRR, double theWR, double theLtransR, double theLthinR,
                                             double theRI, double theWI, double theLtransI, double theLthinI,
                                             bool theHexMesh = true,
                                             Handle(GEOM_Object) P1 = 0,
                                             Handle(GEOM_Object) P2 = 0,
                                             Handle(GEOM_Object) P3 = 0);

  Handle(TColStd_HSequenceOfTransient)
                  MakePipeTShapeChamfer(double theR1, double theW1, double theL1,
                                        double theR2, double theW2, double theL2,
                                        double theRL, double theWL, double theLtransL, double theLthinL,
                                        double theRR, double theWR, double theLtransR, double theLthinR,
                                        double theRI, double theWI, double theLtransI, double theLthinI,
                                        double theH,  double theW, 
                                        bool theHexMesh = true);

  Handle(TColStd_HSequenceOfTransient)
                  MakePipeTShapeChamferWithPosition(double theR1, double theW1, double theL1,
                                                    double theR2, double theW2, double theL2,
                                                    double theH, double theW,
                                                    double theRL, double theWL, double theLtransL, double theLthinL,
                                                    double theRR, double theWR, double theLtransR, double theLthinR,
                                                    double theRI, double theWI, double theLtransI, double theLthinI,
                                                    bool theHexMesh = true,
                                                    Handle(GEOM_Object) P1 = 0,
                                                    Handle(GEOM_Object) P2 = 0,
                                                    Handle(GEOM_Object) P3 = 0);

  Handle(TColStd_HSequenceOfTransient)
                  MakePipeTShapeFillet(double theR1, double theW1, double theL1,
                                       double theR2, double theW2, double theL2,
                                       double theRL, double theWL, double theLtransL, double theLthinL,
                                       double theRR, double theWR, double theLtransR, double theLthinR,
                                       double theRI, double theWI, double theLtransI, double theLthinI,
                                       double theRF, bool theHexMesh = true);

  Handle(TColStd_HSequenceOfTransient)
                  MakePipeTShapeFilletWithPosition(double theR1, double theW1, double theL1,
                                                   double theR2, double theW2, double theL2,
                                                   double theRL, double theWL, double theLtransL, double theLthinL,
                                                   double theRR, double theWR, double theLtransR, double theLthinR,
                                                   double theRI, double theWI, double theLtransI, double theLthinI,
                                                   double theRF, bool theHexMesh = true,
                                                   Handle(GEOM_Object) P1 = 0,
                                                   Handle(GEOM_Object) P2 = 0,
                                                   Handle(GEOM_Object) P3 = 0);
                  
  Handle(GEOM_Object) MakeDividedDisk (double theR, double theRatio, 
				       int theOrientation, int thePattern);
  Handle(GEOM_Object) MakeDividedDiskPntVecR (Handle(GEOM_Object) thePnt, 
					      Handle(GEOM_Object) theVec, 
					      double theR, 
					      double theRatio,
					      int    thePattern);
  
  Handle(GEOM_Object) MakeDividedCylinder (double theR, 
					   double theH,
					   int thePattern);
  
  Handle(GEOM_Object) MakeSmoothingSurface (std::list<Handle(GEOM_Object)> thelPoints, 
					    int                            theNbMax,
					    int                            theDegMax,
					    double                         theDMax);
};

#endif
