// Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GEOMImpl_ICanonicalRecognition.hxx"

#include <Basics_OCCTVersion.hxx>

#include "GEOM_Function.hxx"
#include "GEOM_Object.hxx"
#include "GEOM_PythonDump.hxx"

#if OCC_VERSION_LARGE > 0x07050303
#include <ShapeAnalysis_CanonicalRecognition.hxx>
#endif

#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>
#include <gp_Pln.hxx>

#include <utilities.h>


//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOMImpl_ICanonicalRecognition::GEOMImpl_ICanonicalRecognition (GEOM_Engine* theEngine)
: GEOM_IOperations(theEngine)
{
  MESSAGE("GEOMImpl_ICanonicalRecognition::GEOMImpl_ICanonicalRecognition");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_ICanonicalRecognition::~GEOMImpl_ICanonicalRecognition()
{
  MESSAGE("GEOMImpl_ICanonicalRecognition::~GEOMImpl_ICanonicalRecognition");
}

//=============================================================================
/*!
 * \brief Check if the shape is planar
 */
 //=============================================================================
bool GEOMImpl_ICanonicalRecognition::isPlane(const Handle(GEOM_Object)& theShape, double theTolerance, gp_Pln& thePln)
{
  SetErrorCode(KO);
  if (theShape.IsNull()) {
    SetErrorCode("Error: NULL shape");
    return false;
  }
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("Error: NULL shape");
    return false;
  }

#if OCC_VERSION_LARGE < 0x07050304
  SetErrorCode("Shape type detection aborted. Improper OCCT version: please, use OCCT 7.5.3p5 or newer.");
  return false;
#else
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  SetErrorCode(OK);
  return aRecognition.GetStatus() == 0 && aRecognition.IsPlane(theTolerance, thePln);
#endif
}

//=============================================================================
/*!
 * \brief Check if shape is spherical
 */
 //=============================================================================
bool GEOMImpl_ICanonicalRecognition::isSphere(const Handle(GEOM_Object)& theShape, double theTolerance,
  gp_Sphere& theSphere)
{
  SetErrorCode(KO);
  if (theShape.IsNull()) {
    SetErrorCode("Error: NULL shape");
    return false;
  }
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("Error: NULL shape");
    return false;
  }

#if OCC_VERSION_LARGE < 0x07050304
  SetErrorCode("Shape type detection aborted. Improper OCCT version: please, use OCCT 7.5.3p5 or newer.");
  return false;
#else
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  SetErrorCode(OK);
  return aRecognition.GetStatus() == 0 && aRecognition.IsSphere(theTolerance, theSphere);
#endif
}

//=============================================================================
/*!
 * \brief Check if shape is conical
 */
 //=============================================================================
bool GEOMImpl_ICanonicalRecognition::isCone(const Handle(GEOM_Object)& theShape, double theTolerance,
  gp_Cone& theCone)
{
  SetErrorCode(KO);
  if (theShape.IsNull()) {
    SetErrorCode("Error: NULL shape");
    return false;
  }
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("Error: NULL shape");
    return false;
  }

#if OCC_VERSION_LARGE < 0x07050304
  SetErrorCode("Shape type detection aborted. Improper OCCT version: please, use OCCT 7.5.3p5 or newer.");
  return false;
#else
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  SetErrorCode(OK);
  return aRecognition.GetStatus() == 0 && aRecognition.IsCone(theTolerance, theCone);
#endif
}

//=============================================================================
/*!
 * \brief Check if shape is cylinder
 */
 //=============================================================================
bool GEOMImpl_ICanonicalRecognition::isCylinder(const Handle(GEOM_Object)& theShape, double theTolerance,
  gp_Cylinder& theCylinder)
{
  SetErrorCode(KO);
  if (theShape.IsNull()) {
    SetErrorCode("Error: NULL shape");
    return false;
  }
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("Error: NULL shape");
    return false;
  }

#if OCC_VERSION_LARGE < 0x07050304
  SetErrorCode("Shape type detection aborted. Improper OCCT version: please, use OCCT 7.5.3p5 or newer.");
  return false;
#else
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  SetErrorCode(OK);
  return aRecognition.GetStatus() == 0 && aRecognition.IsCylinder(theTolerance, theCylinder);
#endif
}

//=============================================================================
/*!
 * \brief Check if edge / wire is line
 */
 //=============================================================================
bool GEOMImpl_ICanonicalRecognition::isLine(const Handle(GEOM_Object)& theEdge, double theTolerance,
  gp_Lin& theLine)
{
  SetErrorCode(KO);
  if (theEdge.IsNull()) {
    SetErrorCode("Error: NULL edge");
    return false;
  }
  TopoDS_Shape aShape = theEdge->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("Error: NULL shape");
    return false;
  }

#if OCC_VERSION_LARGE < 0x07050304
  SetErrorCode("Shape type detection aborted. Improper OCCT version: please, use OCCT 7.5.3p5 or newer.");
  return false;
#else
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  SetErrorCode(OK);
  return aRecognition.GetStatus() == 0 && aRecognition.IsLine(theTolerance, theLine);
#endif
}

//=============================================================================
/*!
 * \brief Check if edge / wire is circle
 */
 //=============================================================================
bool GEOMImpl_ICanonicalRecognition::isCircle(const Handle(GEOM_Object)& theEdge, double theTolerance,
  gp_Circ& theCircle)
{
  SetErrorCode(KO);
  if (theEdge.IsNull()) {
    SetErrorCode("Error: NULL edge");
    return false;
  }
  TopoDS_Shape aShape = theEdge->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("Error: NULL shape");
    return false;
  }

#if OCC_VERSION_LARGE < 0x07050304
  SetErrorCode("Shape type detection aborted. Improper OCCT version: please, use OCCT 7.5.3p5 or newer.");
  return false;
#else
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  SetErrorCode(OK);
  return aRecognition.GetStatus() == 0 && aRecognition.IsCircle(theTolerance, theCircle);
#endif
}

//=============================================================================
/*!
 * \brief Check if edge / wire is ellipse
 */
 //=============================================================================
bool GEOMImpl_ICanonicalRecognition::isEllipse(const Handle(GEOM_Object)& theEdge, double theTolerance,
  gp_Elips& theElips)
{
  SetErrorCode(KO);
  if (theEdge.IsNull()) {
    SetErrorCode("Error: NULL edge");
    return false;
  }
  TopoDS_Shape aShape = theEdge->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("Error: NULL shape");
    return false;
  }

#if OCC_VERSION_LARGE < 0x07050304
  SetErrorCode("Shape type detection aborted. Improper OCCT version: please, use OCCT 7.5.3p5 or newer.");
  return false;
#else
  ShapeAnalysis_CanonicalRecognition aRecognition(aShape);
  SetErrorCode(OK);
  return aRecognition.GetStatus() == 0 && aRecognition.IsEllipse(theTolerance, theElips);
#endif
}
