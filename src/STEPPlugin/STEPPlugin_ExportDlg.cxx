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

#include "STEPPlugin_ExportDlg.h"

#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QComboBox>

//=============================================================================
// Constructor
//=============================================================================
STEPPlugin_ExportDlg::STEPPlugin_ExportDlg(QWidget *parent)
  : SUIT_FileDlg (parent, false, true, true),
    myUnitCB     (0)
{
  QLabel* aUnitLabel = new QLabel(tr("STEP_LENGTH_UNITS"), this);

  myUnitCB = new QComboBox(this);
  myUnitCB->addItem(tr("STEP_UNITS_KILOMETER"),  GEOM::LU_KILOMETER);
  myUnitCB->addItem(tr("STEP_UNITS_METER"),      GEOM::LU_METER);
  myUnitCB->addItem(tr("STEP_UNITS_CENTIMETER"), GEOM::LU_CENTIMETER);
  myUnitCB->addItem(tr("STEP_UNITS_MILLIMETER"), GEOM::LU_MILLIMETER);
  myUnitCB->addItem(tr("STEP_UNITS_MICROMETER"), GEOM::LU_MICROMETER);
  myUnitCB->addItem(tr("STEP_UNITS_MILE"),       GEOM::LU_MILE);
  myUnitCB->addItem(tr("STEP_UNITS_FOOT"),       GEOM::LU_FOOT);
  myUnitCB->addItem(tr("STEP_UNITS_INCH"),       GEOM::LU_INCH);
  myUnitCB->addItem(tr("STEP_UNITS_MILLIINCH"),  GEOM::LU_MILLIINCH);
  myUnitCB->addItem(tr("STEP_UNITS_MICROINCH"),  GEOM::LU_MICROINCH);

  // Meters by default.
  myUnitCB->setCurrentIndex(1);

  layout()->addWidget(aUnitLabel);
  layout()->addWidget(myUnitCB);
}

//=============================================================================
// Destructor
//=============================================================================
STEPPlugin_ExportDlg::~STEPPlugin_ExportDlg()
{
}

//=============================================================================
// getUnits
//=============================================================================
GEOM::length_unit STEPPlugin_ExportDlg::getUnits() const
{
  const GEOM::length_unit anUnit =
    (GEOM::length_unit) myUnitCB->itemData(myUnitCB->currentIndex()).toInt();

  return anUnit;
}

//=============================================================================
// getFileName
//=============================================================================
QString STEPPlugin_ExportDlg::getFileName(const QString           &theInitial,
                                          const QString           &theFilters,
			                              const QString           &theCaption,
                                                QWidget           *theParent,
                                                GEOM::length_unit &theUnits)
{
  QStringList aFls         = theFilters.split(";;", QString::SkipEmptyParts);
  QString     aTmpFileName = theInitial;

  aTmpFileName = aTmpFileName.simplified();
  aTmpFileName =
    aTmpFileName.replace(QRegExp("\\*"), "").replace(QRegExp("\\?"), "");

  STEPPlugin_ExportDlg aDlg(theParent);

  aDlg.setFileMode(AnyFile);
  aDlg.setNameFilters(aFls);
  aDlg.setWindowTitle(theCaption);

  if (!aTmpFileName.isEmpty()) {
    aDlg.processPath(aTmpFileName);
  }

  QString aFileName;

  if (aDlg.exec() == QDialog::Accepted) {
    aFileName = aDlg.selectedFile();
    theUnits  = aDlg.getUnits();
  }

  QApplication::processEvents();

  return aFileName;
}
