// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// GEOM GEOMGUI : reading of xml file with list of available plugin actions
// File   : GEOMGUI_XmlHandler.cxx
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S.

#include "GEOMGUI_XmlHandler.h"

#include "GEOMGUI.h"

// SALOME KERNEL includes
#include <utilities.h>

/*!
 * Constructor
 */
GEOMGUI_XmlHandler::GEOMGUI_XmlHandler()
{
}

/*!
 * Destructor
 */
GEOMGUI_XmlHandler::~GEOMGUI_XmlHandler()
{
}

/*!
 * Starts parsing of document. Does some initialization
 *
 * Reimplemented from QXmlDefaultHandler.
 */
bool GEOMGUI_XmlHandler::startDocument()
{
  myErrorProt = "";
  return true;
}

/*!
 * Does different actions depending on the name of the tag and the
 * state you are in document.
 *
 * Reimplemented from QXmlDefaultHandler.
 */
bool GEOMGUI_XmlHandler::startElement (const QString&, const QString&,
                                       const QString& qName,
                                       const QXmlAttributes& atts)
{
  if (qName == "geom-plugins") { // set of plugins
    //myHypothesesMap.clear();
    //myAlgorithmsMap.clear();
  }
  else if (qName == "geom-plugin") { // group of actions
    myPluginData.myName      = atts.value("name");
    myPluginData.myServerLib = atts.value("server-lib");
    myPluginData.myClientLib = atts.value("gui-lib");

    //QString aResName = atts.value("resources");
    //if (aResName != "") {
    //  SUIT_ResourceMgr* resMgr = GEOMGUI::resourceMgr();
    //  QString lang = resMgr->stringValue( resMgr->langSection(), "language", "en" );
    //  resMgr->loadTranslator( "resources", QString( "%1_msg_%2.qm" ).arg( aResName, lang ) );
    //  resMgr->loadTranslator( "resources", QString( "%1_images.qm" ).arg( aResName, lang ) );
    //}
  }
  else if (qName == "actions") { // group of actions
  }
  else if (qName == "action") { // an action
    GEOMGUI_ActionData aData;
    aData.myLabel     = atts.value("label");
    aData.myIcon      = atts.value("icon");
    aData.myMenu      = atts.value("menu");
    aData.myTooltip   = atts.value("tooltip");
    aData.myStatusBar = atts.value("status-bar");

    myPluginData.myListOfActions.append(aData);
  }
  else {
    // error
    return false;
  }
  return true;
}

/*!
 * Reimplemented from QXmlDefaultHandler.
 */
bool GEOMGUI_XmlHandler::endElement (const QString&, const QString&, const QString&)
{
  return true;
}

/*!
 * Reimplemented from QXmlDefaultHandler.
 */
bool GEOMGUI_XmlHandler::characters (const QString& ch)
{
  // we are not interested in whitespaces
  //QString ch_simplified = ch.simplified();
  //if ( ch_simplified.isEmpty() )
  //  return true;
  return true;
}

/*!
 * Returns the default error string.
 *
 * Reimplemented from QXmlDefaultHandler.
 */
QString GEOMGUI_XmlHandler::errorString()
{
  return "the document is not in the quote file format";
}

/*!
 * Returns the error protocol if parsing failed
 *
 * Reimplemented from QXmlDefaultHandler.
 */
QString GEOMGUI_XmlHandler::errorProtocol()
{
  return myErrorProt;
}

/*!
 * Returns exception
 *
 * Reimplemented from QXmlDefaultHandler.
 */
bool GEOMGUI_XmlHandler::fatalError (const QXmlParseException& exception)
{
  myErrorProt += QString("fatal parsing error: %1 in line %2, column %3\n")
    .arg(exception.message())
    .arg(exception.lineNumber())
    .arg(exception.columnNumber());

  return QXmlDefaultHandler::fatalError( exception );
}
