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

#ifndef STLPlugin_ExportDlg_H
#define STLPlugin_ExportDlg_H

#include <SUIT_FileDlg.h>
#include <SALOME_InteractiveObject.hxx>

class QCheckBox;
class SalomeApp_DoubleSpinBox;

class STLPlugin_ExportDlg: public SUIT_FileDlg
{
  Q_OBJECT

public:
  STLPlugin_ExportDlg( const Handle(SALOME_InteractiveObject)& io, QWidget* parent );
  ~STLPlugin_ExportDlg();

  bool   isAscii() const;
  double getDeflection() const;
  bool   isDeflectionRelative() const;

  static QString getFileName( const Handle(SALOME_InteractiveObject)& io,
			      const QString& filters,
			      const QString& caption,
			      QWidget* parent,
			      bool& isAscii,
			      double& deflection,
			      bool& isRelative );

private slots:
  void  modeChanged();

private:
  static double getShapeSize( const Handle(SALOME_InteractiveObject)& io );

private:
  SalomeApp_DoubleSpinBox*         myDeflectionSB;
  QCheckBox*                       myModeCB;
  QCheckBox*                       myFormatCB;
  double                           myShapeSize;
};

#endif // STLPlugin_ExportDlg_H
