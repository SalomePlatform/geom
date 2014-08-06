// Copyright (C) 2013-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:        CurveCreator_UndoOptsDlg.cxx
// Author:      Sergey KHROMOV

#include "CurveCreator_UndoOptsDlg.h"

#include <QButtonGroup>
#include <QGridLayout>
#include <QGroupBox>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

#define UNDO_DEPTH_UNLIMITED  0
#define UNDO_DEPTH_DISABLED   1
#define UNDO_DEPTH_FIX_SIZE   2

//=======================================================================
// function: Constructor
// purpose:
//=======================================================================
CurveCreator_UndoOptsDlg::CurveCreator_UndoOptsDlg(QWidget* parent)
  : QDialog          (parent),
    myUndoDepth      (UNDO_DEPTH_UNLIMITED),
    myOptsBtnGrp     (NULL),
    myBufferSizeEdit (NULL),
    myOkBtn          (NULL),
    myCancelBtn      (NULL)
{
  setWindowTitle(tr("CC_UNDO_OPTIONS_TITLE"));

  // Set Undo/Redo options group
  QGroupBox    *anUndoOptsGrp  =
            new QGroupBox(tr("CC_UNDO_OPTIONS_MODIFY"));
  QGridLayout  *anUndoOptsLO   = new QGridLayout(anUndoOptsGrp);
  QRadioButton *aDisabledRdBtn =
            new QRadioButton(tr("CC_UNDO_OPTIONS_DISABLED"), anUndoOptsGrp);
  QRadioButton *aFixSizeRdBtn  =
            new QRadioButton(tr("CC_UNDO_OPTIONS_FIXED_SIZE"), anUndoOptsGrp);
  QRadioButton *anUnlimRdBtn   =
            new QRadioButton(tr("CC_UNDO_OPTIONS_UNLIMITED"), anUndoOptsGrp);

  myOptsBtnGrp     = new QButtonGroup(anUndoOptsGrp);
  myBufferSizeEdit = new QLineEdit(anUndoOptsGrp);
  anUndoOptsLO->setMargin(9);
  anUndoOptsLO->setSpacing(6);
  anUndoOptsLO->addWidget(aDisabledRdBtn,   0, 0);
  anUndoOptsLO->addWidget(aFixSizeRdBtn,    1, 0);
  anUndoOptsLO->addWidget(anUnlimRdBtn,     2, 0);
  anUndoOptsLO->addWidget(myBufferSizeEdit, 1, 1);
  myOptsBtnGrp->addButton(anUnlimRdBtn,   UNDO_DEPTH_UNLIMITED);
  myOptsBtnGrp->addButton(aDisabledRdBtn, UNDO_DEPTH_DISABLED);
  myOptsBtnGrp->addButton(aFixSizeRdBtn,  UNDO_DEPTH_FIX_SIZE);

  // Set OK/Cancel buttons group
  QGroupBox   *anOkCancelGrp  = new QGroupBox;
  QGridLayout *anOkCancelLO   = new QGridLayout(anOkCancelGrp);

  myOkBtn     = new QPushButton(tr("GEOM_BUT_OK"), anOkCancelGrp);
  myCancelBtn = new QPushButton(tr("GEOM_BUT_CANCEL"), anOkCancelGrp);
  anOkCancelLO->setMargin(9);
  anOkCancelLO->setSpacing(6);
  anOkCancelLO->addWidget(myOkBtn,     0, 0);
  anOkCancelLO->addWidget(myCancelBtn, 0, 1);

  // Set main group
  QGroupBox   *aMainGrp = new QGroupBox;
  QVBoxLayout *aMainLO = new QVBoxLayout(aMainGrp);

  aMainLO->addWidget(anUndoOptsGrp);
  aMainLO->addWidget(anOkCancelGrp);

  setLayout(aMainLO);

  init();
}

//=======================================================================
// function: Destructor
// purpose:
//=======================================================================
CurveCreator_UndoOptsDlg::~CurveCreator_UndoOptsDlg()
{
}

//=======================================================================
// function: setUndoDepth
// purpose:
//=======================================================================
void CurveCreator_UndoOptsDlg::setUndoDepth(const int theDepth)
{
  myUndoDepth = theDepth;

  const int aDepthId = myUndoDepth + 1;
  int       anId     = UNDO_DEPTH_FIX_SIZE;

  if (aDepthId == UNDO_DEPTH_UNLIMITED ||
      aDepthId == UNDO_DEPTH_DISABLED) {
    anId = aDepthId;
  } else if (myUndoDepth > 0) {
    myBufferSizeEdit->setText(QString::number(myUndoDepth));
  }

  myOptsBtnGrp->button(anId)->setChecked(true);
  optionChanged(anId);
}

//=======================================================================
// function: getUndoDepth
// purpose:
//=======================================================================
int CurveCreator_UndoOptsDlg::getUndoDepth() const
{
  return myUndoDepth;
}

//=======================================================================
// function: isEnabled
// purpose:
//=======================================================================
bool CurveCreator_UndoOptsDlg::isEnabled() const
{
  return (myUndoDepth + 1 != UNDO_DEPTH_DISABLED);
}

//=======================================================================
// function: isUnlimited
// purpose:
//=======================================================================
bool CurveCreator_UndoOptsDlg::isUnlimited() const
{
  return (myUndoDepth + 1 == UNDO_DEPTH_UNLIMITED);
}

//=======================================================================
// function: init
// purpose:
//=======================================================================
void CurveCreator_UndoOptsDlg::init()
{
  // Initialize sections group.
  myOptsBtnGrp->setExclusive(true);
  myOptsBtnGrp->button(UNDO_DEPTH_UNLIMITED)->setChecked(true);
  connect(myOptsBtnGrp, SIGNAL(buttonClicked(int)),
          this, SLOT(optionChanged(int)));

  // Initialize line edit.
  QIntValidator *aValidator = new QIntValidator(myBufferSizeEdit);

  aValidator->setBottom(1);
  myBufferSizeEdit->setValidator(aValidator);
  optionChanged(UNDO_DEPTH_UNLIMITED);

  // Init buttons.
  myOkBtn->setDefault(true);

  connect(myOkBtn,     SIGNAL(clicked()), this, SLOT(accept()));
  connect(myCancelBtn, SIGNAL(clicked()), this, SLOT(reject()));

  setTabOrder();
}

//=======================================================================
// function: setTabOrder
// purpose:
//=======================================================================
void CurveCreator_UndoOptsDlg::setTabOrder()
{
  QWidget::setTabOrder(myOptsBtnGrp->button(UNDO_DEPTH_DISABLED),
                       myOptsBtnGrp->button(UNDO_DEPTH_FIX_SIZE));
  QWidget::setTabOrder(myOptsBtnGrp->button(UNDO_DEPTH_FIX_SIZE),
                       myBufferSizeEdit);
  QWidget::setTabOrder(myBufferSizeEdit,
                       myOptsBtnGrp->button(UNDO_DEPTH_UNLIMITED));
  QWidget::setTabOrder(myOptsBtnGrp->button(UNDO_DEPTH_UNLIMITED), myOkBtn);
  QWidget::setTabOrder(myOkBtn, myCancelBtn);
}

//=======================================================================
// function: optionChanged
// purpose:
//=======================================================================
void CurveCreator_UndoOptsDlg::optionChanged(int theId)
{
  switch (theId) {
    case UNDO_DEPTH_FIX_SIZE:
      myBufferSizeEdit->setEnabled(true);
      break;
    case UNDO_DEPTH_UNLIMITED:
    case UNDO_DEPTH_DISABLED:
    default:
      myBufferSizeEdit->setEnabled(false);
      break;
  }
}

//=======================================================================
// function: accept
// purpose:
//=======================================================================
void CurveCreator_UndoOptsDlg::accept()
{
  const int anId = myOptsBtnGrp->checkedId();

  switch (anId) {
    case UNDO_DEPTH_FIX_SIZE:
      myUndoDepth = myBufferSizeEdit->text().toInt();
      break;
    case UNDO_DEPTH_UNLIMITED:
    case UNDO_DEPTH_DISABLED:
      myUndoDepth = anId - 1;
      break;
    default:
      break;
  }

  QDialog::accept();
}
