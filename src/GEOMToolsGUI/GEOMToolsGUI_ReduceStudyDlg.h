// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// internal includes
#include "GEOM_ToolsGUI.hxx"

// GEOM includes
#include <GEOMUtils.hxx>
#include <GeometryGUI.h>
#include <GEOM_Displayer.h>

// Qt includes
#include <QDialog>
#include <QTreeWidget>
#include <QCheckBox>
#include <QButtonGroup>
#include <QGroupBox>

// Cpp includes
#include <set>

class GEOMToolsGUI_TreeWidgetItem : public QTreeWidgetItem
{

public:
  GEOMToolsGUI_TreeWidgetItem( QTreeWidget*, const QStringList&, char*, int = Type );
  GEOMToolsGUI_TreeWidgetItem( QTreeWidgetItem*, const QStringList&, char*, int = Type );
  ~GEOMToolsGUI_TreeWidgetItem();

  bool    isVisible();
  void    setVisible( bool, QIcon& );

  char*   getStudyEntry() const;

private:
  bool    myVisible;
  char*   myStudyEntry;

};

class GEOMTOOLSGUI_EXPORT GEOMToolsGUI_ReduceStudyDlg : public QDialog
{ 
  Q_OBJECT

public:
  GEOMToolsGUI_ReduceStudyDlg( QWidget* );
  ~GEOMToolsGUI_ReduceStudyDlg();

private slots:
  void                          onItemClicked(QTreeWidgetItem*, int );
  void                          onHeaderClicked( int );

  void                          selectionChanged();

  void                          update();

  void                          clickOnOk();
  void                          clickOnHelp();

private:
  void                          init( const std::set<std::string>& theObjectEntries );
  std::set<std::string>         getSelectedObjects() const;

  void                          createTreeWidget( QTreeWidget* );
  QGroupBox*                    createButtonGroup( QButtonGroup* );

  void                          addObjectsToTree( QTreeWidget*, std::set<std::string>& );
  GEOMToolsGUI_TreeWidgetItem*  addSubObject( QTreeWidget*, std::set<std::string>&, GEOM::GEOM_Object_var );
  GEOMToolsGUI_TreeWidgetItem*  findObjectInTree( QTreeWidget*, GEOM::GEOM_Object_var );

  void                          checkVisibleIcon( QTreeWidget* );
  bool                          isObjectDrawable( std::string );

  void                          unpublishObjects( std::set<std::string>& );

  void                          removeObjects( std::set<std::string>& );
  void                          removeObject( std::string& );

  void                          removeEmptyFolders();
  void                          getEmptyFolders( _PTR(SObject), std::set<std::string>& );

  QTreeWidget*                  myTreeKeptObjects;
  QTreeWidget*                  myTreeRemoveObjects;
  std::map<QTreeWidget*,bool>   myMapTreeSelectAll;

  QButtonGroup*                 myGroupIntermediates;
  QButtonGroup*                 myGroupSubObjects;

  QCheckBox*                    myCBRemoveEmptyFolder;
  QCheckBox*                    myCBSoftRemoval;

  std::set<std::string>         myMainEntries;

  std::set<std::string>         myKeptObjects;
  std::set<std::string>         myRemovedObjects;
  std::set<std::string>         myListParents;
  std::set<std::string>         myListSubObjects;

  QIcon                         myVisible;
  QIcon                         myInvisible;

  GEOM_Displayer                myDisplayer;
  SalomeApp_Application*        myApp;
  _PTR(Study)                   myStudy;

};

#endif
