//  File      : GeometryGUI_PartitionDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_PARTITION_H
#define DIALOGBOX_PARTITION_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <qdialog.h>

class QButtonGroup;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QComboBox;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_PartitionDlg
// purpose  :
//=================================================================================
class GeometryGUI_PartitionDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_PartitionDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_PartitionDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;             /* Mouse enter the QWidget */

    GEOM::GEOM_Gen_var myGeom ;                /* Current Geom object */
    GeometryGUI*       myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*  mySelection ;           /* User shape selection */

    GEOM::GEOM_Gen::ListOfIOR  myListShapes ;
    GEOM::GEOM_Gen::ListOfIOR  myListTools ;
    GEOM::GEOM_Gen::ListOfIOR  myListRemoveInside ;
    GEOM::GEOM_Gen::ListOfIOR  myListKeepInside ;

    bool                       myOkListShapes ;        /* to check when argument is defined */
    bool                       myOkListTools ;         /* to check when argument is defined */
    bool                       myOkKeepShape ;         /* to check when argument is defined */
    bool                       myOkRemoveShape ;       /* to check when argument is defined */
    int                        myLimit;
    QLineEdit*                 myEditCurrentArgument;  /* Current LineEdit */    

    /* common buttons */
    QGroupBox*     GroupButtons;
    QPushButton*   buttonOk;
    QPushButton*   buttonCancel;
    QPushButton*   buttonApply;

    /* constructor radiobuttons */
    QButtonGroup*  GroupConstructors;
    QRadioButton*  Constructor1;

    /* Arguments group */
    QGroupBox*     GroupC1;
    QLineEdit*     LineEditC1A1;
    QLabel*        TextLabelC1A1;
    QPushButton*   SelectButtonC1A1;
    QLineEdit*     LineEditC1A2;
    QLabel*        TextLabelC1A2;
    QPushButton*   SelectButtonC1A2;

    /* limit */
    QComboBox*     ComboBox1;
    QLabel*        TextLabelComboBox1;

    /* Keep Inside and Remove Inside feilds */
    QGroupBox*     GroupC2;
    QLineEdit*     LineEditC2A1;
    QLabel*        TextLabelC2A1;
    QPushButton*   SelectButtonC2A1;
    QLineEdit*     LineEditC2A2;
    QLabel*        TextLabelC2A2;
    QPushButton*   SelectButtonC2A2;

private slots:

    //void ConstructorsClicked(int constructorId);
    void ClickOnOk();
    void ClickOnCancel();
    void ClickOnApply();
    void SetEditCurrentArgument() ;
    void LineEditReturnPressed() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;
    void ComboTextChanged() ;
};

#endif // DIALOGBOX_PARTITION_H
