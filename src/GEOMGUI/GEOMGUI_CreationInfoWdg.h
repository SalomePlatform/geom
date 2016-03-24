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

#ifndef GEOMGUI_CREATIONINFOWDG_H
#define GEOMGUI_CREATIONINFOWDG_H

#include "GEOM_GEOMGUI.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

#include <QWidget>

class QLabel;
class QLineEdit;
class QString;
class QTreeWidget;
class QTreeWidgetItem;
class SalomeApp_Application;

/*!
 * \brief A widget showing information about object creation:
 *  - operation icon
 *  - operation name
 *  - parameter values
 *
 * Issue 0021684
 */
class GEOMGUI_EXPORT GEOMGUI_CreationInfoWdg : public QWidget
{
  Q_OBJECT;

 public:
  GEOMGUI_CreationInfoWdg( SalomeApp_Application* app );
  ~GEOMGUI_CreationInfoWdg();

  void setInfo( GEOM::CreationInformationSeq& info );
  void clear();

  int getWinID() { return myWindowID; }

 private:

  QTreeWidgetItem* addOperation(const QPixmap& icon, const QString& name);
  void addParam (QTreeWidgetItem* operation, const QString& name, const QString& value);

 QLabel*      myIconLbl;
  //QLineEdit*   myOperaionLnEd;
  QTreeWidget* myParamsTreeWd;
  int          myWindowID;

};
#endif
