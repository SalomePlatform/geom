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
//  File   : GEOMBase_TransparencyDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_TRANSPARENCYDLG_H
#define DIALOGBOX_TRANSPARENCYDLG_H

#include <qvariant.h>
#include <qdialog.h>

/* #include "SALOME_Selection.h" */
/* #include "SALOME_InteractiveObject.hxx" */
/* #include "GEOM_InteractiveObject.hxx" */
/* #include "GEOM_AISShape.hxx" */

#include "GEOMBase.h"


class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QFrame;
class QLabel;
class QPushButton;
class QSlider;



//=================================================================================
// class    : GEOMBase_TransparencyDlg
// purpose  :
//          : WARNING : that is a MODAL dialog.
//=================================================================================
class GEOMBase_TransparencyDlg : public QDialog
{ 
    Q_OBJECT

public:
    GEOMBase_TransparencyDlg( QWidget* parent = 0,
				 const char* name = 0,
				 SALOME_Selection* Sel = 0,
				 const Handle(AIS_InteractiveContext)& ic = 0,
				 bool modal = TRUE,
				 WFlags fl = 0 );

    ~GEOMBase_TransparencyDlg();

private :

    GEOMBase*                        myGeomBase ;    /* Current GeomGUI object     */
    bool                             myFirstInit ;   /* Inform for the first init  */
    SALOME_Selection*                mySel;          /* User selection             */
    Handle(AIS_InteractiveContext)   myIc ;          /* Interactive context        */
    
    QPushButton* buttonOk;
    QLabel* TextLabelOpaque;
    QLabel* TextLabelTransparent;
    QSlider* Slider1;

public slots:
      
    void ClickOnOk();
    void ClickOnClose();
    void ValueHasChanged( int newValue ) ;
    
protected:
    QGridLayout* GEOMBase_TransparencyDlgLayout;
    QHBoxLayout* Layout1;
    QHBoxLayout* Layout2;
};

#endif // DIALOGBOX_TRANSPARENCYDLG_H

