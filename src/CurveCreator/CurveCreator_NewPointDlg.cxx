// Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "CurveCreator_NewPointDlg.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QRegExpValidator>
#include <QAbstractButton>
#include <QPushButton>
#include <QLocale>

CurveCreator_NewPointDlg::CurveCreator_NewPointDlg(CurveCreator::Dimension theDim, QWidget *parent) :
  QWidget(parent), myX(NULL), myY(NULL), myZ(NULL), myIsEdit(false), myDim(theDim),
  myIsInstantSketchingEnabled(false)
{
  QString aTitle = QString(tr("ADD_NEW_POINT"));
  setWindowTitle(aTitle);

  QFrame* aFrame = new QFrame( this );
  QVBoxLayout* aLayout = new QVBoxLayout( aFrame );

  QFrame* aCoordFrame = new QFrame( aFrame );
  QGridLayout* aCoordLayout = new QGridLayout( aCoordFrame );

  QLabel* aLbl = new QLabel( tr("X_COORD"), this);
  myX = new QDoubleSpinBox(this);
  aCoordLayout->addWidget(aLbl, 0, 0);
  aCoordLayout->addWidget(myX, 0, 1 );

  aLbl = new QLabel( tr("Y_COORD"), this);
  myY = new QDoubleSpinBox(this);
  aCoordLayout->addWidget(aLbl, 1, 0 );
  aCoordLayout->addWidget(myY, 1, 1 );

  myZLabel = new QLabel( tr("Z_COORD"), this);
  myZ = new QDoubleSpinBox(this);
  aCoordLayout->addWidget(myZLabel, 2,0 );
  aCoordLayout->addWidget(myZ, 2,1 );

  if( theDim != CurveCreator::Dim3d ){
    myZ->hide();
    myZLabel->hide();
  }

  myBtnFrame = new QFrame( aFrame );
  QHBoxLayout* aBtnsLayout = new QHBoxLayout( myBtnFrame );

  myAddBtn = new QPushButton( tr( "ADD_BTN" ), myBtnFrame );
  myCancelBtn = new QPushButton( tr( "CANCEL" ), myBtnFrame );

  connect( myCancelBtn, SIGNAL( clicked() ), this, SIGNAL( cancelPoint() ) );

  aBtnsLayout->addWidget( myAddBtn );
  aBtnsLayout->addStretch( 1 );
  aBtnsLayout->addWidget( myCancelBtn );

  aLayout->addWidget( aCoordFrame, 0 );
  aLayout->addWidget( myBtnFrame, 1 );

  clear();
  updateTitle();
}

void CurveCreator_NewPointDlg::setSectionName( const QString& theName )
{
  mySectionName = theName;
  updateTitle();
}

void CurveCreator_NewPointDlg::setEditMode( bool isEdit )
{
  myIsEdit = isEdit;
  if( myIsEdit ){
    myAddBtn->setText(tr("OK"));
    myAddBtn->disconnect( SIGNAL( clicked() ) );
    connect( myAddBtn, SIGNAL( clicked() ), this, SIGNAL( modifyPoint() ) );
  }
  else{
    myAddBtn->setText(tr("ADD_BTN"));
    myAddBtn->disconnect( SIGNAL( clicked() ) );
    connect( myAddBtn, SIGNAL( clicked() ), this, SIGNAL( addPoint() ) );
  }
  updateTitle();
}

void CurveCreator_NewPointDlg::updateTitle()
{
  QString aTitle;
  if( !myIsEdit ){
    if( mySectionName.isEmpty() ){
      aTitle = tr("ADD_NEW_POINT");
    }
    else{
      aTitle = QString(tr("ADD_NEW_POINT_TO_%1")).arg(mySectionName);
    }
  }
  else{
    aTitle = tr("SET_POINT_COORDINATES");
  }
  setWindowTitle(aTitle);
}

CurveCreator::Coordinates CurveCreator_NewPointDlg::getCoordinates() const
{
  CurveCreator::Coordinates aCoords;
  double anX = myX->value();
  aCoords.push_back(anX);
  double anY = myY->value();
  aCoords.push_back(anY);
  if( myDim == CurveCreator::Dim3d ){
    double aZ = myZ->value();
    aCoords.push_back(aZ);
  }
  return aCoords;
}

void CurveCreator_NewPointDlg::clear()
{
  initSpinBox(myX);
  initSpinBox(myY);
  initSpinBox(myZ);
}

void CurveCreator_NewPointDlg::setDimension(CurveCreator::Dimension theDim)
{
  if( theDim == CurveCreator::Dim2d ){
    myZ->hide();
    myZLabel->hide();
  }
  else{
    myZ->show();
    myZLabel->show();
  }
}

void CurveCreator_NewPointDlg::setCoordinates( const CurveCreator::Coordinates& theCoords )
{
  double anX = theCoords[0];
  myX->setValue(anX);
  double anY = theCoords[1];
  myY->setValue(anY);
  if( theCoords.size() == 3 ){
    double aZ = theCoords[2];
    myZ->setValue(aZ);
  }
  if( isInstantSketchingEnabled() )
    emit addPoint();
}

bool CurveCreator_NewPointDlg::isInstantSketchingEnabled() const
{
  return myIsInstantSketchingEnabled;
}

void CurveCreator_NewPointDlg::setInstantSketchingEnabled( const bool theState )
{
  myIsInstantSketchingEnabled = theState;
}

//=======================================================================
// function: initSpinBox
// purpose:
//=======================================================================
void CurveCreator_NewPointDlg::initSpinBox(QDoubleSpinBox *theSpinBox)
{
  const double aCoordMin  = -1.e+15;
  const double aCoordMax  = 1.e+15;
  const double aStep      = 10;
  const int    aPrecision = 6;

  theSpinBox->setDecimals( qAbs( aPrecision ) );
  theSpinBox->setRange(aCoordMin, aCoordMax);
  theSpinBox->setSingleStep(aStep);
  theSpinBox->setValue(0.0);
}
