/****************************************************************************
** Form interface generated from reading ui file 'MeasureGUI_2Sel4LineEdit_QTD.ui'
**
** Created: Tue Oct 9 14:31:27 2007
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MEASUREGUI_2SEL4LINEEDIT_QTD_H
#define MEASUREGUI_2SEL4LINEEDIT_QTD_H

#include "GEOM_MeasureGUI.hxx"

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class QPushButton;
class QLineEdit;

class GEOM_MEASUREGUI_EXPORT MeasureGUI_2Sel4LineEdit_QTD : public QWidget
{
    Q_OBJECT

public:
    MeasureGUI_2Sel4LineEdit_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~MeasureGUI_2Sel4LineEdit_QTD();

    QGroupBox* GroupBox1;
    QLabel* TextLabel1;
    QPushButton* PushButton1;
    QLineEdit* LineEdit1;
    QLabel* TextLabel2;
    QPushButton* PushButton2;
    QLineEdit* LineEdit2;
    QLabel* TextLabel3;
    QLineEdit* LineEdit3;
    QLabel* TextLabel4;
    QLineEdit* LineEdit4;
    QLabel* TextLabel5;
    QLineEdit* LineEdit5;
    QLabel* TextLabel6;
    QLineEdit* LineEdit6;

protected:
    QGridLayout* MeasureGUI_2Sel4LineEdit_QTDLayout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* Layout1;
    QSpacerItem* Spacer8;

protected slots:
    virtual void languageChange();

};

#endif // MEASUREGUI_2SEL4LINEEDIT_QTD_H
