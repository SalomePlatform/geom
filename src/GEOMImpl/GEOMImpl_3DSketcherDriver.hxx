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

//  File   : GEOMImpl_3DSketcherDriver.ixx
//  Module : GEOMImpl
//
#ifndef _GEOMImpl_3DSketcherDriver_HeaderFile
#define _GEOMImpl_3DSketcherDriver_HeaderFile

#include <GEOM_BaseDriver.hxx>

DEFINE_STANDARD_HANDLE( GEOMImpl_3DSketcherDriver, GEOM_BaseDriver );

class GEOMImpl_3DSketcherDriver : public GEOM_BaseDriver {

public:

  Standard_EXPORT GEOMImpl_3DSketcherDriver();
  Standard_EXPORT virtual Standard_Integer Execute(LOGBOOK& log) const;
  Standard_EXPORT virtual void Validate(LOGBOOK&) const {}
  Standard_EXPORT Standard_Boolean MustExecute(const LOGBOOK&) const { return Standard_True; }

  Standard_EXPORT static const Standard_GUID& GetID();
  Standard_EXPORT ~GEOMImpl_3DSketcherDriver() {};

  Standard_EXPORT virtual bool GetCreationInformation(std::string&             theOperationName,
                                                      std::vector<GEOM_Param>& params);

  OCCT_DEFINE_STANDARD_RTTIEXT(GEOMImpl_3DSketcherDriver,GEOM_BaseDriver)
};

#endif
