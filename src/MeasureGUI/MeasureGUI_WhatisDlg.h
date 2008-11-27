//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  GEOM GEOMGUI : GUI for Geometry component
//  File   : MeasureGUI_WhatisDlg.h
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$
//
#ifndef DIALOGBOX_WHATIS_H
#define DIALOGBOX_WHATIS_H

#include "GEOM_MeasureGUI.hxx"

#include "MeasureGUI_Skeleton.h"

//class MeasureGUI_1Sel1TextView_QTD;

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QMessageBox;

class GEOM_MEASUREGUI_EXPORT MeasureGUI_Whatis_QTD : public QWidget
{ 
    Q_OBJECT

public:
    MeasureGUI_Whatis_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~MeasureGUI_Whatis_QTD();

    QGroupBox* GroupBox1;

    QLabel* TextLabel1;
    QPushButton* PushButton1;
    QLineEdit* LineEdit1;

    QTextEdit* TextEdit1;

    QLabel* TextLabel2;
    QLineEdit* LineEdit2;
    QPushButton* PushButton2;

protected:
    QGridLayout* MeasureGUI_Whatis_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
};


//=================================================================================
// class    : MeasureGUI_WhatisDlg
// purpose  :
//=================================================================================
class GEOM_MEASUREGUI_EXPORT MeasureGUI_WhatisDlg : public MeasureGUI_Skeleton
{ 
    Q_OBJECT

public:
    MeasureGUI_WhatisDlg (GeometryGUI* GUI, QWidget* parent);
    ~MeasureGUI_WhatisDlg();

protected:
    // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
    virtual void processObject();

protected slots:
    virtual void ClickOnProperties();

private:
    void         Init();
    bool         getParameters( QString& );
    QString      getKindOfShape( QString& );

private:
    //MeasureGUI_1Sel1TextView_QTD* myGrp;
    MeasureGUI_Whatis_QTD* myGrp;
    //MeasureGUI_Params_Dlg* myParamsDlg;
    QMessageBox* myParamsDlg;
};

#endif // DIALOGBOX_WHATIS_H
