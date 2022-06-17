// Copyright (C) 2022-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <GEOM_Function.hxx>

#define PROXIMITY_ARG_SHAPE1   1
#define PROXIMITY_ARG_SHAPE2   2
#define PROXIMITY_ARG_SAMPLES1 3
#define PROXIMITY_ARG_SAMPLES2 4
#define PROXIMITY_ARG_POINT1   5
#define PROXIMITY_ARG_POINT2   6
#define PROXIMITY_ARG_VALUE    7
#define PROXIMITY_ARG_STATUS1  8
#define PROXIMITY_ARG_STATUS2  9

class GEOMImpl_IProximity
{
public:

  GEOMImpl_IProximity(Handle(GEOM_Function) theFunction) : _func(theFunction) {}

  void SetShapes(Handle(GEOM_Function) theShape1, Handle(GEOM_Function) theShape2)
  {
    _func->SetReference(PROXIMITY_ARG_SHAPE1, theShape1);
    _func->SetReference(PROXIMITY_ARG_SHAPE2, theShape2);
  }

  void GetShapes(Handle(GEOM_Function)& theShape1, Handle(GEOM_Function)& theShape2) const
  {
    theShape1 = _func->GetReference(PROXIMITY_ARG_SHAPE1);
    theShape2 = _func->GetReference(PROXIMITY_ARG_SHAPE2);
  }

  void SetNbSamples(const Standard_Integer thePosition, const Standard_Integer theNbSamples) const
  {
    _func->SetInteger(thePosition, theNbSamples);
  }

  Standard_Integer GetNbSamples(const Standard_Integer thePosition) const
  {
    return _func->GetInteger(thePosition);
  }

  void SetProximityPoints(const gp_Pnt& thePoint1, const gp_Pnt& thePoint2)
  {
    setPoint(PROXIMITY_ARG_POINT1, thePoint1);
    setPoint(PROXIMITY_ARG_POINT2, thePoint2);
  }

  void SetStatusOfPoints(const Standard_Integer theStatus1, const Standard_Integer theStatus2)
  {
    setStatus(PROXIMITY_ARG_STATUS1, theStatus1);
    setStatus(PROXIMITY_ARG_STATUS2, theStatus2);
  }

  void GetProximityPoints(gp_Pnt& thePoint1, gp_Pnt& thePoint2)
  {
    thePoint1 = getPoint(PROXIMITY_ARG_POINT1);
    thePoint2 = getPoint(PROXIMITY_ARG_POINT2);
  }

  void GetStatusOfPoints(Standard_Integer& theStatus1, Standard_Integer& theStatus2)
  {
    theStatus1 = getStatus(PROXIMITY_ARG_STATUS1);
    theStatus2 = getStatus(PROXIMITY_ARG_STATUS2);
  }

  void SetValue(const Standard_Real theValue)
  {
    _func->SetReal(PROXIMITY_ARG_VALUE, theValue);
  }

  Standard_Real GetValue() const
  {
    return _func->GetReal(PROXIMITY_ARG_VALUE);
  }

private:
  void setPoint(const Standard_Integer thePosition, const gp_Pnt& thePoint)
  {
    Handle(TColStd_HArray1OfReal) aCoords = new TColStd_HArray1OfReal(1, 3);
    aCoords->SetValue(1, thePoint.X());
    aCoords->SetValue(2, thePoint.Y());
    aCoords->SetValue(3, thePoint.Z());
    _func->SetRealArray(thePosition, aCoords);
  }

  void setStatus(const Standard_Integer thePosition, const Standard_Integer theStatus)
  {
    _func->SetInteger(thePosition, theStatus);
  }

  gp_Pnt getPoint(const Standard_Integer thePosition)
  {
    Handle(TColStd_HArray1OfReal) aCoords = _func->GetRealArray(thePosition);
    return gp_Pnt(aCoords->Value(1), aCoords->Value(2), aCoords->Value(3));
  }

  Standard_Integer getStatus(const Standard_Integer thePosition)
  {
    return _func->GetInteger(thePosition);
  }

private:
  Handle(GEOM_Function) _func;
};
