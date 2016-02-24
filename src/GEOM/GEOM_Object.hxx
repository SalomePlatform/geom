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

#ifndef _GEOM_Object_HeaderFile
#define _GEOM_Object_HeaderFile

#include "GEOM_BaseObject.hxx"

#include <Aspect_TypeOfMarker.hxx>
#include <Standard_GUID.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TDF_Label.hxx>
#include <TopoDS_Shape.hxx>

#include <list>

class GEOM_Object;
class TFunction_Driver;
class GEOM_Engine;

DEFINE_STANDARD_HANDLE( GEOM_Object, GEOM_BaseObject );

class GEOM_Object : public GEOM_BaseObject
{
 friend class GEOM_Engine;
 
 public:
  struct Color {
    //! Red component of the color
    double R;
    //! Green component of the color
    double G;
    //! Blue component  of the color
    double B;
  };

 private:
  GEOM_Object(TDF_Label& theLabel);

 public:
  Standard_EXPORT GEOM_Object(TDF_Label& theEntry, int theType);
  Standard_EXPORT ~GEOM_Object();

  //Finds a GEOM_Object on the label theLabel
  Standard_EXPORT static Handle(GEOM_Object) GetObject(const TDF_Label& theLabel);

  //Finds a GEOM_Object by a reference, stored on the label theLabel
  Standard_EXPORT static Handle(GEOM_Object) GetReferencedObject(const TDF_Label& theLabel);

  //###########################################################
  //Access to properties
  //###########################################################

  //Returns a value (as TopoDS_Shape) of this GEOM_Object
  Standard_EXPORT TopoDS_Shape GetValue();

  //Sets a color of this GEOM_Object
  Standard_EXPORT void SetColor(const Color& theColor);

  //Returns a color of this GEOM_Object
  Standard_EXPORT Color GetColor();

  //Toggles an auto color mode on this GEOM_Object
  Standard_EXPORT void SetAutoColor(bool theAutoColor);

  //Returns a flag of auto color mode of this GEOM_Object
  Standard_EXPORT bool GetAutoColor();

  //Sets predefined point marker texture
  Standard_EXPORT void SetMarkerStd(const Aspect_TypeOfMarker theType, double theSize);
  
  //Sets custom point marker texture
  Standard_EXPORT void SetMarkerTexture(int theTextureId);

  //Gets point marker type
  Standard_EXPORT Aspect_TypeOfMarker GetMarkerType();

  //Gets point marker scale factor / size
  Standard_EXPORT double GetMarkerSize();

  //Gets custom marker texture ID
  Standard_EXPORT int GetMarkerTexture();

  //Unsets point marker
  Standard_EXPORT void UnsetMarker();

  //###########################################################
  // Sub-shape methods
  //###########################################################

  //Returns false if the object is a sub-shape of another object
  Standard_EXPORT bool IsMainShape();

  //Comfort method
  Standard_EXPORT static Handle(TColStd_HSequenceOfTransient)
    GetLastFunctions( const std::list< Handle(GEOM_Object) >& theObjects );

public:
  OCCT_DEFINE_STANDARD_RTTIEXT(GEOM_Object,GEOM_BaseObject)
};

#endif
