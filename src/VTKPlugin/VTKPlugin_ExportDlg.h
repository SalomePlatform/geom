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

#ifndef VTKPlugin_ExportDlg_H
#define VTKPlugin_ExportDlg_H

// GUI includes
#include <SUIT_FileDlg.h>
#include <SALOME_InteractiveObject.hxx>

class SalomeApp_DoubleSpinBox;

class VTKPlugin_ExportDlg: public SUIT_FileDlg
{
  Q_OBJECT

public:
  VTKPlugin_ExportDlg( const Handle(SALOME_InteractiveObject)&, QWidget* parent );
  ~VTKPlugin_ExportDlg();

  double getDeflection() const;

  static QString getFileName( const Handle(SALOME_InteractiveObject)& io,
                              const QString& filters,
                              const QString& caption,
                              QWidget* parent,
                              double& deflection );

private:
  SalomeApp_DoubleSpinBox*         myDeflectionSB;
  Handle(SALOME_InteractiveObject) myObject;
};

#endif // VTKPlugin_ExportDlg_H
