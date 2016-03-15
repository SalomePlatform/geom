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

#ifndef STEPPlugin_ExportDlg_H
#define STEPPlugin_ExportDlg_H

#include <SUIT_FileDlg.h>
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(STEPPlugin)

class QComboBox;

class STEPPlugin_ExportDlg: public SUIT_FileDlg
{
  Q_OBJECT

public:

  STEPPlugin_ExportDlg(QWidget *parent);
  ~STEPPlugin_ExportDlg();

  GEOM::length_unit getUnits() const;

  static QString getFileName(const QString           &theInitial,
                             const QString           &theFilters,
			                 const QString           &theCaption,
                                   QWidget           *theParent,
                                   GEOM::length_unit &theUnits);

private:

  QComboBox *myUnitCB;

};

#endif // STEPPlugin_ExportDlg_H
