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

// File:        CurveCreator_UndoOptsDlg.h
// Author:      Sergey KHROMOV

#ifndef _CurveCreator_UndoOptsDlg_HeaderFile
#define _CurveCreator_UndoOptsDlg_HeaderFile

#include <QDialog>

class QButtonGroup;
class QLineEdit;
class QPushButton;


class CurveCreator_UndoOptsDlg : public QDialog
{
  Q_OBJECT

public:

  CurveCreator_UndoOptsDlg(QWidget* parent);

  ~CurveCreator_UndoOptsDlg();

  void setUndoDepth(const int theDepth);

  int getUndoDepth() const;

  bool isEnabled() const;

  bool isUnlimited() const;

private:

  void init();

  void setTabOrder();

private slots:

  void optionChanged(int theId);

  void accept();

protected:

  int           myUndoDepth;
  QButtonGroup *myOptsBtnGrp;
  QLineEdit    *myBufferSizeEdit;
  QPushButton  *myOkBtn;
  QPushButton  *myCancelBtn;

};

#endif
