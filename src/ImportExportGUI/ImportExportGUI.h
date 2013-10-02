// Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

//  File   : ImportExportGUI.h
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef IMPORTEXPORTGUI_H
#define IMPORTEXPORTGUI_H

#include "GEOMGUI.h"

//=================================================================================
// class    : ImportExportGUI
// purpose  :
//=================================================================================
class ImportExportGUI: public GEOMGUI
{
public:
    ImportExportGUI(GeometryGUI* parent);
    ~ImportExportGUI();

    bool OnGUIEvent(int commandId, SUIT_Desktop* parent);
};

#endif // IMPORTEXPORTGUI_H
