// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _IGESPlugin_ExportDriver_HXX
#define _IGESPlugin_ExportDriver_HXX

// GEOM includes
#include "GEOM_BaseDriver.hxx"

DEFINE_STANDARD_HANDLE( IGESPlugin_ExportDriver, GEOM_BaseDriver );

class IGESPlugin_ExportDriver : public GEOM_BaseDriver
{
public:
  IGESPlugin_ExportDriver();
  ~IGESPlugin_ExportDriver() {};

  static const Standard_GUID& GetID();
  virtual Standard_Integer    Execute(LOGBOOK& log) const;
  Standard_Boolean            MustExecute( const LOGBOOK& ) const { return Standard_True; }
  virtual void                Validate( LOGBOOK& ) const {}

  virtual bool                GetCreationInformation( std::string& theOperationName,
                                                      std::vector<GEOM_Param>& params );

  OCCT_DEFINE_STANDARD_RTTIEXT(IGESPlugin_ExportDriver,GEOM_BaseDriver)
};

#endif // _IGESPlugin_ExportDriver_HXX
