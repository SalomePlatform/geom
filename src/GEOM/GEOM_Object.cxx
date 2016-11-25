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

#include "GEOM_Object.hxx"

#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>

#include "utilities.h"

// #define FUNCTION_LABEL(theNb) (_label.FindChild(1).FindChild((theNb)))
// #define TYPE_LABEL       2 -- Labels used by GEOM_BaseObject
// #define FREE_LABEL       3
// #define TIC_LABEL        4
#define COLOR_LABEL      5
#define AUTO_COLOR_LABEL 6
#define MARKER_LABEL     7

#define MARKER_LABEL_TYPE 1
#define MARKER_LABEL_SIZE 2
#define MARKER_LABEL_ID   3

//=============================================================================
/*!
 *  GetObject
 */
//=============================================================================

Handle(GEOM_Object) GEOM_Object::GetObject(const TDF_Label& theLabel)
{
  Handle(GEOM_BaseObject) base = GEOM_BaseObject::GetObject(theLabel);
  return Handle(GEOM_Object)::DownCast( base );
}

//=============================================================================
/*!
 *  GetReferencedObject
 */
//=============================================================================

Handle(GEOM_Object) GEOM_Object::GetReferencedObject(const TDF_Label& theLabel)
{
  Handle(GEOM_BaseObject) base = GEOM_BaseObject::GetReferencedObject(theLabel);
  return Handle(GEOM_Object)::DownCast( base );
}

//=============================================================================
/*!
 *  Constructor: private
 */
//=============================================================================

GEOM_Object::GEOM_Object(TDF_Label& theEntry)
  : GEOM_BaseObject(theEntry)
{
}

//=============================================================================
/*!
 *  Constructor: public
 */
//=============================================================================

GEOM_Object::GEOM_Object(TDF_Label& theEntry, int theType)
  : GEOM_BaseObject( theEntry, theType )
{
}

//=============================================================================
/*!
 *  Destructor
 */
//=============================================================================
GEOM_Object::~GEOM_Object()
{
  //MESSAGE("GEOM_Object::~GEOM_Object()");
}

//=============================================================================
/*!
 *  GetValue
 */
//=============================================================================
TopoDS_Shape GEOM_Object::GetValue()
{
  TopoDS_Shape aShape;

  Handle(GEOM_Function) aFunction = GetLastFunction();

  if (!aFunction.IsNull())
    aShape = aFunction->GetValue();

  return aShape;
}

//=============================================================================
/*!
 *  SetColor
 */
//=============================================================================
void GEOM_Object::SetColor(const GEOM_Object::Color& theColor)
{
  Handle(TDataStd_RealArray) anArray = new TDataStd_RealArray();
  anArray->Init( 1, 3 );
  anArray->SetValue( 1, theColor.R );
  anArray->SetValue( 2, theColor.G );
  anArray->SetValue( 3, theColor.B );

  Handle(TDataStd_RealArray) anAttr =
    TDataStd_RealArray::Set(_label.FindChild(COLOR_LABEL), anArray->Lower(), anArray->Upper());
  anAttr->ChangeArray(anArray->Array());
}

//=============================================================================
/*!
 *  GetColor
 */
//=============================================================================
GEOM_Object::Color GEOM_Object::GetColor()
{
  Handle(TDataStd_RealArray) anArray;
  bool isFound = _label.FindChild(COLOR_LABEL).FindAttribute(TDataStd_RealArray::GetID(), anArray);

  GEOM_Object::Color aColor;
  aColor.R = isFound ? anArray->Value( 1 ) : -1;
  aColor.G = isFound ? anArray->Value( 2 ) : -1;
  aColor.B = isFound ? anArray->Value( 3 ) : -1;

  return aColor;
}

//=============================================================================
/*!
 *  SetAutoColor
 */
//=============================================================================
void GEOM_Object::SetAutoColor(bool theAutoColor)
{
  TDataStd_Integer::Set(_label.FindChild(AUTO_COLOR_LABEL), (int)theAutoColor);
}

//=============================================================================
/*!
 *  GetAutoColor
 */
//=============================================================================
bool GEOM_Object::GetAutoColor()
{
  Handle(TDataStd_Integer) anAutoColor;
  if(!_label.FindChild(AUTO_COLOR_LABEL).FindAttribute(TDataStd_Integer::GetID(), anAutoColor)) return false;

  return bool(anAutoColor->Get());
}

//=============================================================================
/*!
 *  SetMarkerStd
 */
//=============================================================================
void GEOM_Object::SetMarkerStd(const Aspect_TypeOfMarker theType, double theSize)
{
  TDF_Label aMarkerLabel = _label.FindChild(MARKER_LABEL);
  TDataStd_Integer::Set(aMarkerLabel.FindChild(MARKER_LABEL_TYPE), (int)theType);
  TDataStd_Real::Set(aMarkerLabel.FindChild(MARKER_LABEL_SIZE), theSize);
}
  
//=============================================================================
/*!
 *  SetMarkerTexture
 */
//=============================================================================
void GEOM_Object::SetMarkerTexture(int theTextureId)
{
  TDF_Label aMarkerLabel = _label.FindChild(MARKER_LABEL);
  TDataStd_Integer::Set(aMarkerLabel.FindChild(MARKER_LABEL_TYPE), (int)Aspect_TOM_USERDEFINED);
  TDataStd_Integer::Set(aMarkerLabel.FindChild(MARKER_LABEL_ID),   theTextureId);
}

//=============================================================================
/*!
 *  GetMarkerType
 */
//=============================================================================
Aspect_TypeOfMarker GEOM_Object::GetMarkerType()
{
  Standard_Integer aType = -1;
  TDF_Label aMarkerLabel = _label.FindChild(MARKER_LABEL, Standard_False);
  if(!aMarkerLabel.IsNull()) {
    TDF_Label aTypeLabel = aMarkerLabel.FindChild(MARKER_LABEL_TYPE, Standard_False);
    Handle(TDataStd_Integer) aTypeAttr;
    if (!aTypeLabel.IsNull() && aTypeLabel.FindAttribute(TDataStd_Integer::GetID(), aTypeAttr))
      aType = aTypeAttr->Get();
  }
  return (Aspect_TypeOfMarker)aType;
}

//=============================================================================
/*!
 *  GetMarkerSize
 */
//=============================================================================
double GEOM_Object::GetMarkerSize()
{
  Standard_Real aSize = 0.;
  TDF_Label aMarkerLabel = _label.FindChild(MARKER_LABEL, Standard_False);
  if(!aMarkerLabel.IsNull()) {
    TDF_Label aSizeLabel = aMarkerLabel.FindChild(MARKER_LABEL_SIZE, Standard_False);
    Handle(TDataStd_Real) aSizeAttr;
    if (!aSizeLabel.IsNull() && aSizeLabel.FindAttribute(TDataStd_Real::GetID(), aSizeAttr))
      aSize = aSizeAttr->Get();
  }
  return aSize;
}

//=============================================================================
/*!
 *  GetMarkerTexture
 */
//=============================================================================
int GEOM_Object::GetMarkerTexture()
{
  Standard_Integer anId = 0;
  if ( GetMarkerType() == Aspect_TOM_USERDEFINED) {
    TDF_Label aMarkerLabel = _label.FindChild(MARKER_LABEL, Standard_False);
    if(!aMarkerLabel.IsNull()) {
      TDF_Label aTypeLabel = aMarkerLabel.FindChild(MARKER_LABEL_ID, Standard_False);
      Handle(TDataStd_Integer) anIdAttr;
      if (!aTypeLabel.IsNull() && aTypeLabel.FindAttribute(TDataStd_Integer::GetID(), anIdAttr))
        anId = anIdAttr->Get();
    }
  }
  return anId;
}

//=============================================================================
/*!
 *  SetAuxData
 */
//=============================================================================
void GEOM_Object::UnsetMarker()
{
  SetMarkerStd((Aspect_TypeOfMarker)-1, 0.);
}

//=============================================================================
/*!
 *  IsSubShape
 */
//=============================================================================
bool GEOM_Object::IsMainShape()
{
  Handle(GEOM_Function) aFunction = GetFunction(1);
  if(aFunction.IsNull() || aFunction->GetDriverGUID() != GetSubShapeID())
    return true; // mkr : IPAL9921
  return false;
}

//================================================================================
/*!
 * \brief Returns GetLastFunction() of given objects
 */
//================================================================================

Handle(TColStd_HSequenceOfTransient)
GEOM_Object::GetLastFunctions( const std::list< Handle(GEOM_Object) >& theObjects )
{
  Handle(TColStd_HSequenceOfTransient) funs = new TColStd_HSequenceOfTransient;
  std::list<Handle(GEOM_Object)>::const_iterator it = theObjects.begin();
  for (; it != theObjects.end(); it++)
  {
    Handle(GEOM_Function) fun = (*it)->GetLastFunction();
    if ( fun.IsNull())
      return Handle(TColStd_HSequenceOfTransient)();
    funs->Append( fun );
  }
  return funs;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT(GEOM_Object, GEOM_BaseObject );

