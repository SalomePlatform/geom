//  File      : GeometryGUI_NbIsosDlg.h
//  Created   : Wed Mar 13 12:27:28 2002
//  Author    : 
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : OPEN CASCADE
//  $Header: 


#ifndef GEOMETRYGUI_NBISOSDLG_H
#define GEOMETRYGUI_NBISOSDLG_H

#include <qdialog.h>

class QLabel;
class QSpinBox;
class QPushButton;
class GeometryGUI;

//=================================================================================
// class    : GeometryGUI_NbIsosDlg
// purpose  :
//=================================================================================
class GeometryGUI_NbIsosDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_NbIsosDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_NbIsosDlg();

    GeometryGUI*  myGeomGUI ;    /* Current GeomGUI object */

    QPushButton*  buttonOk;
    QPushButton*  buttonCancel;
    QLabel*       TextLabel1 ;
    QLabel*       TextLabel2 ;
    QSpinBox*     SpinBoxU ;
    QSpinBox*     SpinBoxV ;
};

#endif // GEOMETRYGUI_NBISOSDLG_H
