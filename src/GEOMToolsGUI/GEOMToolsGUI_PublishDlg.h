// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : GEOMToolsGUI_PublishDlg.h
// Author : Roman NIKOLAEV, Open CASCADE S.A.S.
//

#ifndef GEOMTOOLSGUI_PUBLISHDLG_H
#define GEOMTOOLSGUI_PUBLISHDLG_H

#include "GEOM_ToolsGUI.hxx"

#include <QMap>
#include <QList>
#include <QDialog>
#include <QTreeWidgetItem>




#include <SALOMEDSClient.hxx>

class QTreeWidget;
class QTreeWidgetItem;

class SalomeApp_DataObject;

typedef QPair<QString, QString> Pair;
typedef QList<Pair > BufferedList;
typedef QMap<QString, QTreeWidgetItem*> Entry2ItemMap;

class GEOMTOOLSGUI_EXPORT GEOMToolsGUI_PublishDlg : public QDialog
{ 
  Q_OBJECT

public:
  GEOMToolsGUI_PublishDlg( QWidget* );
  ~GEOMToolsGUI_PublishDlg();

protected:
  void               initData();
  void               buildTree(_PTR(Study) theStudy, SalomeApp_DataObject* theItem);
  QTreeWidgetItem*   findParentItem(_PTR(Study) theStudy, SalomeApp_DataObject* theItem, BufferedList& theList);
  QTreeWidgetItem*   createItem(QTreeWidgetItem* theItem, Pair theAttributes, bool isCheckable);
  void               getDrawableList(QTreeWidgetItem* theItem, QList<QTreeWidgetItem*>& theList);
  void               getTails(QTreeWidgetItem* theItem, QList<QTreeWidgetItem*>& theList);
  
protected slots:
  void clickOnOk();
  void clickOnApply();
  void clickOnSelectAll();
  void clickOnUnSelectAll();
  void onItemClicked(QTreeWidgetItem*, int);
  
private:  
  QTreeWidget*           myTreeWidget;
  SalomeApp_DataObject*  myGeomRoot;
  Entry2ItemMap          myEntryToItem;
  void processAll(QTreeWidgetItem* theItem, Qt::CheckState state);
};



#endif //GEOMTOOLSGUI_PUBLISHDLG_H
