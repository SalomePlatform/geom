// Copyright (C) 2014-2024  CEA, EDF, OPEN CASCADE
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

#ifndef VTKPlugin_GUI_H
#define VTKPlugin_GUI_H

#include "GEOMPluginGUI.h"

class VTKPlugin_GUI: public GEOMPluginGUI
{
  Q_OBJECT
public:
  VTKPlugin_GUI( GeometryGUI* parent );
  ~VTKPlugin_GUI();

  bool OnGUIEvent( int commandId, SUIT_Desktop* );
  bool OnGUIEvent( const QString&, SUIT_Desktop* );

private:
  bool exportVTK( SUIT_Desktop* );
};

#endif // VTKPlugin_GUI_H
