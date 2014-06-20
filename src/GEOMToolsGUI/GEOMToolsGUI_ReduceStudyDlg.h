// Copyright (C) 2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef GEOMTOOLSGUI_REDUCESTUDYDLG_H
#define GEOMTOOLSGUI_REDUCESTUDYDLG_H

#include "GEOM_ToolsGUI.hxx"
#include <QDialog>
#include <QTreeWidget>
#include <QCheckBox>
#include <QButtonGroup>
#include <QGroupBox>

#include <GEOMUtils.hxx>
#include <GeometryGUI.h>
#include <GEOM_Displayer.h>

#include <set>

class GEOMToolsGUI_TreeWidgetItem : public QTreeWidgetItem
{

public:
  GEOMToolsGUI_TreeWidgetItem( QTreeWidget*, const QStringList&, char*, bool, int = Type );
  GEOMToolsGUI_TreeWidgetItem( QTreeWidgetItem*, const QStringList&, char*, bool, int = Type );
  ~GEOMToolsGUI_TreeWidgetItem();

  bool isVisible();
  void setVisible( bool, QIcon& );

  char* getStudyEntry() const;

private:
  char* myStudyEntry;
  bool myVisible;
};

class GEOMTOOLSGUI_EXPORT GEOMToolsGUI_ReduceStudyDlg : public QDialog
{ 
  Q_OBJECT

public:
  GEOMToolsGUI_ReduceStudyDlg( QWidget* );
  ~GEOMToolsGUI_ReduceStudyDlg();

private slots:

  void clickOnOk();
  void clickOnCancel();
  void clickOnHelp();

  void onItemClicked(QTreeWidgetItem*, int );
  void onHeaderClicked( int );

  void selectionChanged();

  void update();

private:

  void init( const std::set<std::string>& theObjectEntries );
  void createTreeWidget( QTreeWidget* );
  QGroupBox* createButtonGroup( QButtonGroup* );
  void checkVisibleIcon( QTreeWidget* );
  void sortObjects( QTreeWidget*, std::set<std::string>& );
  GEOMToolsGUI_TreeWidgetItem* addSubObject( QTreeWidget*, std::set<std::string>&, GEOM::GEOM_Object_var );
  GEOMToolsGUI_TreeWidgetItem* findObjectInTree( QTreeWidget*, GEOM::GEOM_Object_var );

  void unpublishObjects( std::set<std::string>& );
  void removeObjects( std::set<std::string>& );

  std::set<std::string> getSelectedObjects() const;

  QTreeWidget* myTreeKeptObjects;
  QTreeWidget* myTreeRemoveObjects;

  QButtonGroup* myGroupIntermediates;
  QButtonGroup* myGroupSubObjects;

  QCheckBox* myCBRemoveEmptyFolder;
  QCheckBox* myCBSoftRemoval;

  std::set<std::string> myMainEntries;

  QIcon myVisible;
  QIcon myInvisible;

  GEOM_Displayer myDisplayer;
  SalomeApp_Application* myApp;
  SALOMEDS::Study_var myStudy;

  std::set<std::string> myKeptObjects;
  std::set<std::string> myRemovedObjects;
  std::set<std::string> myListParents;
  std::set<std::string> myListSubObjects;

  std::map<QTreeWidget*,bool> myMapTreeSelectAll;
};

#endif
