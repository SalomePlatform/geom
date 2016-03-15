// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef CURVECREATOR_NEWSECTION_H
#define CURVECREATOR_NEWSECTION_H

#include "CurveCreator.hxx"
#include "CurveCreator_ICurve.hxx"

#include <QDockWidget>

//class CurveCreator_Curve;

class QLineEdit;
class QComboBox;
class QCheckBox;
class QPushButton;
class QAbstractButton;
class QDialogButtonBox;
class QFrame;

class CurveCreator_NewSectionDlg : public QWidget
{
  Q_OBJECT
public:
  explicit CurveCreator_NewSectionDlg(QWidget *parent = 0, bool enableClosed = true );

  QString getName() const;
  bool    isClosed() const;
  CurveCreator::SectionType getSectionType() const;

  void    setSectionParameters( const QString& theName, bool isClosed, CurveCreator::SectionType theType );
  void    setSectionName(const QString& theName );
  void    clear();
  void    setEditMode( bool isEdit );
  bool    isEnableClosed() const { return myIsEnableClosed; }

signals:
  void    addSection();
  void    modifySection();
  void    cancelSection();
public slots:
protected slots:
protected:
  void    updateTitle();
private:
  QFrame*             myBtnFrame;
  QLineEdit*          myName;
  QComboBox*          myLineType;
  QCheckBox*          myIsClosed;
  bool                myIsEdit;
  bool                myIsEnableClosed;
  QPushButton*        myAddBtn;
  QPushButton*        myCancelBtn;
};

#endif // CURVECREATOR_NEWSECTION_H
