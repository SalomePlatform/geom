//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org
//
//
//
//  File   : RepairGUI_FreeFacesDlg.h
//  Author : VKN
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_FreeFaces_H
#define DIALOGBOX_FreeFaces_H

#include <qdialog.h>
#include "GEOMBase_Helper.h"

class GEOM_Displayer;
class SALOME_Selection;
class SALOME_Prs;
class QPushButton;
class QLineEdit;

//=================================================================================
// class    : RepairGUI_FreeFacesDlg
// purpose  :
//=================================================================================
class RepairGUI_FreeFacesDlg : public QDialog,
                               public GEOMBase_Helper
{
    Q_OBJECT

public:
    RepairGUI_FreeFacesDlg(QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~RepairGUI_FreeFacesDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

private :

    void Init(SALOME_Selection*);
    void enterEvent(QEvent* e);
    void closeEvent(QCloseEvent* e);
    void activateSelection();
    GEOM_Displayer*           getDisplayer();

private slots:

  void                                  onClose();
  void                                  onDeactivate();
  void                                  onActivate();
  void                                  onSelectionDone();
  void                                  onSetEditCurrentArgument();                              

private :

    GEOM_Displayer*        myDisplayer;
    GEOM::GEOM_Object_var  myObj;
    SALOME_Selection*      mySelection;
    QPushButton*           mySelBtn;
    QLineEdit*             myEdit;

};

#endif // DIALOGBOX_FreeFaces_H
