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

#ifndef _XAOPlugin_IImportExport_HXX_
#define _XAOPlugin_IImportExport_HXX_

#include "GEOM_Function.hxx"

#define IMPORTEXPORTXAO_ARG_DATA   1

class XAOPlugin_IImportExport
{
public:
  XAOPlugin_IImportExport(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetData(const TCollection_AsciiString& data) { _func->SetString(IMPORTEXPORTXAO_ARG_DATA, data); }
  const TCollection_AsciiString GetData() { return _func->GetString(IMPORTEXPORTXAO_ARG_DATA); }

private:
  Handle(GEOM_Function) _func;
};

#endif // _XAOPlugin_IImportExport_HXX_
