// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File      : GEOMGUI_CreationInfoWdg.cxx
// Created   : Fri Jun  7 13:07:59 2013
// Author    : Edward AGAPOV (eap)

#include "GEOMGUI_CreationInfoWdg.h"
 
#include "GEOMImpl_Types.hxx"

#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>

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
  setWindowTitle( tr( "CREATION_INFO_TITLE" ) );

  myParamsTreeWd = new QTreeWidget( this );
  myParamsTreeWd->setColumnCount( 2 );
  myParamsTreeWd->setHeaderLabels( QStringList() << tr( "PARAMETER" ) << tr( "VALUE" ) );
  myParamsTreeWd->header()->setStretchLastSection( true );
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  myParamsTreeWd->header()->setResizeMode( 0, QHeaderView::ResizeToContents );
#else
  myParamsTreeWd->header()->setSectionResizeMode( 0, QHeaderView::ResizeToContents );
#endif

  QVBoxLayout* aLayout = new QVBoxLayout( this );
  aLayout->addWidget( myParamsTreeWd );
  aLayout->setMargin(11);

  // get a free dockable window id
  myWindowID = 10;
  while( app->dockWindow( myWindowID ))
    ++myWindowID;
  ++myWindowID; // pb when a GEOM is a sole module: CreationInfoWdg replaces Python console
}

QTreeWidgetItem* GEOMGUI_CreationInfoWdg::addOperation(const QPixmap& icon, const QString& name)
{
  QTreeWidgetItem* item = new QTreeWidgetItem( myParamsTreeWd );

  item->setIcon( 0, icon );
  item->setText( 0, name );
  if ( name.isEmpty() )
    item->setText( 0, tr("NO_INFO"));

  item->setExpanded( true );

  return item;
}

void GEOMGUI_CreationInfoWdg::addParam (QTreeWidgetItem* operation,
                                        const QString&   name,
                                        const QString&   value)
{
  QTreeWidgetItem* item = new QTreeWidgetItem( operation );

  //item->setFlags( Qt::NoItemFlags );
  item->setExpanded( true );

  item->setText( 0, name );
  item->setText( 1, value );
}

void GEOMGUI_CreationInfoWdg::clear()
{
  myParamsTreeWd->clear();
}

GEOMGUI_CreationInfoWdg::~GEOMGUI_CreationInfoWdg()
{
  //std::cout<<"~GEOMGUI_CreationInfoWdg"<<std::endl;
}

void GEOMGUI_CreationInfoWdg::setInfo( GEOM::CreationInformationSeq& info )
{
  clear();

  QPixmap icon;
  QString operationName;

  try
  {
    if ( &info && info.length() > 0 )
    {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      for ( int j = 0, nb = info.length(); j < nb; ++j )
      {
        QString name = info[j].operationName.in();
        if ( !name.isEmpty() )
        {
          // get plugin_name if any
          QString plugin_name;
          for ( size_t i = 0; i < info[j].params.length(); ++i )
          {
            QString value = info[j].params[i].name.in();
            if ( value == PLUGIN_NAME )
              plugin_name = info[j].params[i].value.in();
          }
          // get icon
          QString prefix = plugin_name.isEmpty() ? "GEOM" : plugin_name;
          if ( name.startsWith( "Import"))
            icon = resMgr->loadPixmap( "GEOM", tr("ICO_IMPORT_SHAPE"), true );
          else
            icon = resMgr->loadPixmap( prefix, tr( ("ICO_"+name).toLatin1().constData() ), false );

          // translate operation name
          operationName = tr( ("MEN_"+name).toLatin1().constData() );
          if ( operationName.startsWith( "MEN_" ))
            operationName = name; // no translation

          QTreeWidgetItem* operation = addOperation( icon, operationName );

          // add parameters
          for ( size_t i = 0; i < info[j].params.length(); ++i )
            addParam( operation,
                      info[j].params[i].name.in(),
                      info[j].params[i].value.in() );
        }
      }
    }
    else
    {
      addOperation( icon, operationName );
    }
  }
  catch (...)
  {
  }
}
