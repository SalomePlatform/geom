// Copyright (C) 2013-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _XAOPlugin_Driver_HXX
#define _XAOPlugin_Driver_HXX

// internal includes
#include "XAOPlugin_Engine.hxx"

// OCCT includes
#include <TFunction_Driver.hxx>

DEFINE_STANDARD_HANDLE(XAOPlugin_Driver, TFunction_Driver);

class XAOPLUGINENGINE_EXPORT XAOPlugin_Driver: public TFunction_Driver
{
public:
  XAOPlugin_Driver();
  ~XAOPlugin_Driver();

  static const Standard_GUID& GetID();
  virtual Standard_Integer Execute(TFunction_Logbook& log) const;
  Standard_Boolean MustExecute(const TFunction_Logbook&) const;
  virtual void Validate(TFunction_Logbook&) const {}

DEFINE_STANDARD_RTTI(XAOPlugin_Driver)
};

#endif // _XAOPlugin_Driver_HXX
