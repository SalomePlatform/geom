//  File      : GeometryGUI_aParameterDlg.h
//  Created   : july 5 2001
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : Open CASCADE
//  $Header$

#ifndef GeometryGUI_aParameterDLG_H
#define GeometryGUI_aParameterDLG_H

#include <qdialog.h>

class QPushButton;
class GeometryGUI;
class QAD_SpinBoxDbl;

//=================================================================================
// class    : GeometryGUI_aParameterDlg
// purpose  :
//=================================================================================
class GeometryGUI_aParameterDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_aParameterDlg( const char* aValue1 = "25",
			       const char* aTitle1 = "Value :",
			       QWidget* parent     = 0,
			       const char* name    = 0, 
			       bool modal          = FALSE,
			       WFlags fl           = 0,
			       const double bottom = -1E6,
			       const double top    = +1E6,
			       const int decimals  = 6 ) ;

    ~GeometryGUI_aParameterDlg();

    void   setValue( double val );
    double getValue();

private:
    GeometryGUI*    myGeomGUI ;    /* Current GeomGUI object */

    QPushButton*    myButtonOk;
    QPushButton*    myButtonCancel;
    QAD_SpinBoxDbl* mySpinBox;
 
};

#endif // GeometryGUI_aParameterDlg.h
