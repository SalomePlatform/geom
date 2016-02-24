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

#ifndef _AdvancedEngine_SmoothingSurfaceDriver_HXX
#define _AdvancedEngine_SmoothingSurfaceDriver_HXX

#include "GEOM_BaseDriver.hxx"

#include <TColgp_HArray1OfPnt.hxx>
#include <TopoDS_Shape.hxx>

DEFINE_STANDARD_HANDLE( AdvancedEngine_SmoothingSurfaceDriver, GEOM_BaseDriver );

class AdvancedEngine_SmoothingSurfaceDriver : public GEOM_BaseDriver
{
public:
 // Methods PUBLIC
  // 
  AdvancedEngine_SmoothingSurfaceDriver();
  virtual Standard_Integer Execute(LOGBOOK& log) const;
  virtual void Validate(LOGBOOK&) const {}
  Standard_Boolean MustExecute(const LOGBOOK&) const { return Standard_True; }

  static const Standard_GUID& GetID();
  ~AdvancedEngine_SmoothingSurfaceDriver() {};
  
  virtual bool GetCreationInformation(std::string&             theOperationName,
				      std::vector<GEOM_Param>& params);
  // Type management
  //
  OCCT_DEFINE_STANDARD_RTTIEXT(AdvancedEngine_SmoothingSurfaceDriver,GEOM_BaseDriver)
private:
  TopoDS_Shape MakeSmoothingSurfaceUnClosed
                    (const Handle(TColgp_HArray1OfPnt) &theListOfPoints,
                     const Standard_Integer            theNbMax,
                     const Standard_Integer            theDegMax,
                     const Standard_Real               theDMax) const;
};

#endif // _AdvancedEngine_SmoothingSurfaceDriver_HXX
