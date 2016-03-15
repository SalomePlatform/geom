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

// File   : GEOMToolsGUI_MarkerDlg.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef GEOMTOOLSGUI_MARKERDLG_H
#define GEOMTOOLSGUI_MARKERDLG_H

#include "GEOM_ToolsGUI.hxx"

#include <QDialog>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(GEOM_Gen)

class QButtonGroup;
class QStackedWidget;
class QtxComboBox;
class SalomeApp_Study;

class GEOMTOOLSGUI_EXPORT GEOMToolsGUI_MarkerDlg : public QDialog
{
  Q_OBJECT

public:
  GEOMToolsGUI_MarkerDlg( QWidget* = 0 );
  ~GEOMToolsGUI_MarkerDlg();

  void                             setStandardMarker( GEOM::marker_type, GEOM::marker_size );
  void                             setCustomMarker( int );
  GEOM::marker_type                getMarkerType() const;
  GEOM::marker_size                getStandardMarkerScale() const;
  int                              getCustomMarkerID() const;

  void                             accept();

protected:
  void                             keyPressEvent( QKeyEvent* );

private:
  void                             init();
  void                             addTexture( int, bool = false ) const;
  SalomeApp_Study*                 getStudy() const;

private slots:
  void                             browse();
  void                             help();

private:
  GEOM::GEOM_IInsertOperations_var myOperation;
  QButtonGroup*                    myTypeGroup;
  QStackedWidget*                  myWGStack;
  QtxComboBox*                     myStdTypeCombo;
  QtxComboBox*                     myStdScaleCombo;
  QtxComboBox*                     myCustomTypeCombo;
};

#endif // GEOMTOOLSGUI_MARKERDLG_H
