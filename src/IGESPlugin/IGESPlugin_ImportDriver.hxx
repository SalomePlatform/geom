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

#ifndef _IGESPlugin_ImportDriver_HXX
#define _IGESPlugin_ImportDriver_HXX

// GEOM includes
#include "GEOM_BaseDriver.hxx"

// OCCT includes
#include <TFunction_Logbook.hxx>

DEFINE_STANDARD_HANDLE( IGESPlugin_ImportDriver, GEOM_BaseDriver );

class IGESPlugin_ImportDriver : public GEOM_BaseDriver
{
public:
  IGESPlugin_ImportDriver();
  ~IGESPlugin_ImportDriver() {};

  static const Standard_GUID& GetID();
  virtual Standard_Integer    Execute( TFunction_Logbook& log ) const;
  Standard_Boolean            MustExecute( const TFunction_Logbook& ) const;
  virtual void                Validate( TFunction_Logbook& ) const {}
 
  virtual bool                GetCreationInformation( std::string&             theOperationName,
						      std::vector<GEOM_Param>& params );

  static
  TCollection_AsciiString     GetValue( const TCollection_AsciiString&,
					const TCollection_AsciiString&,
					TCollection_AsciiString& );

DEFINE_STANDARD_RTTI( IGESPlugin_ImportDriver )
};

#endif // _IGESPlugin_ImportDriver_HXX
