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

#ifndef RepairGUI_InspectObjectDlg_H
#define RepairGUI_InspectObjectDlg_H

// GEOM includes
#include <GEOMBase_Helper.h>

// Qt includes
#include <QDialog>
#include <QTreeWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>

class RepairGUI_InspectObjectDlg : public QDialog, public GEOMBase_Helper
{ 
  Q_OBJECT

  class TreeWidgetItem;
  class Delegate;

public:
  RepairGUI_InspectObjectDlg( SUIT_Desktop* );
  ~RepairGUI_InspectObjectDlg();

private slots:
  void                    onEditMainShape();

  void                    onItemClicked( QTreeWidgetItem*, int );
  void                    onItemChanged( QTreeWidgetItem*, int );
  void                    onItemSelectionChanged();
  void                    onItemExpanded( QTreeWidgetItem* );
  void                    onHeaderClicked( int );
  void                    onItemDoubleClicked( QTreeWidgetItem*, int );

  void                    onViewSelectionChanged();

  void                    onWindowActivated( SUIT_ViewWindow* );
  void                    onCloseView( SUIT_ViewWindow* );

  void                    clickOnShow();
  void                    clickOnShowOnly();
  void                    clickOnHide();
  void                    clickOnPublish();
  void                    clickOnHelp();

private:
  void                    init();
  void                    checkVisibleIcon();
  void                    addSubObjects( TreeWidgetItem* );
  void                    displayItem( TreeWidgetItem* );
  void                    setItemDisplayStatus( TreeWidgetItem* theItem, bool theIsVisible );
  void                    setMainObjectTransparency( double );
  void                    restoreParam();

  SalomeApp_Application*     myApp;
  QPointer<SUIT_ViewWindow>  myViewWindow;

  QIcon                      myVisible;
  QIcon                      myInvisible;

  QTreeWidget*               myTreeObjects;
  QLineEdit*                 myEditMainShape;

  bool                       myIsSelectAll;
  double                     myTransparency;

};

#endif
