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

#include "CurveCreator_NewSectionDlg.h"
//#include "CurveCreator_Curve.hxx"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>

CurveCreator_NewSectionDlg::CurveCreator_NewSectionDlg( QWidget *parent, bool enableClosed ) :
  QWidget(parent), myIsEnableClosed( enableClosed )
{
  QVBoxLayout* aMainLayout = new QVBoxLayout( this );
  aMainLayout->setMargin( 0 );

  QFrame* aFrame = new QFrame( this );
  aMainLayout->addWidget( aFrame );

  QVBoxLayout* aLayout = new QVBoxLayout( aFrame );
  aLayout->setMargin( 0 );

  QFrame* aCoordFrame = new QFrame( aFrame );
  QGridLayout* aCoordLayout = new QGridLayout( aCoordFrame );

  QLabel* aLbl = new QLabel(tr("SECTION_NAME"), this);
  myName = new QLineEdit(this);
  aCoordLayout->addWidget(aLbl, 0, 0);
  aCoordLayout->addWidget(myName, 0 , 1);

  aLbl = new QLabel(tr("SECTION_LINE_TYPE"));
  myLineType = new QComboBox(this);

  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap aPolylinePixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_POLYLINE")));
  QPixmap aSplinePixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_SPLINE")));

//  QPixmap aPolylinePixmap = QPixmap(tr(":images/ICON_POLYLINE"));
//  QPixmap aSplinePixmap = QPixmap(tr(":images/ICON_SPLINE"));
  myLineType->addItem(aPolylinePixmap, tr("SECTION_POLYLINE_TYPE"));
  myLineType->addItem(aSplinePixmap, tr("SECTION_SPLINE_TYPE"));
  myLineType->setCurrentIndex(0);
  aCoordLayout->addWidget(aLbl, 1, 0);
  aCoordLayout->addWidget(myLineType, 1 , 1);

  aLbl = new QLabel(tr("SECTION_LINE_CLOSED"));
  myIsClosed = new QCheckBox(this);
  aCoordLayout->addWidget(aLbl, 2, 0);
  aCoordLayout->addWidget(myIsClosed, 2, 1);
  if ( !myIsEnableClosed ) {
    aLbl->hide();
    myIsClosed->hide();
  }

  myBtnFrame = new QFrame( aFrame );
  QHBoxLayout* aBtnsLayout = new QHBoxLayout( myBtnFrame );

  myAddBtn = new QPushButton( tr( "SECTION_ADD_BTN" ), myBtnFrame );
  myCancelBtn = new QPushButton( tr( "SECTION_CANCEL_BTN" ), myBtnFrame );

  connect( myAddBtn,  SIGNAL( clicked() ), this, SIGNAL( addSection() ) );
  connect( myCancelBtn, SIGNAL( clicked() ), this, SIGNAL( cancelSection() ) );

  aBtnsLayout->addWidget( myAddBtn );
  aBtnsLayout->addStretch( 1 );
  aBtnsLayout->addWidget( myCancelBtn );

  aLayout->addWidget( aCoordFrame, 0 );
  aLayout->addWidget( myBtnFrame, 1 );
}

void CurveCreator_NewSectionDlg::setSectionParameters( const QString& theName, bool isClosed, CurveCreator::SectionType theType )
{
  myName->setText(theName);
  myIsClosed->setChecked(isClosed);
  if( theType == CurveCreator::Polyline )
    myLineType->setCurrentIndex(0);
  else
    myLineType->setCurrentIndex(1);
}

void CurveCreator_NewSectionDlg::clear()
{
  myName->setText("");
  myIsClosed->setChecked(true);
  myLineType->setCurrentIndex(0);
}

void CurveCreator_NewSectionDlg::setEditMode( bool isEdit )
{
  myIsEdit = isEdit;
  if( myIsEdit ){
    myAddBtn->setText(tr("SECTION_OK_BTN"));
    myAddBtn->disconnect( SIGNAL( clicked() ) );
    connect( myAddBtn, SIGNAL( clicked() ), this, SIGNAL( modifySection() ) );
  }
  else{
    myAddBtn->setText(tr("SECTION_ADD_BTN"));
    myAddBtn->disconnect( SIGNAL( clicked() ) );
    connect( myAddBtn, SIGNAL( clicked() ), this, SIGNAL( addSection() ) );
  }
  updateTitle();
}

QString CurveCreator_NewSectionDlg::getName() const
{
  return myName->text();
}

bool CurveCreator_NewSectionDlg::isClosed() const
{
  return myIsClosed->isChecked();
}

CurveCreator::SectionType CurveCreator_NewSectionDlg::getSectionType() const
{
  if( myLineType->currentIndex() == 0 )
    return CurveCreator::Polyline;
  else
    return CurveCreator::Spline;
}

void CurveCreator_NewSectionDlg::updateTitle()
{
  QString aTitle;
  if( !myIsEdit )
    aTitle = tr("ADD_NEW_SECTION");
  else
    aTitle = QString(tr("SET_SECTION_PARAMETERS"));
  setWindowTitle(aTitle);
}

void CurveCreator_NewSectionDlg::setSectionName( const QString& theName )
{
  myName->setText(theName);
}
