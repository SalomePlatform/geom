// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File      : GEOMGUI_CreationInfoWdg.cxx
// Created   : Fri Jun  7 13:07:59 2013
// Author    : Edward AGAPOV (eap)

#include "GEOMGUI_CreationInfoWdg.h"

#include <SalomeApp_Application.h>
#include <SUIT_Desktop.h>

#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

GEOMGUI_CreationInfoWdg::GEOMGUI_CreationInfoWdg( SalomeApp_Application* app )
//:QWidget( app->desktop() )
{
  setWindowTitle(tr("CREATION_INFO_TITLE"));

  QFrame* frame = new QFrame( this );

  QVBoxLayout* myLayout = new QVBoxLayout( this );
  myLayout->addWidget( frame );
  myLayout->setMargin(0);

  QGroupBox* operationGB = new QGroupBox( tr( "OPERATION" ), frame );

  myIconLbl      = new QLabel( operationGB );
  myOperaionLnEd = new QLineEdit( operationGB );
  myParamsTreeWd = new QTreeWidget( frame );
  myParamsTreeWd->setColumnCount( 2 );
  myParamsTreeWd->setHeaderLabels( QStringList() << tr( "PARAMETER" ) << tr( "VALUE" ) );
  myParamsTreeWd->header()->setStretchLastSection( true );
  myParamsTreeWd->header()->setResizeMode( 0, QHeaderView::ResizeToContents );

  QHBoxLayout* operationLay = new QHBoxLayout( operationGB );
  operationLay->addWidget( myIconLbl );
  operationLay->addWidget( myOperaionLnEd );
  operationLay->setMargin(5);

  QVBoxLayout* aLayout = new QVBoxLayout( frame );
  aLayout->addWidget( operationGB );
  aLayout->addWidget( myParamsTreeWd );

  // get a free dockable window id
  myWindowID = 1;
  while( app->dockWindow( myWindowID ))
    ++myWindowID;
}

void GEOMGUI_CreationInfoWdg::setOperation(const QPixmap& icon, const QString& name)
{
  myIconLbl->setPixmap( icon );
  myOperaionLnEd->setText( name );
  myParamsTreeWd->clear();

  if ( name.isEmpty() )
    myOperaionLnEd->setText( tr("NO_INFO"));
}

void GEOMGUI_CreationInfoWdg::addParam (const QString& name, const QString& value)
{
  QTreeWidgetItem* item = new QTreeWidgetItem( myParamsTreeWd );

  item->setFlags( item->flags() | Qt::ItemIsEditable );
  item->setExpanded( true );

  item->setText( 0, name );
  item->setText( 1, value );
}

void GEOMGUI_CreationInfoWdg::clear()
{
  myIconLbl->setPixmap( QPixmap() );
  myOperaionLnEd->setText( "" );
  myParamsTreeWd->clear();
}

GEOMGUI_CreationInfoWdg::~GEOMGUI_CreationInfoWdg()
{
  std::cout<<"~GEOMGUI_CreationInfoWdg"<<std::endl;
}
