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

// GEOM GEOMGUI : GUI for Geometry component
// File   : GEOMToolsGUI_DeleteDlg.h
// Author : Dmitry Matveitchev, Open CASCADE S.A.S.
//
#ifndef GEOMTOOLSGUI_DELETEDLG_H
#define GEOMTOOLSGUI_DELETEDLG_H

#include "GEOM_ToolsGUI.hxx"

#include <QDialog>
#include <QMap>

class GEOMTOOLSGUI_EXPORT GEOMToolsGUI_DeleteDlg : public QDialog
{ 
  Q_OBJECT

public:
  GEOMToolsGUI_DeleteDlg( QWidget*, const QMap<QString, QString>&, bool = false );
  ~GEOMToolsGUI_DeleteDlg();
};

#endif // GEOMTOOLSGUI_DELETEDLG_H
