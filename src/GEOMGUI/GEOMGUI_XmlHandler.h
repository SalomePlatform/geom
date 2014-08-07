// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// GEOM GEOMGUI : reading of xml file with list of available plugin actions
// File   : GEOMGUI_XmlHandler.h
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S.

#ifndef GEOMGUI_XMLHANDLER_H
#define GEOMGUI_XMLHANDLER_H

// GEOM includes
#include "GEOMGUI.h"

// Qt includes
#include <QXmlDefaultHandler>
#include <QMap>
#include <QList>

//GUI includes
#include <QtxMap.h>

//class HypothesisData;
//class HypothesesSet;

struct GEOMGUI_ActionData
{
  QString myLabel; // unique ID
  QString myIcon;
  QString myMenu;
  QString myTooltip;
  QString myStatusBar;

  //int     myAccel;
  //bool    myToggle;
  //QString myShortcut;
};

struct GEOMGUI_PluginData
{
  QString myName;
  QString myServerLib;
  QString myClientLib;

  //QList<GEOMGUI_ActionData*> myListOfActions;
  QList<GEOMGUI_ActionData> myListOfActions;
};

class GEOMGUI_EXPORT GEOMGUI_XmlHandler : public QXmlDefaultHandler
{
public:
  GEOMGUI_XmlHandler();
  virtual ~GEOMGUI_XmlHandler();

  bool     startDocument();
  bool     startElement( const QString&, const QString&, 
                         const QString&, const QXmlAttributes& );
  bool     endElement( const QString&, const QString&, const QString& );
  bool     characters( const QString& ); 

  QString  errorString();
  QString  errorProtocol();
  bool     fatalError( const QXmlParseException& );

public:
  GEOMGUI_PluginData       myPluginData;

private:
  QString                  myErrorProt;
};

#endif // GEOMGUI_XMLHANDLER_H
