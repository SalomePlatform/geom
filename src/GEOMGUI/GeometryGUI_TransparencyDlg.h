//  File      : GeometryGUI_TransparencyDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_TRANSPARENCYDLG_H
#define DIALOGBOX_TRANSPARENCYDLG_H

#include <qvariant.h>
#include <qdialog.h>

#include "SALOME_Selection.h"
#include "SALOME_InteractiveObject.hxx"
#include "GEOM_InteractiveObject.hxx"
#include "GEOM_AISShape.hxx"


class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QFrame;
class QLabel;
class QPushButton;
class QSlider;
class GeometryGUI;



//=================================================================================
// class    : GeometryGUI_TransparencyDlg
// purpose  :
//          : WARNING : that is a MODAL dialog.
//=================================================================================
class GeometryGUI_TransparencyDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_TransparencyDlg( QWidget* parent = 0,
				 const char* name = 0,
				 SALOME_Selection* Sel = 0,
				 const Handle(AIS_InteractiveContext)& ic = 0,
				 bool modal = TRUE,
				 WFlags fl = 0 );

    ~GeometryGUI_TransparencyDlg();

private :

    GeometryGUI*                     myGeomGUI ;     /* Current GeomGUI object     */
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
    QGridLayout* GeometryGUI_TransparencyDlgLayout;
    QHBoxLayout* Layout1;
    QHBoxLayout* Layout2;
};

#endif // DIALOGBOX_TRANSPARENCYDLG_H

