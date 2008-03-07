//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : GEOMBase_aWarningDlg.cxx
//  Author : Dmitry Matveitchev
//  Module : GEOM
//  $Header: /home/server/cvs/GEOM/GEOM_SRC/src/GEOMBase/GEOMBase_aWarningDlg.cxx

#include "GEOMBase_aWarningDlg.h"
#include <stdio.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qvalidator.h>
#include <qstring.h>
#include <qscrollview.h>

#ifndef WNT
using namespace std;
#endif

GEOMBase_aWarningDlg::GEOMBase_aWarningDlg( QWidget* parent, const char* name, const QString theText, int theNum )
  :QDialog( parent, name )
{
  if(!name)
    setName( "Warning" );
  if ( theNum < 15 )
    resize(296, (120 + (14*theNum)) ); 
  else
    resize(296, 300);
  setCaption(name); /* appears on the title bar */
  setSizeGripEnabled(TRUE);

  QGridLayout* topLayout = new QGridLayout(this); 
  topLayout->setSpacing(6);
  topLayout->setMargin(11);

  QGroupBox* mainGrp = new QGroupBox(this, "mainGrp");
  mainGrp->setColumnLayout(0, Qt::Vertical);
  mainGrp->layout()->setSpacing(0);
  mainGrp->layout()->setMargin(0);

  QGridLayout* mainGrpLayout = new QGridLayout(mainGrp->layout());
  mainGrpLayout->setAlignment(Qt::AlignTop);
  mainGrpLayout ->setSpacing(6);
  mainGrpLayout->setMargin(11);
  topLayout->addWidget(mainGrp, 0, 0);
  
  QLabel* TextLabel = new QLabel(mainGrp, "TextLabel");
  TextLabel->setText( QObject::tr( "GEOM_REALLY_DELETE" ).arg( theNum ) );  
  mainGrpLayout->addWidget(TextLabel, 0, 0);
 
  QScrollView* viewer = new QScrollView (mainGrp, "viewer");
  viewer->setResizePolicy( QScrollView::AutoOneFit );
  QLabel* TextLabel1 = new QLabel(viewer, "TextLabel1");
  TextLabel1->setText( theText );
  TextLabel1->setAlignment(Qt::AlignTop);
  viewer->addChild(TextLabel1);
  mainGrpLayout->addWidget(viewer, 1, 0);

  //Create Buttons

  QGroupBox* btnGrp = new QGroupBox(this, "btnGrp");
  btnGrp->setColumnLayout(0, Qt::Vertical);
  btnGrp->layout()->setSpacing(0);
  btnGrp->layout()->setMargin(0);
  QGridLayout* btnGrpLayout = new QGridLayout(btnGrp->layout());
  btnGrpLayout->setAlignment(Qt::AlignTop);
  btnGrpLayout->setSpacing(6);
  btnGrpLayout->setMargin(11);
  topLayout->addWidget(btnGrp, 1, 0);

  /* No button */
  myButtonOk = new QPushButton(btnGrp, "buttonOk");
  myButtonOk->setText(tr("GEOM_BUT_YES"));
  myButtonOk->setAutoDefault(TRUE);
  btnGrpLayout->addWidget(myButtonOk, 0, 0);

  btnGrpLayout->addItem(new QSpacerItem(5, 5, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 1);

  /* Yes button */
  myButtonCancel = new QPushButton(btnGrp, "buttonCancel");
  myButtonCancel->setText(tr("GEOM_BUT_NO"));
  myButtonCancel->setAutoDefault(TRUE);
  myButtonCancel->setDefault(TRUE);
  btnGrpLayout->addWidget(myButtonCancel, 0, 2);

  /* signals and slots connections */
  connect(myButtonOk, SIGNAL(clicked()), this, SLOT(accept()));
  connect(myButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


//====================================================================================== 
// function : ~GEOMBase_aWarningDlg() destructor
// purpose  : Destroys the object and frees any allocated resources
//====================================================================================== 
GEOMBase_aWarningDlg::~GEOMBase_aWarningDlg()
{
  // no need to delete child widgets, Qt does it all for us
}



