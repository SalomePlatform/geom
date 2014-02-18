// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <QWidget>

class QString;
class QLabel;
class QLineEdit;
class QTreeWidget;
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

  int getWinID() { return myWindowID; }

  void clear();
  void setOperation(const QPixmap& icon, const QString& name);
  void addParam    (const QString& name, const QString& value);

 private:

  QLabel*      myIconLbl;
  QLineEdit*   myOperaionLnEd;
  QTreeWidget* myParamsTreeWd;
  int          myWindowID;

};
#endif
