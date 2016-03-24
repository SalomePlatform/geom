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

#include "STEPPlugin_ImportDlg.h"

#include <QApplication>
#include <QLayout>
#include <QCheckBox>

//=============================================================================
// Constructor
//=============================================================================
STEPPlugin_ImportDlg::STEPPlugin_ImportDlg(QWidget *parent)
  : SUIT_FileDlg (parent, true, true, true),
    myCheckBox   (0)
{
  myCheckBox = new QCheckBox(tr("STEP_CREATE_ASSEMBLIES"), this);

  myCheckBox->setChecked(true);

  layout()->addWidget(myCheckBox);
}

//=============================================================================
// Destructor
//=============================================================================
STEPPlugin_ImportDlg::~STEPPlugin_ImportDlg()
{
}

//=============================================================================
// IsCreateAssemblies
//=============================================================================
bool STEPPlugin_ImportDlg::IsCreateAssemblies() const
{
  return myCheckBox->isChecked();
}

//=============================================================================
// getOpenFileNames
//=============================================================================
QStringList STEPPlugin_ImportDlg::getOpenFileNames
                                  (const QString &theInitial,
                                   const QString &theFilters,
                                   const QString &theCaption,
                                         QWidget *theParent,
                                         bool    &IsCreateAssemblies)
{
  STEPPlugin_ImportDlg anImpDlg(theParent);
  QStringList aFilters = theFilters.split(";;", QString::SkipEmptyParts);

  anImpDlg.setFileMode(ExistingFiles);

  if (aFilters.isEmpty()) {
    anImpDlg.setNameFilter(tr("ALL_FILES_FILTER")); // All files (*)
  } else {
    anImpDlg.setNameFilters(aFilters);
  }

  if (!theCaption.isEmpty()) {
    anImpDlg.setWindowTitle(theCaption);
  }

  if (!theInitial.isEmpty()) {
    anImpDlg.processPath(theInitial);
  }

  QStringList aFileNames;

  if (anImpDlg.exec() == QDialog::Accepted) {
    aFileNames = anImpDlg.selectedFiles();
    IsCreateAssemblies = anImpDlg.IsCreateAssemblies();
  }

  QApplication::processEvents();

  return aFileNames;
}
