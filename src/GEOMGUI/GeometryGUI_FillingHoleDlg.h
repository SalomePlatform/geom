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
//  File   : GeometryGUI_FillingHoleDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef GEOMETRYGUI_FILLINGHOLE_H
#define GEOMETRYGUI_FILLINGHOLE_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <qvariant.h>
#include <qdialog.h>

#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_FillingHoleDlg
// purpose  :
//=================================================================================
class GeometryGUI_FillingHoleDlg : public QDialog
{ 
    Q_OBJECT

public :
    GeometryGUI_FillingHoleDlg( QWidget* parent = 0,
				const char* name = 0,
				SALOME_Selection* Sel = 0,		
				Handle (AIS_InteractiveContext) ic = 0,       
				bool modal = FALSE,
				WFlags fl = 0 );

    ~GeometryGUI_FillingHoleDlg();

private :
      
    void Init( SALOME_Selection* Sel, Handle (AIS_InteractiveContext) ic ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                /* mouse enter the QWidget */
    void ResetStateOfDialog() ;
    
    /* Interactive and local context management see also : bool myUseLocalContext() */
    Handle (AIS_InteractiveContext) myIC ;                 /* Interactive context */ 
    Standard_Integer                myLocalContextId ;     /* identify a local context used by this method */
    bool                            myUseLocalContext ;    /* true when this method as opened a local context  */

    GEOM::GEOM_Gen_var           myGeom ;                /* Current Geom object */
    GeometryGUI*                    myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*               mySelection ;           /* User shape selection */

    TopoDS_Shape                    myShape ;
    char*                           myShapeIOR ;
    bool                            myOkShape ;

    bool                            myOkSelectSubMode ;     /* true = sub mode selection activated */
    QLineEdit*                      myEditCurrentArgument;  /* Current LineEdit */   
    int                             myConstructorId ;       /* Current constructor id = radio button id */
 
    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;

    QGroupBox* GroupC1;
    QLabel* TextLabelC1A1;
    QPushButton* SelectButtonC1A1;
    QLineEdit* LineEditC1A1;
    QCheckBox* CheckBox1;

    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonApply;
    QPushButton* buttonClose;


private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnOk();
    void ClickOnApply() ;
    void ClickOnClose();

    void LineEditReturnPressed() ;
    void SetEditCurrentArgument() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;
    void ActivateUserSelection() ;

protected:

    QGridLayout* GeometryGUI_FillingHoleDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupC1Layout;
    QHBoxLayout* Layout2;
    QGridLayout* GroupButtonsLayout;
};

#endif // GEOMETRYGUI_FILLINGHOLE_H
